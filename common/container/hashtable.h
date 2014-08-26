#ifndef HASH_TABLE_H
#define	HASH_TABLE_H

#include <pthread.h>
#include "bucket.h"

template<class t_key, class t_value>
class LcHashTable
{
public:
	LcHashTable() :
		m_uiMinIndex(0),
		m_uiMaxIndex(0),
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
		m_uiMinIndex = uiMaxBucketsSize;
		m_uiMaxIndex = 0;
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
		if(uiInsertIdx < m_uiMinIndex)
		{
			m_uiMinIndex = uiInsertIdx;
		}
		if(uiInsertIdx > m_uiMaxIndex)
		{
			m_uiMaxIndex = uiInsertIdx;
		}

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
		}

		pthread_mutex_unlock(&m_TableLock);
		return 1;
	}
	
private:
	unsigned int m_uiMinIndex;
	unsigned int m_uiMaxIndex;
	unsigned int m_uiMaxBucketsSize;
	unsigned int m_uiBucketCnt;
	pthread_mutex_t m_TableLock;
	char**	m_szppBuckets;
};


#endif
