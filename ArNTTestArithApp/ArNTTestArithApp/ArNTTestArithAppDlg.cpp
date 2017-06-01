// ArNTTestArithAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "ArNTTestArithAppDlg.h"
#include "ArNTTestArithState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define IDM_EXITAPP  0x1000


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


// CArNTTestArithAppDlg �Ի���

 CArNTTestArithAppDlg* pMainDlg = NULL;

 #define TIMER_AUTODETECT 1
 #define TIMER_STARTDETECT 2
  #define TIMER_DLGMINSIZE 3

CArNTTestArithAppDlg::CArNTTestArithAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArNTTestArithAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));     
	lf.lfHeight = 12;   
	CCommonFunc::SafeStringCpy(lf.lfFaceName, LF_FACESIZE, L"����");
	m_FontBtn.CreateFontIndirect(&lf);

	m_pBtnChkSelCameral = NULL;

	LONGSTR strAppPath = {0};
	CCommonFunc::GetAppPath(strAppPath, STR_LEN(strAppPath));
	CCommonFunc::SafeStringPrintf(m_strDefectImgPath, STR_LEN(m_strDefectImgPath), L"%s\\DefectImg", strAppPath);
	CCommonFunc::CreateDir(m_strDefectImgPath);

	m_iDelyTime = 0; 
}

void CArNTTestArithAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTIMGSRC, m_lstImgSrc);
	DDX_Control(pDX, IDC_LSTDEFECTDB, m_lstDefectDB);
	DDX_Control(pDX, IDC_LSTDEFECTIMG, m_lstDefectImg);
	//DDX_Control(pDX, IDC_LSTCAMDEFECT, m_lstCamDefect);
	DDX_Control(pDX, IDC_TOPDETECTHISTIMG, m_lstTopDetectHistImgInfo);
	DDX_Control(pDX, IDC_BOTTOMDETECTHISTIMG, m_lstBottomDetectHistImg);
	DDX_Control(pDX, IDC_LISTCAMLIST, m_lstCameralList);
	DDX_Control(pDX, IDC_LISTDEFECT, m_lstDefect);
	DDX_Control(pDX, IDC_CMBSELCAM, m_cmbSelCameral);
	DDX_Control(pDX, IDC_LSTERROR, m_lstErrorInfo);
	DDX_Control(pDX, IDC_WAVE_DRAW, m_picDraw);
	DDX_Control(pDX, IDC_COMBO_SELRCTALGO, m_SelectAlgo);
}

BEGIN_MESSAGE_MAP(CArNTTestArithAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//	ON_BN_CLICKED(IDC_BTNTESTSENDDEFECT, &CArNTTestArithAppDlg::OnBnClickedBtntestsenddefect)
	//	ON_BN_CLICKED(IDC_BTNSENDDEFECTIMG, &CArNTTestArithAppDlg::OnBnClickedBtnsenddefectimg)
	//	ON_BN_CLICKED(IDC_BTNCURRSTEEL, &CArNTTestArithAppDlg::OnBnClickedBtncurrsteel)
	ON_LBN_SELCHANGE(IDC_LISTCAMLIST, &CArNTTestArithAppDlg::OnLbnSelchangeListcamlist)
	ON_BN_CLICKED(IDC_BTNSHOWIMG, &CArNTTestArithAppDlg::OnBnClickedBtnshowimg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINREVIEW, &CArNTTestArithAppDlg::OnDeltaposSpinreview)
	ON_BN_CLICKED(IDC_BTNDETECT, &CArNTTestArithAppDlg::OnBnClickedBtndetect)
	ON_BN_CLICKED(IDC_CHKAUTOWRITEDB, &CArNTTestArithAppDlg::OnBnClickedChkautowritedb)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GETDTIMG, &CArNTTestArithAppDlg::OnBnClickedGetdtimg)
	ON_BN_CLICKED(IDC_BTNCROPDTIMG, &CArNTTestArithAppDlg::OnBnClickedBtncropdtimg)
	ON_BN_CLICKED(IDC_BTNSEARCHDEFECT, &CArNTTestArithAppDlg::OnBnClickedBtnsearchdefect)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTNSETDIR, &CArNTTestArithAppDlg::OnBnClickedBtnsetdir)
	ON_BN_CLICKED(IDC_BTNCURRSIMULIMGPATH, &CArNTTestArithAppDlg::OnBnClickedBtncurrsimulimgpath)
	ON_BN_CLICKED(IDC_BTNREGAPP, &CArNTTestArithAppDlg::OnBnClickedBtnregapp)
	ON_MESSAGE(UM_NTF_CAMERAL_ARRAY, &CArNTTestArithAppDlg::OnNtfCameralArray)
	ON_BN_CLICKED(IDC_CHECKDETECT, &CArNTTestArithAppDlg::OnBnClickedCheckdetect)
	ON_BN_CLICKED(IDC_BTNFROMCURRPOS, &CArNTTestArithAppDlg::OnBnClickedBtnfromcurrpos)
	ON_WM_CLOSE()
	ON_COMMAND(IDM_EXITAPP, &CArNTTestArithAppDlg::OnExitApp)
	ON_EN_CHANGE(IDC_EDTIMGNUM, &CArNTTestArithAppDlg::OnEnChangeEdtimgnum)
	ON_BN_CLICKED(IDC_BTNCLEAR, &CArNTTestArithAppDlg::OnBnClickedBtnclear)
	ON_BN_CLICKED(IDC_BTNJUMPTO, &CArNTTestArithAppDlg::OnBnClickedBtnjumpto)
	ON_WM_NCHITTEST()
	//ON_EN_CHANGE(IDC_EDTDELYTIME, &CArNTTestArithAppDlg::OnEnChangeEdtdelytime)
	ON_BN_CLICKED(IDC_BUTTON_ADDALGO, &CArNTTestArithAppDlg::OnBnClickedButtonAddalgo)
END_MESSAGE_MAP()


// CArNTTestArithAppDlg ��Ϣ�������

