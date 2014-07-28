#include "epollnet.h"
#include "../baseconfig.h"
#include "../io/FileUtil.h"

LcEpollNet::LcEpollNet()
{
	m_uiPacketHeadSize = HEADSIZE;
	m_uiMinPacketSize = HEADSIZE;
	m_uiMaxPacketSize = HEADSIZE * 2048;
	m_lsnSocket = -1;

	m_pIOQueue = NULL;
	m_epSocket = -1;
	m_pEpollEvs = NULL;
	m_szpPackMem = NULL;
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

	m_pIOQueue = new OverLap[pBaseConfig->m_uiConcurrentNum];
	m_szpPackMem = new char [pBaseConfig->m_uiConcurrentNum * pBaseConfig->m_uiMaxPacketSize];

	if(m_pIOQueue == NULL || m_szpPackMem == NULL)
	{
		m_txlNetLog->Write("no enough memory for server!");
		return -1;
	}

	for(int i = 0; i < (int)pBaseConfig->m_uiConcurrentNum; i++)
	{
		m_pIOQueue[i].szpComBuf = m_szpPackMem + (i * MAX_PACKET_SIZE);
	}

	if(BindAndLsn(pBaseConfig->m_iBackLog, pBaseConfig->m_usServPort))
	{
		m_txlNetLog->Write("Bind Socket error!");
		return -1;
	}
	return 0;
}

int LcEpollNet::BindAndLsn(const int& iBackLog, const unsigned short& usPort)
{
	sockaddr_in svrSockAddr;
	svrSockAddr.sin_family = AF_INET;
	svrSockAddr.sin_port = htons(usPort);
	svrSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_epSocket = epoll_create(16);
	if(m_epSocket == -1)
	{
		m_txlNetLog->Write("epoll_create error!");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = m_lsnSocket;
	if(epoll_ctl(m_epSocket, EPOLL_CTL_ADD, m_lsnSocket, &ev))
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

int LcEpollNet::InitDefault()
{
	m_pIOQueue = new OverLap[10240];
	m_szpPackMem = new char [10240 * 8192];

	if(m_pIOQueue == NULL || m_szpPackMem == NULL)
	{
		std::cout << "epoll init error, no enough memory." << std::endl;
		return -1;
	}

	for(int i = 0; i < 10240; i++)
	{
		m_pIOQueue[i].szpComBuf = m_szpPackMem + (i * MAX_PACKET_SIZE);
	}
	return 0;
}

void* LcEpollNet::Thread_NetServ(void* param)
{
	LcEpollNet* pNet = (LcEpollNet*)param;


	return NULL;
}
