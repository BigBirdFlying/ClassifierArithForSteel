#include "StdAfx.h"
#include "HFile.h"


///ini文件
HFile_ini::HFile_ini(void)
{
}

HFile_ini::~HFile_ini(void)
{
}

int HFile_ini::ReadAllKeyAndValue(CString strSectionName,std::vector<KeyAndValue> &vecKeyAndValue)
{
	TCHAR szKey[1024] = {0};
	CString strKey = _T("");
	CString strKeyName = _T("");
	CString strKeyValue = _T("");
	TCHAR szBuffer[65536] = {0};

	int nBufferSize = GetPrivateProfileSection(strSectionName, szBuffer, 65536, m_strFileName);

	int nItem = 0;
	for (int n = 0, i = 0; n < nBufferSize; n++)
	{
 		if (szBuffer[n] == 0)
 		{
 			szKey[i] = 0;
			strKey = szKey;

			KeyAndValue tagKeyAndValue;
			tagKeyAndValue.strKey = strKey.Left(strKey.Find('='));
			tagKeyAndValue.strValue = strKey.Mid(strKey.Find('=') + 1);
			vecKeyAndValue.push_back(tagKeyAndValue);

			i = 0;
			nItem++;
 		}
		else
		{
			szKey[i] = szBuffer[n];

			i++;
		}
	}
	return 1;
}

int HFile_ini::Read(CString strSectionName,CString strKeyName,CString& strKeyValue)
{
	DWORD dd=::GetPrivateProfileString(strSectionName, strKeyName, _T(""), strKeyValue.GetBuffer(1024), 1024, m_strFileName);
	strKeyValue.ReleaseBuffer();
	if(0==dd)
	{
		return 0;
	}
	return 1;
}

int HFile_ini::Write(CString strSectionName,CString strKeyName,CString strKeyValue)
{
	DWORD dd=::WritePrivateProfileString(strSectionName, strKeyName, strKeyValue, m_strFileName);;
	if(0==dd)
	{
		return 0;
	}
	return 1;
}



///txt文件
HFile_txt::HFile_txt(void)
{
}

HFile_txt::~HFile_txt(void)
{
}

int HFile_txt::Open(CString strMode)
{
	//char szStr[1024] = {0};
	//wcstombs(szStr, m_strFileName, m_strFileName.GetLength());//Unicode转换为ASCII
	//const char * p = szStr;

	USES_CONVERSION;//声明标示符,调用函数，T2A和W2A均支持ATL和MFC中字符转换
	char *scFileName = T2A(m_strFileName);
	char *scMode=T2A(strMode);

	fp=fopen(scFileName,scMode);
	if(NULL==fp)
	{
		return 0;
	}
	return 1;
}

int HFile_txt::Read(char StrLine[1024])
{
	while (!feof(fp))
	{ 
		fgets(StrLine,1024,fp);
	}
	return 1;
}

int HFile_txt::Write(char StrLine[1024])
{
	if(NULL != StrLine)
	{
		 fputs(StrLine, fp); //写入文件
	}
	return 1;
}

int HFile_txt::Close()
{
	fclose(fp);
	return 1;
}



///xml文件
HFile_xml::HFile_xml(void)
{	
	m_hr=m_pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
}

HFile_xml::~HFile_xml(void)
{
	//m_pDoc->Release();
}

int HFile_xml::LoadFile(CString strFileName)
{	
	if(!SUCCEEDED(m_hr))
	{
		AfxMessageBox(L"无法创建DOMDocument对象，请检查是否安装了MSXML库！");
		return 0;
	}
	else
	{
		m_pDoc->load((_variant_t)strFileName);
		return 1;
	}
}

int HFile_xml::SaveFile(CString strFileName)
{	
	if(!SUCCEEDED(m_hr))
	{
		AfxMessageBox(L"无法创建DOMDocument对象，请检查是否安装了MSXML库！");
		return 0;
	}
	else
	{
		m_pDoc->save((_variant_t)strFileName);
		return 1;
	}
}

