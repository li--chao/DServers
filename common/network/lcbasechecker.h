#ifndef LC_BASE_CHECKER_H
#define LC_BASE_CHECKER_H

#include "netcommon.h"

class LcBaseChecker
{
public:
	virtual int CheckPacketHead(const char* szpComBuf, const unsigned int& uiHeadSize) = 0;
	virtual int CheckPacketEnd(const char* szpComBuf) = 0;

};


#endif
