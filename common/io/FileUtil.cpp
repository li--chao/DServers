#include "FileUtil.h"

FileUtil::FileUtil()
{

}

FileUtil::~FileUtil()
{

}

int FileUtil::CreateDir(char *szpBasePath, char *szpDirName)
{
	char *pDir, *pTmp;
	char szaFullName[256];

	pDir = strchr(szpDirName, '/'); // a/b/c
	while(pDir) //递归创建文件夹
	{
		*pDir = 0;
		sprintf(szaFullName, "%s/%s", szpBasePath, szpDirName);
		*pDir = '/';

		if(access(szaFullName, F_OK) && mkdir(szaFullName, S_IRWXU))
		{
			return -1;
		}

		pTmp = pDir + 1;
		pDir = strchr(pTmp, '/'); // b/c
	}

	sprintf(szaFullName, "%s/%s", szpBasePath, szpDirName);
	if(access(szaFullName, F_OK) && mkdir(szaFullName, S_IRWXU))
	{
		return -1;
	}

	return 0;
}

int FileUtil::DeleteDir(char *szpBasePath, char *szpDirName)
{
	int ret;
	char *p;
	DIR *pDirHandle;
	struct dirent64 *pDirTerm;
	char szaFileSpec[256];

	if(*szpDirName == 0)
		strcpy(szaFileSpec, szpBasePath);
	else
		sprintf(szaFileSpec, "%s/%s", szpBasePath, szpDirName);

	pDirHandle = opendir(szaFileSpec);
	if(pDirHandle == NULL)
	{
		return -1;
	}

	while((pDirTerm = readdir64(pDirHandle)))
	{
		p = pDirTerm->d_name;
		if(*p == '.' && (*(p+1) == 0 || (*(p+1) == '.' && *(p+2) == 0))) //忽略, ".", ".."
			continue;

		if(pDirTerm->d_type == DT_DIR)
		{
			strcpy(szaFileSpec, szpDirName);
			if(*szaFileSpec)
			{
				strcat(szaFileSpec, "/");
			}
			strcat(szaFileSpec, pDirTerm->d_name);
			ret = DeleteDir(szpBasePath, szaFileSpec);
			if(ret)
			{
				closedir(pDirHandle);
				return -1;
			}
		}
		else
		{
			if(*szpDirName == 0)
				sprintf(szaFileSpec, "%s/%s", szpBasePath, pDirTerm->d_name);
			else
				sprintf(szaFileSpec, "%s/%s/%s", szpBasePath, szpDirName, pDirTerm->d_name);
			unlink(szaFileSpec);
		}
	}

	closedir(pDirHandle);

	if(*szpDirName == 0)
		strcpy(szaFileSpec, szpBasePath);
	else
		sprintf(szaFileSpec, "%s/%s", szpBasePath, szpDirName);
	rmdir(szaFileSpec);

	return 0;
}

int FileUtil::GetFileNumOfDir(char *szpDirName, unsigned int &uiFileNum)
{
	struct dirent64 *pDirTerm;

	DIR *pDirHandle = opendir(szpDirName);
	if(pDirHandle == NULL)
	{
		return -1;
	}

	uiFileNum = 0;
	while((pDirTerm = readdir64(pDirHandle)))
	{
		char *p = pDirTerm->d_name;
		if(*p == '.' && (*(p+1) == 0 || (*(p+1) == '.' && *(p+2) == 0))) //忽略, ".", ".."
			continue;

		if(pDirTerm->d_type == DT_DIR)
		{
			closedir(pDirHandle);
			return -1;
		}
		else
		{
			uiFileNum++;
		}
	}

	closedir(pDirHandle);

	return 0;
}

