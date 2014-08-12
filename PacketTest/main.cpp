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

#define TARGET_IP "192.168.1.8"
#define TARGET_PORT 3030

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
	while(1)
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		int ret = connect(sock, (sockaddr*)&servAddr, uiSocketLen);
		printf("*%d) connect ret = %d errno = %s(%d)\n", idx++, ret, strerror(errno), errno);
		
		sleep(2);
	}

	pthread_mutex_lock(&deadlock);
	pthread_mutex_lock(&deadlock);

	return 0;
}
