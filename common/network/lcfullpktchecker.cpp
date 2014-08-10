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
	unsigned int uiPacketLen = *(unsigned int*)(pOverLap->szpRecvComBuf + 8);

	if(uiIdentifyCode != IDENTIFY_CODE)
	{
		return 2;
	}
	return 0;
}

int LcFullPktChecker::CheckPacketEnd(OverLap* pOverLap)
{


	return 0;
}
