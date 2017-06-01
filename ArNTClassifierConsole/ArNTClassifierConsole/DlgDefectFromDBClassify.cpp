// DlgDefectFromDBClassify.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgDefectFromDBClassify.h"
#include "ArNTClassifierConsoleDlg.h"
#include "ArNTInteractionFunc.h"

HDBOper					g_hDBOper[16];				//数据库操作
SteelDefectInfoSet		g_tDefectSet[16];			//缺陷查询集
long					g_iDefectMaxIndex[16];		//最大缺陷索引
ConfigInfo_ClassifyRelation			g_ConfigInfo_ClassifyRelation;			//分类配置信息
ClassifierObject					g_ClassifierObject;						//分类器对象
HFile_log				m_LogOper;

bool GetDefectsAndClassify(HDBOper& hDBOper,CString strDefectsDBIP,CString strDBName,CString strDefectsImgIP,int iCamIndex,ConfigInfo_ClassifyRelation tConfigInfo_ClassifyRelation,HWND hWnd,int msgID)
{
	bool bIsAllSuccess=true;
	//读数据库
	bool bIsConn=ReadDefectRecordFromDB(hDBOper,strDefectsDBIP,strDBName,L"SqlSever",L"",g_iDefectMaxIndex[iCamIndex],g_tDefectSet[iCamIndex]);
	if(!bIsConn)
	{
		::PostMessage(hWnd, WM_MSG_DEFECTIMG_OPENFAIL, 1, 0);///应该加互斥
	}
	//显示列表
	::PostMessage(hWnd, msgID, 0, 0);
	//循环读取并分类
	for(int i=0;i<g_tDefectSet[iCamIndex].iItemNum;i++)//对每条缺陷记录进行处理
	{
		SteelDefectInfoSet& tDefectSet=g_tDefectSet[iCamIndex];
		//按缺陷号分开处理
		if(tDefectSet.Items[i].iDefectNo%CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectInteral!=CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectOffset)
		{
			continue;
		}
		//获取图像
		cv::Mat srcImg;
		bool bIsSuccess=GetDefectImg(strDefectsImgIP,iCamIndex,tDefectSet.Items[i].iSteelNo,tDefectSet.Items[i].iImageIndex,CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgWidth,CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgHeight,srcImg);
		if(bIsSuccess)
		{
			//切割图像
			cv::Rect rect(tDefectSet.Items[i].iLeftInImg,tDefectSet.Items[i].iTopInImg,abs(tDefectSet.Items[i].iRightInImg-tDefectSet.Items[i].iLeftInImg),abs(tDefectSet.Items[i].iBottomInImg-tDefectSet.Items[i].iTopInImg));
			cv::Mat srcImg_ROI;
			GetDefectImgROI(srcImg,rect,srcImg_ROI);
			//进入分类
			int iPerClass=DefectImgClassify(srcImg_ROI,tConfigInfo_ClassifyRelation,g_ClassifierObject);
			//类别内外转换
			int iInterClass=tConfigInfo_ClassifyRelation.iClassExterToInter[iPerClass];
			int iFinalClass=0;
			for(int j=0;j<CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.iItemNum;j++)
			{
				if(iInterClass==CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[j].iInternalClassNo)
				{
					iFinalClass=CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[j].iExternalClassNo;
				}
			}
			//分类后写入数据库
			WriteDefectClassToDB(hDBOper,iFinalClass,tDefectSet.Items[i].iDefectNo);
			//更新列表
			tDefectSet.Items[i].iPerClass=iFinalClass;
			::PostMessage(hWnd, msgID, i+1, 0);
		}
		else
		{
			//文件打开失败
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
		
		//数据库名
		CString strDBName=L"";
		strDBName.Format(L"ClientDefectDB%d",pThreadParam->index);
		//多个缺陷图像服务器时
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
		//处理函数
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
		//创建SingleLock对象
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
					strLogInfo.Format(L"图像文件打开失败！");
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
					strLogInfo.Format(L"%d号相机已完成%d条缺陷记录的分类！目前处理到的最大缺陷号为%d。",pThreadParam->index,pThreadParam->nData,g_iDefectMaxIndex[pThreadParam->index]);
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
// DlgDefectFromDBClassify 对话框

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
			//等待线程结束
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


// DlgDefectFromDBClassify 消息处理程序
BOOL DlgDefectFromDBClassify::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
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

	CRect rectStateDBDataInfo;   // 标签控件客户区的位置和大小 
    //GetDlgItem(IDC_STATIC_DBDATAINFO)->GetClientRect(&rectStateDBDataInfo);    // 获取标签控件客户区Rect  
	GetDlgItem(IDC_STATIC_DBDATAINFO)->GetWindowRect(&rectStateDBDataInfo);    // 获取标签控件客户区Rect 
	
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
			m_listDBData[i]->InsertColumn(0, _T("缺陷号"), LVCFMT_CENTER, rect.Width()/5, 0);   
			m_listDBData[i]->InsertColumn(1, _T("钢板号"), LVCFMT_CENTER, rect.Width()/5, 1);   
			m_listDBData[i]->InsertColumn(2, _T("相机号"), LVCFMT_CENTER, rect.Width()/5, 2); 
			m_listDBData[i]->InsertColumn(3, _T("原类别"), LVCFMT_CENTER, rect.Width()/5, 3); 
			m_listDBData[i]->InsertColumn(4, _T("识别类别"), LVCFMT_CENTER, rect.Width()/5, 4); 
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
	

	//检测配置文件是否已载入成功
	m_hDetectConfig.m_ThreadParam.hWnd=m_hWnd;
	m_hDetectConfig.m_ThreadParam.nData=0;
	m_hDetectConfig.m_ThreadParam.index=100;
	m_hDetectConfig.m_ThreadParam.bExit=false;
	m_hDetectConfig.m_ThreadParam.pMutex = new CMutex();
	m_hDetectConfig.BeginThread(ThreadProc);
	m_hDetectConfig.ResumeThread();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgDefectFromDBClassify::OnBnClickedButtonConnectDb()
{
	// TODO: 在此添加控件通知处理程序代码
	InitInfo();
	//载入分类器
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
	//获取应用程序路径
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	CString strFileName = szFileName;
	int nIndex = strFileName.ReverseFind('\\');
	m_strAppPath = strFileName.Left(nIndex);
	//
	//显示信息
	CString strInfo=L"";
	strInfo.Format(L"缺陷数据库位置：%s",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectDBIP);
	m_listBoxShowLoadInfo.AddString(strInfo);
	for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iDefectImgIP;i++)
	{
		strInfo.Format(L"缺陷图像位置：%s",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.strDefectImgIP[i]);
		m_listBoxShowLoadInfo.AddString(strInfo);
	}
	strInfo.Format(L"检测相机数量：%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iCameraNum);
	m_listBoxShowLoadInfo.AddString(strInfo);
	strInfo.Format(L"相机图像宽度：%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgWidth);
	m_listBoxShowLoadInfo.AddString(strInfo);
	strInfo.Format(L"相机图像高度：%d",CArNTClassifierConsoleDlg::m_ConfigInfo_Local.iImgHeight);
	m_listBoxShowLoadInfo.AddString(strInfo);

	//得到ini配置文件路径
	m_FileIniOper.m_strFileName.Format(L"%s\\ClassifierInteraction.ini",m_strAppPath);
	CString strValue=L"";

	m_FileIniOper.Read(L"ImgSize",L"Norm",strValue);
	g_ConfigInfo_ClassifyRelation.bIsNorm=_ttoi(strValue);
	if(true==g_ConfigInfo_ClassifyRelation.bIsNorm)
	{
		strInfo.Format(L"分类图像是否规范化：是");
	}
	else
	{
		strInfo.Format(L"分类图像是否规范化：否");
	}
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"ImgSize",L"Width",strValue);
	g_ConfigInfo_ClassifyRelation.iImgNormWidth=_ttoi(strValue);
	strInfo.Format(L"图像规范化宽度：%d",g_ConfigInfo_ClassifyRelation.iImgNormWidth);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"ImgSize",L"Height",strValue);
	g_ConfigInfo_ClassifyRelation.iImgNormHeight=_ttoi(strValue);
	strInfo.Format(L"图像规范化高度：%d",g_ConfigInfo_ClassifyRelation.iImgNormHeight);
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
			strInfo.Format(L"选取特征：%s",strValue);
			m_listBoxShowLoadInfo.AddString(strInfo);
		}
	}
	m_FileIniOper.Read(L"Classifier",L"ClassNum",strValue);
	g_ConfigInfo_ClassifyRelation.iClassNum=_ttoi(strValue);
	strInfo.Format(L"类别数量：%d",g_ConfigInfo_ClassifyRelation.iClassNum);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"Classifier",L"ClassifierName",strValue);
	iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
	WideCharToMultiByte(CP_ACP, 0, strValue, -1, g_ConfigInfo_ClassifyRelation.strClassifierName, iLength, NULL, NULL);
	strInfo.Format(L"分类器名称：%s",strValue);
	m_listBoxShowLoadInfo.AddString(strInfo);

	m_FileIniOper.Read(L"Classifier",L"ClassifierFileName",strValue);
	iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
	WideCharToMultiByte(CP_ACP, 0, strValue, -1, g_ConfigInfo_ClassifyRelation.strClassifierFileName, iLength, NULL, NULL);
	strInfo.Format(L"分类器文件：%s",strValue);
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

	//提示信息
	m_listBoxShowLoadInfo.AddString(L"分类相应设置文件加载成功！");
}
//写缺陷信息到列表
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
//更新类别信息
void DlgDefectFromDBClassify::UpdateDefectInfoToList(int iCamNo,int iRow)
{
	CString strValue;
	strValue.Format(L"%d",g_tDefectSet[iCamNo].Items[iRow].iPerClass);
	m_listDBData[iCamNo]->SetItemText(iRow, 4, strValue); 
}
//8个相机的消息处理函数
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
		
		m_listShowWarnInfo.AddString(L"文件打开失败！");
	}
	else if(iJudge==1)
	{
		m_listShowWarnInfo.AddString(L"数据库连接失败！");
	}
	return 1;
}