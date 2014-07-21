#include "epollnet.h"

LCEpollNet::LCEpollNet()
{
	m_uiPacketHeadSize = HEADSIZE;
	m_uiMinPacketSize = HEADSIZE;
	m_uiMaxPacketSize = HEADSIZE * 2048;
	m_lsnSocket = -1;

	m_pIOQueue = NULL;
	m_epSocket = -1;
	m_pEpollEvs = NULL;
}

LCEpollNet::~LCEpollNet()
{


}

int LCEpollNet::Init()
{

	return 0;
}

int LCEpollNet::BindAndLsn(const int& iBackLog, const unsigned short& usPort)
{

	return 0;
}
