#include <string.h>
#include "epollnet.h"
#include "../baseconfig.h"
#include "../io/FileUtil.h"

LcEpollNet::LcEpollNet()
{
	m_lsnSocket = -1;

	m_pIORecvQueue = NULL;
	m_pIOWorkQueue = NULL;
	m_pIOSndQueue = NULL;
	m_epSocket = -1;
	m_pEpollEvs = NULL;
	m_szpRecvPackMem = NULL;
	m_szpSndPackMem = NULL;
	m_pChecker = NULL;
}

LcEpollNet::~LcEpollNet()
{


}

int LcEpollNet::Init(BaseConfig* pBaseConfig, TextLog& textLog)
{
	m_txlNetLog = &textLog;
	m_pBaseConfig = pBaseConfig;

	struct timeval tvTimeOut;
	tvTimeOut.tv_sec = 0;
	tvTimeOut.tv_usec = 300;

	struct linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0;
	unsigned int uiReuseAddr = 1;	


	m_lsnSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_lsnSocket < -1)
	{
		m_txlNetLog->Write("socket create error!");
		return -1;
	}

	setsockopt(m_lsnSocket, SOL_SOCKET, SO_RCVTIMEO, &tvTimeOut, sizeof(tvTimeOut));
	setsockopt(m_lsnSocket, SOL_SOCKET, SO_SNDTIMEO, &tvTimeOut, sizeof(tvTimeOut));
	setsockopt(m_lsnSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&uiReuseAddr, sizeof(uiReuseAddr));
	setsockopt(m_lsnSocket, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));

	if(FileUtil::SetNoBlock(m_lsnSocket))
	{
		m_txlNetLog->Write("SetNoBlock m_lsnSocket error!");
		return -1;
	}

	m_pIORecvQueue = new OverLap[pBaseConfig->m_uiMaxOverLapNum];
	m_pIOWorkQueue = new OverLap[pBaseConfig->m_uiMaxOverLapNum];
	m_pIOSndQueue = new OverLap[pBaseConfig->m_uiMaxOverLapNum];

	m_szpRecvPackMem = new char [pBaseConfig->m_uiMaxOverLapNum * pBaseConfig->m_uiMaxPacketSize];
	m_szpWorkPackMem = new char [pBaseConfig->m_uiMaxOverLapNum * pBaseConfig->m_uiMaxPacketSize];
	m_szpSndPackMem = new char [pBaseConfig->m_uiMaxOverLapNum * pBaseConfig->m_uiMaxPacketSize];
	m_pEpollEvs = new struct epoll_event[pBaseConfig->m_uiConcurrentNum];
	m_pChecker = new LcFullPktChecker();

	if(m_pIORecvQueue == NULL || m_pIOWorkQueue == NULL || m_pIOSndQueue == NULL ||
	   m_szpRecvPackMem == NULL || m_szpWorkPackMem == NULL || m_szpSndPackMem == NULL || 
	   m_pEpollEvs == NULL || m_pChecker == NULL)
	{
		m_txlNetLog->Write("no enough memory for server!");
		return -1;
	}

	if(m_IONetConnQue.Init(pBaseConfig->m_uiMaxOverLapNum))
	{
		m_txlNetLog->Write("io conn queue init error");
		return -1;
	}

	if(m_IONetWorkQue.Init(pBaseConfig->m_uiMaxOverLapNum))
	{
		m_txlNetLog->Write("io work queue init error");
		return -1;
	}

	for(int i = 0; i < (int)pBaseConfig->m_uiMaxOverLapNum; i++)
	{
		m_pIORecvQueue[i].szpComBuf = m_szpRecvPackMem + (i * pBaseConfig->m_uiMaxPacketSize);
		m_pIOWorkQueue[i].szpComBuf = m_szpWorkPackMem + (i * pBaseConfig->m_uiMaxPacketSize);
		m_pIOSndQueue[i].szpComBuf = m_szpSndPackMem + (i * pBaseConfig->m_uiMaxPacketSize);

		m_IONetConnQue.Push((long)&m_pIORecvQueue[i]);
	}

	if(BindAndLsn(pBaseConfig->m_iBackLog, pBaseConfig->m_usServPort))
	{
		return -1;
	}
	return 0;
}

void LcEpollNet::RemoveConnect(OverLap* pOverLap)
{
	struct epoll_event ev;
	m_IONetConnQue.Push((long)pOverLap);
	epoll_ctl(m_epSocket, EPOLL_CTL_DEL, pOverLap->fd, &ev);
	close(pOverLap->fd);
	pOverLap->u64SessionID = 0;
	pOverLap->fd = -1;
}

void LcEpollNet::GetRequest(long& lptr)
{
	m_IONetWorkQue.Pop(lptr);
}

void LcEpollNet::ReleaseRequest(const long& lptr)
{
	m_IONetWorkQue.Push(lptr);
}

