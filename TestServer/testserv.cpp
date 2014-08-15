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
		switch(uiOperateCode)
		{
		case TEST_PROTOCOL:
			HandleTestProtocol(pOverLap);
			break;

		}
		m_pExtServNet->ReleaseRequest(lptr);
//		m_pExtServNet->SendData((long)pOverLap);
		
	}
	return 0;
}

int TestServ::HandleTestProtocol(OverLap* pOverLap)
{
	TestProtocolUn unTestProtocol;
	memcpy(unTestProtocol.m_szaPacketBuff, pOverLap->szpComBuf, pOverLap->uiPacketLen);

	m_pLog->Write("uiOperateCode = %u, uiPacketLen = %u, pOverLap->uiPacketLen = %u, pOverLap->u64PacketRecv = %llu, pOverLap->u64PacketSnd = %llu", unTestProtocol.m_tplTestProtocol.m_phPrtcolHead.m_uiOperateCode, unTestProtocol.m_tplTestProtocol.m_phPrtcolHead.m_uiPacketLength, pOverLap->uiPacketLen, pOverLap->u64PacketRecv, pOverLap->u64PacketSnd);
	return 0;
}