BOOL CArNTTestArithAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		/*CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}*/
		//pSysMenu->RemoveMenu(0, MF_BYPOSITION);
		//pSysMenu->RemoveMenu(0, MF_BYPOSITION);
		//pSysMenu->RemoveMenu(0, MF_BYPOSITION);
		//pSysMenu->RemoveMenu(0, MF_BYPOSITION);
		//pSysMenu->RemoveMenu(0, MF_BYPOSITION);
		//pSysMenu->RemoveMenu(0, MF_BYPOSITION);
		//pSysMenu->AppendMenu(MF_SEPARATOR);
		//pSysMenu->AppendMenu(MF_SEPARATOR);
		//pSysMenu->AppendMenu(MF_STRING, IDM_EXITAPP, _T("�˳��㷨����"));
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MAIN);
	//����ͼ��
	SetIcon(hIcon, FALSE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	ReadConfig();

	LONGSTR strVerison = {0};
	CCommonFunc::GetVersionInfo(strVerison);
	TINYSTR strNowTime = {0};
	CCommonFunc::GetNowTime(strNowTime, STR_LEN(strNowTime));
	LONGSTR strCaption = {0};
	CCommonFunc::SafeStringPrintf(strCaption, STR_LEN(strCaption), L"%s_%s", strVerison, strNowTime);
	this->SetWindowText(strCaption);

	SetImgSrcInfo();
	SetDefectDBInfo();
	SetDefectImgInfo();
	ShowCameralList();

	m_DlgShowImg.Create(IDD_SHOWDLG, this);
	m_DlgShowCropImg.Create(IDD_SHOWDLG, this);
	m_DlgSearchDefect.Create(IDD_SEARCHDEFECT, this);

	pMainDlg = this;
	LONGSTR  strOnlineDetectHelperVersion = {0};
	GetOnlineDetectHelperVersion(strOnlineDetectHelperVersion);
	SetDlgItemText(IDC_EDTONLINEHLEPRVER, strOnlineDetectHelperVersion);

	SetDlgItemInt(IDC_EDTSTARTDETECTLEN, 0);
	this->SetDlgItemInt(IDC_EDTDELYTIME, m_TestArithStateConfig.iDelyTime , 1);
	this->GetDlgItem(IDC_EDTDELYTIME)->EnableWindow(false);
	if(m_TestArithStateConfig.bDelyProcess)
	{
		CButton* pChkDelay = (CButton*)GetDlgItem(IDC_CHKDELAY);
		pChkDelay->SetCheck(1);
	}

	if(ArNT_TRUE == ArNTTestArithState::GetState()->m_bAutoStartDetect)
	{
		SetTimer(TIMER_STARTDETECT, 6000, NULL);
	}

	//�򿪺���С��
	SetTimer(TIMER_DLGMINSIZE, 10000, NULL);

	//����㷨ѡ��
	m_SelectAlgo.InsertString(0,L"Ϳ���㷨");
	m_SelectAlgo.InsertString(1,L"��Ϳ���㷨");
	int num=m_SelectAlgo.GetCount();
	if(m_iAlgoSelect>=0 && m_iAlgoSelect<num)
	{
		m_SelectAlgo.SetCurSel(m_iAlgoSelect);
		ArNTDefectArith_H::m_gArithSelection=m_iAlgoSelect;
		LONGSTR strLogInfo = {0};
		if(0==ArNTDefectArith_H::m_gArithSelection)
		{
			CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"�����ʱ����㷨Ϊ<Ϳ���ϼ���㷨>");
		}
		else if(1==ArNTDefectArith_H::m_gArithSelection)
		{
			CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"�����ʱ����㷨Ϊ<��Ϳ���ϼ���㷨>");
		}
		AddLog(strLogInfo);
		SetDlgItemText(IDC_EDIT_ALGOINFO, strLogInfo);
	}
	else
	{
		m_SelectAlgo.SetCurSel(0);
		ArNTDefectArith_H::m_gArithSelection=0;
		LONGSTR strLogInfo = {0};
		if(0==ArNTDefectArith_H::m_gArithSelection)
		{
			CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"�����ʱ����㷨Ϊ<Ϳ���ϼ���㷨>");
		}
		else if(1==ArNTDefectArith_H::m_gArithSelection)
		{
			CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"�����ʱ����㷨Ϊ<��Ϳ���ϼ���㷨>");
		}
		AddLog(strLogInfo);
		SetDlgItemText(IDC_EDIT_ALGOINFO, strLogInfo);
	}

	//�����̳߳�
	INIT_THREAD_POOL(8);
	int m_iCurrSelCameralID=0;
	THREAD_POOL_POST(threadShowDefectNum,&m_iCurrSelCameralID, sizeof(m_iCurrSelCameralID));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CArNTTestArithAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_EXITAPP)
	{
		OnExitApp();
		return;
	}
	
	
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

void CArNTTestArithAppDlg::OnPaint()
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
		//----------���������ñ���ͼƬ----------------------------
		CPaintDC dc(this); 
		CRect rect;
		GetClientRect(&rect);
		CDC   dcMem;   
		dcMem.CreateCompatibleDC(&dc);   
		CBitmap   bmpBackground;   
		bmpBackground.LoadBitmap(IDB_BACKGROUND);   //IDB_BITMAP�����Լ���ͼ��Ӧ��ID 

		BITMAP   bitmap;   
		bmpBackground.GetBitmap(&bitmap);   
		CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);   
		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);  

		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CArNTTestArithAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


///�����̳߳غ���
IMPL_THREAD_POOL(CArNTTestArithAppDlg)
IMPL_THREAD_POOL_ENTRY(CArNTTestArithAppDlg, threadShowDefectNum,ShortInt)
//---------------------------------------------------------
//���Է���ͼ��
//void CArNTTestArithAppDlg::OnBnClickedBtntestsenddefect()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	ArNTDefectInfo  Info = {sizeof(ArNTDefectInfo)};
//	Info.iCameralNo = 1;
//	Info.iSteelNo = GetDlgItemInt(IDC_EDTCURRSTEELNO);
//	static ShortInt iDefectClass = 1;
//	Info.iClass = iDefectClass;
//	iDefectClass++;
//	iDefectClass = iDefectClass % 7 + 1;
//	Info.iGrade = 7;
//	Info.iArea = 200;
//	Info.iCycle = 0;
//	Info.iTopInImage = 220;
//	Info.iBottomInImage = 410;
//	Info.iLeftInImage = 700;
//	Info.iRightInImage = 900;
//
//	Info.iLeftInSteel = 100;
//	Info.iRightInSteel = 210;
//	Info.iTopInSteel = GetDlgItemInt(IDC_EDTTOPINLEN) - 1000;
//	Info.iBottomInSteel = Info.iTopInSteel + 100;
//	
//	ArNTTestArithState::GetState()->AskAddDefect(&Info, 1, Info.iCameralNo);
//	//����ͼ��
//
//
//
//}

//////////////////////////////////////////////////////////////////
void  CArNTTestArithAppDlg::ReadConfig()
{
	m_iAlgoSelect=0;

	LONGSTR				strAppPath={0};
	LONGSTR				strConfigFile={0};
	CCommonFunc::GetAppPath(strAppPath, STR_LEN(strAppPath));
	CCommonFunc::SafeStringPrintf(strConfigFile, STR_LEN(strConfigFile), L"%s\\ArNTTestArithApp.xml", strAppPath);
	if(m_XMLOperator.LoadXML(strConfigFile))
	{
		LONGSTR strValue = {0};
		if(m_XMLOperator.GetValueByString(L"�㷨���Գ�������#��ʱ��������#Ĭ��֧����ʱ����", strValue))
		{
			if(_wtoi(strValue) > 0) 
			{
				m_TestArithStateConfig.bDelyProcess = ArNT_TRUE;
			}
			else
			{
				m_TestArithStateConfig.bDelyProcess = ArNT_FALSE;
			}
		}
		if(m_XMLOperator.GetValueByString(L"�㷨���Գ�������#��ʱ��������#��ʱʱ��", strValue))
		{
			m_TestArithStateConfig.iDelyTime = _wtoi(strValue);
		}
		if(m_XMLOperator.GetValueByString(L"�㷨���Գ�������#�㷨����#�㷨ѡ��", strValue))
		{
			m_iAlgoSelect = _wtoi(strValue);
		}
	}
}
//�ӿں���

