#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../BaseCommon.h"

#define UNFLOCK(fd) close(fd)

enum ECfgError
{
	E_Cfg_EOF,
	E_Cfg_Comment,
	E_Cfg_Blank,
	E_Cfg_Error,
	E_Cfg_Content
};

enum EFileLockError
{
	E_File_Lock_Success, //锁成功
	E_File_Lock_Failed, //锁失败
	E_File_Lock_None //文件打不开
};


class FileUtil
{
public:
	FileUtil();
	~FileUtil();

	static int CreateDir(char *szpBasePath, char *szpDirName);
	static int DeleteDir(char *szpBasePath, char *szpDirName);
	static int GetFileNumOfDir(char *szpDirName, unsigned int &uiFileNum);
	static int GetFirstFileOfDir(char *szpDirName, char *szpExtName, unsigned int uiExtLen, char *szpFirstFile);
	static int GetMinNumFileNameOfDir(char *szpDirName, char *szpExtName, char *szpFirstFile);

	static int SetNoBlock(int fd);
	static int SetBlock(int fd);

	static int CheckFilePath(char *szpFilePath, unsigned int &uiPathLen);
	static int GetPathDepth(char *szpFilePath);
	static ECfgError GetCfgKey(FILE *fp, char *szpKey, unsigned int uiIOLen, char *&szpValue);
	static ECfgError GetLine(FILE *fp, char *szpKey, unsigned int uiIOLen);

	static EFileLockError NonBlockFLock(char *szpLockFile, int &fd);

	static int SwitchFile(char *szpBakFile, char *szpOldFile, char *szpNewFile);
	static int ResumeFile(char *szpBakFile, char *szpOldFile, char *szpNewFile);

	static int IsDirOfFileExist(char *szpFileName, unsigned char ucIsCreate = 0);

	static int CreateFile(char *szpFileName);
	static int CreateNewFile(char *szpFileName);
	static void RemoveFile(char *szpFileName);

	static int LoadFile(char *szpFileName, char *szpBuf, unsigned int uiBufLen);
	static int SaveFile(char *szpFileName, char *szpBuf, unsigned int uiBufLen);
	static int WriteFile(char *szpFileName, unsigned long long u64FilePos, char *szpBuf, unsigned int uiBufLen);
	static int ReadFile(char** szpData, unsigned int* uiDataLen, const char *szpFileNmae);

	static int GetDiskSz7OldTm(char *szpDatPath, unsigned long long &u64CurSize, char *szpOldFile);

private:

};

#endif
