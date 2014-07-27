#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

class BaseConfig
{
public:
	virtual int ReadCfgFile(const char* szpFileName) = 0;
	virtual int CheckCfg() = 0;
};

#endif