//---------------------------------------------------------------------------
//���յ��ְ�״̬��Ϣ
void  CArNTTestArithAppDlg::OnNtfSteelStatus(ArNTSteelStatus& status)
{
	/*SetDlgItemText(IDC_EDTCURRSTEELID, status.strTopID);
	SetDlgItemInt(IDC_EDTTOPINLEN, status.iTopSteelLen);

	SetDlgItemText(IDC_EDTCURRSTEELID2, status.strBottomID);
	SetDlgItemInt(IDC_EDTTOPINLEN2, status.iBottomSteelLen);*/

	LONGSTR strInfo = {0};
	LongFloat fTopLen = (status.iTopSteelLen / 1000.0);
	LongFloat fBottomLen = (status.iBottomSteelLen / 1000.0);

	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"�ϱ���ˮ��%d �Ѿ������%4.2f�ף��±���ˮ��%d �Ѿ������%4.2f��",
		status.iTopSequeceNo, fTopLen, status.iBottomSequeceNo, fBottomLen);

	SetDlgItemText(IDC_EDTDETECTSTATUSINFO, strInfo);

	//2016-07-22���
	m_iTopSequeceNo=status.iTopSequeceNo;
	m_iBottomSequeceNo=status.iBottomSequeceNo;
}

//---------------------------------------------------------------------------
//
void   CArNTTestArithAppDlg::OnNewTopSteelIn(ULongInt iSequenceNo)
{
	LONGSTR strInfo = {0};
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"�µĸְ���룬��ˮ��%d", iSequenceNo);
	SetDlgItemText(IDC_EDTTOPDELYINFO, strInfo);
	CButton* pChkDelay = (CButton*)GetDlgItem(IDC_CHKDELAY);
	if(pChkDelay->GetCheck() == true)
	{
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"�µĸְ�(��ˮ��%d)����,��ʼ������ʱ���", iSequenceNo);
		SetDlgItemText(IDC_EDTTOPDELYINFO, strInfo);
		m_iDelyTime = GetDlgItemInt(IDC_EDTDELYTIME);
		//���ø�ѡ��
		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECKDETECT);
		pBtn->SetCheck(false);
		OnBnClickedCheckdetect();
		ArNTTestArithState::GetState()->PostDelayDetect(m_iDelyTime);
	}else
	{
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%s", L"δ������ʱ���");
		SetDlgItemText(IDC_EDTTOPDELYINFO, strInfo);

	}
}

//---------------------------------------------------------------------------
//
void   CArNTTestArithAppDlg::OnRcvDefectImg(UShortInt	iCameralNo, 
								 ULongInt	iSteelNo,
								 UShortInt  iImageIndex,
								 ULongInt	iImageSize,
								 UShortInt	iImgWidth,
								 UShortInt	iImgHeight,
								 PImgDataType	pImgeBuffer)
{

	m_DlgShowImg.m_DlgShowPanel.ShowImg(pImgeBuffer, iImgWidth, iImgHeight);
	LONGSTR strInfo = {0};
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"���յ���ˮ��%d �����%d ͼ���%d��ȱ��ͼ��",
								  iSteelNo,
								  iCameralNo,
								  iImageIndex);
	m_DlgShowImg.SetWindowText(strInfo);
	m_DlgShowImg.ShowWindow(SW_SHOW);
	
}
//---------------------------------------------------------------------------
//
void   CArNTTestArithAppDlg::OnRcvCropDefectImg(UShortInt	iCameralNo, 
												ULongInt	iSteelNo,
												UShortInt	iImageIndex,
												ULongInt	iImageSize,
												UShortInt	iImgWidth,
												UShortInt	iImgHeight,
												PImgDataType	pImgeBuffer,
												StrPointer  strSender)
{
	LongInt iSender = _wtoi(strSender);
	if(iSender == (LongInt)this)
	{
		m_DlgShowCropImg.m_DlgShowPanel.ShowImg(pImgeBuffer, iImgWidth, iImgHeight);
		LONGSTR strInfo = {0};
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"���յ���ˮ��%d �����%d ͼ���%d��ָ������ȱ��ͼ��",
								  iSteelNo,
								  iCameralNo,
								  iImageIndex);
		m_DlgShowCropImg.SetWindowText(strInfo);
		m_DlgShowCropImg.ShowWindow(SW_SHOW);
	}
	if(iSender == (LongInt)&m_DlgSearchDefect)
	{
		m_DlgSearchDefect.OnRcvCropDefectImg(iCameralNo, iSteelNo, iImageIndex, iImageSize, iImgWidth, iImgHeight, pImgeBuffer);
	}

}

//---------------------------------------------------------------------------
//
void   CArNTTestArithAppDlg::OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum)
{
	m_DlgSearchDefect.OnRcvSearchDefect(pInfo, iDefectNum);
}

//---------------------------------------------------------------------------
//
void   CArNTTestArithAppDlg::OnNtfCameralID(ShortInt iIDNum, PTinyInt pCameralID)
{
	for(ShortInt i = 0; i < iIDNum; i++)
	{
		CString strCameral;
		strCameral.Format(L"%d", pCameralID[i]);
		m_cmbSelCameral.AddString(strCameral);
	}
	m_cmbSelCameral.SetCurSel(0);
	if(NULL == m_pBtnChkSelCameral)
	{
		LongInt iTempNum = iIDNum;
		SendMessage(UM_NTF_CAMERAL_ARRAY, (WPARAM)iTempNum);
	}
}
//---------------------------------------------------------------------------
//
void	CArNTTestArithAppDlg::OnRcvDetectDefect(PArNTDefectInfo pDefetcArray, ShortInt iDefectNum)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();

	ImgPanelDefectSet DefectInfoSet = {0};
	DefectInfoSet.iItemNum = iDefectNum;

	m_csAddDefectInfo.Enter();
	
	LONGSTR strInfo = {0};
	ULongInt iImageIndex = 0;
	for(ShortInt i = 0; i < iDefectNum; i++)
	{
		
		iImageIndex = pDefetcArray[i].iImgIndex;
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo),
			L"[%05d]��ˮ��%d �����%d ͼ������:%d ���:%dλ��(%d %d %d %d) �ְ�λ��:(%d %d %d %d)",
			pMainDlg->m_lstDefect.GetCount(),
				pDefetcArray[i].iSteelNo,
				pDefetcArray[i].iCameralNo,
				pDefetcArray[i].iImgIndex,
				pDefetcArray[i].iClass,
				pDefetcArray[i].iLeftInImage, 
				pDefetcArray[i].iRightInImage,
				pDefetcArray[i].iTopInImage,
				pDefetcArray[i].iBottomInImage,
				pDefetcArray[i].iLeftInSteel,
				pDefetcArray[i].iRightInSteel,
				pDefetcArray[i].iTopInSteel,
				pDefetcArray[i].iBottomInSteel);

		DefectInfoSet.Items[i].iLeft = pDefetcArray[i].iLeftInImage;
		DefectInfoSet.Items[i].iRight = pDefetcArray[i].iRightInImage;
		DefectInfoSet.Items[i].iTop =  pDefetcArray[i].iTopInImage;
		DefectInfoSet.Items[i].iBottom = pDefetcArray[i].iBottomInImage;
		DefectInfoSet.Items[i].iDefectNo = pDefetcArray[i].iID;


		LONGSTR strTempImagePathName = {0};
		CCommonFunc::SafeStringPrintf(strTempImagePathName, STR_LEN(strTempImagePathName),
										L"%s\\%d.bmp",m_strDefectImgPath, iImageIndex );
		if(CCommonFunc::FileExist(strTempImagePathName) == ArNT_FALSE)
		{
			pState->m_GrayBmp.CreateBmpFile(strTempImagePathName, pState->m_SrcBmpData);
		}

		pMainDlg->m_lstDefect.AddString(strInfo);
		if(pMainDlg->m_lstDefect.GetCount() > 256) pMainDlg->m_lstDefect.DeleteString(0);
	}
	
	if(ArNT_FALSE == pState->m_bLocalMode)
	{
		SetDlgItemInt(IDC_EDTDEFECTNUM,  pState->m_iRcvDefectNum);
	}else
	{
		SetDlgItemInt(IDC_EDTDEFECTNUM,  pState->m_SimulCameralDefectOper.iRcvDefectNum);
		
		m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
		m_DlgShowImg.SetWindowText(pState->m_strCurrSimulImgPathName);
		m_DlgShowImg.ShowWindow(SW_SHOW);
		m_DlgShowImg.m_DlgShowPanel.AddDefectInfo(DefectInfoSet);
	}
	m_csAddDefectInfo.Leave();
}

