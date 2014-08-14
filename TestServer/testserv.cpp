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
		unsigned int uiOperateCode = *(unsigned int*)(pOverLap->szpComBuf + OFFSET_OPERATE_LEN);
		TestProtocolUn testProtocolUn;
		memcpy(testProtocolUn.m_szaPacketBuff, pOverLap->szpComBuf, pOverLap->uiPacketLen);
		m_pLog->Write("In work Queue %d:%d:%d", pOverLap->uiPacketLen, *(unsigned int*)(pOverLap->szpComBuf + OFFSET_PACKET_LEN), testProtocolUn.m_tplTestProtocol.m_phPrtcolHead.m_uiPacketLength);
		switch(uiOperateCode)
		{
		case TEST_PROTOCOL:
			m_pLog->Write("test protocol");
			break;

		}
		m_pExtServNet->ReleaseRequest(lptr);
//		m_pExtServNet->SendData((long)pOverLap);
		
	}
	return 0;
}
