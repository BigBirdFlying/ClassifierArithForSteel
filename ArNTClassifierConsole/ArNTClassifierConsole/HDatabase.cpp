#include "StdAfx.h"
#include "HDatabase.h"

HDatabase::HDatabase(void)
{
	::CoInitialize(NULL);
	if (!SUCCEEDED(m_pConnection.CreateInstance(__uuidof(Connection))))
	{
		m_pConnection = NULL;
		TRACE(_T("Database CreateInstance failed"));
	}
	if (!SUCCEEDED(m_pRecordset.CreateInstance(__uuidof(Recordset))))
	{
		m_pRecordset = NULL;
		TRACE(_T("Recordset CreateInstance Failed!"));
	}
}

HDatabase::~HDatabase(void)
{

}

int HDatabase::OpenDatabase(CString strDatabaseName)
{
	CString strConnect;
	strConnect.Format(L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source= %s",strDatabaseName);
	
	LPCTSTR lpszConnect=strConnect;
	long nOptions=adConnectUnspecified;
	
	ASSERT(m_pConnection != NULL);
	ASSERT(lpszConnect != NULL);
	ASSERT(AfxIsValidString(lpszConnect));
	
	//打开数据库连接
	try
	{
		if(SUCCEEDED(m_pConnection->Open(_bstr_t(lpszConnect), _T(""), _T(""), nOptions)))
		{
			return 1;
		}
	}
	catch (_com_error& e)
	{
		TRACE(_T("%s\n"), e.ErrorMessage());
		return 0;
	}
}


int HDatabase::OpenTable(CString strTableName)
{
	CString strSource;
	strSource.Format(L"SELECT * FROM %s",strTableName);
	
	LPCTSTR lpszSource=strSource;
	long nCursorType=adOpenDynamic;
	long nLockType=adLockOptimistic;
	long nOptions=adCmdText;
	
	
	ASSERT(m_pConnection != NULL);
	ASSERT(m_pRecordset != NULL);
	ASSERT(lpszSource != NULL);
	ASSERT(AfxIsValidString(lpszSource));

	//打开记录集
	try
	{
		return (SUCCEEDED(m_pRecordset->Open(_variant_t(lpszSource),
										m_pConnection.GetInterfacePtr(),
										(CursorTypeEnum)nCursorType, 
										(LockTypeEnum)nLockType,
										nOptions)));
	}
	catch(_com_error e)
	{
		TRACE(_T("%s\n"), e.ErrorMessage());
		return FALSE;
	}
}

int HDatabase::Find(CString strSql)
{
	if (m_pRecordset->BOF && m_pRecordset->adoEOF)
	{
		return 0;
	}
	m_pRecordset->MoveFirst();
	int n = 0;
	m_pRecordset->Find(_bstr_t(strSql), 0, adSearchForward);
}

int HDatabase::Add(CString strSql)
{
	if (m_pRecordset->BOF && m_pRecordset->adoEOF)
	{
		return 0;
	}

	try
	{
		//m_pRecordset.CreateInstance("ADODB.Recordset"); //为Recordset对象创建实例
		//CString sql;
		//sql.Format(L"insert into %s(Detection_Time_Char,Steel_Char,Image_Path) values('%s','%s','%s')",strTable,strCurTime,strChar,strImagePath);
		_bstr_t strCmd=strSql;				
		m_pRecordset=m_pConnection->Execute(strCmd,&m_RecordsAffected,adCmdText);
	}
	catch(_com_error &e)
	{
		AfxMessageBox(e.Description());
		return 0;
	}
	return 1;;
}