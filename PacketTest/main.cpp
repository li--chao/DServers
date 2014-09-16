#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../common/packet.h"

#define TARGET_IP "192.168.0.224"
#define TARGET_PORT 3040

pthread_mutex_t deadlock;

int main(int argc, char** argv)
{
	pthread_mutex_init(&deadlock, NULL);
	sockaddr_in servAddr;
	unsigned int uiSocketLen = sizeof(servAddr);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(TARGET_IP);
	servAddr.sin_port = htons(TARGET_PORT);

	char szpSndBuff[1024];

	int idx = 1;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int ret = connect(sock, (sockaddr*)&servAddr, uiSocketLen);
	if(ret == 0)
	{
		printf("connect success\n");
	}
	else
	{
		printf("connect error\n");
		return 1;
	}

/**
	TestProtocol cTestProtocol;
	cTestProtocol.m_uiTestDataA = 10;
	cTestProtocol.m_uiTestDataB = 20;
	cTestProtocol.m_phPrtcolHead.m_uiIdentifyCode = IDENTIFY_CODE;
	cTestProtocol.m_phPrtcolHead.m_uiOperateCode = TEST_PROTOCOL;
	cTestProtocol.m_phPrtcolHead.m_uiPacketLength = sizeof(cTestProtocol);
	cTestProtocol.m_usEndCode = END_CODE;
	TestProtocolUn cTestProtocolUn;
	cTestProtocolUn.m_tplTestProtocol = cTestProtocol;
	printf("packet len = %u\n", *(unsigned int*)(cTestProtocolUn.m_szaPacketBuff + 8));
	printf("testdataA = %u\n", *(unsigned int*)(cTestProtocolUn.m_szaPacketBuff + 12));
	printf("testdataB = %u\n", *(unsigned int*)(cTestProtocolUn.m_szaPacketBuff + 16));

	TestProtocolUn cRecvTstProtcl;
	for(int i = 0; i < 100; i++)
	{
		send(sock, cTestProtocolUn.m_szaPacketBuff, cTestProtocol.m_phPrtcolHead.m_uiPacketLength, MSG_NOSIGNAL);
		recv(sock, cRecvTstProtcl.m_szaPacketBuff, 128, 0);
		printf("recv data: A = %u B = %u\n", cRecvTstProtcl.m_tplTestProtocol.m_uiTestDataA, cRecvTstProtcl.m_tplTestProtocol.m_uiTestDataB);
	}
**/

	UnGetSessionID unGetSessionID;
	unGetSessionID.m_GetSessionID.m_uiIdentifyCode = IDENTIFY_CODE;
	unGetSessionID.m_GetSessionID.m_phPrtcolHead.m_uiIdentifyCode = IDENTIFY_CODE;
	unGetSessionID.m_GetSessionID.m_phPrtcolHead.m_uiOperateCode = GET_SESSION_ID;
	unGetSessionID.m_GetSessionID.m_phPrtcolHead.m_uiPacketLength = sizeof(unGetSessionID.m_GetSessionID);
	unGetSessionID.m_GetSessionID.m_usEndCode = END_CODE;

	UnGetSessionIDRespd unGetSessionIDRespd;
	send(sock, unGetSessionID.m_szaPacketBuff, sizeof(unGetSessionID.m_GetSessionID), MSG_NOSIGNAL);
	recv(sock, unGetSessionIDRespd.m_szaPacketBuff, sizeof(unGetSessionIDRespd.m_GetSessionIDRespd), 0);

	printf("identify code = %u(expected: %u)\n", unGetSessionIDRespd.m_GetSessionIDRespd.m_phPrtcolHead.m_uiIdentifyCode, IDENTIFY_CODE);
	printf("sessionID = %llu\n", unGetSessionIDRespd.m_GetSessionIDRespd.m_u64SessionID);
	printf("end code = %u\n", unGetSessionIDRespd.m_GetSessionIDRespd.m_usEndCode);

	UnGetIOPacket unGetIOPacket;
	unGetIOPacket.m_GetIOPacket.m_phPrtcolHead.m_uiIdentifyCode = IDENTIFY_CODE;
	unGetIOPacket.m_GetIOPacket.m_phPrtcolHead.m_uiOperateCode = GET_IO_PACKET;
	unGetIOPacket.m_GetIOPacket.m_phPrtcolHead.m_uiPacketLength = sizeof(unGetIOPacket.m_GetIOPacket);
	unGetIOPacket.m_GetIOPacket.u64SessionID = unGetSessionIDRespd.m_GetSessionIDRespd.m_u64SessionID;
	unGetIOPacket.m_GetIOPacket.m_usEndCode = END_CODE;

	for(int i = 0; i < 2; i++)
	{
		UnGetIOPacketRespd unGetIOPacketRespd;
		send(sock, unGetIOPacket.m_szaPacketBuff, sizeof(unGetIOPacket.m_GetIOPacket), MSG_NOSIGNAL);
//		recv(sock, unGetIOPacketRespd.m_szaPacketBuff, sizeof(unGetIOPacketRespd.m_GetIOPacketRespd), 0);
//		printf("packet recv = %llu\n", unGetIOPacketRespd.m_GetIOPacketRespd.m_u64PacketRecv);
//		printf("packet send = %llu\n", unGetIOPacketRespd.m_GetIOPacketRespd.m_u64PacketSend);
	}

/*
	UnHeartBeat heartBeatPack;
	heartBeatPack.m_HeartBeat.m_phPrtcolHead.m_uiIdentifyCode = IDENTIFY_CODE;
	heartBeatPack.m_HeartBeat.m_phPrtcolHead.m_uiOperateCode = HEART_BEAT;
	heartBeatPack.m_HeartBeat.m_phPrtcolHead.m_uiPacketLength = sizeof(heartBeatPack.m_HeartBeat);
	heartBeatPack.m_HeartBeat.m_u64TimeStamp = time(NULL);
	heartBeatPack.m_HeartBeat.m_u64SessionID = unGetSessionIDRespd.m_GetSessionIDRespd.m_u64SessionID;
	heartBeatPack.m_HeartBeat.m_usEndCode = END_CODE;
	while(1)
	{
		send(sock, heartBeatPack.m_szaPacketBuff, sizeof(heartBeatPack.m_HeartBeat), 0);
		sleep(3);
	}
*/
	pthread_mutex_lock(&deadlock);
	pthread_mutex_lock(&deadlock);

	return 0;
}
