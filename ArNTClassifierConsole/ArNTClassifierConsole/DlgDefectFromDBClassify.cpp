// DlgDefectFromDBClassify.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgDefectFromDBClassify.h"
#include "ArNTClassifierConsoleDlg.h"
#include "ArNTInteractionFunc.h"

HDBOper					g_hDBOper[16];				//���ݿ����
SteelDefectInfoSet		g_tDefectSet[16];			//ȱ�ݲ�ѯ��
long					g_iDefectMaxIndex[16];		//���ȱ������
ConfigInfo_ClassifyRelation			g_ConfigInfo_ClassifyRelation;			//����������Ϣ
ClassifierObject					g_ClassifierObject;						//����������
HFile_log				m_LogOper;

bool GetDefectsAndClassify(HDBOper& hDBOper,CString strDefectsDBIP,CString strDBName,CString strDefectsImgIP,int iCamIndex,ConfigInfo_ClassifyRelation tConfigInfo_ClassifyRelation,HWND hWnd,int msgID)
{
	bool bIsAllSuccess=true;
	//�����ݿ�
	bool bIsConn=ReadDefectRecordFromDB(hDBOper,strDefectsDBIP,strDBName,L"SqlSever",L"",g_iDefectMaxIndex[iCamIndex],g_tDefectSet[iCamIndex]);
	if(!bIsConn)
	{
		::PostMessage(hWnd, WM_MSG_DEFECTIMG_OPENFAIL, 1, 0);///Ӧ�üӻ���
	}
	//��ʾ�б�
	::PostMessage(hWnd, msgID, 0, 0);
	//ѭ����ȡ������
	for(int i=0;i<g_tDefectSet[iCamIndex].iItemNum;i++)//��ÿ��ȱ�ݼ�¼���д���
	{
		SteelDefectInfoSet& tDefectSet=g_tDefectSet[iCamIndex];
		//��ȱ�ݺŷֿ�����
		if(tDefectSet.Items[i].iDefectNo%CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectInteral!=CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectOffset)
		{
			continue;
		}
		//��ȡͼ��
		cv::Mat srcImg;
		bool bIsSuccess=GetDefectImg(strDefectsImgIP,iCamIndex,tDefectSet.Items[i].iSteelNo,tDefectSet.Items[i].iImageIndex,CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgWidth,CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgHeight,srcImg);
		if(bIsSuccess)
		{
			//�и�ͼ��
			cv::Rect rect(tDefectSet.Items[i].iLeftInImg,tDefectSet.Items[i].iTopInImg,abs(tDefectSet.Items[i].iRightInImg-tDefectSet.Items[i].iLeftInImg),abs(tDefectSet.Items[i].iBottomInImg-tDefectSet.Items[i].iTopInImg));
			cv::Mat srcImg_ROI;
			GetDefectImgROI(srcImg,rect,srcImg_ROI);
			//�������
			int iPerClass=DefectImgClassify(srcImg_ROI,tConfigInfo_ClassifyRelation,g_ClassifierObject);
			//�������ת��
			int iInterClass=tConfigInfo_ClassifyRelation.iClassExterToInter[iPerClass];
			int iFinalClass=0;
			for(int j=0;j<CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.iItemNum;j++)
			{
				if(iInterClass==CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[j].iInternalClassNo)
				{
					iFinalClass=CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[j].iExternalClassNo;
				}
			}
			//�����д�����ݿ�
			WriteDefectClassToDB(hDBOper,iFinalClass,tDefectSet.Items[i].iDefectNo);
			//�����б�
			tDefectSet.Items[i].iPerClass=iFinalClass;
			::PostMessage(hWnd, msgID, i+1, 0);
		}
		else
		{
			//�ļ���ʧ��
			//::PostMessage(hWnd, WM_MSG_DEFECTIMG_OPENFAIL, 0, 0);
			bIsAllSuccess=false;
		}
	}
	return bIsAllSuccess;
}

