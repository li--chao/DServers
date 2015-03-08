#ifndef HASH_TABLE_H
#define	HASH_TABLE_H

#include <pthread.h>
#include "bucket.h"

template<class t_key, class t_value>
class LcHashTable
{
public:
	LcHashTable() :
		m_uiMaxBucketsSize(0),
		m_uiBucketCnt(0),
		m_szppBuckets(NULL)
	{

	}

	~LcHashTable()
	{
		delete m_szppBuckets;
	}

	int Init(const unsigned int& uiMaxBucketsSize)
	{
		m_uiMaxBucketsSize = uiMaxBucketsSize;
		m_szppBuckets = new char*[uiMaxBucketsSize];
		memset(m_szppBuckets, 0, uiMaxBucketsSize * sizeof(char*));
		if(m_szppBuckets == NULL)
		{
			return 1;
		}

		if(pthread_mutex_init(&m_TableLock, NULL))
		{
			return 2;
		}

		return 0;
	}

	int DeleteByKey(const t_key& _key)
	{
		unsigned int uiHashIdx = _key % m_uiMaxBucketsSize;
		pthread_mutex_lock(&m_TableLock);
		
		char* szpBucket = m_szppBuckets[uiHashIdx];
		char* szpPreBucket = NULL;
		while(szpBucket)
		{
			t_key __key;
			LcBucket::GetBucketKey(szpBucket, __key);
			if(__key == _key)
			{
				if(szpPreBucket == NULL)
					m_szppBuckets[uiHashIdx] = LcBucket::GetNextBucketAddr(szpBucket);
				else
					*(char**)szpPreBucket = LcBucket::GetNextBucketAddr(szpBucket);
				delete szpBucket;
				szpBucket = 0;
				m_uiBucketCnt--;
				pthread_mutex_unlock(&m_TableLock);
				return 0;
			}

			szpPreBucket = szpBucket;
			szpBucket = *(char**)szpBucket;
		}
		pthread_mutex_unlock(&m_TableLock);
		return 1;
	}

	int DeleteByKey(const t_key& _key, t_value& _val)
	{
		unsigned int uiHashIdx = _key % m_uiMaxBucketsSize;
		pthread_mutex_lock(&m_TableLock);
		
		char* szpBucket = m_szppBuckets[uiHashIdx];
		char* szpPreBucket = NULL;
		while(szpBucket)
		{
			t_key __key;
			LcBucket::GetBucketKey(szpBucket, __key);
			if(__key == _key)
			{
				if(szpPreBucket == NULL)
					m_szppBuckets[uiHashIdx] = LcBucket::GetNextBucketAddr(szpBucket);
				else
					*(char**)szpPreBucket = LcBucket::GetNextBucketAddr(szpBucket);

			    LcBucket::GetBucketValue(szpBucket, _key, _val);	
				delete szpBucket;
				szpBucket = 0;
				m_uiBucketCnt--;
				pthread_mutex_unlock(&m_TableLock);
				return 0;
			}

			szpPreBucket = szpBucket;
			szpBucket = *(char**)szpBucket;
		}
		pthread_mutex_unlock(&m_TableLock);
		return 1;
	}

	int Insert(const t_key& _key, const t_value& _value)
	{
		unsigned int uiInsertIdx = _key % m_uiMaxBucketsSize;
		char* szpHashData = new char[sizeof(char*) + sizeof(_key) + sizeof(_value)];
		if(szpHashData == NULL)		//	分配内存失败
		{
			return 1;
		}
		
		pthread_mutex_lock(&m_TableLock);
		char* szpBuckets = m_szppBuckets[uiInsertIdx];
		char* szpTmpBuckets = szpBuckets;

		while(szpTmpBuckets)
		{
			t_key __key;
			LcBucket::GetBucketKey(szpTmpBuckets, __key);
			if(__key == _key)  //	key已经存在
			{
				pthread_mutex_unlock(&m_TableLock);
				return 2;
			}

			szpTmpBuckets = LcBucket::GetNextBucketAddr(szpTmpBuckets);
		}

		*(char**)szpHashData = szpBuckets;
		*(t_key*)(szpHashData + sizeof(char*)) = _key;
		*(t_value*)(szpHashData + sizeof(char*) + sizeof(_key)) = _value;
		m_szppBuckets[uiInsertIdx] = szpHashData;
		m_uiBucketCnt++;
		pthread_mutex_unlock(&m_TableLock);
		return 0;
	}