//-----------------------------------------------------------------
//
LongInt CArNTTestArithAppDlg::GetCurrStartDetectLen(void)
{
	LongInt iLen = GetDlgItemInt(IDC_EDTSTARTDETECTLEN);
	return iLen;

}
//-----------------------------------------------------------------
//
void    CArNTTestArithAppDlg::AddErrorInfo(CString strError)
{

	m_lstErrorInfo.AddString(strError);
}
//-----------------------------------------------------------------
//
void   CArNTTestArithAppDlg::DelayDetect(ShortInt iMaxDelyTime, ShortInt iCurrDelyTime)
{
	LONGSTR strInfo = {0};
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"������ʱ���,������ʱʱ��%d�룬�Ѿ���ʱ%d��", iMaxDelyTime, iCurrDelyTime);
	SetDlgItemText(IDC_EDTTOPDELYINFO, strInfo);
	m_iDelyTime = GetDlgItemInt(IDC_EDTDELYTIME);
	//���ø�ѡ��
	if(iCurrDelyTime >= iMaxDelyTime)
	{
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%s", L"��ʱ�����ɣ���ʼ���");
		SetDlgItemText(IDC_EDTTOPDELYINFO, strInfo);
		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECKDETECT);
		pBtn->SetCheck(true);
		OnBnClickedCheckdetect();
	}
}


///////////////////////////////////////////////////////////////////
//˽�к���

void  CArNTTestArithAppDlg::SetImgSrcInfo(void)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	while(m_lstImgSrc.GetCount()) m_lstImgSrc.DeleteString(0);
	
	ShortInt iImageSourceNum = 0;
	PArNTImageSourceContext pImageSource = pState->GetImageSourceContext(iImageSourceNum);
	for(ShortInt i = 0; i <  iImageSourceNum; i++)
	{
		LONGSTR strInfo = {0};
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"����%d(IP:%s Port:%d) ����:", 
									i + 1, 
									pImageSource[i].Info.strIP, 
									pImageSource[i].Info.iPort);

		for(ShortInt j = 0; j < pImageSource[i].iCameralNum; j++)
		{
			TINYSTR strTemp = {0};
			CCommonFunc::SafeStringPrintf(strTemp, STR_LEN(strTemp), L"���%d��", 
										 pImageSource[i].iCameralID[j]);
			CCommonFunc::SafeStringCat(strInfo, STR_LEN(strInfo), strTemp);	
		}

		m_lstImgSrc.AddString(strInfo);

	}
}
//-----------------------------------------------------------------
//����ȱ�����ݿ���Ϣ
void  CArNTTestArithAppDlg::SetDefectDBInfo(void)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	while(m_lstDefectDB.GetCount()) m_lstDefectDB.DeleteString(0);
	ShortInt iDBServiceNum = 0;
	PArNTDefectDBServiceInfo pDBService = pState->GetDefectDBService(iDBServiceNum);
	for(ShortInt i = 0; i <  iDBServiceNum; i++)
	{
		LONGSTR strInfo = {0};
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"����%d(IP:%s Port:%d) ����:", 
									i + 1, 
									pDBService[i].loc.strIP, 
									pDBService[i].loc.iPort);

		for(ShortInt j = 0; j < pDBService[i].iCameralNum; j++)
		{
			TINYSTR strTemp = {0};
			CCommonFunc::SafeStringPrintf(strTemp, STR_LEN(strTemp), L"���%d��", pDBService[i].CameralArray[j]);
			CCommonFunc::SafeStringCat(strInfo, STR_LEN(strInfo), strTemp);	
		}
		m_lstDefectDB.AddString(strInfo);
	}
}

//-----------------------------------------------------------------
//����ȱ��ͼ����Ϣ
void   CArNTTestArithAppDlg::SetDefectImgInfo(void)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	while(m_lstDefectImg.GetCount()) m_lstDefectImg.DeleteString(0);
	
	ShortInt iDefectImgServiceNum = 0;
	PArNTDefectImageServiceInfo pImageService = pState->GetDefectImageSeriviceInfo(iDefectImgServiceNum);
	for(ShortInt i = 0; i <  iDefectImgServiceNum; i++)
	{
		LONGSTR strInfo = {0};
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"����%d(IP:%s Port:%d) ����:", 
										i + 1, 
										pImageService[i].loc.strIP, 
										pImageService[i].loc.iPort);
		for(ShortInt j = 0; j < pImageService[i].iCameralNum; j++)
		{
			TINYSTR strTemp = {0};
			CCommonFunc::SafeStringPrintf(strTemp, STR_LEN(strTemp), L"���%d��", pImageService[i].CameralArray[j]);
			CCommonFunc::SafeStringCat(strInfo, STR_LEN(strInfo), strTemp);	
		}
		m_lstDefectImg.AddString(strInfo);
	}
}
//-----------------------------------------------------------------
//
void   CArNTTestArithAppDlg::AdjustChkSelCameral(void)
{
	CWnd* pControl = GetDlgItem(IDC_EDTDEFECTNUM);
	CRect rcWindowRect;
	pControl->GetWindowRect(&rcWindowRect);
	CWnd::GetDesktopWindow()->MapWindowPoints(this, &rcWindowRect);



	ShortInt iControlWidth = 50;
	for(ShortInt i = 0; i < m_iCameralNum; i++)
	{
		CRect rcNewPos;
		rcNewPos.left = rcWindowRect.right + 5 + i * iControlWidth;
		rcNewPos.right = rcNewPos.left + iControlWidth;
		rcNewPos.top = rcWindowRect.top;
		rcNewPos.bottom = rcWindowRect.bottom;
		if(m_pBtnChkSelCameral)
		{
			m_pBtnChkSelCameral[i].MoveWindow(&rcNewPos);
		}
	}
}
//-----------------------------------------------------------------
//
void  CArNTTestArithAppDlg::ShowCameralList(void)
{
	while(m_lstCameralList.GetCount()) m_lstCameralList.DeleteString(0);
	ArNTArithState* pState = ArNTTestArithState::GetState();
	ShortInt iImageSourceNum = 0;
	PArNTImageSourceContext pImageSource = pState->GetImageSourceContext(iImageSourceNum);
	for(ShortInt i = 0; i < iImageSourceNum;i++)
	{
		for(ShortInt j = 0; j < pImageSource[i].iCameralNum; j++)
		{
			TINYSTR strCamInfo = {0};
			DWORD dwClientID = pImageSource[i].iCameralID[j];
			CCommonFunc::SafeStringPrintf(strCamInfo, STR_LEN(strCamInfo), 
												L"%d�����",
												dwClientID);
			int iIndex = m_lstCameralList.AddString(strCamInfo);
			m_lstCameralList.SetItemData(iIndex, dwClientID);
		}
	}
}