UINT ThreadProc(LPVOID pParam)
{
	ThreadParam* pThreadParam = (ThreadParam*)pParam;
	while (!pThreadParam->bExit)
	{
		
		//���ݿ���
		CString strDBName=L"";
		strDBName.Format(L"ClientDefectDB%d",pThreadParam->index);
		//���ȱ��ͼ�������ʱ
		CString strDefectImgIP;
		for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectImgIP;i++)
		{
			CString strImgPath;
			strImgPath.Format(L"\\\\%s\\CamDefectImage%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectImgIP[i],pThreadParam->index);
			int IsExist=PathFileExists(strImgPath);
			if(IsExist)
			{
				strDefectImgIP=CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectImgIP[i];
			}
		}
		//������
		bool bIsAllSuccessClassifiy=true;
		switch(pThreadParam->index)
		{
		case 1:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG1);
			break;
		case 2:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG2);
			break;
		case 3:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG3);
			break;
		case 4:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG4);
			break;
		case 5:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG5);
			break;
		case 6:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG6);
			break;
		case 7:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG7);
			break;
		case 8:
			bIsAllSuccessClassifiy=GetDefectsAndClassify(g_hDBOper[pThreadParam->index],CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP,strDBName,strDefectImgIP,pThreadParam->index,g_ConfigInfo_ClassifyRelation,pThreadParam->hWnd, WM_THREADMSG8);
			break;
		case 100:
			if(CArNTClassifierConsoleDlg::m_ConfigInfo_Local.bIsEnd==true)
			{
				if(true==CArNTClassifierConsoleDlg::m_ConfigInfo_Local.bAutoClassify)
				{
					::PostMessage(pThreadParam->hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CONNECT_DB, BN_CLICKED), 0);
				}
				pThreadParam->bExit=true;
			}
			break;
		default:break;
		}		
		Sleep(500);
		//����SingleLock����
		if(pThreadParam->index != 100)
		{
			pThreadParam->nData+=g_tDefectSet[pThreadParam->index].iItemNum;
			if(!bIsAllSuccessClassifiy)
			{
				CSingleLock SingleLock(pThreadParam->pMutex);
				SingleLock.Lock();
				if (SingleLock.IsLocked())
				{
					::PostMessage(pThreadParam->hWnd, WM_MSG_DEFECTIMG_OPENFAIL, 0, 0);
					CString strLogInfo;
					strLogInfo.Format(L"ͼ���ļ���ʧ�ܣ�");
					m_LogOper.AddLogInfo(strLogInfo);
				}
				SingleLock.Unlock();
			}
			if(pThreadParam->nData>=100)
			{
				CSingleLock SingleLock(pThreadParam->pMutex);
				SingleLock.Lock();
				if (SingleLock.IsLocked())
				{
					CString strLogInfo;
					strLogInfo.Format(L"%d����������%d��ȱ�ݼ�¼�ķ��࣡Ŀǰ���������ȱ�ݺ�Ϊ%d��",pThreadParam->index,pThreadParam->nData,g_iDefectMaxIndex[pThreadParam->index]);
					m_LogOper.AddLogInfo(strLogInfo);
					pThreadParam->nData=0;
				}
				SingleLock.Unlock();
			}
		}
		//
	}
	return 0;
}
// DlgDefectFromDBClassify �Ի���

IMPLEMENT_DYNAMIC(DlgDefectFromDBClassify, CDialog)

DlgDefectFromDBClassify::DlgDefectFromDBClassify(CWnd* pParent /*=NULL*/)
	: CDialog(DlgDefectFromDBClassify::IDD, pParent)
{

}

DlgDefectFromDBClassify::~DlgDefectFromDBClassify()
{
	for(int i=1;i<=CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum;i++)
	{
		if (m_hThread[i].pThread != NULL)
		{
			//�ȴ��߳̽���
			::WaitForSingleObject(m_hThread[i].pThread, INFINITE);
			delete m_hThread[i].pThread;
			m_hThread[i].pThread = NULL;
		}
		if (m_hThread[i].m_ThreadParam.pMutex != NULL)
		{
			delete m_hThread[i].m_ThreadParam.pMutex;
			m_hThread[i].m_ThreadParam.pMutex = NULL;
		}
	}	
}

