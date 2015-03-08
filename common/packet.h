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
//	TestProtocol()
//	{
//		m_phPrtcolHead.m_uiOperateCode = TEST_PROTOCOL;
//	}
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

struct GetSessionID
{
	PacketHead m_phPrtcolHead;
	unsigned int m_uiIdentifyCode;
	unsigned short m_usEndCode;
	inline bool CheckID()
	{
		return (m_uiIdentifyCode == IDENTIFY_CODE);
	}
}; 

union UnGetSessionID
{
	GetSessionID m_GetSessionID;
	char m_szaPacketBuff[MAX_PACKET_SIZE];
};

struct GetSessionIDRespd
{
	PacketHead m_phPrtcolHead;
	unsigned long long m_u64SessionID;
	unsigned short m_usEndCode;
};

union UnGetSessionIDRespd
{
	GetSessionIDRespd m_GetSessionIDRespd;
	char m_szaPacketBuff[MAX_PACKET_SIZE];
};

struct GetIOPacket
{
	PacketHead m_phPrtcolHead;
	unsigned long long u64SessionID;
	unsigned short m_usEndCode;
};

union UnGetIOPacket
{
	GetIOPacket m_GetIOPacket;
	char m_szaPacketBuff[MAX_PACKET_SIZE];
};

struct GetIOPacketRespd
{
	PacketHead m_phPrtcolHead;
	unsigned long long m_u64PacketRecv;
	unsigned long long m_u64PacketSend;
	unsigned short m_usEndCode;
};

union UnGetIOPacketRespd
{
	GetIOPacketRespd m_GetIOPacketRespd;
	char m_szaPacketBuff[MAX_PACKET_SIZE];
};

struct HeartBeat
{
	PacketHead m_phPrtcolHead;
	unsigned long long m_u64TimeStamp;
	unsigned long long m_u64SessionID;
	unsigned short m_usEndCode;
};

union UnHeartBeat
{
	HeartBeat m_HeartBeat;
	char m_szaPacketBuff[MAX_PACKET_SIZE];
};

#pragma pack()

#define HEADSIZE sizeof(PacketHead)

#endif

