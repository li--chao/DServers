#include "epollcli.h"
#include "cluster.h"

LcEpollCli::LcEpollCli() :
m_pBaseConfig(NULL)
{



}

int LcEpollCli::Init(BaseConfig* pBaseConfig, TextLog* pLog)
{
	m_pBaseConfig = pBaseConfig;
	m_pCoreLog = pLog;

	m_pEpollEvs = new struct epoll_event[pBaseConfig->m_uiCliMaxOverLapNum];
	m_szpRecvPackMem = new char[pBaseConfig->m_uiMaxPacketSize * pBaseConfig->m_uiCliMaxOverLapNum];
	m_szpWorkPackMem = new char[pBaseConfig->m_uiMaxPacketSize * pBaseConfig->m_uiCliMaxOverLapNum];
	m_szpSndPackMem = new char[pBaseConfig->m_uiMaxPacketSize * pBaseConfig->m_uiCliMaxOverLapNum];
	m_pChecker = new LcFullPktChecker();
	m_pIORecvQue = new OverLap[pBaseConfig->m_uiCliMaxOverLapNum];
	m_pIOWorkQue = new OverLap[pBaseConfig->m_uiCliMaxOverLapNum];
	m_pIOSndQue = new OverLap[pBaseConfig->m_uiCliMaxOverLapNum];

	if(m_pEpollEvs == NULL || m_szpRecvPackMem == NULL || m_szpWorkPackMem == NULL || m_szpSndPackMem == NULL ||
	   m_pChecker == NULL || m_pIORecvQue == NULL || m_pIOWorkQue == NULL || m_pIOSndQue == NULL)
	{
		return 1;
	}

	if(m_IONetConnMemQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetWorkMemQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetWorkMemQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetSndMemQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_ConnTable.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(pthread_mutex_init(&m_SndSync, NULL))
	{
		return -1;
	}

	for(unsigned int u = 0; u < pBaseConfig->m_uiCliMaxOverLapNum; u++)
	{
		m_pIORecvQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);
		m_pIOWorkQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);
		m_pIOSndQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);

		m_IONetConnMemQue.Push((long)&m_pIORecvQue[u]);
		m_IONetWorkMemQue.Push((long)&m_pIOWorkQue[u]);
		m_IONetSndMemQue.Push((long)&m_pIOSndQue[u]);
	}

	m_epSocket = epoll_create(m_pBaseConfig->m_uiCliConcurrentNum);
	if(m_epSocket == -1)
	{
		return 1;
	}

	return 0;
}

int LcEpollCli::StartThread()
{
	pthread_t eplNetThrd;
	
	int ret = 0;
	ret += pthread_create(&eplNetThrd, NULL, Thread_EpollNet, this);
	
	return ret;
}

void LcEpollCli::RequestSnd(long& lAddr)
{
	m_IONetSndMemQue.Pop(lAddr);
}

void LcEpollCli::ReleaseRequest(const long& lAddr)
{
	m_IONetSndMemQue.Push(lAddr);
}

int LcEpollCli::PushRequest(OverLap* pOverLap)
{
	if(pOverLap == NULL || pOverLap->uiComLen == 0)
	{
		return 1;
	}

	OverLap* pConnOverLap;
	if(m_ConnTable.Search(pOverLap->u64SessionID, pConnOverLap))
	{
		m_pCoreLog->Write("%llu can't be found", pOverLap->u64SessionID);
		return 2;
	}

	pthread_mutex_lock(&m_SndSync);
	OverLap* pTmp = pConnOverLap;
	while(pTmp->pSndList)
	{
		pTmp = pTmp->pSndList;
	}

	pTmp->pSndList = pOverLap;
	pthread_mutex_unlock(&m_SndSync);

	struct epoll_event ev;
	ev.events = EPOLLOUT | EPOLLIN | EPOLLET;
	ev.data.ptr = (void*)pConnOverLap;
	if(epoll_ctl(m_epSocket, EPOLL_CTL_MOD, pConnOverLap->fd, &ev) == -1)
	{
		m_pCoreLog->Write("epoll_ctl_mod EPOLLOUT error");
		RemoveConnect(pConnOverLap);
	}
	return 0;
}