int HFile_xml::Write()
{
	MSXML2::IXMLDOMElementPtr xmlRoot;
	//xml写入
	xmlRoot=m_pDoc->createElement((_bstr_t)"Book");
	xmlRoot->setAttribute("id",(const char*)"ss");
	m_pDoc->appendChild(xmlRoot);
	MSXML2::IXMLDOMElementPtr pNode;
	//
	pNode=m_pDoc->createElement((_bstr_t)"Author");
	pNode->Puttext((_bstr_t)(const char*)"wkp");
	xmlRoot->appendChild(pNode);
	//
	pNode=m_pDoc->createElement("Title");
	pNode->Puttext("whi");
	xmlRoot->appendChild(pNode);
	//
	return 1;
}

int HFile_xml::Read(CString strPath,CString& strValue)
{
	//int iPos=strPath.Find(L"#");
	//while(-1 != iPos)
	//{
	//	CString strNode=strPath.Left(iPos);
	//	int len=strPath.GetLength();
	//	CString strOffset=strPath.Right(len-1-iPos);
	//	
	//	MSXML2::IXMLDOMNodePtr pNode;
	//	pNode=m_pDoc->selectSingleNode((char*)(_bstr_t)strNode);
	//}
	
	//MSXML2::IXMLDOMNodePtr pNode;
	//pNode=m_pDoc->selectSingleNode("//Book");
	//MSXML2::DOMNodeType nodeType;
	//pNode->get_nodeType(&nodeType);
	//CString strName;
	//strName=(char*)pNode->GetnodeName();
	////
	//MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap=NULL;
	//MSXML2::IXMLDOMNodePtr pAttrItem;
	//_variant_t variantvalue;
	//pNode->get_attributes(&pAttrMap);
	//long count;
	//count=pAttrMap->get_length(&count);
	//pAttrMap->get_item(0,&pAttrItem);
	////
	//pAttrItem->get_nodeTypedValue(&variantvalue);
	//CString strId=L"";
	//strId=(char*)(_bstr_t)variantvalue;
	//
	MSXML2::IXMLDOMNodePtr pNode;
	_variant_t variantvalue;
	pNode=m_pDoc->selectSingleNode((char*)(_bstr_t)strPath);
	pNode->get_nodeTypedValue(&variantvalue);
	strValue=(char*)(_bstr_t)variantvalue;
	return 1;
}

///Log日志文件
HFile_log::HFile_log(void)
{
}

HFile_log::~HFile_log(void)
{
}

void HFile_log::SetLogFilePath(CString strAppPath)
{
	m_strLogFilePath.Format(L"%s\\MyLog",strAppPath);
	int IsExist=PathFileExists(m_strLogFilePath);
	if(!IsExist)
	{
		CreateDirectory(m_strLogFilePath, NULL);
	}	
}

void HFile_log::AddLogInfo(CString strLogInfo)
{
	CTime CurTime = CTime::GetCurrentTime();
	CString strDate=L"";
	CString strTime=L"";
	strDate.Format(L"%04d-%02d-%02d",CurTime.GetYear(),CurTime.GetMonth(),CurTime.GetDay());
	strTime.Format(L"%02d:%02d:%02d>>",CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond());

	CString strLogFileAllName;
	strLogFileAllName.Format(L"%s\\%s.txt",m_strLogFilePath,strDate);

	USES_CONVERSION;
	const char * time=T2A(strTime.GetBuffer(0));
	const char * info=T2A(strLogInfo.GetBuffer(0));
	const char * fileallname=T2A(strLogFileAllName.GetBuffer(0));
	TXTFILE_OPEN_START(fileallname)
		TXTFILE_ADD(time,info)//TXTFILE_ADD("图像索引",(float)ImgIndex)
	TXTFILE_CLOSE_END
}