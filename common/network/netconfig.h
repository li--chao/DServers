#ifndef NET_CONFIG_H
#define NET_CONFIG_H


struct PacketSizeConfig
{
	PacketSizeConfig()
	{
		m_uiMaxPacketSize = 0;
		m_uiMinPacketSize = 0;
		m_uiPacketHeadSize = 0;
	}

	unsigned int m_uiMaxPacketSize;
	unsigned int m_uiMinPacketSize;
	unsigned int m_uiPacketHeadSize
};


struct NetParamConfig
{
	struct NetParamConfig()
	{
		m_uiPeerIP = 0;
		m_usPeerPort = 0;
		m_uiThreadNum = 1;
		m_uiConcurrentNum = 0;
	}

	unsigned int m_uiPeerIP;
	unsigned short m_usPeerPort;
	unsigned int m_uiThreadNum;
	unsigned int m_uiConcurrentNum;
};

#endif
