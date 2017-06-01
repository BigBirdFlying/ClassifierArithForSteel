#pragma once
#include "StdAfx.h"
#import  "msxml6.dll" 
using namespace MSXML2;//	//fseek(fp,0,SEEK_END);	

#define uchar unsigned char
#define TXTFILE_OPEN_START(strFile)		FILE* fp=fopen(strFile,"a+");\
										fputs("START\n", fp);\
										char ss[256]={0};
#define TXTFILE_ADD(sKey,sVal)			sprintf(ss, "%s:%s\n",sKey,sVal);\
										fputs(ss, fp);\
										memset(ss,0,sizeof(char)*256);
#define TXTFILE_CLOSE_END				fputs("END", fp);\
										fclose(fp);
										

typedef struct KeyAndValue
{
	CString strKey;
	CString strValue;
};

class HFile_ini
{
public:
	HFile_ini(void);
	~HFile_ini(void);

	CString m_strFileName;

	int ReadAllKeyAndValue(CString strSectionName,std::vector<KeyAndValue> &vecKeyAndValue);
	int Read(CString strSectionName,CString strKeyName,CString& strKeyValue);
	int Write(CString strSectionName,CString strKeyName,CString strKeyValue);
};


class HFile_txt
{
public:
	HFile_txt(void);
	~HFile_txt(void);

	CString m_strFileName;
	FILE *fp;

	int Open(CString strMode);
	int Read(char StrLine[1024]);
	int Write(char StrLine[1024]);
	int Close();
};

class HFile_xml
{
public:
	HFile_xml(void);
	~HFile_xml(void);

	HRESULT m_hr;
	MSXML2::IXMLDOMDocumentPtr m_pDoc; 
	CString m_strFileName;

	int LoadFile(CString strFileName);
	int SaveFile(CString strFileName);

	int Write();
	int Read(CString strPath,CString& strValue);
};

class HFile_log
{
public:
	HFile_log(void);
	~HFile_log(void);

public:
	CString m_strLogFilePath;

public:
	void SetLogFilePath(CString strFilePath);
	void AddLogInfo(CString strLogInfo);
};