int LcEpollCli::Connect(const char* szpNodeIP, const unsigned short& usNodePort, int& fd)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1)
	{
		return -1;
	}

	sockaddr_in nodeSockAddr;
	nodeSockAddr.sin_family = AF_INET;
	nodeSockAddr.sin_addr.s_addr = inet_addr(szpNodeIP);
	nodeSockAddr.sin_port = htons(usNodePort);

	while(connect(fd, (sockaddr*)&nodeSockAddr, sizeof(nodeSockAddr)) == -1)
	{
		if(errno == EINTR || errno == EINPROGRESS)
		{
			continue;
		}

		close(fd);
		fd = -1;
		return 1;
	}

	long lAddr = 0;
	m_IONetConnMemQue.Pop(lAddr);
	OverLap* pOverLap = (OverLap*)lAddr;
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void*)pOverLap;

	if(epoll_ctl(m_epSocket, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		m_IONetConnMemQue.Push((long)pOverLap);
		close(fd);
		return 2;
	}

	pOverLap->fd = fd;
	pOverLap->u64SessionID = Cluster::MkPeerID(szpNodeIP, usNodePort);
	pOverLap->uiComLen = m_pBaseConfig->m_uiMaxPacketSize;
	pOverLap->uiFinishLen = 0;
	pOverLap->uiPeerIP = inet_addr(szpNodeIP);
	pOverLap->usPeerPort = usNodePort;
	pOverLap->u64PacketRecv = 0;
	pOverLap->u64PacketSnd = 0;
	pOverLap->uiPacketLen = 0;
	pOverLap->u64HeartBeatRecv = time(NULL);

	m_ConnTable.Insert(pOverLap->u64SessionID, pOverLap);

	return 0;
}

void* LcEpollCli::Thread_EpollNet(void* vparam)
{
	LcEpollCli* pCliNet = (LcEpollCli*)vparam;
	while(1)
	{
		int nfds = epoll_wait(pCliNet->m_epSocket, pCliNet->m_pEpollEvs, pCliNet->m_pBaseConfig->m_uiCliConcurrentNum, -1);
		if(nfds < 0 && errno == EINTR)
		{
			continue;
		}
		else if(nfds < 0)
		{
			break;
		}
		for(int i = 0; i < nfds; i++)
		{
			if(pCliNet->m_pEpollEvs[i].events & EPOLLIN)
			{
				pCliNet->EpollRecv((OverLap*)pCliNet->m_pEpollEvs[i].data.ptr);
			}
			else if(pCliNet->m_pEpollEvs[i].events & EPOLLOUT)
			{
				pCliNet->EpollSend((OverLap*)pCliNet->m_pEpollEvs[i].data.ptr);
			}
			else
			{
				OverLap* pOverLap = (OverLap*)pCliNet->m_pEpollEvs[i].data.ptr;
				pCliNet->RemoveConnect(pOverLap);
			}
		}
	}	

	return NULL;
}

void LcEpollCli::EpollRecv(OverLap* pOverLap)
{
	int ret = 0;
	bool bIsHeadChecked = false;
	while(1)
	{
		ret = recv(pOverLap->fd, pOverLap->szpComBuf + pOverLap->uiFinishLen, pOverLap->uiComLen, 0);
		if(ret == 0 || ret > (int)pOverLap->uiComLen)
		{
			RemoveConnect(pOverLap);
			m_pCoreLog->Write("connect %llu closed");
			return;
		}
		else if(ret == -1 && errno == EAGAIN)
		{
			break;
		}
		else if(ret == -1)
		{
			RemoveConnect(pOverLap);
			m_pCoreLog->Write("connect %llu closed", pOverLap->u64SessionID);
			return;
		}

		pOverLap->uiComLen -= ret;
		pOverLap->uiFinishLen += ret;
		
		switch(CheckPacket(pOverLap, bIsHeadChecked))
		{
		case 0:
			continue;
		case 1:
		case 2:
			RemoveConnect(pOverLap);
			return;
		case 3:
		case 4:
			continue;
		}
	}
}

