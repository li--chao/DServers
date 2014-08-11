#include "lcfullpktchecker.h"

LcFullPktChecker::LcFullPktChecker()
{



}

LcFullPktChecker::~LcFullPktChecker()
{


}

int LcFullPktChecker::CheckPacketHead(OverLap* pOverLap, const unsigned int& uiHeadSize)
{
	if(pOverLap->uiFinishLen < uiHeadSize)
	{
		return 1;
	}
	unsigned int uiIdentifyCode = *(unsigned int*)pOverLap->szpRecvComBuf;

	if(uiIdentifyCode != IDENTIFY_CODE)
	{
		return 2;
	}
	return 0;
}

int LcFullPktChecker::CheckPacketEnd(OverLap* pOverLap, const unsigned int& uiHeadSize, const unsigned int& uiMaxPacketSize)
{
	unsigned int uiBodyLen = *(unsigned int*)(pOverLap->szpRecvComBuf + 8);
	if(pOverLap->uiFinishLen < uiBodyLen)
	{
		return 1;	//  not full packet
	}
	
	if(*(unsigned short*)(pOverLap->szpRecvComBuf + uiBodyLen - sizeof(unsigned short)) != END_CODE)
	{
		return 2;  //  error packet
	}

	pOverLap->uiFinishLen -= uiBodyLen;
	pOverLap->uiComLen = uiMaxPacketSize - pOverLap->uiFinishLen;
	if(pOverLap->uiFinishLen > 0)
	{
		memcpy(pOverLap->szpRecvComBuf, pOverLap->szpRecvComBuf + uiBodyLen, pOverLap->uiFinishLen);
	}
	return 0;
}