void DlgDefectFromDBClassify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEFECT_DB_IP, m_ComboDefectDBIP);
	DDX_Control(pDX, IDC_COMBO_DEFECT_IMG_IP, m_ComboDefectImgIP);
	DDX_Control(pDX, IDC_COMBO_DB_TYPE, m_ComboDBType);
	DDX_Control(pDX, IDC_LIST_SHOW_LOAD_INFO, m_listBoxShowLoadInfo);
	DDX_Control(pDX, IDC_LIST_DBDATA_1, m_listDBData1);
	DDX_Control(pDX, IDC_LIST_DBDATA_2, m_listDBData2);
	DDX_Control(pDX, IDC_LIST_DBDATA_3, m_listDBData3);
	DDX_Control(pDX, IDC_LIST_DBDATA_4, m_listDBData4);
	DDX_Control(pDX, IDC_LIST_DBDATA_5, m_listDBData5);
	DDX_Control(pDX, IDC_LIST_DBDATA_6, m_listDBData6);
	DDX_Control(pDX, IDC_LIST_DBDATA_7, m_listDBData7);
	DDX_Control(pDX, IDC_LIST_DBDATA_8, m_listDBData8);
	DDX_Control(pDX, IDC_LIST_SHOW_WARN_INFO, m_listShowWarnInfo);
}


BEGIN_MESSAGE_MAP(DlgDefectFromDBClassify, CDialog)
	ON_MESSAGE(WM_THREADMSG1, OnMsgFunc1)
	ON_MESSAGE(WM_THREADMSG2, OnMsgFunc2)
	ON_MESSAGE(WM_THREADMSG3, OnMsgFunc3)
	ON_MESSAGE(WM_THREADMSG4, OnMsgFunc4)
	ON_MESSAGE(WM_THREADMSG5, OnMsgFunc5)
	ON_MESSAGE(WM_THREADMSG6, OnMsgFunc6)
	ON_MESSAGE(WM_THREADMSG7, OnMsgFunc7)
	ON_MESSAGE(WM_THREADMSG8, OnMsgFunc8)
	ON_MESSAGE(WM_MSG_DEFECTIMG_OPENFAIL, OnMsgDefectImgOpenFail)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_DB, &DlgDefectFromDBClassify::OnBnClickedButtonConnectDb)
END_MESSAGE_MAP()