	inline unsigned int BucketCnt()
	{
		pthread_mutex_lock(&m_TableLock);
		unsigned int uiBucketCnt = m_uiBucketCnt;
		pthread_mutex_unlock(&m_TableLock);
		return uiBucketCnt;
	}

	int Search(const t_key& _key, char*& szpValue)
	{
		unsigned int uiBucketIdx = _key % m_uiMaxBucketsSize;

		pthread_mutex_lock(&m_TableLock);
		char* szpBucket = m_szppBuckets[uiBucketIdx];
		while(szpBucket)
		{
			t_key __key;
			LcBucket::GetBucketKey(szpBucket, __key);
			if(__key == _key)
			{
				szpValue = LcBucket::GetBucketValue(szpBucket, __key);
				pthread_mutex_unlock(&m_TableLock);
				return 0;
			}
			szpBucket = LcBucket::GetNextBucketAddr(szpBucket); 
		}

		szpValue = 0;
		pthread_mutex_unlock(&m_TableLock);
		return 1;
	}

	int Search(const t_key& _key, t_value& _val)
	{
		unsigned int uiBucketIdx = _key % m_uiMaxBucketsSize;

		pthread_mutex_lock(&m_TableLock);
		char* szpBucket = m_szppBuckets[uiBucketIdx];
		while(szpBucket)
		{
			t_key __key;
			LcBucket::GetBucketKey(szpBucket, __key);
			if(__key == _key)
			{
				char* szpValue = LcBucket::GetBucketValue(szpBucket, __key);
				_val = *(t_value*)szpValue;
				pthread_mutex_unlock(&m_TableLock);
				return 0;
			} 
			szpBucket = LcBucket::GetNextBucketAddr(szpBucket); 
		}

		pthread_mutex_unlock(&m_TableLock);
		return 1;
	}

	int Update(const t_key& _key, const t_value& _val)
	{
		unsigned int uiBucketIdx = _key % m_uiMaxBucketsSize;

		pthread_mutex_lock(&m_TableLock);
		char* szpBucket = m_szppBuckets[uiBucketIdx];
		while(szpBucket)
		{
			t_key __key;
			LcBucket::GetBucketKey(szpBucket, __key);
			if(__key == _key)
			{
				char* szpValue = LcBucket::GetBucketValue(szpBucket, __key);
				memcpy(szpValue, (char*)&_val, sizeof(_val));
				pthread_mutex_unlock(&m_TableLock);
				return 0;
			}
			szpBucket = LcBucket::GetNextBucketAddr(szpBucket);
		}
		pthread_mutex_unlock(&m_TableLock);
		return 1;
	}

	void GetKeys(t_key *__pKey, unsigned int& uiBucketCnt)
	{
		char *p;
		unsigned int u = 0;
		unsigned int uiCurBucketCnt = 0;
		pthread_mutex_lock(&m_TableLock);
		while(uiCurBucketCnt < m_uiBucketCnt)
		{
			if(m_szppBuckets[u] == NULL)
			{
				u++;
				continue;
			}

			p = m_szppBuckets[u];
			while(p)
			{
				__pKey[uiCurBucketCnt++] = *(t_key*)(p + sizeof(char*));
				p = *(char**)p;
			}

			u++;
		}

		uiBucketCnt = uiCurBucketCnt;
		pthread_mutex_unlock(&m_TableLock);
	}

	void GetValues(t_value *__pVal, unsigned int& uiBucketCnt)
	{
		char* p;
		unsigned int u = 0;
		unsigned int uiCurBucketCnt = 0;
		pthread_mutex_lock(&m_TableLock);
		while(uiCurBucketCnt < m_uiBucketCnt)
		{
			if(m_szppBuckets[u] == NULL)
			{
				u++;
				continue;
			}

			p = m_szppBuckets[u];
			while(p)
			{
				__pVal[uiCurBucketCnt++] = *(t_value*)(p + sizeof(char*) + sizeof(t_key));
				p = *(char**)p;
			}
			
			u++;
		}

		uiBucketCnt = uiCurBucketCnt;
		pthread_mutex_unlock(&m_TableLock);
	}

private:
	unsigned int m_uiMaxBucketsSize;
	unsigned int m_uiBucketCnt;
	pthread_mutex_t m_TableLock;
	char**	m_szppBuckets;
};


#endif
