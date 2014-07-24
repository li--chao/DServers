#include "epollnet.h"

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

int LcEpollNet::Init()
{
	return 0;
}

int LcEpollNet::BindAndLsn(const int& iBackLog, const unsigned short& usPort)
{
	
	return 0;
}

int LcEpollNet::BindAndLsn()
{
	m_lsnSocket = socket(AF_INET, SOCK_STREAM, 0);	
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