//-----------------------------------------------------------------
//
ArNT_BOOL   CArNTTestArithAppDlg::ReadFirstImgData(TinyInt iCameralNo, LONGSTR& strImgFileName)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(iCameralNo);
	if(NULL == pInfo)
	{
		return ArNT_FALSE;
	}
	pInfo->iCurrImgIndex = 0;
	return pState->GetCurrImgData(strImgFileName);
}
//-----------------------------------------------------------------
//
ArNT_BOOL  CArNTTestArithAppDlg::ReadNextImgData(TinyInt iCameralNo, LONGSTR& strImgFileName)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();

	PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(iCameralNo);
	if(NULL == pInfo)
	{
		return ArNT_FALSE;
	}
	
	if(pInfo->iCurrImgIndex + 1 >= pInfo->Info.iMaxImgIndex)
	{
		pInfo->iCurrImgIndex  = pInfo->Info.iMaxImgIndex -1;
		return ArNT_FALSE;
	}
	pInfo->iCurrImgIndex++;

	SetDlgItemInt(IDC_EDTCURRINDEX, pInfo->iCurrImgIndex);

	return pState->GetCurrImgData(strImgFileName);
}
//-----------------------------------------------------------------
//
ArNT_BOOL  CArNTTestArithAppDlg::ReadPrieoImgData(TinyInt iCameralNo, LONGSTR& strImgFileName)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();

	PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(iCameralNo);
	if(NULL == pInfo)
	{
		return ArNT_FALSE;
	}
	if(pInfo->iCurrImgIndex <= 0) 
	{
		pInfo->iCurrImgIndex = 0;
		return ArNT_FALSE;
	}
	pInfo->iCurrImgIndex--;
	return pState->GetCurrImgData(strImgFileName);
}
//-----------------------------------------------------------------
//
ArNT_BOOL  CArNTTestArithAppDlg::ReadLastImgData(TinyInt iCameralNo, LONGSTR& strImgFileName)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();

	PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(iCameralNo);
	if(NULL == pInfo)
	{
		return ArNT_FALSE;
	}
	pInfo->iCurrImgIndex  = pInfo->Info.iMaxImgIndex -1;
	return pState->GetCurrImgData(strImgFileName);
}


//-----------------------------------------------------------------
//
void CArNTTestArithAppDlg::OnLbnSelchangeListcamlist()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LongInt iIndex = m_lstCameralList.GetCurSel();
	DWORD dwClientID = m_lstCameralList.GetItemData(iIndex);
	SHORTSTR strInfo = {0};
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d�����", dwClientID);
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	pState->m_iCurrSelCamerID = dwClientID;
	pState->m_iRcvDefectNum = 0;
	SetDlgItemText(IDC_EDTSELCAM, strInfo);
	PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(pState->m_iCurrSelCamerID);
	if(pInfo)
	{
		SetDlgItemText(IDC_EDTCAMIMGPATH, pInfo->Info.strImgPath);
		SetDlgItemInt(IDC_EDTIMGINDEX, pInfo->iCurrImgIndex);
		SetDlgItemInt(IDC_EDTIMGNUM, pInfo->Info.iMaxImgIndex);
	}
}
//-----------------------------------------------------------------
//
void CArNTTestArithAppDlg::OnBnClickedBtnshowimg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();

	if(ArNT_FALSE == pState->m_bLocalMode)
	{
		PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(pState->m_iCurrSelCamerID);
		LONGSTR strImgFileName = {0};
		if(pState->GetCurrImgData(strImgFileName))
		{
			m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
			m_DlgShowImg.SetWindowText(strImgFileName);
			m_DlgShowImg.ShowWindow(SW_SHOW);
		}
	}else
	{
		if(ArNT_TRUE == pState->GetSimulImgData())
		{
			ImgPanelDefectSet InfoSet = {0};
			InfoSet.iItemNum = 0;
			m_DlgShowImg.m_DlgShowPanel.AddDefectInfo(InfoSet);			
			m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
			m_DlgShowImg.SetWindowText(pState->m_strCurrSimulImgPathName);
			m_DlgShowImg.ShowWindow(SW_SHOW);	
		}
	}
}

void CArNTTestArithAppDlg::OnDeltaposSpinreview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	if(ArNT_FALSE == pState->m_bLocalMode)
	{
		PArNTInnerDetectHistImgInfo pInfo = pState->FindDetecHistImgInfo(pState->m_iCurrSelCamerID);
		if(NULL == pInfo)
		{
			return;
		}
		LONGSTR strImgFileName = {0};
		ArNT_BOOL bSuccess = ArNT_TRUE;
		if(pNMUpDown->iDelta < 0)
		{
			bSuccess = ReadNextImgData(pState->m_iCurrSelCamerID, strImgFileName);
		}else
		{
			bSuccess = ReadPrieoImgData(pState->m_iCurrSelCamerID, strImgFileName);

		}
		SetDlgItemInt(IDC_EDTIMGINDEX, pInfo->iCurrImgIndex);
		if(ArNT_TRUE == bSuccess)
		{
			m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
			m_DlgShowImg.SetWindowText(strImgFileName);
			m_DlgShowImg.ShowWindow(SW_SHOW);
		}
	}else
	{
		if(pNMUpDown->iDelta < 0)
		{
			if(pState->MoveToNextSimulImg() == ArNT_FALSE) return;
		}else
		{
			if(pState->MoveToPrieoSimulImg() ==  ArNT_FALSE) return;
		}

		SetDlgItemInt(IDC_EDTCURRINDEX, pState->m_iCurrSimulImgIndex);
		LONGSTR strImageFileName = {0};
		pState->GetImageNameByIndex(strImageFileName, pState->m_iCurrSimulImgIndex);
		SetDlgItemText(IDC_EDTCURRSIMULIMGPATH, strImageFileName);
		
		
		if(ArNT_TRUE == pState->GetSimulImgData())
		{
			ImgPanelDefectSet InfoSet = {0};
			m_DlgShowImg.m_DlgShowPanel.AddDefectInfo(InfoSet);
			m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
			m_DlgShowImg.SetWindowText(pState->m_strCurrSimulImgPathName);
			
			m_DlgShowImg.ShowWindow(SW_SHOW);	
		}
	}

	*pResult = 0;
}