int FileUtil::GetFirstFileOfDir(char *szpDirName, char *szpExtName, unsigned int uiExtLen, char *szpFirstFile)
{
	DIR *pDirHandle = opendir(szpDirName);
	if(pDirHandle == NULL)
	{
		return -2;
	}

	struct dirent64 *pDirTerm;
	while((pDirTerm = readdir64(pDirHandle)))
	{
		char *p = pDirTerm->d_name;
		if(*p == '.' && (*(p+1) == 0 || (*(p+1) == '.' && *(p+2) == 0))) //忽略, ".", ".."
			continue;

		if(pDirTerm->d_type == DT_DIR)
		{
			continue;
		}
		
		unsigned int uiNameLen = strlen(pDirTerm->d_name);
		if(uiNameLen <= uiExtLen)
		{
			continue;
		}

		p = pDirTerm->d_name + (uiNameLen - uiExtLen);
		if(strcmp(p, szpExtName) == 0)
		{
			closedir(pDirHandle);
			sprintf(szpFirstFile, "%s/%s", szpDirName, pDirTerm->d_name);
			return 0;
		}
	}

	closedir(pDirHandle);

	return -1;
}

int FileUtil::GetMinNumFileNameOfDir(char *szpDirName, char *szpExtName, char *szpFirstFile)
{
	DIR *pDirHandle = opendir(szpDirName);
	if(pDirHandle == NULL)
	{
		return -2;
	}

	unsigned long long u64Min = 0xFFFFFFFFFFFFFFFF;
	unsigned long long u64Num = 0;
	
	struct dirent64 *pDirTerm;
	while((pDirTerm = readdir64(pDirHandle)))
	{
		if(*(pDirTerm->d_name) == '.') //忽略, ".", ".."
		{
			continue;
		}

		if(pDirTerm->d_type == DT_DIR)
		{
			continue;
		}

		char *pExt = strrchr(pDirTerm->d_name, '.');
		if(!pExt)
		{
			continue;
		}

		if(strcmp(pExt + 1, szpExtName) == 0)
		{
			u64Num = strtoull(pDirTerm->d_name, NULL, 10);
			if(u64Min > u64Num)
			{
				u64Min = u64Num;
			}
		}
	}

	closedir(pDirHandle);

	if(u64Min == 0xFFFFFFFFFFFFFFFF)
	{
		return -1;
	}
	sprintf(szpFirstFile, "%s/%llu.%s", szpDirName, u64Min, szpExtName);
	return 0;	
}

int FileUtil::SetNoBlock(int fd)
{
	int opts;

	opts = fcntl(fd, F_GETFL);
	if(opts < 0)
	{
		//printf("fcntl get error, code=%u\r\n", errno);
		return -1;
	}

	opts |= O_NONBLOCK;
	if(fcntl(fd, F_SETFL, opts) < 0)
	{
		//printf("fcntl get error, code=%u\r\n", errno);
		return -1;
	}

	return 0;
}

int FileUtil::SetBlock(int fd)
{
	int opts;

	opts = fcntl(fd, F_GETFL);
	if(opts < 0)
	{
		//printf("fcntl get error, code=%u\r\n", errno);
		return -1;
	}

	opts &= ~O_NONBLOCK;
	if(fcntl(fd, F_SETFL, opts) < 0)
	{
		//printf("fcntl get error, code=%u\r\n", errno);
		return -1;
	}

	return 0;
}

int FileUtil::CheckFilePath(char *szpFilePath, unsigned int &uiPathLen)
{
	char *pSrc, *pDst;

	if(*szpFilePath != '/') // /home/gyc/a/
	{
		return -1;
	}

	while(uiPathLen > 1 && *(szpFilePath + uiPathLen - 1) == '/') //把最后的'/'去掉
	{
		*(szpFilePath + uiPathLen - 1) = 0;
		uiPathLen--;
	}

	pSrc = szpFilePath;
	pDst = pSrc;
	while(*pSrc) // 检查是否有"//", 并去掉
	{
		if(*pSrc == '/')
		{
			*pDst = *pSrc;
			pDst++;
			pSrc++;
			while(*pSrc == '/')
				pSrc++;
		}
		else if(*pSrc < 0)
		{
			if(*(pSrc + 1))
			{
				*(short *)pDst = *(short *)pSrc;
				pDst += 2;
				pSrc += 2;
			}
			else //扔掉半个汉字
			{
				break;
			}
		}
		else
		{
			*pDst = *pSrc;
			pDst++;
			pSrc++;
		}
	}

	*pDst = 0;
	uiPathLen = pDst - szpFilePath;

	return 0;
}