// DlgDefectFromDBClassify ��Ϣ�������
BOOL DlgDefectFromDBClassify::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ComboDefectDBIP.InsertString(0,L"192.168.0.100");
	m_ComboDefectDBIP.InsertString(1,L"192.168.0.1");
	m_ComboDefectDBIP.InsertString(2,L"192.168.0.101");
	m_ComboDefectDBIP.InsertString(3,L"192.168.1.100");
	m_ComboDefectDBIP.InsertString(4,L"192.169.0.100");
	m_ComboDefectDBIP.SetCurSel(0);
	//
	m_ComboDefectImgIP.InsertString(0,L"192.168.0.100");
	m_ComboDefectImgIP.InsertString(1,L"192.168.0.101");
	m_ComboDefectImgIP.InsertString(2,L"192.168.0.102");
	m_ComboDefectImgIP.InsertString(3,L"192.168.1.101");
	m_ComboDefectImgIP.InsertString(4,L"192.169.0.100");
	m_ComboDefectImgIP.SetCurSel(1);
	//
	m_ComboDBType.InsertString(0,L"Access");
	m_ComboDBType.InsertString(1,L"SqlSever");
	m_ComboDBType.InsertString(2,L"Oracle");
	m_ComboDBType.InsertString(3,L"Mysql");
	m_ComboDBType.SetCurSel(1);

	CRect rectStateDBDataInfo;   // ��ǩ�ؼ��ͻ�����λ�úʹ�С 
    //GetDlgItem(IDC_STATIC_DBDATAINFO)->GetClientRect(&rectStateDBDataInfo);    // ��ȡ��ǩ�ؼ��ͻ���Rect  
	GetDlgItem(IDC_STATIC_DBDATAINFO)->GetWindowRect(&rectStateDBDataInfo);    // ��ȡ��ǩ�ؼ��ͻ���Rect 
	
	m_listDBData[1]=&m_listDBData1;
	m_listDBData[2]=&m_listDBData2;
	m_listDBData[3]=&m_listDBData3;
	m_listDBData[4]=&m_listDBData4;
	m_listDBData[5]=&m_listDBData5;
	m_listDBData[6]=&m_listDBData6;
	m_listDBData[7]=&m_listDBData7;
	m_listDBData[8]=&m_listDBData8;
	for(int i=1;i<=8;i++)
	{
		int left_edge=-10;
		int right_edge=20;
		int interval=10;
		int top_head=-10;
		int bottom_head=30;
		int width_mean=(rectStateDBDataInfo.Width()-left_edge-right_edge-interval)/2;
		int height_mean=(rectStateDBDataInfo.Height()-top_head-bottom_head-interval*(CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum/2))/(CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum/2);
		
		if(i<=CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum)
		{
			m_listDBData[i]->ShowWindow(true);
			if(i%2==1)
			{				
				int L_begin=left_edge;
				int T_begin=top_head+(i/2)*(height_mean+interval);
				m_listDBData[i]->MoveWindow(rectStateDBDataInfo.left+L_begin,rectStateDBDataInfo.top+T_begin,width_mean,height_mean);
			}
			else
			{
				int L_begin=left_edge+width_mean+interval;
				int T_begin=top_head+((i-1)/2)*(height_mean+interval);
				m_listDBData[i]->MoveWindow(rectStateDBDataInfo.left+L_begin,rectStateDBDataInfo.top+T_begin,width_mean,height_mean);
			}
			CRect rect;    
			m_listDBData[i]->GetClientRect(&rect);  
			m_listDBData[i]->InsertColumn(0, _T("ȱ�ݺ�"), LVCFMT_CENTER, rect.Width()/5, 0);   
			m_listDBData[i]->InsertColumn(1, _T("�ְ��"), LVCFMT_CENTER, rect.Width()/5, 1);   
			m_listDBData[i]->InsertColumn(2, _T("�����"), LVCFMT_CENTER, rect.Width()/5, 2); 
			m_listDBData[i]->InsertColumn(3, _T("ԭ���"), LVCFMT_CENTER, rect.Width()/5, 3); 
			m_listDBData[i]->InsertColumn(4, _T("ʶ�����"), LVCFMT_CENTER, rect.Width()/5, 4); 
		}
		else
		{
			m_listDBData[i]->ShowWindow(false);
		}
	}
	//
	TCHAR szAppName[MAX_PATH];
	::GetModuleFileName(NULL, szAppName, MAX_PATH);
	CString strAppName = szAppName;
	int nIndex = strAppName.ReverseFind('\\');
	CString strAppPath = strAppName.Left(nIndex);
	m_LogOper.SetLogFilePath(strAppPath);
	

	//��������ļ��Ƿ�������ɹ�
	m_hDetectConfig.m_ThreadParam.hWnd=m_hWnd;
	m_hDetectConfig.m_ThreadParam.nData=0;
	m_hDetectConfig.m_ThreadParam.index=100;
	m_hDetectConfig.m_ThreadParam.bExit=false;
	m_hDetectConfig.m_ThreadParam.pMutex = new CMutex();
	m_hDetectConfig.BeginThread(ThreadProc);
	m_hDetectConfig.ResumeThread();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void DlgDefectFromDBClassify::OnBnClickedButtonConnectDb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InitInfo();
	//���������
	DefectImgClassify_Init(g_ConfigInfo_ClassifyRelation,g_ClassifierObject);
	//
	CString strCameraList;
	strCameraList.Format(L"%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectCameraList);
	for(int i=1;i<=CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum;i++)
	{
		CString strCamNo;
		strCamNo.Format(L"%d",i);
		int iIndex=strCameraList.Find(strCamNo);
		if(-1 != iIndex)
		{	
			m_hThread[i].m_ThreadParam.hWnd=m_hWnd;
			m_hThread[i].m_ThreadParam.nData=0;
			m_hThread[i].m_ThreadParam.index=i;
			m_hThread[i].m_ThreadParam.bExit=false;
			m_hThread[i].m_ThreadParam.pMutex = new CMutex();
			m_hThread[i].BeginThread(ThreadProc);
			m_hThread[i].ResumeThread();
		}
	}
	m_hDetectConfig.EndThread();
	GetDlgItem(IDC_BUTTON_CONNECT_DB)->EnableWindow(false);
}

