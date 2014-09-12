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

	if(m_IONetConnQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetWorkQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetSndQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_ConnTable.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	for(unsigned int u = 0; u < pBaseConfig->m_uiCliMaxOverLapNum; u++)
	{
		m_pIORecvQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);
		m_pIOWorkQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);
		m_pIOSndQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);

		m_IONetConnQue.Push((long)&m_pIORecvQue[u]);
		m_IONetWorkQue.Push((long)&m_pIOWorkQue[u]);
		m_IONetSndQue.Push((long)&m_pIOSndQue[u]);
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
	m_IONetSndQue.Pop(lAddr);
}

int LcEpollCli::PushRequest(const int& fd, OverLap* pOverLap)
{

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
	m_IONetConnQue.Pop(lAddr);
	OverLap* pOverLap = (OverLap*)lAddr;
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void*)pOverLap;

	if(epoll_ctl(m_epSocket, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		m_IONetConnQue.Push((long)pOverLap);
		close(fd);
		return 2;
	}

	pOverLap->fd = fd;
	pOverLap->u64SessionID = Cluster::MkPeerID(szpNodeIP, usNodePort);
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

			}
			else if(pCliNet->m_pEpollEvs[i].events & EPOLLOUT)
			{

			}
		}
	}	

	return NULL;
}