void LcEpollCli::EpollSend(OverLap* pOverLap)
{
	int fd = pOverLap->fd;

	pthread_mutex_lock(&m_SndSync);
	OverLap* pSndList = pOverLap->pSndList;
	pOverLap->pSndList = NULL;
	pthread_mutex_unlock(&m_SndSync);

	while(pSndList)
	{
		while(pSndList->uiSndComLen > 0)
		{
			int ret = send(fd, pSndList->szpComBuf + pSndList->uiSndFinishLen, pSndList->uiSndComLen, MSG_NOSIGNAL);
			if(ret == -1 && errno == EAGAIN)
			{
				PushRequest(pSndList);
				return;
			}
			else if(ret == 0 || ret > (int)pSndList->uiSndComLen)
			{
				RemoveConnect(pOverLap);
				return;
			}
			else if(ret == -1)
			{
				RemoveConnect(pOverLap);
				return;
			}

			pSndList->uiSndFinishLen += ret;
			pSndList->uiSndComLen -= ret;
		}

		pOverLap->u64PacketSnd += 1;
		OverLap* pTmpOverLap = pSndList;
		pSndList = pSndList->pSndList;
		pTmpOverLap->pSndList = NULL;
		m_IONetSndMemQue.Push((long)pTmpOverLap);
	}
}

void LcEpollCli::RemoveConnect(OverLap* pOverLap)
{
	if(m_ConnTable.DeleteByKey(pOverLap->u64SessionID))
	{
		return;
	}

	struct epoll_event ev;
	epoll_ctl(m_epSocket, EPOLL_CTL_DEL, pOverLap->fd, &ev);
	close(pOverLap->fd);
	pOverLap->u64SessionID = 0;
	pOverLap->fd = -1;
	ReleaseSndList(pOverLap);
	m_IONetConnMemQue.Push((long)pOverLap);
}

void LcEpollCli::ReleaseSndList(OverLap* pOverLap)
{
	pthread_mutex_lock(&m_SndSync);
	OverLap* pSndOverLap = pOverLap->pSndList;
	pOverLap->pSndList = NULL;
	pthread_mutex_unlock(&m_SndSync);
	while(pSndOverLap)
	{
		OverLap* pTmpOverLap = pSndOverLap;
		pSndOverLap = pSndOverLap->pSndList;
		m_IONetSndMemQue.Push((long)pTmpOverLap);
		pTmpOverLap->pSndList = NULL;
	}
}

int LcEpollCli::CheckPacket(OverLap* pOverLap, bool& bIsHeadChked)
{
	while(1)
	{
		if(!bIsHeadChked)
		{
			switch(m_pChecker->CheckPacketHead(pOverLap, m_pBaseConfig->m_uiHeadPacketSize))
			{
			case 0:	//	包头校验成功
				bIsHeadChked = true;
				break;
			case 1:	//	包长度不够
				return 3;
			case 2:	//	校验失败
				return 1;
			}

			unsigned int uiPacketLen = *(unsigned int*)(pOverLap->szpComBuf + OFFSET_PACKET_LEN);
			switch(m_pChecker->CheckPacketEnd(pOverLap, m_pBaseConfig->m_uiHeadPacketSize, m_pBaseConfig->m_uiMaxPacketSize))
			{
			case 0:	//	校验成功
				bIsHeadChked = false;
				//	to send to work queue
				memcpy(pOverLap->szpComBuf, pOverLap->szpComBuf + uiPacketLen, pOverLap->uiFinishLen - uiPacketLen);
				pOverLap->uiFinishLen -= uiPacketLen;
				pOverLap->uiComLen = m_pBaseConfig->m_uiMaxPacketSize - pOverLap->uiFinishLen;
			case 1:	//	长度不够
				return 4;
			case 2:	//	校验失败
				return 2;
			}
		}
	}
	return 0;
}
