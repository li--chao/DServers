#ifndef _BASE_COMMON_H_
#define _BASE_COMMON_H_

#define NODE_ADDR_LEN (sizeof(unsigned int)+sizeof(unsigned short))
#define MAP_ADDR_LEN (NODE_ADDR_LEN+sizeof(unsigned short))

#define MAX_UINT 0xFFFFFFFF
#define MAX_USHORT 0xFFFF
#define MAX_UCHAR 0xFF

#define MAX_U64_STR_SIZE 32
#define FILE_PATH_SIZE 100
#define FILE_NAME_SIZE 200
#define FIELD_NAME_SIZE 64
#define MD5_STRID_SIZE 16
#define PWD_STRID_SIZE 32

typedef struct ClusterNode
{
	unsigned int uiNodeIP;
	unsigned short usNodePort;
	unsigned short usClusterNo;
	unsigned char ucClusterType;
}ClusterNode;

#endif
