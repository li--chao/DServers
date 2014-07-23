#ifndef PACKET_H
#define PACKET_H

#define QUEUE_LENGTH 10240
#define MAX_PACKET_SIZE 8192
#define MIN_PACKET_SIZE 2
#define DEFAULT_PORT 2020
#define IDENTIFY_CODE 0x00143B3B
#define END_CODE 0x2B2B

#pragma pack(1)

struct PacketHead
{
	unsigned int m_uiIdentifyCode; 
	unsigned int m_uiOperateCode;
	unsigned long long m_u64SessionID;
	unsigned int m_uiPacketLength;
};

struct TestProtocol
{
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

