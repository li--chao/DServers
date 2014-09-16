#ifndef CLUSTER_H
#define CLUSTER_H

#include <stdio.h>
#include "netnode.h"
#include "../io/FileUtil.h"
#include "../container/hashtable.h"
#include "netcommon.h"


struct Cluster
{
	Cluster() :
	m_iNodeNum(0),
	m_iMaxNodeNum(-1),
	m_eClusterType(E_ClusterType_None),
	m_pCli(NULL)
	{

	}

	int LoadClusterInfo(const char* szpCfgFile)
	{
		if(pthread_mutex_init(&m_clustermtx, NULL))
		{
			return 3;
		}

		FILE* fp = fopen(szpCfgFile, "rb");
		if(fp == NULL)
		{
			return -1;
		}

		char szaFileBuff[256];
		while(1)
		{
			char* szpValue = NULL;
			ECfgError ret = FileUtil::GetCfgKey(fp, szaFileBuff, 255, szpValue);
			if(ret == E_Cfg_EOF)
			{
				break;
			}
			else if(ret != E_Cfg_Content)
			{
				continue;
			}

			if(strcmp(szaFileBuff, "MaxNodeNum") == 0)
			{
				m_iMaxNodeNum = atoi(szpValue);
				if(m_NetNodeTable.Init(m_iMaxNodeNum))
				{
					return 1;
				}
			}
			else if(strcmp(szaFileBuff, "Node") == 0)
			{
				if(m_NetNodeTable.BucketCnt() == (unsigned int)m_iMaxNodeNum)
				{
					break;
				}
				else if(m_iMaxNodeNum == -1)
				{
					fclose(fp);
					return 2;
				}
				char *szpIP = NULL;
				char *szpPort = NULL;
				GetNodeInfo(szpValue, szpIP, szpPort);
			}

		}
		fclose(fp);
		return 0;
	}

	static unsigned long long MkPeerID(const char* szpIP, const unsigned short& usPort)
	{
		char szaTmp[6];
		unsigned int uiNodeIP = inet_addr(szpIP);
		*(unsigned int*)szaTmp = uiNodeIP;
		*(unsigned short*)(szaTmp + sizeof(uiNodeIP)) = usPort;
		return *(unsigned long long*)szaTmp;
	}

	static unsigned long long MkPeerID(const unsigned int& uiPeerIP, const unsigned short& usPort)
	{
		char szaTmp[6];
		*(unsigned int*)szaTmp = uiPeerIP;
		*(unsigned short*)(szaTmp + sizeof(uiPeerIP)) = usPort;
		return *(unsigned long long*)szaTmp;
	}

	void BroadHeartBeat()
	{
		NetNode szaNetNode[m_NetNodeTable.BucketCnt()];
		unsigned int uiNodeCount = 0;
		m_NetNodeTable.GetValues(szaNetNode, uiNodeCount);

		pthread_mutex_lock(&m_clustermtx);
		for(unsigned int u = 0; u < uiNodeCount; u++)
		{
			long lAddr = 0;
			if(szaNetNode[u].fd == -1)
			{
				if(m_pCli->Connect(szaNetNode[u].szaNodeIP, szaNetNode[u].usNodePort, szaNetNode[u].fd))
				{
					printf("connect to cluster %d, node %s:%u failed\n", m_eClusterType, szaNetNode[u].szaNodeIP, szaNetNode[u].usNodePort);
					continue;
				}

				m_NetNodeTable.Update(Cluster::MkPeerID(szaNetNode[u].szaNodeIP, szaNetNode[u].usNodePort), szaNetNode[u]);
			}

			m_pCli->RequestSnd(lAddr);
			OverLap* pOverLap = (OverLap*)lAddr;
			pOverLap->u64SessionID = Cluster::MkPeerID(szaNetNode[u].szaNodeIP, szaNetNode[u].usNodePort);
			if(m_pCli->PushRequest(pOverLap) == 2)
			{
				szaNetNode[u].fd = -1;
				m_NetNodeTable.Update(Cluster::MkPeerID(szaNetNode[u].szaNodeIP, szaNetNode[u].usNodePort), szaNetNode[u]);
				m_pCli->ReleaseRequest((long)pOverLap);
			}
		}
		pthread_mutex_unlock(&m_clustermtx);
	}

	int m_iNodeNum;
	int m_iMaxNodeNum;
	LcHashTable<unsigned long long, NetNode> m_NetNodeTable;
	EClusterType m_eClusterType;
	LcAbstractCli *m_pCli;

private:
	void GetNodeInfo(char* szpSrc, char*& szpIP, char*& szpPort)
	{
		char* p = strchr(szpSrc, ':');
		if(!p || *(p + 1) == 0)
		{
			return;
		}

		char szaTmp[6];
		NetNode node;
		strncpy(node.szaNodeIP, szpSrc, p - szpSrc);
		node.NodeNo = m_iNodeNum;
		node.szaNodeIP[p - szpSrc] = '\0';
		unsigned int uiNodeIP = inet_addr(node.szaNodeIP);
		node.usNodePort = (unsigned short)atoi(p + 1);
		*(unsigned int*)szaTmp = uiNodeIP;
		*(unsigned short*)(szaTmp + sizeof(uiNodeIP)) = node.usNodePort;
		m_NetNodeTable.Insert(*(unsigned long long*)szaTmp, node);
		m_iNodeNum++;

	}

	pthread_mutex_t m_clustermtx;
};

#endif