int FileUtil::GetPathDepth(char *szpFilePath)
{
	int depth;
	char *pSrc;

	depth = 0;
	pSrc = szpFilePath;
	while(*pSrc)
	{
		if(*pSrc == '/')
		{
			depth++;
			pSrc++;
		}
		else if(*pSrc < 0)
		{
			if(*(pSrc + 1))
			{
				pSrc += 2;
			}
			else
			{
				break;
			}
		}
		else
		{
			pSrc++;
		}
	}

	return depth;
}

ECfgError FileUtil::GetCfgKey(FILE *fp, char *szpKey, unsigned int uiIOLen, char *&szpValue)
{
	ECfgError err = GetLine(fp, szpKey, uiIOLen);
	if(err != E_Cfg_Content)
		return err;

	char *p = strchr(szpKey, '=');
	if(p == NULL || *(p+1) == 0) //错误
		return E_Cfg_Error;

	*p++ = 0;
	szpValue = p;

	return E_Cfg_Content;
}

ECfgError FileUtil::GetLine(FILE *fp, char *szpContent, unsigned int uiIOLen)
{
	char *p = fgets(szpContent, uiIOLen, fp);
	if(p == NULL)
		return E_Cfg_EOF;

	if(*szpContent == '#') //注释
		return E_Cfg_Comment;

	p = szpContent;
	while(*p)
	{
		if(*p == '\r' || *p == '\n')
		{
			*p = 0;
			break;
		}
		p++;
	}
	if(*szpContent == 0) //空行
		return E_Cfg_Blank;

	return E_Cfg_Content;
}

EFileLockError FileUtil::NonBlockFLock(char *szpLockFile, int &fd)
{
	fd = open(szpLockFile, O_RDONLY);
	if(fd == -1)
	{
		return E_File_Lock_None;
	}

	if(flock(fd, LOCK_EX|LOCK_NB))
	{
		close(fd);
		return E_File_Lock_Failed;
	}

	return E_File_Lock_Success;
}

int FileUtil::SwitchFile(char *szpBakFile, char *szpOldFile, char *szpNewFile)
{
	if(access(szpBakFile, F_OK) == 0 && unlink(szpBakFile)) //删除bak
	{
		return -1;
	}
	if(access(szpOldFile, F_OK) == 0 && rename(szpOldFile, szpBakFile)) //重名old=>bak
	{
		return -1;
	}
	if(rename(szpNewFile, szpOldFile)) //重名new=>old
	{
		return -1;
	}

	return 0;
}

int FileUtil::ResumeFile(char *szpBakFile, char *szpOldFile, char *szpNewFile)
{
	if(access(szpOldFile, F_OK) && access(szpBakFile, F_OK) == 0 && access(szpNewFile, F_OK) == 0) //old不存在, bak和new存在
	{
		if(rename(szpNewFile, szpOldFile)) //重名new=>old
		{
			return -1;
		}
	}

	return 0;
}

int FileUtil::IsDirOfFileExist(char *szpFileName, unsigned char ucIsCreate)
{
	char *p = strrchr(szpFileName, '/');
	if(p)
	{
		*p = 0;
		if(access(szpFileName, F_OK)) //szpFileName不存在
		{
			if(ucIsCreate == 0)
			{
				*p = '/';
				return -1;
			}
			else
			{
				if(mkdir(szpFileName, S_IRWXU))
				{
					*p = '/';
					return -1;
				}
			}
		}
		*p = '/';
	}

	return 0;
}

int FileUtil::CreateFile(char *szpFileName)
{
	if(access(szpFileName, F_OK) == 0)
	{
		return 0;
	}

	return CreateNewFile(szpFileName);
}