void CArNTTestArithAppDlg::OnBnClickedBtndetect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//ArNTTestArithState::GetState()->DetectCurrImg();
	ArNTTestArithState::GetState()->DetectLastImg();
	LongFloat fArithTime = ArNTTestArithState::GetState()->GetArithTime();

	SHORTSTR strTime = {0};
	CCommonFunc::SafeStringPrintf(strTime, STR_LEN(strTime), L"%4.2f����", fArithTime);
	SetDlgItemText(IDC_EDTARITHTIME, strTime);	
}

void CArNTTestArithAppDlg::OnBnClickedChkautowritedb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CButton* pChkAutoDetect = (CButton*)GetDlgItem(IDC_CHKAUTOWRITEDB);
	if(pChkAutoDetect->GetCheck())
	{
		this->SetTimer(TIMER_AUTODETECT, 100, NULL);
	}else
	{
		ArNTTestArithState::GetState()->m_iRcvDefectNum = 0;
		//while(m_lstDefect.GetCount() > 0) m_lstDefect.DeleteString(0);
		this->KillTimer(TIMER_AUTODETECT);
	}

}

void CArNTTestArithAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	if(nIDEvent == TIMER_AUTODETECT)
	{
		LONGSTR strImgFileName = {0};
		ArNTTestArithState* pState = ArNTTestArithState::GetState();
		if(pState->m_bLocalMode == ArNT_FALSE)
		{
			if(ReadNextImgData(pState->m_iCurrSelCamerID, strImgFileName) ==  ArNT_TRUE)
			{
				ArNTTestArithState::GetState()->DetectCurrImg();	
			}
		}else
		{
			if(pState->MoveToNextSimulImg() == ArNT_FALSE) 
			{
				//pState->MoveToFirstSimulImg();
				KillTimer(TIMER_AUTODETECT);
			}

			SetDlgItemInt(IDC_EDTCURRINDEX, pState->m_iCurrSimulImgIndex);
			LONGSTR strImageFileName = {0};
			pState->GetImageNameByIndex(strImageFileName, pState->m_iCurrSimulImgIndex);
			SetDlgItemText(IDC_EDTCURRSIMULIMGPATH, strImageFileName);

			if(ArNT_TRUE == pState->GetSimulImgData())
			{
				m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
				m_DlgShowImg.SetWindowText(pState->m_strCurrSimulImgPathName);
				ImgPanelDefectSet InfoSet = {0};
				m_DlgShowImg.ShowWindow(SW_SHOW);	
				m_DlgShowImg.m_DlgShowPanel.AddDefectInfo(InfoSet);
				pState->DetectLastImg();
			}
		}
	}
	//
	if(TIMER_STARTDETECT == nIDEvent)
	{
		KillTimer(TIMER_STARTDETECT);
		OnBnClickedBtnfromcurrpos();
		for(ShortInt i = 0; i < m_iCameralNum; i++)
		{
			ShortInt iCamral = i + 1;
			if(pState->IsCameralEnableAutoDetect(iCamral))
			{
				m_pBtnChkSelCameral[i].SetCheck(true);
			}
		}		
		Sleep(2000);

		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECKDETECT);
		pBtn->SetCheck(true);
		OnBnClickedCheckdetect();
	}
	if(TIMER_DLGMINSIZE == nIDEvent)
	{
		KillTimer(TIMER_DLGMINSIZE);
		this->ShowWindow(SW_MINIMIZE);
	}

	CDialog::OnTimer(nIDEvent);
}

void CArNTTestArithAppDlg::OnBnClickedGetdtimg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LONGSTR strWnd = {0};
	CCommonFunc::SafeStringPrintf(strWnd, STR_LEN(strWnd), L"%d", (LongInt)this);
	
	ArNTTestArithState::GetState()->AskSearchDefectImg(1106, 3, 10, strWnd);
}

void CArNTTestArithAppDlg::OnBnClickedBtncropdtimg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//ArNTTestArithState::GetState()->AskCropDefectImg(1106, 3, 10, 100, 300, 20, 200, L"MainDlg");
}

void CArNTTestArithAppDlg::OnBnClickedBtnsearchdefect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iSel = m_cmbSelCameral.GetCurSel();
	CString strSelCam;
	m_cmbSelCameral.GetLBText(iSel, strSelCam);
	int iCamera = _wtoi(strSelCam.GetBuffer(0));
	int iSteelNo = GetDlgItemInt(IDC_EDTSELSTEELNO);
	ArNTTestArithState::GetState()->AskSearchDefect(iSteelNo, iCamera);
	m_DlgSearchDefect.ShowWindow(SW_SHOW);
}

void CArNTTestArithAppDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	if(TRUE == bShow)
	{
		ArNTTestArithState* pState = ArNTTestArithState::GetState();
		ShortInt iCameralNum = 0;
		PTinyInt pCameralID = pState->GetCameralIDArray(iCameralNum);
		OnNtfCameralID(iCameralNum, pCameralID);

		AdjustChkSelCameral();
	}

}

void CArNTTestArithAppDlg::OnBnClickedBtnsetdir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	LONGSTR strSimulDir = {0};
	CCommonFunc::ShowDirDialog(L"ѡ��ģ��ͼ���Ŀ¼", strSimulDir);
	SetDlgItemText(IDC_EDTSIMULDIR, strSimulDir);
	pState->SearchSimulImg(strSimulDir);

	SetDlgItemInt(IDC_EDTSIMULIMGNUM, pState->m_iSimulImgNum);
}

void CArNTTestArithAppDlg::OnBnClickedBtncurrsimulimgpath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	CFileDialog dlg(TRUE);
	if(dlg.DoModal() == IDOK)
	{
		CCommonFunc::SafeStringCpy(pState->m_strCurrSimulImgPathName, STR_LEN(pState->m_strCurrSimulImgPathName),
			dlg.GetPathName().GetBuffer(0));
		SetDlgItemText(IDC_EDTCURRSIMULIMGPATH, pState->m_strCurrSimulImgPathName);		
	}

}

void CArNTTestArithAppDlg::OnBnClickedBtnregapp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	pState->RegDBNtfApp();
}

LRESULT  CArNTTestArithAppDlg::OnNtfCameralArray(WPARAM wParam, LPARAM lParam)
{
	LongInt iCameralNum = (LongInt)wParam;
	m_iCameralNum = (ShortInt)iCameralNum;
	m_pBtnChkSelCameral = new CButton [m_iCameralNum];
	for(ShortInt i = 0; i < m_iCameralNum; i++)
	{
		
		TINYSTR strCaption = {0};
		CCommonFunc::SafeStringPrintf(strCaption, STR_LEN(strCaption), L"���%d", i + 1);	
		CRect rcRegion(0, 0, 20, 20) ;
		m_pBtnChkSelCameral[i].Create(strCaption, BS_AUTOCHECKBOX, rcRegion, this, BTN_SELCAME_ID + i);
		m_pBtnChkSelCameral[i].SetFont(&m_FontBtn);
		m_pBtnChkSelCameral[i].ShowWindow(SW_SHOW);
	}
	AdjustChkSelCameral();
	return 0;
}

