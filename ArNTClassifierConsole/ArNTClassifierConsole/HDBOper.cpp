#include "StdAfx.h"
#include "HDBOper.h"

HDBOper::HDBOper(void)
{
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

HDBOper::~HDBOper(void)
{
	::CoUninitialize();
}

//-----------------------------------------------------------
//描述：连接数据库
bool   HDBOper::ConnectDB(CString strDBServer, CString strDBName, CString strUser, CString strPassWd, CString strType)
{
	m_pConnection.CreateInstance(__uuidof(Connection));
	CString strConnect;
	strConnect.Format(L"Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True; User ID=%s;Initial Catalog=%s;Data Source=%s",strPassWd,strUser,strDBName,strDBServer);

	m_pConnection->CursorLocation = adUseClient;
	m_pConnection->Open(_bstr_t( strConnect), L"", L"", -1);
	
	m_pCommand.CreateInstance(__uuidof(Command));
	m_pCommand->ActiveConnection = m_pConnection;
	m_pCommand->CommandType = adCmdText;
	m_pSearchSet.CreateInstance(__uuidof(Recordset));	

	m_bConnected = true;

	return true;
}

bool   HDBOper::SearchDefect(SteelDefectInfoSet& DefectSet,long& iDefectMaxIndex,CString strSQLCondition)
{
	if(false == m_bConnected) 
	{
		return false;
	}			
	m_pSearchSet->Open((_variant_t)strSQLCondition,_variant_t((IDispatch *)m_pConnection,true),adOpenDynamic,adLockPessimistic,adCmdText);
	long lRecordCount = m_pSearchSet->GetRecordCount();
	DefectSet.iItemNum=0;
	if(lRecordCount > 0) 
	{
		m_pSearchSet->MoveFirst();
		int iRecordIndex = 0;
		while(!m_pSearchSet->adoEOF)
		{
			_variant_t Value;		
			Value = m_pSearchSet->GetCollect("DefectNo");
			DefectSet.Items[DefectSet.iItemNum].iDefectNo=Value.intVal;
			if(DefectSet.Items[DefectSet.iItemNum].iDefectNo>iDefectMaxIndex)
			{
				iDefectMaxIndex=DefectSet.Items[DefectSet.iItemNum].iDefectNo;
			}
				
			Value = m_pSearchSet->GetCollect("SteelNo");
			DefectSet.Items[DefectSet.iItemNum].iSteelNo=Value.intVal;

			Value = m_pSearchSet->GetCollect("CameraNo");
			DefectSet.Items[DefectSet.iItemNum].iCameraNo=Value.iVal;

			Value = m_pSearchSet->GetCollect("ImageIndex");
			DefectSet.Items[DefectSet.iItemNum].iImageIndex=Value.iVal;

			Value = m_pSearchSet->GetCollect("Class");
			DefectSet.Items[DefectSet.iItemNum].iClass=Value.iVal;
			DefectSet.Items[DefectSet.iItemNum].iPerClass=0;

			Value = m_pSearchSet->GetCollect("Grade");
			DefectSet.Items[DefectSet.iItemNum].iGrade=Value.iVal;

			Value = m_pSearchSet->GetCollect("LeftInImg");
			DefectSet.Items[DefectSet.iItemNum].iLeftInImg=Value.iVal;

			Value = m_pSearchSet->GetCollect("RightInImg");
			DefectSet.Items[DefectSet.iItemNum].iRightInImg=Value.iVal;

			Value = m_pSearchSet->GetCollect("TopInImg");
			DefectSet.Items[DefectSet.iItemNum].iTopInImg=Value.iVal;

			Value = m_pSearchSet->GetCollect("BottomInImg");
			DefectSet.Items[DefectSet.iItemNum].iBottomInImg=Value.iVal;

			Value = m_pSearchSet->GetCollect("Area");
			DefectSet.Items[DefectSet.iItemNum].iArea=Value.intVal;		

			m_pSearchSet->MoveNext();
			iRecordIndex++;
			DefectSet.iItemNum++;

			if(DefectSet.iItemNum>1024)
			{
				break;
			}
		}
	}

	m_pSearchSet->Close();
	return true;
}

bool   HDBOper::GetDefectMaxNum(long& iDefectNoIndex,CString strSQLCondition)
{
	long iDefectNum = 0;
	if(false == m_bConnected) 
	{
		return false;
	}			
	m_pSearchSet->Open((_variant_t)strSQLCondition,_variant_t((IDispatch *)m_pConnection,true),adOpenDynamic,adLockPessimistic,adCmdText);
	long lRecordCount = m_pSearchSet->GetRecordCount();
	iDefectNum = lRecordCount;
	if(lRecordCount > 0) 
	{
		m_pSearchSet->MoveFirst();
		int iRecordIndex = 0;
		while(!m_pSearchSet->adoEOF)
		{
			_variant_t Value;		
			Value = m_pSearchSet->GetCollect("DefectNo");
			iDefectNoIndex=Value.intVal;
			m_pSearchSet->MoveNext();
		}
	}
	
	m_pSearchSet->Close();
	return true;
}

bool   HDBOper::UpdateDefectClass(CString strSQLCondition)
{
	if(false == m_bConnected) 
	{
		return false;
	}			
	m_pCommand->CommandText = _bstr_t( strSQLCondition);
	m_pCommand->Execute(NULL, NULL, adCmdText);
	return true;
}