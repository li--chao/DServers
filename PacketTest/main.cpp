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
		printf("recv data: %u %u\n", cRecvTstProtcl.m_tplTestProtocol.m_uiTestDataA, cRecvTstProtcl.m_tplTestProtocol.m_uiTestDataB);
		sleep(2);
	}
	
	pthread_mutex_lock(&deadlock);
	pthread_mutex_lock(&deadlock);

	return 0;
}
