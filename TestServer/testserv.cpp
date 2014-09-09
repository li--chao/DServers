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
		int ret = 0;
		switch(uiOperateCode)
		{
		case TEST_PROTOCOL:
			ret = HandleTestProtocol(pOverLap);
			break;
		case GET_SESSION_ID:
			ret = HandleGetSessionID(pOverLap);
			break;
		case GET_IO_PACKET:
			ret = HandleGetIOPacket(pOverLap);
			break;
		case HEART_BEAT:
			ret = HandleHeartBeat(pOverLap);
			break;
		}
		m_pExtServNet->ReleaseRequest(lptr);
		if(ret)
		{
			m_pExtServNet->RemoveConnectByID(pOverLap->u64SessionID);
		}
		
	}
	return 0;
}

int TestServ::HandleTestProtocol(OverLap* pOverLap)
{
	TestProtocolUn unTestProtocol;
	memcpy(unTestProtocol.m_szaPacketBuff, pOverLap->szpComBuf, pOverLap->uiPacketLen);

	m_pLog->Write("uiOperateCode = %u, uiPacketLen = %u, pOverLap->uiPacketLen = %u, pOverLap->u64PacketRecv = %llu, pOverLap->u64PacketSnd = %llu", unTestProtocol.m_tplTestProtocol.m_phPrtcolHead.m_uiOperateCode, unTestProtocol.m_tplTestProtocol.m_phPrtcolHead.m_uiPacketLength, pOverLap->uiPacketLen, pOverLap->u64PacketRecv, pOverLap->u64PacketSnd);

	
	long lSndAddr = 0;
	m_pExtServNet->RequestSnd(lSndAddr);

	OverLap* pSndOverLap = (OverLap*)lSndAddr;
	pSndOverLap->u64SessionID = pOverLap->u64SessionID;
	pSndOverLap->uiSndComLen = pOverLap->uiPacketLen;
	memcpy(pSndOverLap->szpComBuf, unTestProtocol.m_szaPacketBuff, pOverLap->uiPacketLen);
	m_pExtServNet->SendData(pSndOverLap);
	return 0;
}

int TestServ::HandleGetSessionID(OverLap* pOverLap)
{
	UnGetSessionID unGetSessionID;
	memcpy(unGetSessionID.m_szaPacketBuff, pOverLap->szpComBuf, pOverLap->uiPacketLen);
	if(!unGetSessionID.m_GetSessionID.CheckID())
	{
		m_pLog->Write("check identify code error when request get session id");
		return 1;
	}

	UnGetSessionIDRespd unGetSessionIDRespd;
	unGetSessionIDRespd.m_GetSessionIDRespd.m_phPrtcolHead.m_uiIdentifyCode = IDENTIFY_CODE;
	unGetSessionIDRespd.m_GetSessionIDRespd.m_phPrtcolHead.m_uiOperateCode = GET_SESSION_ID;
	unGetSessionIDRespd.m_GetSessionIDRespd.m_phPrtcolHead.m_uiPacketLength = sizeof(unGetSessionIDRespd.m_GetSessionIDRespd);
	unGetSessionIDRespd.m_GetSessionIDRespd.m_u64SessionID = pOverLap->u64SessionID;
	unGetSessionIDRespd.m_GetSessionIDRespd.m_usEndCode = END_CODE;

	long lSndAddr = 0;
	m_pExtServNet->RequestSnd(lSndAddr);
	OverLap* pSndOverLap = (OverLap*)lSndAddr;
	pSndOverLap->u64SessionID = pOverLap->u64SessionID;
	pSndOverLap->uiSndComLen = sizeof(unGetSessionIDRespd.m_GetSessionIDRespd);
	memcpy(pSndOverLap->szpComBuf, unGetSessionIDRespd.m_szaPacketBuff, pSndOverLap->uiSndComLen);
	m_pExtServNet->SendData(pSndOverLap);
	return 0;
}

int TestServ::HandleGetIOPacket(OverLap* pOverLap)
{
	UnGetIOPacket unGetIOPacket;
	memcpy(unGetIOPacket.m_szaPacketBuff, pOverLap->szpComBuf, pOverLap->uiPacketLen);
	if(unGetIOPacket.m_GetIOPacket.u64SessionID != pOverLap->u64SessionID)
	{
		m_pLog->Write("session id not match %llu(expected: %llu)", unGetIOPacket.m_GetIOPacket.u64SessionID, pOverLap->u64SessionID);
		return 1;
	}

	UnGetIOPacketRespd unGetIOPacketRespd;
	unGetIOPacketRespd.m_GetIOPacketRespd.m_phPrtcolHead.m_uiIdentifyCode = IDENTIFY_CODE;
	unGetIOPacketRespd.m_GetIOPacketRespd.m_phPrtcolHead.m_uiOperateCode = GET_IO_PACKET;
	unGetIOPacketRespd.m_GetIOPacketRespd.m_phPrtcolHead.m_uiPacketLength = sizeof(unGetIOPacketRespd.m_GetIOPacketRespd);
	unGetIOPacketRespd.m_GetIOPacketRespd.m_u64PacketRecv = pOverLap->u64PacketRecv;
	unGetIOPacketRespd.m_GetIOPacketRespd.m_u64PacketSend = pOverLap->u64PacketSnd;
	unGetIOPacketRespd.m_GetIOPacketRespd.m_usEndCode = END_CODE;

	long lSndAddr = 0;
	m_pExtServNet->RequestSnd(lSndAddr);
	OverLap* pSndOverLap = (OverLap*)lSndAddr;
	pSndOverLap->u64SessionID = pOverLap->u64SessionID;
	pSndOverLap->uiSndComLen = sizeof(unGetIOPacketRespd.m_GetIOPacketRespd);
	memcpy(pSndOverLap->szpComBuf, unGetIOPacketRespd.m_szaPacketBuff, pSndOverLap->uiSndComLen);
	m_pExtServNet->SendData(pSndOverLap);
	return 0;
}

int TestServ::HandleHeartBeat(OverLap* pOverLap)
{

	m_pLog->Write("HandleHeartBeat");

	return 0;
}