void DlgDefectFromDBClassify::InitInfo()
{
	int iLength;
	//��ȡӦ�ó���·��
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	CString strFileName = szFileName;
	int nIndex = strFileName.ReverseFind('\\');
	m_strAppPath = strFileName.Left(nIndex);
	//
	//��ʾ��Ϣ
	CString strInfo=L"";
	strInfo.Format(L"ȱ�����ݿ�λ�ã�%s",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP);
	m_listBoxShowLoadInfo.AddString(strInfo);
	for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectImgIP;i++)
	{
		strInfo.Format(L"ȱ��ͼ��λ�ã�%s",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectImgIP[i]);
		m_listBoxShowLoadInfo.AddString(strInfo);
	}
	strInfo.Format(L"������������%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum);
	m_listBoxShowLoadInfo.AddString(strInfo);
	strInfo.Format(L"���ͼ���ȣ�%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgWidth);
	m_listBoxShowLoadInfo.AddString(strInfo);
	strInfo.Format(L"���ͼ��߶ȣ�%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgHeight);
	m_listBoxShowLoadInfo.AddString(strInfo);

	//�õ�ini�����ļ�·��
	m_FileIniOper.m_strFileName.Format(L"%s\\ClassifierInteraction.ini",m_strAppPath);
	CString strValue=L"";

	m_FileIniOper.Read(L"ImgSize",L"Norm",strValue);
	g_ConfigInfo_ClassifyRelation.bIsNorm=_ttoi(strValue);
	if(true==g_ConfigInfo_ClassifyRelation.bIsNorm)
	{
		strInfo.Format(L"����ͼ���Ƿ�淶������");
	}
	else
	{
		strInfo.Format(L"����ͼ���Ƿ�淶������");
	}
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"ImgSize",L"Width",strValue);
	g_ConfigInfo_ClassifyRelation.iImgNormWidth=_ttoi(strValue);
	strInfo.Format(L"ͼ��淶����ȣ�%d",g_ConfigInfo_ClassifyRelation.iImgNormWidth);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"ImgSize",L"Height",strValue);
	g_ConfigInfo_ClassifyRelation.iImgNormHeight=_ttoi(strValue);
	strInfo.Format(L"ͼ��淶���߶ȣ�%d",g_ConfigInfo_ClassifyRelation.iImgNormHeight);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"Feats",L"FeatsNum",strValue);
	g_ConfigInfo_ClassifyRelation.iFeatsNum=_ttoi(strValue);
	for(int i=0;i<g_ConfigInfo_ClassifyRelation.iFeatsNum;i++)
	{
		CString strKey=L"";
		strKey.Format(L"Feat%d",i);
		if(0==m_FileIniOper.Read(L"Feats",strKey,strValue))
		{
			break;
		}
		else
		{
			iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
			WideCharToMultiByte(CP_ACP, 0, strValue, -1, g_ConfigInfo_ClassifyRelation.strFeatName[i], iLength, NULL, NULL);
			strInfo.Format(L"ѡȡ������%s",strValue);
			m_listBoxShowLoadInfo.AddString(strInfo);
		}
	}
	m_FileIniOper.Read(L"Classifier",L"ClassNum",strValue);
	g_ConfigInfo_ClassifyRelation.iClassNum=_ttoi(strValue);
	strInfo.Format(L"���������%d",g_ConfigInfo_ClassifyRelation.iClassNum);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"Classifier",L"ClassifierName",strValue);
	iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
	WideCharToMultiByte(CP_ACP, 0, strValue, -1, g_ConfigInfo_ClassifyRelation.strClassifierName, iLength, NULL, NULL);
	strInfo.Format(L"���������ƣ�%s",strValue);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"Classifier",L"ClassifierFileName",strValue);
	iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
	WideCharToMultiByte(CP_ACP, 0, strValue, -1, g_ConfigInfo_ClassifyRelation.strClassifierFileName, iLength, NULL, NULL);
	strInfo.Format(L"�������ļ���%s",strValue);
	m_listBoxShowLoadInfo.AddString(strInfo);

	for(int i=0;i<g_ConfigInfo_ClassifyRelation.iClassNum;i++)
	{
		CString strKey=L"";
		strKey.Format(L"Class%d",i);
		m_FileIniOper.Read(L"ClassConversion",strKey,strValue);
		g_ConfigInfo_ClassifyRelation.iClassExterToInter[i]=_ttoi(strValue);
		strInfo.Format(L"Class%d: %d",i,g_ConfigInfo_ClassifyRelation.iClassExterToInter[i]);
		m_listBoxShowLoadInfo.AddString(strInfo);
	}

	//��ʾ��Ϣ
	m_listBoxShowLoadInfo.AddString(L"������Ӧ�����ļ����سɹ���");
}
//дȱ����Ϣ���б�
void DlgDefectFromDBClassify::WriteDefectInfoToList(int iCamNo)
{
	m_listDBData[iCamNo]->DeleteAllItems();
	for(int i=0;i<g_tDefectSet[iCamNo].iItemNum;i++)
	{
		CString strValue;
		strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[i].iDefectNo);
		m_listDBData[iCamNo]->InsertItem(i, strValue);
		strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[i].iSteelNo);
		m_listDBData[iCamNo]->SetItemText(i, 1, strValue); 
		strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[i].iCameraNo);
		m_listDBData[iCamNo]->SetItemText(i, 2, strValue); 
		strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[i].iClass);
		m_listDBData[iCamNo]->SetItemText(i, 3, strValue); 
		strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[i].iPerClass);
		m_listDBData[iCamNo]->SetItemText(i, 4, strValue); 
	}
}
//���������Ϣ
void DlgDefectFromDBClassify::UpdateDefectInfoToList(int iCamNo,int iRow)
{
	CString strValue;
	strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[iRow].iPerClass);
	m_listDBData[iCamNo]->SetItemText(iRow, 4, strValue); 
}
//8���������Ϣ������
LRESULT DlgDefectFromDBClassify::OnMsgFunc1(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(1);
	}
	else
	{
		UpdateDefectInfoToList(1,iJudge-1);
	}
	return 1;
}

