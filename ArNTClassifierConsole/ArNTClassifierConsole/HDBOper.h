#pragma once
// µº»Î ado ø‚ -----------------------------------------------------------
#pragma warning(disable:4146)
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF","adoEOF"), rename("BOF","adoBOF") rename(L"LockTypeEnum", "adoLockTypeEnum") 
#import "C:\Program Files (x86)\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF","adoEOF"), rename("BOF","adoBOF") rename(L"LockTypeEnum", "adoLockTypeEnum") 
#pragma warning(default:4146)

typedef struct tagSteelDefectInfo
{
	long iDefectNo;
	long iSteelNo;
	int iCameraNo;
	int iImageIndex;
	int iClass;
	int iPerClass;
	int iGrade;
	int iLeftInImg;
	int iRightInImg;
	int iTopInImg;
	int iBottomInImg;
	int iArea;
}SteelDefectInfo,*PSteelDefectInfo;
typedef struct tagSteelDefectInfoSet
{
	enum {MAX_ITEMNUM = 10240};
	SteelDefectInfo Items[MAX_ITEMNUM];
	int iItemNum;
}SteelDefectInfoSet,*PSteelDefectInfoSet;

class HDBOper
{
public:
	HDBOper(void);
	~HDBOper(void);

	_ConnectionPtr			m_pConnection;
	_CommandPtr				m_pCommand;
	_RecordsetPtr			m_pSearchSet;

	bool					m_bConnected;
	
	bool   HDBOper::ConnectDB(CString strDBServer, CString strDBName, CString strUser, CString strPassWd, CString strType);
	bool   HDBOper::SearchDefect(SteelDefectInfoSet& DefectSet,long& iDefectMaxIndex,CString strSQLCondition);
	bool   HDBOper::GetDefectMaxNum(long& iDefectNoIndex,CString strSQLCondition);
	bool   HDBOper::UpdateDefectClass(CString strSQLCondition);
};
