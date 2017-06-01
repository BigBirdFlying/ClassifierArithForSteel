// ArNTClassifierConsoleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "ArNTClassifierConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ConfigInfo_Local CArNTClassifierConsoleDlg::m_ConfigInfo_Local={0};
ConfigInfo_ClassConvertSet CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet={0};
ConfigInfo_Segmentation	CArNTClassifierConsoleDlg::m_ConfigInfo_Segmentation={0};
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CArNTClassifierConsoleDlg �Ի���




CArNTClassifierConsoleDlg::CArNTClassifierConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArNTClassifierConsoleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CArNTClassifierConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_MainTab);
}

BEGIN_MESSAGE_MAP(CArNTClassifierConsoleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CArNTClassifierConsoleDlg::OnTcnSelchangeTabMain)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CArNTClassifierConsoleDlg ��Ϣ�������

BOOL CArNTClassifierConsoleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������ 
	CRect mainTabRect;   // ��ǩ�ؼ��ͻ�����λ�úʹ�С 
	m_MainTab.GetClientRect(&mainTabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   
	mainTabRect.left += 1;                  
	mainTabRect.right -= 1;   
	mainTabRect.top += 22;   
	mainTabRect.bottom -= 1;
	//
	InitLoadConfig();
	//
	
	//ͨ�������к������ļ���ʽȷ���ֶ��Զ�ģʽ
	if(m_ConfigInfo_Local.bAutoClassify==true)
	{
		m_DlgDefectClassify.Create(IDD_DIALOG_DEFECTCLASS, &m_MainTab);	
		m_MainTab.InsertItem(0, _T("ȱ�����߷���"));  
		m_DlgDefectClassify.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_SHOWWINDOW); 
	}
	else
	{
		CString strCommandLine=::GetCommandLine();
		CString strCommand=L"-Mod";
		int iPosCommand=strCommandLine.Find(strCommand);
		if(iPosCommand>0)
		{
			CString strCommandOffset=L"";
			CString strCommandValue=L"";
			strCommandOffset = strCommandLine.Right(strCommandLine.GetLength()-iPosCommand-strCommand.GetLength()-1);
			int iPosCommandCut=strCommandOffset.Find(L"-");
			if(iPosCommandCut>0)
			{
				strCommandValue = strCommandOffset.Left(iPosCommandCut-1);
			}
			else
			{
				strCommandValue = strCommandLine.Right(strCommandLine.GetLength()-iPosCommand-strCommand.GetLength()-1);
			}

			int a=strCommandValue.Compare(L"collect");
			int b=strCommandValue.Compare(L"train");
			int c=strCommandValue.Compare(L"classify");
			if(0==strCommandValue.Compare(L"collect"))
			{
				m_DlgSampleCollection.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_SHOWWINDOW);   
				m_DlgClassifier.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW);  
				m_DlgDefectClassify.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW); 
			}
			else if(0==strCommandValue.Compare(L"train"))
			{
				m_DlgSampleCollection.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW);   
				m_DlgClassifier.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_SHOWWINDOW);  
				m_DlgDefectClassify.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW); 
			}
			else if(0==strCommandValue.Compare(L"classify"))
			{
				m_DlgDefectClassify.Create(IDD_DIALOG_DEFECTCLASS, &m_MainTab);
				
				m_MainTab.InsertItem(0, _T("ȱ�����߷���"));  
				m_DlgDefectClassify.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_SHOWWINDOW); 
			}
		}
		else
		{
			m_MainTab.InsertItem(0, _T("ȱ�������ɼ�"));  
			m_MainTab.InsertItem(1, _T("���������ѵ��")); 
			m_MainTab.InsertItem(2, _T("ȱ�����߷���")); 
			m_MainTab.InsertItem(3, _T("ϵͳ���߰���")); 

			m_DlgSampleCollection.Create(IDD_DIALOG_SAMPLE_COLLECTION, &m_MainTab);
			m_DlgClassifier.Create(IDD_DIALOG_CLASSIFIER, &m_MainTab); 
			m_DlgDefectClassify.Create(IDD_DIALOG_DEFECTCLASS, &m_MainTab);
			m_DlgHelp.Create(IDD_DIALOG_HELP, &m_MainTab);
	 
			m_DlgSampleCollection.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_SHOWWINDOW);   
			m_DlgClassifier.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW);  
			m_DlgDefectClassify.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW); 
			m_DlgHelp.SetWindowPos(NULL, mainTabRect.left, mainTabRect.top, mainTabRect.Width(), mainTabRect.Height(), SWP_HIDEWINDOW); 
		}
	}


	//��ӱ���
	CString strTime=L"";
	CTime CurTime = CTime::GetCurrentTime();
	strTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d",CurTime.GetYear(),CurTime.GetMonth(),CurTime.GetDay(),CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond());
	CString strTitle=L"";
	strTitle.Format(L"���߼��ϵͳȱ����ȡ�������̨---����ʼ����ʱ��Ϊ��%s  �������Ƽ���ѧ����о�Ժ���޹�˾��",strTime);
	SetWindowText(strTitle);

	//
	TCHAR szAppName[MAX_PATH];
	::GetModuleFileName(NULL, szAppName, MAX_PATH);
	CString strAppName = szAppName;
	int nIndex = strAppName.ReverseFind('\\');
	CString strAppPath = strAppName.Left(nIndex);
	m_LogOper.SetLogFilePath(strAppPath);
	m_LogOper.AddLogInfo(L"����ʼ���У�");

	//����
	//string str;
	//for(int i=0;i<10;i++)
	//{
	//	str=format("D:\\Works\\C++��ϰ\\�������㷨�ṹ\\������\\%d.bmp",i);
	//	int a=0;
	//}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CArNTClassifierConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CArNTClassifierConsoleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CArNTClassifierConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CArNTClassifierConsoleDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	CRect tabRect; 
    // ��ȡ��ǩ�ؼ��ͻ���Rect����������������ʺϷ��ñ�ǩҳ   
    m_MainTab.GetClientRect(&tabRect);   
    tabRect.left += 1;   
    tabRect.right -= 1;   
    tabRect.top += 22;   
    tabRect.bottom -= 1;   
  
    switch (m_MainTab.GetCurSel())   
    {    
    case 0:   
        m_DlgSampleCollection.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);   
        m_DlgClassifier.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);  
		m_DlgDefectClassify.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW); 
		m_DlgHelp.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW); 
        break;    
    case 1:   
        m_DlgSampleCollection.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);   
        m_DlgClassifier.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);  
		m_DlgDefectClassify.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW); 
		m_DlgHelp.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW); 
        break; 
	case 2:   
        m_DlgSampleCollection.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);   
        m_DlgClassifier.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);  
		m_DlgDefectClassify.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		m_DlgHelp.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW); 
        break; 
	case 3:   
        m_DlgSampleCollection.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);   
        m_DlgClassifier.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);  
		m_DlgDefectClassify.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW); 
		m_DlgHelp.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW); 
        break; 
    default:   
        break;   
    }   
}


