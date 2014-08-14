#ifndef PACKET_H
#define PACKET_H

#define QUEUE_LENGTH 10240
#define MAX_PACKET_SIZE 8192
#define MIN_PACKET_SIZE 20
#define IDENTIFY_CODE 0x00143B3B
#define END_CODE 0x2B2B
#define MEM_OFFSET_IN_CLASS(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER )
#define OFFSET_PACKET_LEN (sizeof(unsigned int) * 2)
#define OFFSET_OPERATE_LEN (sizeof(unsigned int))

#include "operatecode.h"

#pragma pack(1)

struct PacketHead
{
	unsigned int m_uiIdentifyCode; 
	unsigned int m_uiOperateCode;
	unsigned int m_uiPacketLength;
};

struct TestProtocol
{
	TestProtocol()
	{
		m_phPrtcolHead.m_uiOperateCode = TEST_PROTOCOL;
	}
	PacketHead m_phPrtcolHead;
	unsigned int m_uiTestDataA;
	unsigned int m_uiTestDataB;
	unsigned short m_usEndCode;    //  每条协议都必须加
};

union TestProtocolUn
{
	TestProtocol m_tplTestProtocol;
	char m_szaPacketBuff[MAX_PACKET_SIZE];
}; 

#pragma pack()

#define HEADSIZE sizeof(PacketHead)

#endif

