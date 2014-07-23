#include <iostream>
#include "../common/network/epollnet.h"

int main(int argc, char** argv)
{
	LcEpollNet epollNet;
	epollNet.InitDefault();
	return 0;
}
