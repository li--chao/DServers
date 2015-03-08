#ifndef BUCKET_H
#define BUCKET_H

class LcBucket
{
public:
	template<class t_key>
	static void GetBucketKey(const char* szpHashDataBuff, t_key& _key)
	{
		_key = *(t_key*)(szpHashDataBuff + sizeof(char*));
	}

	static char* GetNextBucketAddr(const char* szpHashDataBuff)
	{
		return *(char**)szpHashDataBuff;
	}

	template<class t_key, class t_value>
	static void GetBucketValue(const char* szpHashDataBuff, const t_key& _key, t_value& _value)
	{
		_value = *(t_value*)(szpHashDataBuff + sizeof(char*) + sizeof(_key));
	}

	template<class t_key>
	static char* GetBucketValue(char* szpHashDataBuff, const t_key& _key)
	{
		return szpHashDataBuff + sizeof(char*) + sizeof(_key);
	}
};


#endif