void CArNTClassifierConsoleDlg::InitLoadConfig()
{
	m_ConfigInfo_Local.bIsEnd=false;
	//��ȡӦ�ó���·��
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	CString strFileName = szFileName;
	int nIndex = strFileName.ReverseFind('\\');
	m_strAppPath = strFileName.Left(nIndex);
	CString strConfigName;
	//1������ArNTClassifierConsole.xml�ļ�
	strConfigName.Format(L"%s\\ArNTClassifierConsole.xml",m_strAppPath);
	HFile_xml hFileXml;
	int iJudge=hFileXml.LoadFile(strConfigName);
	if(1==iJudge)
	{
		CString strValue;

		hFileXml.Read(L"//����������̨//���߷���ģ��//�������//�����Զ���������",strValue);
		if(_ttoi(strValue)>0)
		{
			m_ConfigInfo_Local.bAutoClassify=true;
		}
		else
		{
			m_ConfigInfo_Local.bAutoClassify=false;
		}

		hFileXml.Read(L"//����������̨//���߷���ģ��//�������//�������б�",strValue);
		m_ConfigInfo_Local.iDefectCameraList=_ttoi(strValue);

		hFileXml.Read(L"//����������̨//���߷���ģ��//�������//���ȱ���������",strValue);
		m_ConfigInfo_Local.iDefectInteral=_ttoi(strValue);
		if(m_ConfigInfo_Local.iDefectInteral<=0)
		{
			m_ConfigInfo_Local.iDefectInteral=1;//Ҫ����ĸ�ģ�����Ϊ0
		}

		hFileXml.Read(L"//����������̨//���߷���ģ��//�������//���ȱ����������",strValue);
		m_ConfigInfo_Local.iDefectOffset=_ttoi(strValue);

		hFileXml.Read(L"//����������̨//���߷���ģ��//ȱ�����ݿ�//������IP��ַ",strValue);
		m_ConfigInfo_Local.strDefectDBIP.Format(L"%s",strValue);

		hFileXml.Read(L"//����������̨//���߷���ģ��//ȱ��ͼ��洢Ŀ¼//�������//����������",strValue);
		m_ConfigInfo_Local.iDefectImgIP=_ttoi(strValue);

		for(int i=0;i<m_ConfigInfo_Local.iDefectImgIP;i++)
		{
			CString strKey;
			strKey.Format(L"//����������̨//���߷���ģ��//ȱ��ͼ��洢Ŀ¼//�������//������IP��ַ%d",i);
			hFileXml.Read(strKey,strValue);
			m_ConfigInfo_Local.strDefectImgIP[i].Format(L"%s",strValue);
		}

		hFileXml.Read(L"//����������̨//���߷���ģ��//����ܸ���",strValue);
		m_ConfigInfo_Local.iCameraNum=_ttoi(strValue);

		hFileXml.Read(L"//����������̨//���߷���ģ��//ͼ��ԭʼ�ߴ�//ͼ����",strValue);
		m_ConfigInfo_Local.iImgWidth=_ttoi(strValue);

		hFileXml.Read(L"//����������̨//���߷���ģ��//ͼ��ԭʼ�ߴ�//ͼ��߶�",strValue);
		m_ConfigInfo_Local.iImgHeight=_ttoi(strValue);
	}
	else
	{
		AfxMessageBox(TEXT("�����ļ�ArNTClassifierConsole.xml����ʧ�ܣ�"));
	}

	//2��ArNTClassTable.xml
	strConfigName.Format(L"%s\\ArNTClassTable.xml",m_strAppPath);
	iJudge=hFileXml.LoadFile(strConfigName);
	if(1==iJudge)
	{
		m_ConfigInfo_ClassConvertSet.iItemNum=0;
		CString strValue;
		hFileXml.Read(L"//ȱ������//ȱ�����//�������",strValue);
		int iClassTotal=_ttoi(strValue);
		for(int i=0;i<iClassTotal;i++)
		{
			CString strNode=L"";
			
			strNode.Format(L"//ȱ������//ȱ�����//���%d//�ڲ����",i);
			hFileXml.Read(strNode,strValue);
			m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iInternalClassNo=_ttoi(strValue);
			
			strNode.Format(L"//ȱ������//ȱ�����//���%d//����",i);
			hFileXml.Read(strNode,strValue);
			int iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
			WideCharToMultiByte(CP_ACP, 0, strValue, -1, m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].strClassName, iLength, NULL, NULL);

			strNode.Format(L"//ȱ������//ȱ�����//���%d//�ⲿ���",i);
			hFileXml.Read(strNode,strValue);
			m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iExternalClassNo=_ttoi(strValue);

			m_ConfigInfo_ClassConvertSet.iItemNum++;
		}
	}
	else
	{
		AfxMessageBox(TEXT("�����ļ�ArNTClassTable.xml����ʧ�ܣ�"));
	}
	//3������ArNTProcessParam.xml
	strConfigName.Format(L"%s\\ArNTProcessParam.xml",m_strAppPath);
	iJudge=hFileXml.LoadFile(strConfigName);
	if(1==iJudge)
	{
		CString strValue;
		hFileXml.Read(L"//ArNTProcessParam//ProductionLineNumber",strValue);
		m_ConfigInfo_Segmentation.iProductionLineNo=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//DefectMaxNumForEvery",strValue);
		m_ConfigInfo_Segmentation.iDefectMaxNum=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//EdgeNoDetectPixel",strValue);
		m_ConfigInfo_Segmentation.iEdgeNoDetect=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//EdgeThreshold",strValue);
		m_ConfigInfo_Segmentation.iEdgeThreshold=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//FilterThreshold",strValue);
		m_ConfigInfo_Segmentation.iFilterThreshold=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//AreaThreshold",strValue);
		m_ConfigInfo_Segmentation.iAreaThreshold=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//SegmentationThreshold",strValue);
		m_ConfigInfo_Segmentation.iSegmentationThreshold=_ttoi(strValue);

		hFileXml.Read(L"//ArNTProcessParam//All//DefectDetectGrade",strValue);
		m_ConfigInfo_Segmentation.iDefectDetectGrade=_ttoi(strValue);

		m_ConfigInfo_Segmentation.tSteelInfo.iCamera=0;
	}
	else
	{
		AfxMessageBox(TEXT("�����ļ�ArNTProcessParam.xml����ʧ�ܣ�"));
	}
	//
	m_ConfigInfo_Local.bIsEnd=true;
}
void CArNTClassifierConsoleDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UINT nRet=MessageBox(_T(">>�Ƿ��˳�����������̨���棡\n>>�����ѡ��!"),_T("��ʾ��Ϣ"),MB_YESNO);
	if(nRet == IDYES)
	{
		m_LogOper.AddLogInfo(L"�˳�����������̨����");
		HANDLE hself=GetCurrentProcess();
		TerminateProcess(hself,0);
	}
	else
	{
		m_LogOper.AddLogInfo(L"ȡ����һ���˳�����������̨����");
		return;
	}

	CDialog::OnClose();
}
