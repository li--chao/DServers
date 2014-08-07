#include "testserv.h"

TestServ::TestServ()
{

}

TestServ::TestServ(TextLog* pLog, LcAbstractNet* pServNet)
{
	m_pLog = pLog;
	m_pExtServNet = pServNet;
}

int TestServ::MainFun()
{
	while(1)
	{
		long lptr = 0;
		m_pExtServNet->GetRequest(lptr);
		OverLap* pOverLap = (OverLap*)lptr;
		memcpy(pOverLap->szpSndComBuf, pOverLap->szpRecvComBuf, pOverLap->uiComLen);
		m_pExtServNet->SendData((long)pOverLap);
		
	}
	return 0;
}
