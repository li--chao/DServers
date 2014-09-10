#ifndef CLUSTER_H
#define CLUSTER_H

#include "netnode.h"
#include "../io/FileUtil.h"

struct Cluster
{
	Cluster() :
	m_iNodeNum(0),
	m_iMaxNodeNum(-1),
	m_pNetNode(NULL)
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
				m_pNetNode = new NetNode[m_iMaxNodeNum];
				if(m_pNetNode == NULL)
				{
					return 1;
				}
			}
			else if(strcmp(szaFileBuff, "Node") == 0)
			{
				if(m_iNodeNum == m_iMaxNodeNum)
				{
					break;
				}
				else if(m_iMaxNodeNum == -1 || m_pNetNode == NULL)
				{
					fclose(fp);
					return 2;
				}
				char *szpIP = NULL;
				char *szpPort = NULL;
				GetNodeInfo(szpValue, szpIP, szpPort, &m_pNetNode[m_iNodeNum]);
			}

		}
		fclose(fp);
		return 0;
	}

	int m_iNodeNum;
	int m_iMaxNodeNum;
	NetNode* m_pNetNode;

private:
	void GetNodeInfo(char* szpSrc, char*& szpIP, char*& szpPort, NetNode* pNetNode)
	{
		char* p = strchr(szpSrc, ':');
		if(!p || *(p + 1) == 0)
		{
			return;
		}

		strncpy(pNetNode->szaNodeIP, szpSrc, p - szpSrc);
		pNetNode->szaNodeIP[p - szpSrc] = '\0';
		pNetNode->usNodePort = (unsigned short)atoi(p + 1);
		pNetNode->eNodeStatus = E_Node_Status_Stop;
		m_iNodeNum++;
	}
};

#endif
