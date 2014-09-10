#ifndef NET_NODE_H
#define NET_NDOE_H

enum ENodeStatus
{
	E_Node_Status_Stop = 0,
	E_Node_Status_Run = 1
};

struct NetNode
{

	ENodeStatus eNodeStatus;
	char szaNodeIP[16];
	unsigned short usNodePort;

};

#endif
