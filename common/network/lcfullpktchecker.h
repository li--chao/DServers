#ifndef LC_FULL_PKT_CHECKER_H
#define LC_FULL_PKT_CHECKER_H

#include "lcbasechecker.h"

class LcFullPktChecker : public LcBaseChecker
{
public:
	LcFullPktChecker();
	virtual ~LcFullPktChecker();
	int CheckPacketHead(OverLap* pOverLap, const unsigned int& uiHeadSize);
	int CheckPacketEnd(OverLap* pOverLap, const unsigned int& uiHeadSize, const unsigned int& uiMaxPacketSize);
};


#endif