int FileUtil::CreateNewFile(char *szpFileName)
{
	FILE *fp = fopen(szpFileName, "w");
	if(fp)
	{
		fclose(fp);
		return 0;
	}
	return -1;
}

void FileUtil::RemoveFile(char *szpFileName)
{
	if(access(szpFileName, F_OK) == 0) //删除Dat
	{
		unlink(szpFileName);
	}
}

int FileUtil::LoadFile(char *szpFileName, char *szpBuf, unsigned int uiBufLen)
{
	if(access(szpFileName, F_OK)) //文件不存在
	{
		return 1;
	}

	FILE *fp = fopen(szpFileName, "rb");
	if(!fp) //磁盘错误
	{
		return -1;
	}

	int ret = fread(szpBuf, uiBufLen, 1, fp);
	fclose(fp);
	if(ret != 1) //磁盘错误
	{
		return -1;
	}

	return 0;
}

int FileUtil::SaveFile(char *szpFileName, char *szpBuf, unsigned int uiBufLen)
{
	FILE *fp = fopen(szpFileName, "wb");
	if(!fp) //磁盘错误
	{
		return -1;
	}

	int ret = fwrite(szpBuf, uiBufLen, 1, fp);
	fclose(fp);
	if(ret != 1) //磁盘错误
	{
		return -1;
	}

	return 0;
}

int FileUtil::WriteFile(char *szpFileName, unsigned long long u64FilePos, char *szpBuf, unsigned int uiBufLen)
{
	FILE *fp = fopen(szpFileName, "rb+");
	if(!fp) //磁盘错误
	{
		return -1;
	}

	if(u64FilePos)
	{
		 if(fseeko64(fp, u64FilePos, SEEK_SET)) //磁盘错误
		 {
			 fclose(fp);
			 return -1;
		 }
	}

	int ret = fwrite(szpBuf, uiBufLen, 1, fp);
	fclose(fp);
	if(ret != 1) //磁盘错误
	{
		return -1;
	}

	return 0;
}

int FileUtil::ReadFile(char** szpData, unsigned int* uiDataLen, const char *szpFileNmae) {
  FILE *fp = fopen(szpFileNmae, "r");
  if(fp == NULL) {
    return -1;
  }

  struct stat filestats;
  int fd = fileno(fp);
  fstat(fd, &filestats);
  *uiDataLen = filestats.st_size;
  *szpData = (char*)malloc(*uiDataLen + 1);
  if(*szpData == NULL) {
    return -1;
  }

  int start = 0;
  int bytes_read = 0;
  while ((bytes_read = fread(*szpData + start, 1, *uiDataLen - start, fp))) {
    start += bytes_read;
  }

  *(*szpData + *uiDataLen) = '\0';
  return 0;
}

int FileUtil::GetDiskSz7OldTm(char *szpDatPath, unsigned long long &u64CurSize, char *szpOldFile)
{
	struct dirent64 *pDirTerm;
	DIR *pDirHandle = opendir(szpDatPath);
	if(pDirHandle == NULL)
	{
		return -1;
	}

	struct stat64 stInfo;
	char szaFileName[FILE_NAME_SIZE];
	time_t tMin = 0;
	u64CurSize = 0;
	*szpOldFile = 0;
	while((pDirTerm = readdir64(pDirHandle)))
	{
		char *p = pDirTerm->d_name;
		if(*p == '.' && (*(p+1) == 0 || (*(p+1) == '.' && *(p+2) == 0))) //忽略, ".", ".."
		{
			continue;
		}
		if(pDirTerm->d_type == DT_DIR)
		{
			continue;
		}

		sprintf(szaFileName, "%s/%s", szpDatPath, pDirTerm->d_name);
		if(stat64(szaFileName, &stInfo)) //删除破损文件
		{
			unlink(szaFileName);
			continue;
		}

		if(tMin == 0 || tMin > stInfo.st_atime)
		{
			tMin = stInfo.st_atime;
			strcpy(szpOldFile, szaFileName);
		}
		u64CurSize += stInfo.st_size;
	}

	closedir(pDirHandle);

	return 0;
}