void CArNTTestArithAppDlg::OnBnClickedCheckdetect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECKDETECT);
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	if(pBtn->GetCheck())
	{
		
		for(ShortInt i = 0; i < m_iCameralNum; i++)
		{
			if(m_pBtnChkSelCameral[i].GetCheck())
			{
				ShortInt iCamralNo = i + 1;
				pState->StartDetectCameral(iCamralNo);
			}
		}
	}else
	{
		for(ShortInt i = 0; i < m_iCameralNum; i++)
		{
			ShortInt iCamralNo = i + 1;
			pState->StopDetectCameral(iCamralNo);			
		}

	}
}

void CArNTTestArithAppDlg::OnBnClickedBtnfromcurrpos()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	for(ShortInt i = 0; i < m_iCameralNum; i++)
	{
		//if(m_pBtnChkSelCameral[i].GetCheck())
		{
			ShortInt iCamralNo = i + 1;
			pState->JumpToMaxImage(iCamralNo);
		}
	}
}
void CArNTTestArithAppDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//CDialog::OnClose();
	UINT nRet=MessageBox(_T("�Ƿ��˳�ȱ���㷨����"),_T("�˳�ϵͳ"),MB_YESNO);
	if(nRet != IDYES)
	{
		LONGSTR strLogInfo = {0};
		CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"��ȡ����һ���˳��㷨����Ĳ���");
		AddLog(strLogInfo);
		this->ShowWindow(SW_SHOW);
		return;
	}
	else
	{
		LONGSTR strLogInfo = {0};
		CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"��ǿ���˳����㷨����");
		AddLog(strLogInfo);
		HANDLE hself=GetCurrentProcess();
		TerminateProcess(hself,0);
		//CDialog::OnClose();
	}
	
	//this->ShowWindow(SW_MINIMIZE);
	//CWnd* pDeskTop = CWnd::GetDesktopWindow();
	//CRect rcClient;
	//pDeskTop->GetClientRect(&rcClient);
	//this->SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), rcClient.right - 200, 40, 0, 0, SWP_NOSIZE);

}

void CArNTTestArithAppDlg::OnExitApp()
{
	ArNTTestArithState::GetState()->m_ExitApp=ArNT_TRUE;
	Sleep(5000);
	CDialog::OnOK();
}

void CArNTTestArithAppDlg::OnEnChangeEdtimgnum()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CArNTTestArithAppDlg::OnBnClickedBtnclear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	while(m_lstDefect.GetCount() > 0) m_lstDefect.DeleteString(0);
}

void CArNTTestArithAppDlg::OnBnClickedBtnjumpto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	LongInt iIndex = GetDlgItemInt(IDC_EDTCURRINDEX);
	pState->JumpToIndex(iIndex);
	if(ArNT_TRUE == pState->GetSimulImgData())
	{
			m_DlgShowImg.m_DlgShowPanel.ShowImg(pState->m_SrcBmpData);
			m_DlgShowImg.SetWindowText(pState->m_strCurrSimulImgPathName);
			ImgPanelDefectSet InfoSet = {0};
			m_DlgShowImg.m_DlgShowPanel.AddDefectInfo(InfoSet);
			m_DlgShowImg.ShowWindow(SW_SHOW);	
			SetDlgItemText(IDC_EDTCURRSIMULIMGPATH, pState->m_strCurrSimulImgPathName);
	}
}

LRESULT CArNTTestArithAppDlg::OnNcHitTest(CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(point.y < 50)
	{
		return HTCAPTION;
	}else
	{
		return CDialog::OnNcHitTest(point);
	}	
}

void CArNTTestArithAppDlg::DrawWave(CDC *pDC, CRect &rectPicture)   
{   
    float fDeltaX;     // x������������ͼ����������   
    float fDeltaY;     // y��ÿ���߼���λ��Ӧ������ֵ   
    int nX;      // ������ʱ���ڴ洢��ͼ��ĺ�����   
    int nY;      // ������ʱ���ڴ洢��ͼ���������   
    CPen newPen;       // ���ڴ����»���   
    CPen *pOldPen;     // ���ڴ�žɻ���   
    CBrush newBrush;   // ���ڴ����»�ˢ   
    CBrush *pOldBrush; // ���ڴ�žɻ�ˢ   
  
    // ����fDeltaX��fDeltaY   
    fDeltaX = (float)rectPicture.Width() / (POINT_COUNT - 1);   
    fDeltaY = (float)rectPicture.Height() / ArNTTestArithState::GetState()->m_iMaxDefectNum;   
  
    // ������ɫ�»�ˢ   
    newBrush.CreateSolidBrush(RGB(0,0,0));   
    // ѡ���»�ˢ�������ɻ�ˢ��ָ�뱣�浽pOldBrush   
    pOldBrush = pDC->SelectObject(&newBrush);   
    // �Ժ�ɫ��ˢΪ��ͼ�ؼ�����ɫ���γɺ�ɫ����   
    pDC->Rectangle(rectPicture);   
    // �ָ��ɻ�ˢ   
    pDC->SelectObject(pOldBrush);   
    // ɾ���»�ˢ   
    newBrush.DeleteObject();   

	//x coordinate  
	CPen  m_X_pen;  
	m_X_pen.CreatePen(PS_SOLID,2,RGB(255, 0, 0));  
	pDC->SelectObject(&m_X_pen);  
	pDC->SetViewportOrg(rectPicture.left, rectPicture.bottom);  
	int n = 0;  
	int nTemp=0;  
	CString strXnum;
	while (nTemp <= rectPicture.right)  
   {  
		pDC->LineTo(10*fDeltaX*n,0);  
		pDC->LineTo(10*fDeltaX*n,-5);  
		pDC->MoveTo(10*fDeltaX*n,0);
		strXnum.Format(_T("%d"),n*4);
		pDC->TextOut(10*fDeltaX*n,0,strXnum);  
		n++;  
		nTemp +=10*fDeltaX;  
	}  
	pDC->LineTo(rectPicture.Width(), 0);   
 
	//Y coordinate 
	CPen  m_Y_pen;  
	m_Y_pen.CreatePen(PS_SOLID,2,RGB(255, 255, 0));  
	pDC->SelectObject(&m_Y_pen);  
	pDC->SetViewportOrg(rectPicture.left, rectPicture.top);
	int index = 1;  
	nTemp = 0; 
	n=0;
	float fDelta = (float)rectPicture.Height() / 10;  
	while (nTemp < rectPicture.bottom)  
	{  
		pDC->LineTo(rectPicture.left, -(rectPicture.top-index * fDelta*n));  
		pDC->LineTo(5, -(rectPicture.top-index * fDelta*n));  
		pDC->LineTo(rectPicture.left, -(rectPicture.top-index * fDelta*n));   
		nTemp +=fDelta; 
		n++;
	}
	CString strYmax;
	strYmax.Format(_T("%d"), ArNTTestArithState::GetState()->m_iMaxDefectNum);
	CString strYmin;
	strYmin.Format(_T("%d"),0);
	pDC->TextOut(rectPicture.left-20, rectPicture.top,strYmax);
	pDC->TextOut(rectPicture.left-20, rectPicture.bottom,strYmin);


	pDC->SetViewportOrg(rectPicture.left, rectPicture.top);
    // ����ʵ�Ļ��ʣ��ֶ�Ϊ1����ɫΪ��ɫ   
    newPen.CreatePen(PS_SOLID, 2, RGB(0,255,0));   
    // ѡ���»��ʣ������ɻ��ʵ�ָ�뱣�浽pOldPen   
    pOldPen = pDC->SelectObject(&newPen);   
    // ����ǰ���ƶ�����ͼ�ؼ����ڵ����½ǣ��Դ�Ϊ���ε���ʼ��   
    pDC->MoveTo(rectPicture.left, rectPicture.bottom);   
    // ����m_nzValues������ÿ�����Ӧ������λ�ã����������ӣ������γ�����   
    for (int i=0; i<POINT_COUNT; i++)   
    {   
        nX = rectPicture.left + (int)(i * fDeltaX);   
        nY = rectPicture.bottom - (int)(m_nzValues[i] * fDeltaY);   
        pDC->LineTo(nX, nY);   
    }   
  
    // �ָ��ɻ���   
    pDC->SelectObject(pOldPen);   
    // ɾ���»���   
    newPen.DeleteObject();  
	newBrush.DeleteObject();
	m_X_pen.DeleteObject(); 
	m_Y_pen.DeleteObject();
	pOldPen->DeleteObject();
	pOldBrush->DeleteObject();

	m_picDraw.ReleaseDC(pDC);
}  

