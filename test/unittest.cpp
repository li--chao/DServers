#include <iostream>
#include "../common/network/epollnet.h"
#include "../servconfig.h"
#include "../common/log/TextLog.h"
#include "../common/network/netqueue.h"
#include "../common/container/hashtableiter.h"
#include "assert.h"

pthread_mutex_t deadlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
{
	LcHashTable<int, int> int_hash_table;
	int_hash_table.Init(10747);
	int_hash_table.Insert(2, 3);

	int ret = int_hash_table.Insert(2, 3);
	assert(ret == 2);

	assert(int_hash_table.BucketCnt() == 1);
	char *p = NULL;

	int search_ret = int_hash_table.Search(2, p);
	assert(search_ret == 0);
	assert(*(int*)p == 3);

	search_ret = int_hash_table.Search(3, p);
	assert(search_ret == 1);
	assert(p == NULL);

	ret = int_hash_table.Insert(10749, 4);
	assert(ret == 0);
	assert(int_hash_table.BucketCnt() == 2);

	ret = int_hash_table.DeleteByKey(2);
	assert(ret == 0);
	assert(int_hash_table.BucketCnt() == 1);

	ret = int_hash_table.Search(10749, p);
	assert(ret == 0);
	assert(*(int*)p == 4);

	int val = 0;
	ret = int_hash_table.DeleteByKey(10749, val);
	assert(ret == 0);
	assert(val == 4);
	assert(int_hash_table.BucketCnt() == 0);

	ret = int_hash_table.Search(2, p);
	assert(ret == 1);
	assert(p == 0);
	assert(int_hash_table.BucketCnt() == 0);

	ret = int_hash_table.Search(3, p);
	assert(ret == 1);
	assert(p == 0);
	assert(int_hash_table.BucketCnt() == 0);

	ret = int_hash_table.Search(4, p);
	assert(ret == 1);
	assert(p == 0);
	assert(int_hash_table.BucketCnt() == 0);

	LcHashTable<int, unsigned int> testHash;
	testHash.Init(10747);
	testHash.Insert(2, 3);
	testHash.Insert(4, 5);

	unsigned int* pVals = new unsigned int[testHash.BucketCnt()];
	int* pKeys = new int[testHash.BucketCnt()];
	testHash.GetValues(pVals);
	testHash.GetKeys(pKeys);
	assert(pVals[0] == 3);
	assert(pVals[1] == 5);
	assert(pKeys[0] == 2);
	assert(pKeys[1] == 4);
	return 0;
}