LRESULT DlgDefectFromDBClassify::OnMsgFunc2(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(2);
	}
	else
	{
		UpdateDefectInfoToList(2,iJudge-1);
	}
	return 1;
}
LRESULT DlgDefectFromDBClassify::OnMsgFunc3(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(3);
	}
	else
	{
		UpdateDefectInfoToList(3,iJudge-1);
	}
	return 1;
}

LRESULT DlgDefectFromDBClassify::OnMsgFunc4(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(4);
	}
	else
	{
		UpdateDefectInfoToList(4,iJudge-1);
	}
	return 1;
}
LRESULT DlgDefectFromDBClassify::OnMsgFunc5(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(5);
	}
	else
	{
		UpdateDefectInfoToList(5,iJudge-1);
	}
	return 1;
}

LRESULT DlgDefectFromDBClassify::OnMsgFunc6(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(6);
	}
	else
	{
		UpdateDefectInfoToList(6,iJudge-1);
	}
	return 1;
}
LRESULT DlgDefectFromDBClassify::OnMsgFunc7(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(7);
	}
	else
	{
		UpdateDefectInfoToList(7,iJudge-1);
	}
	return 1;
}

LRESULT DlgDefectFromDBClassify::OnMsgFunc8(WPARAM wParam, LPARAM lParam)
{
	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		WriteDefectInfoToList(8);
	}
	else
	{
		UpdateDefectInfoToList(8,iJudge-1);
	}
	return 1;
}

LRESULT DlgDefectFromDBClassify::OnMsgDefectImgOpenFail(WPARAM wParam, LPARAM lParam)
{
	if(m_listShowWarnInfo.GetCount()>100)
	{
		for(int i=0;i<m_listShowWarnInfo.GetCount();i++)
		{
			m_listShowWarnInfo.DeleteString(i);
		}
	}

	int iJudge=(int)wParam;
	if(iJudge==0)
	{
		
		m_listShowWarnInfo.AddString(L"�ļ���ʧ�ܣ�");
	}
	else if(iJudge==1)
	{
		m_listShowWarnInfo.AddString(L"���ݿ�����ʧ�ܣ�");
	}
	return 1;
}