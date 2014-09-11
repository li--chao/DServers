#include "epollcli.h"

LcEpollCli::LcEpollCli() :
m_pBaseConfig(NULL)
{



}

int LcEpollCli::Init(BaseConfig* pBaseConfig, TextLog* pLog, Cluster* pCluster)
{
	m_pBaseConfig = pBaseConfig;
	m_pCoreLog = pLog;
	m_pClusters = pCluster;

	m_pEpollEvs = new struct epoll_event[pBaseConfig->m_uiCliMaxOverLapNum];
	m_szpRecvPackMem = new char[pBaseConfig->m_uiMaxPacketSize * pBaseConfig->m_uiCliMaxOverLapNum];
	m_szpWorkPackMem = new char[pBaseConfig->m_uiMaxPacketSize * pBaseConfig->m_uiCliMaxOverLapNum];
	m_szpSndPackMem = new char[pBaseConfig->m_uiMaxPacketSize * pBaseConfig->m_uiCliMaxOverLapNum];
	m_pChecker = new LcFullPktChecker();
	m_pIORecvQue = new OverLap[pBaseConfig->m_uiCliMaxOverLapNum];
	m_pIOWorkQue = new OverLap[pBaseConfig->m_uiCliMaxOverLapNum];
	m_pIOSndQue = new OverLap[pBaseConfig->m_uiCliMaxOverLapNum];

	if(m_pEpollEvs == NULL || m_szpRecvPackMem == NULL || m_szpWorkPackMem == NULL || m_szpSndPackMem == NULL ||
	   m_pChecker == NULL || m_pIORecvQue == NULL || m_pIOWorkQue == NULL || m_pIOSndQue == NULL)
	{
		return 1;
	}

	if(m_IONetConnQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetWorkQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	if(m_IONetSndQue.Init(pBaseConfig->m_uiCliMaxOverLapNum))
	{
		return -1;
	}

	for(unsigned int u = 0; u < pBaseConfig->m_uiCliMaxOverLapNum; u++)
	{
		m_pIORecvQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);
		m_pIOWorkQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);
		m_pIOSndQue[u].szpComBuf = m_szpRecvPackMem + (u * pBaseConfig->m_uiMaxPacketSize);

		m_IONetConnQue.Push((long)&m_pIORecvQue[u]);
		m_IONetWorkQue.Push((long)&m_pIOWorkQue[u]);
		m_IONetSndQue.Push((long)&m_pIOSndQue[u]);
	}
	return 0;
}

int LcEpollCli::StartThread()
{
	pthread_t hrtBtThrd;
	pthread_t eplNetThrd;
	
	int ret = 0;
	ret += pthread_create(&hrtBtThrd, NULL, Thread_HeartBeatSnd, this);
	ret += pthread_create(&eplNetThrd, NULL, Thread_EpollNet, this);
	
	return ret;
}

void* LcEpollCli::Thread_HeartBeatSnd(void* vparam)
{

	return NULL;
}

void* LcEpollCli::Thread_EpollNet(void* vparam)
{


	return NULL;
}