void LcEpollNet::SendData(const long& lptr)
{
	OverLap* pOverLap = (OverLap*)lptr;
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.ptr = (void*)pOverLap;
	if(epoll_ctl(m_epSocket, EPOLL_CTL_MOD, pOverLap->fd, &ev) == -1)
	{
		m_txlNetLog->Write("SendData Error");
		RemoveConnect(pOverLap);
	}
}

int LcEpollNet::BindAndLsn(const int& iBackLog, const unsigned short& usPort)
{
	sockaddr_in svrSockAddr;
	svrSockAddr.sin_family = AF_INET;
	svrSockAddr.sin_port = htons(usPort);
	svrSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_epSocket = epoll_create(64);
	if(m_epSocket == -1)
	{
		m_txlNetLog->Write("epoll_create error!");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = m_lsnSocket;
	if(epoll_ctl(m_epSocket, EPOLL_CTL_ADD, m_lsnSocket, &ev) == -1)
	{
		m_txlNetLog->Write("epoll_ctl add lsnSocket error!");
		return -1;
	}

	if(bind(m_lsnSocket, (sockaddr*)&svrSockAddr, sizeof(svrSockAddr)) == -1)
	{
		m_txlNetLog->Write("bind lsnSocket error!");
		return -1;
	}

	if(listen(m_lsnSocket, iBackLog) == -1)
	{
		m_txlNetLog->Write("listen lsnSocket error!");
		return -1;
	}
	return 0;
}

int LcEpollNet::StartThread()
{
	pthread_t posix_thrd;
	int ret = pthread_create(&posix_thrd, NULL, Thread_NetServ, this);
	if(ret)
	{
		m_txlNetLog->Write("pthread_create Thread_NetServ error!");
		return -1;
	}

	return 0;
}

void* LcEpollNet::Thread_NetServ(void* param)
{
	LcEpollNet* pNet = (LcEpollNet*)param;

	sockaddr_in cliSockAddr;
	socklen_t addrLen = sizeof(sockaddr);

	while(1)
	{
		int nfds = epoll_wait(pNet->m_epSocket, pNet->m_pEpollEvs, pNet->m_pBaseConfig->m_uiConcurrentNum, -1);
		if(nfds < 0 && errno == EINTR)
		{
			continue;
		}
		else if(nfds < 0)
		{
			pNet->m_txlNetLog->Write("epoll_wait error!");
			return NULL;
		}

		for(int i = 0; i < nfds; i++)
		{
			if(pNet->m_pEpollEvs[i].data.fd == pNet->m_lsnSocket)
			{
				int fd = accept(pNet->m_lsnSocket, (sockaddr*)&cliSockAddr, &addrLen);
				if(fd == -1)
				{
					pNet->m_txlNetLog->Write("accept from %s:%u error", inet_ntoa(cliSockAddr.sin_addr), ntohs(cliSockAddr.sin_port));
					continue;
				}

				if(FileUtil::SetNoBlock(fd))
				{
					close(fd);
					pNet->m_txlNetLog->Write("peer(%s:%u, fd: %u) set no block error", inet_ntoa(cliSockAddr.sin_addr), ntohs(cliSockAddr.sin_port), fd);
					continue;
				}

				pNet->m_txlNetLog->Write("accept from %s:%u success", inet_ntoa(cliSockAddr.sin_addr), ntohs(cliSockAddr.sin_port));
				pNet->EpollAccept(fd, cliSockAddr.sin_addr.s_addr, cliSockAddr.sin_port);
			}
			else if(pNet->m_pEpollEvs[i].events & EPOLLIN)
			{
				pNet->EpollRecv((OverLap*)pNet->m_pEpollEvs[i].data.ptr);
			}
			else if(pNet->m_pEpollEvs[i].events & EPOLLOUT)
			{
				pNet->EpollSend((OverLap*)pNet->m_pEpollEvs[i].data.ptr);	
			}
		}
	}

	pNet->m_txlNetLog->Write("thread is going to be over");
	return NULL;
}

void LcEpollNet::EpollAccept(const int& fd, const unsigned int& uiPeerIP, const unsigned short& usPeerPort)
{
	long ptr = 0;
	m_IONetConnQue.Pop(ptr);
	OverLap* pOverLap = (OverLap*)ptr;

	pOverLap->uiComLen = m_pBaseConfig->m_uiMaxPacketSize;
	pOverLap->uiFinishLen = 0;
	pOverLap->uiPeerIP = uiPeerIP;
	pOverLap->usPeerPort = usPeerPort;
	pOverLap->fd = fd;
	pOverLap->u64LastRecvPack = time(0);

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void*)pOverLap;

	if(epoll_ctl(m_epSocket, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		m_IONetConnQue.Push(ptr);
		close(fd);
		char szaPeerIP[20];
		sprintf(szaPeerIP, "%u.%u.%u.%u", *((unsigned char*)&uiPeerIP), *((unsigned char*)&uiPeerIP + 1), *((unsigned char*)&uiPeerIP + 2), *((unsigned char*)&uiPeerIP + 3));
		m_txlNetLog->Write("add Peer(%s:%u) to epoll error", szaPeerIP, usPeerPort);
	}
}

void LcEpollNet::EpollRecv(OverLap* pOverLap)
{
	int ret = 0;
	char szaPeerIP[20];
	IP2Str(pOverLap->uiPeerIP, szaPeerIP);
	bool bIsHeadChked = false;

	while(1)
	{
		ret = recv(pOverLap->fd, pOverLap->szpComBuf + pOverLap->uiFinishLen, pOverLap->uiComLen, 0);
		if(ret == 0 || ret > (int)pOverLap->uiComLen)
		{
			RemoveConnect(pOverLap);
			m_txlNetLog->Write("peer(%s:%u) close", szaPeerIP, ntohs(pOverLap->usPeerPort));
			return;
		}
		else if(ret == -1 && errno == EAGAIN)
		{
			// no data
			break;
		}
		else if(ret == -1)
		{
			// error when recv data
			RemoveConnect(pOverLap);
			m_txlNetLog->Write("recv data from peer(%s:%u) error", szaPeerIP, ntohs(pOverLap->usPeerPort));
			return;
		}

		m_txlNetLog->Write("got data(%d byte) from peer(%s:%u)", ret, szaPeerIP, ntohs(pOverLap->usPeerPort));
		pOverLap->uiComLen -= ret;
		pOverLap->uiFinishLen += ret;

		switch(CheckPacket(pOverLap, bIsHeadChked))
		{
		case 0:
			continue;
		case 1:		//	包头校验失败，删除连接
		case 2:		//	包尾校验失败，删除连接
			RemoveConnect(pOverLap);
			return;
		case 3:		//	长度小于包头长度，继续从缓存中读取数据
		case 4:		//	长度小于全包长度，继续从缓存中读取数据
			continue;
		}
	}

}

void LcEpollNet::EpollSend(OverLap* pOverLap)
{
/**
	while(1)
	{
		int ret = send(pOverLap->fd, pOverLap->szpComBuf, pOverLap->uiComLen, MSG_NOSIGNAL);
		m_txlNetLog->Write("send ret(%d), %s(%d)", ret, strerror(errno), errno);
		if(ret == -1 && errno == EAGAIN)
			continue;
		else
			break;
	}
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void*)pOverLap;
	if(epoll_ctl(m_epSocket, EPOLL_CTL_MOD, pOverLap->fd, &ev) == -1)
	{
		m_txlNetLog->Write("EPOll_CTL_MOD error when EpollSend");
		RemoveConnect(pOverLap);
	}
**/
}

int LcEpollNet::CheckPacket(OverLap* pOverLap, bool& bIsHeadChked)
{
	while(1)
	{
		if(!bIsHeadChked)	// 校验包头
		{
			switch(m_pChecker->CheckPacketHead(pOverLap, m_pBaseConfig->m_uiHeadPacketSize))
			{
			case 0:	//	包头校验成功，bIsHeadChked设为true
				bIsHeadChked = true;
				m_txlNetLog->Write("check head success");
				break;
			case 1:	//	读取包的长度小于包头的长度
				m_txlNetLog->Write("%d bytes is not long enough to check head", pOverLap->uiFinishLen);
				return 3;
			case 2:	//	校验失败
				m_txlNetLog->Write("check head error, connect will be closed");
				return 1;
			}
		}

		unsigned int uiPacketLen = *(unsigned int*)(pOverLap->szpComBuf + OFFSET_PACKET_LEN);
		switch(m_pChecker->CheckPacketEnd(pOverLap, m_pBaseConfig->m_uiHeadPacketSize, m_pBaseConfig->m_uiMaxPacketSize))	//	校验包尾
		{
		case 0:	//	包尾校验成功，bIsHeadChked设为false，移动内存，并重新给重叠结构的uiComLen和uiFinishLen赋值校验下一个包，然后将完整的包送到工作线程
			m_txlNetLog->Write("check end success");
			bIsHeadChked = false;
			memcpy(pOverLap->szpComBuf, pOverLap->szpComBuf + uiPacketLen, pOverLap->uiFinishLen - uiPacketLen);
			pOverLap->uiFinishLen -= uiPacketLen;
			pOverLap->uiComLen = m_pBaseConfig->m_uiMaxPacketSize - pOverLap->uiFinishLen;
			SendToWorkQue(pOverLap, uiPacketLen);
/**
			long lWorkMem;
			m_IONetMemQue.Pop(lWorkMem);
			char* szpWorkMem = (char*)lWorkMem;
			memcpy(szpWorkMem, pOverLap->szpRecvComBuf, uiPacketLen);
			m_IONetWorkQue.Push(lWorkMem);
**/
			break;
		case 1:	//	读取包的长度小于包的长度
			m_txlNetLog->Write("packet len: %d, read len: %d not long enough to check end", uiPacketLen, pOverLap->uiFinishLen);
			return 4;
		case 2:	//	包尾校验失败
			m_txlNetLog->Write("check end error, connect will be closed");
			return 2;
		}
	}

	return 0;
}

void LcEpollNet::SendToWorkQue(OverLap* pOverLap, const unsigned int& uiPacketLen)
{

}
