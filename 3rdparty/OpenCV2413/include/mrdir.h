#ifndef MRDIR_H
#define MRDIR_H
#include "string"
#include "iostream"
#include "vector"
using namespace std;

#ifdef WIN32
#include <tchar.h>
#include "windows.h"

static wchar_t * ANSIToUnicode(const char* str)
{
	int textlen;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t));
	memset(result, 0, (textlen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

static char * UnicodeToANSI(const wchar_t* str)
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen + 1)*sizeof(char));
	memset(result, 0, sizeof(char)* (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

static wchar_t * UTF8ToUnicode(const char* str)
{
	int textlen;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t));
	memset(result, 0, (textlen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

static char * UnicodeToUTF8(const wchar_t* str)
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen + 1)*sizeof(char));
	memset(result, 0, sizeof(char)* (textlen + 1));
	WideCharToMultiByte(CP_UTF8, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

static char* ANSIToUTF8(const char* str)
{
	return UnicodeToUTF8(ANSIToUnicode(str));
}

static char* UTF8ToANSI(const char* str)
{
	return UnicodeToANSI(UTF8ToUnicode(str));
}
#if _UNICODE
static std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return NULL;

	WCHAR *pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst) return NULL;

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if (pwszDst[0] == 0xFEFF)                    // skip Oxfeff
	for (int i = 0; i < nSize; i++)
		pwszDst[i] = pwszDst[i + 1];

	wstring wcharString(pwszDst);
	delete pwszDst;

	return wcharString;
}

static std::wstring s2ws(const string& s)
{
	return Ansi2WChar(s.c_str(), s.size());
}
static string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}
#endif

//��ȡ��ǰ�ļ������������ļ��У�������vector<string>��
static bool getAllSubdirs(string strDir, vector<string> &subdirs)
{
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	string file2find = strDir + "/*.*";
#if _UNICODE
	hError = FindFirstFile((LPCTSTR)s2ws((LPCSTR)(file2find).c_str()).c_str(), &FindData);
#else
	hError = FindFirstFile((LPCTSTR)file2find.c_str(), &FindData);
#endif
	if (hError == INVALID_HANDLE_VALUE)
	{
		std::cout << "δ����ļ���" << std::endl;
		return 0;
	}
	else
	{
		do
		{
			//����.��..;��.��������Ŀ¼��..��������Ŀ¼;
			if (lstrcmp(FindData.cFileName, TEXT(".")) == 0 || lstrcmp(FindData.cFileName, TEXT("..")) == 0)
			{
				continue;
			}
			//��ȡ�ļ����ԣ���������ļ���;
			if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}
			//������ļ���
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
#if _UNICODE
				subdirs.push_back(WChar2Ansi(FindData.cFileName));
#else
				subdirs.push_back(FindData.cFileName);
#endif
			}
			//ѭ����������һ���ļ�;

		} while (::FindNextFile(hError, &FindData));
	}
	//�رվ��;
	FindClose(hError);
	return 0;
}

//��ȡĿ¼�����е��ļ�
static bool getAllFilesinDir(string strDir, vector<string> &files,string ext="*.*")//��ȡ��ǰ�ļ������������ļ��У�������vector<string>��
{
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	string file2find = strDir + "/"+ext;
#if _UNICODE
	hError = FindFirstFile((LPCTSTR)s2ws((LPCSTR)(file2find).c_str()).c_str(), &FindData);
#else
	hError = FindFirstFile((LPCTSTR)file2find.c_str(), &FindData);
#endif
	if (hError == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	else
	{
		do
		{
			if (lstrcmp(FindData.cFileName, TEXT(".")) == 0 || lstrcmp(FindData.cFileName, TEXT("..")) == 0)
			{
				continue;
			}
			else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}
			else if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
#if _UNICODE
				files.push_back(WChar2Ansi(FindData.cFileName));
#else
				files.push_back(FindData.cFileName);
#endif
			}
		} while (::FindNextFile(hError, &FindData));
	}
	FindClose(hError);
	return 0;
}

#include "io.h"
static bool exist(const char*filepath)
{
	if ((_access(filepath, 0)) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

#else

#include <dirent.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <unistd.h>

static bool getAllSubdirs(string strDir, vector<string> &subdirs)
{
	DIR *dp;                      // ������Ŀ¼��ָ��  
	struct dirent *entry;         // ����dirent�ṹָ�뱣�����Ŀ¼  
	struct stat statbuf;          // ����statbuf�ṹ�����ļ�����  
	if((dp = opendir(strDir.c_str())) == NULL) // ��Ŀ¼����ȡ��Ŀ¼��ָ�룬�жϲ����Ƿ�ɹ�  
	{    
		return 0;  
	}                    // �л�����ǰĿ¼ 
	while((entry = readdir(dp)) != NULL)  // ��ȡ��һ��Ŀ¼��Ϣ�����δ����ѭ��  
	{  
		lstat(entry->d_name, &statbuf); // ��ȡ��һ����Ա����  
		if(S_IFDIR &statbuf.st_mode)    // �ж���һ����Ա�Ƿ���Ŀ¼  
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)  
				continue;  
			subdirs.push_back(entry->d_name); 
		}  
	}                                                    // �ص��ϼ�Ŀ¼  
	closedir(dp);                                                 // �ر���Ŀ¼�� 
	return 0;
}

static bool getAllFilesinDir(string strDir, vector<string> &files,string ext="*.*")
{
	struct dirent* ent = NULL;  
	DIR *pDir;  
	pDir = opendir(strDir.c_str());  
	if (pDir == NULL) {  
		return false;  
	}
	while (NULL != (ent = readdir(pDir))) {  
		if (ent->d_type == 8) {  
			files.push_back(ent->d_name);
		}
		else {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
		}
	}
	closedir(pDir);  
	return true;
}
#endif

#endif
