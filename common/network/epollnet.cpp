#include <string.h>
#include "epollnet.h"
#include "../baseconfig.h"
#include "../io/FileUtil.h"

LcEpollNet::LcEpollNet()
{
	m_lsnSocket = -1;

	m_pIOQueue = NULL;
	m_epSocket = -1;
	m_pEpollEvs = NULL;
	m_szpRecvPackMem = NULL;
	m_szpSndPackMem = NULL;
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

	m_pIOQueue = new OverLap[pBaseConfig->m_uiMaxOverLapNum];
	m_szpRecvPackMem = new char [pBaseConfig->m_uiMaxOverLapNum * pBaseConfig->m_uiMaxPacketSize];
	m_szpSndPackMem = new char [pBaseConfig->m_uiMaxOverLapNum * pBaseConfig->m_uiMaxPacketSize];
	m_pEpollEvs = new struct epoll_event[pBaseConfig->m_uiConcurrentNum];

	if(m_pIOQueue == NULL || m_szpRecvPackMem == NULL || m_szpSndPackMem == NULL)
	{
		m_txlNetLog->Write("no enough memory for server!");
		return -1;
	}

	if(m_IONetMemQue.Init(pBaseConfig->m_uiMaxOverLapNum))
	{
		m_txlNetLog->Write("io memory queue init error");
		return -1;
	}

	if(m_IONetWorkQue.Init(pBaseConfig->m_uiMaxOverLapNum))
	{
		m_txlNetLog->Write("io work queue init error");
		return -1;
	}

	for(int i = 0; i < (int)pBaseConfig->m_uiMaxOverLapNum; i++)
	{
		m_pIOQueue[i].szpRecvComBuf = m_szpRecvPackMem + (i * MAX_PACKET_SIZE);
		m_pIOQueue[i].szpSndComBuf = m_szpSndPackMem + (i * MAX_PACKET_SIZE);
		m_IONetMemQue.Push((long)&m_pIOQueue[i]);
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
	m_IONetMemQue.Push((long)pOverLap);
	epoll_ctl(m_epSocket, EPOLL_CTL_DEL, pOverLap->fd, &ev);
	close(pOverLap->fd);
	pOverLap->u64SessionID = 0;
	pOverLap->fd = -1;
}

void LcEpollNet::GetRequest(long& lptr)
{
	m_IONetWorkQue.Pop(lptr);
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
	m_IONetMemQue.Pop(ptr);
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
		m_IONetMemQue.Push(ptr);
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

	while(1)
	{
		ret = recv(pOverLap->fd, pOverLap->szpRecvComBuf + pOverLap->uiFinishLen, pOverLap->uiComLen, 0);
		if(ret == 0 || ret > (int)pOverLap->uiComLen)
		{
			// to do remove connection
//			struct epoll_event ev;
//			m_IONetMemQue.Push((long)pOverLap);
//			close(pOverLap->fd);
//			epoll_ctl(m_epSocket, EPOLL_CTL_DEL, pOverLap->fd, &ev);
//			pOverLap->fd = -1;
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
			break;
		}

		m_txlNetLog->Write("got data(%d byte) from peer(%s:%u)", ret, szaPeerIP, ntohs(pOverLap->usPeerPort));
//		pOverLap->uiComLen = ret;
	}

	m_IONetWorkQue.Push((long)pOverLap);
/**
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.ptr = (void*)pOverLap;
	if(epoll_ctl(m_epSocket, EPOLL_CTL_MOD, pOverLap->fd, &ev) == -1)
	{
		m_txlNetLog->Write("epoll_ctl_mod error when recv data, close peer(%s:%u)", szaPeerIP, ntohs(pOverLap->usPeerPort));
		RemoveConnect(pOverLap);
	}
**/
}

void LcEpollNet::EpollSend(OverLap* pOverLap)
{
	while(1)
	{
		int ret = send(pOverLap->fd, pOverLap->szpSndComBuf, pOverLap->uiComLen, MSG_NOSIGNAL);
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
}
