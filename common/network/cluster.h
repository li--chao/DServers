#ifndef CLUSTER_H
#define CLUSTER_H

#include "netnode.h"
#include "../io/FileUtil.h"
#include "../container/hashtable.h"

enum EClusterType
{
	E_ClusterType_None = -1,
	E_ClusterType_A,
	E_ClusterType_B
};


struct Cluster
{
	Cluster() :
	m_iNodeNum(0),
	m_iMaxNodeNum(-1),
	m_eClusterType(E_ClusterType_None)
	{

	}

	int LoadClusterInfo(const char* szpCfgFile)
	{
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

	static unsigned long long MkPeerID(char* szpIP, const unsigned short& usPort)
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

	int m_iNodeNum;
	int m_iMaxNodeNum;
	LcHashTable<unsigned long long, NetNode> m_NetNodeTable;
	EClusterType m_eClusterType;

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
		node.szaNodeIP[p - szpSrc] = '\0';
		unsigned int uiNodeIP = inet_addr(node.szaNodeIP);
		node.usNodePort = (unsigned short)atoi(p + 1);
		*(unsigned int*)szaTmp = uiNodeIP;
		*(unsigned short*)(szaTmp + sizeof(uiNodeIP)) = node.usNodePort;
		m_NetNodeTable.Insert(*(unsigned long long*)szaTmp, node);
		m_iNodeNum++;

	}
};

#endif