void   CArNTTestArithAppDlg::threadShowDefectNum(PShortInt pInterval,LongInt iDataLen)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();	
	int RcvDefectNum_Before=0;
	int DefectNum=0;
	memset(m_nzValues, 0, sizeof(int) * POINT_COUNT);
	CRect rectPicture;
	m_picDraw.GetClientRect(&rectPicture); 
	while(ArNT_TRUE)
	{ 	  
		// �������е�����Ԫ��ǰ��һ����λ����һ��Ԫ�ض���   
		for (int i=0; i<POINT_COUNT-1; i++)   
		{   
			m_nzValues[i] = m_nzValues[i+1];   
		}   
		// Ϊ���һ��Ԫ�ظ�һ��80���ڵ������ֵ�����ͣ�   
		//m_nzValues[POINT_COUNT-1] = rand() % 80;   
		if(ArNT_FALSE == pState->m_bLocalMode)
		{
			DefectNum=pState->m_iRcvDefectNum-RcvDefectNum_Before;
			RcvDefectNum_Before=pState->m_iRcvDefectNum;
			if(DefectNum<=ArNTTestArithState::GetState()->m_iMaxDefectNum)
			{
				m_nzValues[POINT_COUNT-1]=DefectNum;
			}
			else
			{
				m_nzValues[POINT_COUNT-1]=ArNTTestArithState::GetState()->m_iMaxDefectNum;
			}
		}else
		{
			DefectNum=pState->m_SimulCameralDefectOper.iRcvDefectNum-RcvDefectNum_Before;
			RcvDefectNum_Before=pState->m_SimulCameralDefectOper.iRcvDefectNum;
			if(DefectNum<=ArNTTestArithState::GetState()->m_iMaxDefectNum)
			{
				m_nzValues[POINT_COUNT-1]=DefectNum;
			}
			else
			{
				m_nzValues[POINT_COUNT-1]=ArNTTestArithState::GetState()->m_iMaxDefectNum;
			}
		}
		// ��ȡ��ͼ�ؼ��Ŀͻ�������   
		// ���ͻ��������Դ��ڵ����Ͻ�Ϊԭ�㣬������������Ļ���Ͻ�Ϊԭ�����Ļ���꣩   
		//m_picDraw.GetClientRect(&rectPicture);   	  
		// ���Ʋ���ͼ   
		if(ArNT_TRUE ==ArNTTestArithState::GetState()->m_ExitApp) break;
		DrawWave(m_picDraw.GetDC(), rectPicture);
		
		Sleep(1000);
	}
}

void	CArNTTestArithAppDlg::AddLog(StrPointer strLogInfo)
{
	LONGSTR strAppName = {0};
	LONGSTR m_strLogPath = {0};
	LONGSTR m_strAppPath = {0};
	CCommonFunc::GetAppPath(m_strAppPath, STR_LEN(m_strAppPath));
	CCommonFunc::GetAppName(strAppName, STR_LEN(strAppName));
	CCommonFunc::SafeStringPrintf(m_strLogPath, STR_LEN(m_strLogPath), L"%s\\Log\\%s", m_strAppPath, strAppName);
	
	SYSTEMTIME NowTime;
	GetLocalTime(&NowTime);
	CCommonFunc::CreateDir(m_strLogPath);

	LONGSTR strLogFile = {0};
	CCommonFunc::SafeStringPrintf(strLogFile, STR_LEN(strLogFile), L"%s\\%d%02d%02d.txt", m_strLogPath, NowTime.wYear, NowTime.wMonth, NowTime.wDay);
	SHORTSTR strTime = {0};
	CCommonFunc::SafeStringPrintf(strTime, STR_LEN(strTime), L"%02dʱ%02d��%02d��", NowTime.wHour, NowTime.wMinute, NowTime.wSecond);
	LONGSTR strTempLogInfo = {0};
	CCommonFunc::SafeStringPrintf(strTempLogInfo, STR_LEN(strTempLogInfo), L"[%s]:%s", strTime, strLogInfo);
	CCommonFunc::WriteStringToFile( strLogFile, strTempLogInfo);
}// CArNTTestArithAppDlg.cpp : ʵ���ļ�

void CArNTTestArithAppDlg::OnBnClickedButtonAddalgo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSel=m_SelectAlgo.GetCurSel();
	
	CString str=L"";
	if(0==nSel)
	{
		str.Format(L"�Ƿ����<Ϳ�����㷨>��ȱ��ʶ��?");
	}
	else if(1==nSel)
	{
		str.Format(L"�Ƿ����<��Ϳ�����㷨>��ȱ��ʶ��?");
	}
	UINT nRet=MessageBox(str,_T("��ʾ��Ϣ"),MB_YESNO);
	LONGSTR				strAppPath={0};
	LONGSTR				strConfigFile={0};
	CCommonFunc::GetAppPath(strAppPath, STR_LEN(strAppPath));
	CCommonFunc::SafeStringPrintf(strConfigFile, STR_LEN(strConfigFile), L"%s\\ArNTTestArithApp.xml", strAppPath);
	LONGSTR strNum = {0};
	CCommonFunc::SafeStringPrintf(strNum, STR_LEN(strNum), L"%d", nSel);

	if(nRet == IDYES)
	{
		if(m_XMLOperator.ChangeValueByString(L"�㷨���Գ�������#�㷨����#�㷨ѡ��", strNum))
		{
			m_XMLOperator.SaveXML(strConfigFile);
			ArNTDefectArith_H::m_gArithSelection=nSel;
			LONGSTR strLogInfo = {0};
			if(0==ArNTDefectArith_H::m_gArithSelection)
			{
				CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"����㷨����Ϊ<Ϳ���ϼ���㷨>");
			}
			else if(1==ArNTDefectArith_H::m_gArithSelection)
			{
				CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"����㷨����Ϊ<��Ϳ���ϼ���㷨>");
			}
			AddLog(strLogInfo);
			SetDlgItemText(IDC_EDIT_ALGOINFO, strLogInfo);
		}
	}
	else
	{
		LONGSTR strLogInfo = {0};
		CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"��ȡ���˼���㷨�ĸ���");
		AddLog(strLogInfo);
	}	
}
