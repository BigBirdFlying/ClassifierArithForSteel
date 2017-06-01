// DlgSampleCollection.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgSampleCollection.h"
#include "ImgProcessPrimary.h"
#include "CvvImage.h" 
#include "H_DefectDetectInterface.h"
#include "H_Common.h"
#include "DlgConfigDefectClass.h"
#include "ArNTClassifierConsoleDlg.h"

#define random(x) (rand()%x)
//���ߺ���������Ķ��պ���
char * get_desc_by_id(MY_DESCRIBE_COMM_TYPE *desc,unsigned id)  
{  
	int i=0;  
	static char  no_match[10]={0};  
	while(desc[i].id!=0xffff)  
	{  
		if(desc[i].id==id) 
		{
			return desc[i].desc; 
		}
		i++;  
	}      
	return "";  
} 
int get_id_by_desc(MY_DESCRIBE_COMM_TYPE *desc,char *des)  
{  
	int i=0;   
	while(strcmp(desc[i].desc,"end")!=0)  
	{  
		if(strcmp(desc[i].desc,des)==0)  
		{
			return desc[i].id;
		}
		i++;  
	}      
	return 0;  
} 
// DlgSampleCollection �Ի���

IMPLEMENT_DYNAMIC(DlgSampleCollection, CDialog)

DlgSampleCollection::DlgSampleCollection(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSampleCollection::IDD, pParent)
{
	//��״ͼͼ�꣬Ŀǰ���õ���5��
	m_hIconTree[0] = theApp.LoadIcon(IDI_ICON1);   
    m_hIconTree[1] = theApp.LoadIcon(IDI_ICON2);
	m_hIconTree[2] = theApp.LoadIcon(IDI_ICON3);
	m_hIconTree[3] = theApp.LoadIcon(IDI_ICON4);
	m_hIconTree[4] = theApp.LoadIcon(IDI_ICON5);
	m_treeImageList.Create(16, 16, ILC_COLOR32, 3, 3);//
	for (int i=0; i<5; i++)   
    {   
        m_treeImageList.Add(m_hIconTree[i]);   
    } 
	//�������б�
	m_sampleImageList.Create(64, 64, ILC_COLOR32, 3, 3); //|ILC_MASK
	//ȱ������������˫���˵�ѡ��
	m_menuAddDefect.LoadMenu(IDR_MENU_ADD_DEFECT);
	//
}

DlgSampleCollection::~DlgSampleCollection()
{
}

void DlgSampleCollection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_IMAGEPATH, m_TreeImagePath);
	DDX_Control(pDX, IDC_LIST_IMGSAMPLE, m_ListImgSample);
	DDX_Control(pDX, IDC_LOGO, m_Logo);
	DDX_Control(pDX, IDC_COMBO_SEG_SELECT, m_comboBoxSelectSegArith);
	DDX_Control(pDX, IDC_LIST_SHOW_LOGINFO, m_listBoxShowLogInfo);
	DDX_Control(pDX, IDC_COMBO_TEST_FUNC, m_comboTestImageProcessFunc);
}


BEGIN_MESSAGE_MAP(DlgSampleCollection, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_OPENDIR, &DlgSampleCollection::OnBnClickedButtonOpendir)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_IMAGEPATH, &DlgSampleCollection::OnTvnSelchangedTreeImagepath)
	ON_BN_CLICKED(IDC_BUTTON_AUTOSEG, &DlgSampleCollection::OnBnClickedButtonAutoseg)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_IMGSAMPLE, &DlgSampleCollection::OnLvnItemActivateListImgsample)
	ON_BN_CLICKED(IDC_BUTTON_SAMPLESET_PATH, &DlgSampleCollection::OnBnClickedButtonSamplesetPath)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGDEFECTCLASS, &DlgSampleCollection::OnBnClickedButtonConfigdefectclass)
	ON_CBN_SELCHANGE(IDC_COMBO_TEST_FUNC, &DlgSampleCollection::OnCbnSelchangeComboTestFunc)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// DlgSampleCollection ��Ϣ�������
void DlgSampleCollection::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

	CBitmap bitmap;  
	CClientDC pDC(this);  
	CDC dcMemory;  
	dcMemory.CreateCompatibleDC(&pDC);  

	bitmap.LoadBitmap(IDB_BITMAP_LOGO);
	dcMemory.SelectObject(&bitmap);  
	pDC.StretchBlt(10,5,100,100,&dcMemory,0,0,300,300,SRCCOPY);  

	bitmap.DeleteObject();  
	dcMemory.DeleteDC();
}
//���ں�����ʼ��
BOOL DlgSampleCollection::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//���ز��߶��ձ�
	LOAD_PRODUCTLINE(ProductLine);
	ProductLine enumProductLine;
	enumProductLine=TY_Common_0X;
	for(int i=0;i<1000;i++)
	{
		CString strName;
		USES_CONVERSION;
		strName = A2T(MY_DESCRIBE_BYID(ProductLine,enumProductLine));
		if(strName!="")
		{
			m_comboBoxSelectSegArith.AddString(strName);
		}
		enumProductLine=(ProductLine)(enumProductLine+1);
	}
	m_comboBoxSelectSegArith.SetCurSel(0);
	m_listBoxShowLogInfo.AddString(L"�����б������ɣ�");

	//
	m_comboTestImageProcessFunc.AddString(L"Img_Gray_Change_Y");
	m_comboTestImageProcessFunc.AddString(L"Img_Gray_Change_X");
	m_comboTestImageProcessFunc.AddString(L"Img_Gray_Change_Integral");
	m_comboTestImageProcessFunc.AddString(L"Img_Otsu");
	m_comboTestImageProcessFunc.AddString(L"Img_SSR");
	m_comboTestImageProcessFunc.AddString(L"Img_Sobel");
	m_comboTestImageProcessFunc.AddString(L"Img_LBP");
	m_comboTestImageProcessFunc.AddString(L"Img_Relief");
	m_comboTestImageProcessFunc.AddString(L"Img_Hist");
	m_comboTestImageProcessFunc.AddString(L"Img_Expansion_And_Extrusion");
	m_comboTestImageProcessFunc.AddString(L"Img_Transform");
	m_comboTestImageProcessFunc.AddString(L"Img_DWT");
	m_comboTestImageProcessFunc.AddString(L"Img_GrayStatistics");
	m_comboTestImageProcessFunc.AddString(L"Img_Feather");
	m_comboTestImageProcessFunc.AddString(L"Img_BrightLight");
	m_comboTestImageProcessFunc.AddString(L"Img_HighContrast");
	m_comboTestImageProcessFunc.AddString(L"Img_Spread");
	m_comboTestImageProcessFunc.AddString(L"Img_Sketch");
	m_comboTestImageProcessFunc.AddString(L"Img_Wind");
	m_comboTestImageProcessFunc.AddString(L"Img_Cartoons");
	m_comboTestImageProcessFunc.AddString(L"Img_BrightStretching");
	m_comboTestImageProcessFunc.AddString(L"Img_Surf");
	m_comboTestImageProcessFunc.AddString(L"Img_Meanshift");
	m_comboTestImageProcessFunc.AddString(L"Img_Clustering");
	m_comboTestImageProcessFunc.AddString(L"Img_Fold");
	m_comboTestImageProcessFunc.AddString(L"Img_GaborFilter");
	m_comboTestImageProcessFunc.AddString(L"Img_QuWu");

	//ͼ��������
	GetDlgItem(IDC_IMAGESHOW)->GetWindowRect(&m_ImgRegion);
	ScreenToClient(&m_ImgRegion);

	//m_comboBoxSelectSegArith.AddString(L"TY_Common_0X");
	//m_comboBoxSelectSegArith.AddString(L"BP_ChongGang_1X");
	//m_comboBoxSelectSegArith.AddString(L"BP_ChongGang_2X");
	//m_comboBoxSelectSegArith.AddString(L"BP_ChongGang_3X");
	//m_comboBoxSelectSegArith.AddString(L"BP_BeiHaiChengDe_1X");
	//m_comboBoxSelectSegArith.AddString(L"BP_BeiHaiChengDe_2X");
	//m_comboBoxSelectSegArith.AddString(L"BP_LiuGang_1X");
	//m_comboBoxSelectSegArith.AddString(L"KHB_NanGang_5000X");
	//m_comboBoxSelectSegArith.AddString(L"KHB_XiangTan_3800X");
	//m_comboBoxSelectSegArith.AddString(L"KHB_XiangTan_5000X");
	//m_comboBoxSelectSegArith.AddString(L"KHB_ChongGang_4100X");
	//m_comboBoxSelectSegArith.AddString(L"KHB_XingChengTeGang_1X");
	//m_comboBoxSelectSegArith.AddString(L"RZ_ChongGang_1780X");
	//m_comboBoxSelectSegArith.AddString(L"RZ_DingXin_1780X");
	//m_comboBoxSelectSegArith.AddString(L"RZ_DingXin_850X");
	//m_comboBoxSelectSegArith.AddString(L"RZ_LiuGang_1450X");
	//m_comboBoxSelectSegArith.AddString(L"GR_BeiHaiChengDe_1X");
	//m_comboBoxSelectSegArith.AddString(L"GR_BeiHaiChengDe_2X");
	//m_comboBoxSelectSegArith.AddString(L"GR_BeiHaiChengDe_3X");
	//m_comboBoxSelectSegArith.AddString(L"LZ_TianChengBuXiu_1X");
	//m_comboBoxSelectSegArith.AddString(L"LB_XiNanLv_1X");
	//m_comboBoxSelectSegArith.AddString(L"LB_XinJiangZhongHe_1X");
	//m_comboBoxSelectSegArith.AddString(L"BC_HuaiGangBangCai_1X");
	//m_comboBoxSelectSegArith.AddString(L"TY_Common_999X");

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

///��ͼ��Ŀ¼�İ�ť��Ӧ����
void DlgSampleCollection::OnBnClickedButtonOpendir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ���������� 
	char szPath[MAX_PATH];//���ѡ���Ŀ¼·��     
    ZeroMemory(szPath, sizeof(szPath));
	LPWSTR strPathName = CA2W(szPath);
      
    BROWSEINFO bi;     
    bi.hwndOwner = m_hWnd;     
    bi.pidlRoot = NULL;     
    bi.pszDisplayName = strPathName;     
    bi.lpszTitle = TEXT("��ѡ����Ҫ�򿪵�Ŀ¼��");     
    bi.ulFlags = 0;     
    bi.lpfn = NULL;     
    bi.lParam = 0;     
    bi.iImage = 0;     
    //����ѡ��Ŀ¼�Ի���  
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);         
    if(lp && SHGetPathFromIDList(lp, strPathName))     
    {  
		m_ImgTreePathName.Format(L"%s",strPathName);  
		SetDlgItemText(IDC_EDIT_FILEPATH, m_ImgTreePathName); 
		CString strLog;
		strLog.Format(L"���صĴ�����Ŀ¼Ϊ��%s",m_ImgTreePathName);
		m_listBoxShowLogInfo.AddString(strLog);
    }  
    else 
	{
        AfxMessageBox(TEXT("��Ч��Ŀ¼��������ѡ��"));
		return;
	}
	//��ʾ���ṹ
	m_TreeImagePath.DeleteAllItems();
	m_TreeImagePath.SetImageList(&m_treeImageList, TVSIL_NORMAL);
	
	m_treeNum=0;
	m_IsUnfold=true;
	m_hRoot[0] = m_TreeImagePath.InsertItem(m_ImgTreePathName, 0, 0); 
	FindFileToTree(m_ImgTreePathName,0+1); 
	
	CString strLog;
	strLog.Format(L"���صĴ�����ͼ�����");
	m_listBoxShowLogInfo.AddString(strLog);
}
///������������ͼ��Ŀ¼
void DlgSampleCollection::OnBnClickedButtonSamplesetPath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char szPath[MAX_PATH];//���ѡ���Ŀ¼·��     
    ZeroMemory(szPath, sizeof(szPath));
	LPWSTR strPathName = CA2W(szPath);
      
    BROWSEINFO bi;     
    bi.hwndOwner = m_hWnd;     
    bi.pidlRoot = NULL;     
    bi.pszDisplayName = strPathName;     
    bi.lpszTitle = TEXT("��ѡ����Ҫ�򿪵�Ŀ¼��");     
    bi.ulFlags = 0;     
    bi.lpfn = NULL;     
    bi.lParam = 0;     
    bi.iImage = 0;     
    //����ѡ��Ŀ¼�Ի���  
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);     
      
    if(lp && SHGetPathFromIDList(lp, strPathName))     
    {  
		m_SampleSetPathName.Format(L"%s",strPathName);  
		SetDlgItemText(IDC_EDIT_SAMPLESET_PATH, m_SampleSetPathName); 
		CString strLog;
		strLog.Format(L"���ص�������Ŀ¼Ϊ��%s",m_SampleSetPathName);
		m_listBoxShowLogInfo.AddString(strLog);
    }  
    else 
	{
        AfxMessageBox(TEXT("��Ч��Ŀ¼��������ѡ��"));
		return;
	}
	//Ϊ�˵����ƶ�ID��������������������Ϊ16(MAX_CLASS_NUM)64
	for(int k=ID_ADDNEW;k<ID_ADDNEW+MAX_NUM_CLASS;k++)
	{
		m_menuAddDefect.GetSubMenu(0)->DeleteMenu(k,MF_BYCOMMAND);
	}

	CString strLog;
	strLog.Format(L"ȱ��������ؿ�ʼ��");
	m_listBoxShowLogInfo.AddString(strLog);

	//�������ID��Ӧ���浽�ṹ����
	m_NumToClassSet.iItemNum=0;
	m_menuAddDefect.GetSubMenu(0)->AppendMenuW(MF_STRING ,ID_ADDNEW, L"�����"); 
	m_NumToClassSet.Items[m_NumToClassSet.iItemNum].iId=ID_ADDNEW;
	m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass=L"�����";	
	strLog.Format(L"���ص�ȱ�����%s",m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass);
	m_listBoxShowLogInfo.AddString(strLog);
	m_NumToClassSet.iItemNum++;
	
	//�����ӵ��˵�
    CString strWildcard = m_SampleSetPathName;
	strWildcard += _T("\\*.*");
	CFileFind finder;
	BOOL bFind = finder.FindFile(strWildcard);
	int FistID=ID_ADDNEW;//�����˵�����׵�ַ
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			//�ҵ��ļ��е�����			
			CString strFileName = finder.GetFileName();		
			if(strFileName != L"�����")
			{
				m_menuAddDefect.GetSubMenu(0)->AppendMenuW(MF_STRING ,++FistID, strFileName); 
				m_NumToClassSet.Items[m_NumToClassSet.iItemNum].iId=FistID;
				m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass.Format(L"%s",strFileName);;
				strLog.Format(L"���ص�ȱ�����%s",m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass);
				m_listBoxShowLogInfo.AddString(strLog);
				m_NumToClassSet.iItemNum++;
			}
			continue;
		}
	} 
	finder.Close();

	strLog.Format(L"ȱ�����������ϣ�");
	m_listBoxShowLogInfo.AddString(strLog);
}

///��Ӧȱ��������ð�ť
void DlgSampleCollection::OnBnClickedButtonConfigdefectclass()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	INT_PTR nRes;             // ���ڱ���DoModal�����ķ���ֵ     
    DlgConfigDefectClass confDlg;           // ����Ի�����CTipDlg��ʵ��   
	confDlg.m_strSampleImgPath = m_SampleSetPathName;
	if(m_SampleSetPathName==L"")
	{
		AfxMessageBox(TEXT("��ѡ��������Ŀ¼"));
		return;
	}
    nRes = confDlg.DoModal();  // �����Ի���   
    if (IDOK == nRes)     // �ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL���������return�������������ִ��
	{
        for(int k=ID_ADDNEW;k<ID_ADDNEW+MAX_NUM_CLASS;k++)
		{
			m_menuAddDefect.GetSubMenu(0)->DeleteMenu(k,MF_BYCOMMAND);
		}
		//
		CString strLog;
		strLog.Format(L"ȱ��������ؿ�ʼ��");
		m_listBoxShowLogInfo.AddString(strLog);
		//�������ID��Ӧ���浽�ṹ����
		m_NumToClassSet.iItemNum=0;
		m_menuAddDefect.GetSubMenu(0)->AppendMenuW(MF_STRING ,ID_ADDNEW, L"�����"); 
		m_NumToClassSet.Items[m_NumToClassSet.iItemNum].iId=ID_ADDNEW;
		m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass=L"�����";
		strLog.Format(L"���ص�ȱ�����%s",m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass);
		m_listBoxShowLogInfo.AddString(strLog);
		m_NumToClassSet.iItemNum++;
		//�����ӵ��˵�
		CString strWildcard = m_SampleSetPathName;
		strWildcard += _T("\\*.*");
		CFileFind finder;
		BOOL bFind = finder.FindFile(strWildcard);
		int FistID=ID_ADDNEW;
		while (bFind)
		{
			bFind = finder.FindNextFile();
			if (finder.IsDots())
			{
				continue;
			}
			if (finder.IsDirectory())
			{
				//�ҵ��ļ��е�����			
				CString strFileName = finder.GetFileName();		
				if(strFileName != L"�����")
				{
					m_menuAddDefect.GetSubMenu(0)->AppendMenuW(MF_STRING ,++FistID, strFileName); 
					m_NumToClassSet.Items[m_NumToClassSet.iItemNum].iId=FistID;
					m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass.Format(L"%s",strFileName);
					strLog.Format(L"���ص�ȱ�����%s",m_NumToClassSet.Items[m_NumToClassSet.iItemNum].strClass);
					m_listBoxShowLogInfo.AddString(strLog);
					m_NumToClassSet.iItemNum++;
				}
				continue;
			}
		} 
		finder.Close();
		strLog.Format(L"ȱ�����������ϣ�");
		m_listBoxShowLogInfo.AddString(strLog);
	}
}
///������д�ļ�
void DlgSampleCollection::FindFileToTree(LPCTSTR lpszFileName,int iIndex)
{	
	if(iIndex>=5)
	{
		if(true==m_IsUnfold)
		{
			m_IsUnfold=false;
			AfxMessageBox(TEXT("Ŀ¼��ȳ���5�㣬�������ֲ���չ��"));
		}
		return;
	}
	
	CString strWildcard = lpszFileName;
	strWildcard += _T("\\*.*");

	CFileFind finder;
	BOOL bFind = FALSE;
	bFind = finder.FindFile(strWildcard);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			//�ҵ�Ŀ¼�ļ���·��
			CString strFilePath = finder.GetFilePath();
			//�ҵ�Ŀ¼�ļ�����
			CString strFileName = finder.GetFileName();
			m_hRoot[iIndex] = m_TreeImagePath.InsertItem(strFileName, iIndex, iIndex, m_hRoot[iIndex-1], TVI_LAST); 
			FindFileToTree(strFilePath,iIndex+1);
			continue;
		}

		//����ҵ��ļ�����Ŀ¼��������
		CString strFileName = finder.GetFileName();
		int n=strFileName.Find(L".");       //���ҵ�һ��","��λ��
		int m=strFileName.GetLength();
		CString strPostfix= strFileName.Right(m-n); 
		//�ж��Ƿ�Ϊbmp�ļ�
		if(strPostfix==L".bmp")
		{
			m_hRoot[iIndex] = m_TreeImagePath.InsertItem(strFileName, iIndex, iIndex, m_hRoot[iIndex-1], TVI_LAST); 
			m_TreeImagePath.SetItemData(m_hRoot[iIndex], ++m_treeNum);
		}
	} 
	finder.Close();
}
///�ı�������Ŀʱִ����ʾͼ��Ĺ���
void DlgSampleCollection::OnTvnSelchangedTreeImagepath(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	CString strText; // ���ڵ�ı�ǩ�ı��ַ���            
    HTREEITEM hItem = m_TreeImagePath.GetSelectedItem(); // ��ȡ��ǰѡ�нڵ�ľ��          
    strText = m_TreeImagePath.GetItemText(hItem);// ��ȡѡ�нڵ�ı�ǩ�ı��ַ���    
	//ѭ���ҵ����ո��ڵ㣬�ڵ�Ĳ����ı�ʱҪ����
	CString strTextTemp[5];
	for(int i=4;i>=0;i--)
	{
		HTREEITEM hItemParent=m_TreeImagePath.GetParentItem(hItem);
		if(NULL != hItemParent)
		{
			strTextTemp[i]=m_TreeImagePath.GetItemText(hItemParent);
			hItem=hItemParent;
		}
	}
	//�ڵ�ƴ���ҵ����յ�·��
	CString strImgPathNameTemp[5];
	CString strImgPathName=L"";
	for(int i=0;i<5;i++)
	{
		if(strTextTemp[i] != L"")
		{
			if(i>=1)
			{
				if(strImgPathNameTemp[i-1]==L"")
				{
					strImgPathNameTemp[i].Format(L"%s",strTextTemp[i]);
				}
				else
				{
					strImgPathNameTemp[i].Format(L"%s\\%s",strImgPathNameTemp[i-1],strTextTemp[i]);
				}
			}
		}
	}
	strImgPathName.Format(L"%s\\%s",strImgPathNameTemp[4],strText);
	if(strImgPathName.Find(L".bmp")>0)
	{
		ShowImgToMFCFromPath(IDC_IMAGESHOW,strImgPathName);
		//��ʾ��ͼʱ��ʼ��
		m_IndexToSitImgSet.iItemNum=0;
		m_ListImgSample.DeleteAllItems();
		int n=m_sampleImageList.GetImageCount();
		while(n!=0)
		{
			m_sampleImageList.Remove(--n);
		}
	}
}
///����·����ʾͼ��MFC��ͼ��ؼ���
void DlgSampleCollection::ShowImgToMFCFromPath(int ID,LPCTSTR lpszImgPathName)
{
	CDC* pDC = GetDlgItem(ID)->GetDC();  
	HDC hDC = pDC->GetSafeHdc(); 

	string strimagepath=CStringA(lpszImgPathName);
	m_grayImage=cv::imread(strimagepath,0);
	cvtColor(m_grayImage, m_colorImage, CV_GRAY2RGB);
	IplImage img = m_grayImage;  
	CvvImage cimg;  
	cimg.CopyOf( &img );  
	CRect rect;  
	GetDlgItem(ID)->GetClientRect(&rect);  
	cimg.DrawToHDC(hDC, &rect);  
	ReleaseDC( pDC ); 
}
///����Mat��ʾͼ��MFC��ͼ��ؼ���
void DlgSampleCollection::ShowImgToMFCFormMat(int ID,cv::Mat src)
{
	CDC* pDC = GetDlgItem(ID)->GetDC();  
	HDC hDC = pDC->GetSafeHdc(); 
	IplImage img = src;  
	CvvImage cimg;  
	cimg.CopyOf( &img );  
	CRect rect;  
	GetDlgItem(ID)->GetClientRect(&rect);  
	cimg.DrawToHDC(hDC, &rect);  
	ReleaseDC( pDC ); 
}
///�����ⲿdll�ļ�����ȱ���������ȡ
void DlgSampleCollection::OnBnClickedButtonAutoseg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ʼ���
	//m_ListImgSample.DeleteAllItems();
	//m_IndexToSitImgSet.iItemNum=0;
	//int n=m_sampleImageList.GetImageCount();
	//while(n!=0)
	//{
	//	m_sampleImageList.Remove(--n);
	//}
	//
	if(m_grayImage.rows>0 && m_grayImage.cols>0)
	{
		//
		PH_DefectSet pH_DefectSet=new H_DefectSet;
		pH_DefectSet->iItemNum=0;
		//ConfigInfo_Segmentation tConfigInfo_Segmentation={0};
		///�ָ�Dll�ļ��ӿ�
		CString strSelect;   
		int nSel = m_comboBoxSelectSegArith.GetCurSel();     
		m_comboBoxSelectSegArith.GetLBText(nSel, strSelect); 
		int iLength = WideCharToMultiByte(CP_ACP, 0, strSelect, -1, NULL, 0, NULL, NULL);  
		char cSelect[64]={0};
		WideCharToMultiByte(CP_ACP, 0, strSelect, -1, cSelect, iLength, NULL, NULL);	
		LOAD_PRODUCTLINE(ProductLine);
		int iProductLine=MY_DESCRIBE_BYDESC(ProductLine,cSelect);
		H_DefectDetectInterfaceFunc(iProductLine,m_grayImage.data,m_grayImage.cols,m_grayImage.rows,CArNTClassifierConsoleDlg::m_ConfigInfo_Segmentation,pH_DefectSet);
		///���ȱ������ͼ���
		int m=m_sampleImageList.GetImageCount();
		for(int n=0;n<pH_DefectSet->iItemNum;n++)
		{
			cv::Rect rect(pH_DefectSet->Items[n].iLeftInImage,pH_DefectSet->Items[n].iTopInImage,pH_DefectSet->Items[n].iRightInImage-pH_DefectSet->Items[n].iLeftInImage,pH_DefectSet->Items[n].iBottomInImage-pH_DefectSet->Items[n].iTopInImage);
			cv::Mat small_img,small_norm;
			m_grayImage(rect).copyTo(small_img);
			cv::resize(small_img,small_norm,cv::Size(64,64));
			cv::Mat small_norm_color;
			cvtColor(small_norm, small_norm_color, CV_GRAY2RGBA);
			//��ɫͼ����ʾ��ͼ��ؼ�
			CBitmap bitmap;
			m_hbitmap=CreateBitmap( small_norm_color.cols,small_norm_color.rows,1,32,small_norm_color.data);
			bitmap.Attach(m_hbitmap);
			m_sampleImageList.Add(&bitmap,RGB(0,0,0));
			m_ListImgSample.SetImageList(&m_sampleImageList, LVSIL_NORMAL);
			CString strNum=L"";
			strNum.Format(L"%d",n+m);
			m_ListImgSample.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, n+m, strNum, LVIS_SELECTED , LVIS_SELECTED, n+m, 0);

			string strnum=CStringA(strNum);
			cv::rectangle(m_colorImage,cv::Point(rect.tl().x,rect.tl().y),cv::Point(rect.br().x,rect.br().y),cv::Scalar(0,255,255),2);
			cv::putText(m_colorImage, strnum, cv::Point((rect.tl().x+rect.br().x)/2,(rect.tl().y+rect.br().y)/2), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 0, 255), 2);
			//��õ�ͼ���ŵ��ṹ��
			m_IndexToSitImgSet.Items[m_IndexToSitImgSet.iItemNum].iIndex=n+m;
			m_IndexToSitImgSet.Items[m_IndexToSitImgSet.iItemNum].rect=rect;
			m_IndexToSitImgSet.iItemNum++;
		}
		//��ʾ��ɫͼ��ͼ��ؼ�
		ShowImgToMFCFormMat(IDC_IMAGESHOW,m_colorImage);
	}
}
///˫������ͼ��ʱ������������
void DlgSampleCollection::OnLvnItemActivateListImgsample(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;	
	//�����
	CPoint point;
	GetCursorPos(&point);
	//�˵�ѡ���ID
	int id = m_menuAddDefect.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
	for(int i=0;i<m_NumToClassSet.iItemNum;i++)
	{
		//�Ա����Ǹ�ID��ѡ����
		if(id==m_NumToClassSet.Items[i].iId)
		{
			//ȷ�����ĸ�����ͼƬ��ѡ��
			POSITION pos = m_ListImgSample.GetFirstSelectedItemPosition();
			//��ȡ�и�ĻҶ�ͼ
			cv::Rect rect=m_IndexToSitImgSet.Items[(int)pos-1].rect;
			cv::Mat cutImg;
			m_grayImage(rect).copyTo(cutImg);		
			//�����и��ͼ��
			CString strSaveFilePath=L"";
			strSaveFilePath.Format(L"%s\\%s",m_SampleSetPathName,m_NumToClassSet.Items[i].strClass);
			int filenum=0;
			CFileFind finder;
			bool IsExist=finder.FindFile(strSaveFilePath);
			if(!IsExist)
			{
				CreateDirectory(strSaveFilePath, NULL);//û��Ŀ¼�򴴽�Ŀ¼����Ҫ�ǡ�����𡱵���
			}
			else
			{
				strSaveFilePath.Format(L"%s\\%s\\*.*",m_SampleSetPathName,m_NumToClassSet.Items[i].strClass);
				//�����ļ������м����ļ����Ա���ļ����б��
				bool bFind=finder.FindFile(strSaveFilePath);
				while (bFind)
				{
					bFind = finder.FindNextFile();
					if (finder.IsDots())
					{
						continue;
					}				
					filenum++;
				}
			}		
			//ȷ����������ͼƬ���ļ���
			CString strSaveFileImg=L"";
			CString strImgFileLabel=L"";
			strSaveFileImg.Format(L"%s\\%s\\%d.bmp",m_SampleSetPathName,m_NumToClassSet.Items[i].strClass,filenum+1);
			strImgFileLabel.Format(L"%s\\%d.bmp",m_NumToClassSet.Items[i].strClass,filenum+1);
			IsExist=finder.FindFile(strSaveFileImg);
			if(IsExist)//����������ļ�
			{
				int offset=random(100);
				strSaveFileImg.Format(L"%s\\%s\\%d_%d.bmp",m_SampleSetPathName,m_NumToClassSet.Items[i].strClass,filenum+1,offset);
				strImgFileLabel.Format(L"%s\\%d_%d.bmp",m_NumToClassSet.Items[i].strClass,filenum+1,offset);
			}
			//��ʽ����ͼƬ
			string strsaveimage=CStringA(strSaveFileImg);
			IplImage* cutimage=&(IplImage)cutImg;
			const char* p=strsaveimage.data();
			cvSaveImage(p,cutimage);
			//��������ͼ���б����ʾ
			CString strNum=L"";
			strNum.Format(L"%d_AddTo_%s",(int)pos-1,strImgFileLabel);
			m_ListImgSample.SetItem((int)pos-1,0,LVIF_TEXT|LVIF_IMAGE|LVIF_STATE,strNum,(int)pos-1, LVIS_SELECTED , LVIS_SELECTED,0);
			finder.Close();
		}
	}
}
void DlgSampleCollection::OnCbnSelchangeComboTestFunc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strImgProcessFunc;   
    int nSel= m_comboTestImageProcessFunc.GetCurSel();    
    m_comboTestImageProcessFunc.GetLBText(nSel, strImgProcessFunc); 
	char cstrImgProcessFunc[128];
	int iLength = WideCharToMultiByte(CP_ACP, 0, strImgProcessFunc, -1, NULL, 0, NULL, NULL);  
	WideCharToMultiByte(CP_ACP, 0, strImgProcessFunc, -1, cstrImgProcessFunc, iLength, NULL, NULL);
	if(m_grayImage.rows>0 && m_grayImage.cols>0)
	{
		H_TestImageProcessFunc(m_grayImage,cstrImgProcessFunc);
	}
}

void DlgSampleCollection::OnLButtonDown(UINT nFlags, CPoint point)
{
	///����һ
	m_iMouseMode=1;
	m_bIsEffectMove=false;
	m_ptBegin=m_ptEnd=point;
	///������

	CDialog::OnLButtonDown(nFlags, point);
}

void DlgSampleCollection::OnMouseMove(UINT nFlags, CPoint point)
{
	///����һ	
	if(m_grayImage.rows>0 && m_grayImage.cols>0)
	{
		if(m_ptBegin.x>m_ImgRegion.left && m_ptBegin.x<m_ImgRegion.right && m_ptBegin.y>m_ImgRegion.top && m_ptBegin.y<m_ImgRegion.bottom)
		{
			if(point.x>m_ImgRegion.left && point.x<m_ImgRegion.right && point.y>m_ImgRegion.top && point.y<m_ImgRegion.bottom)
			{
				if(m_iMouseMode==1)
				{
					//��������ƶ���Χ
					CRect rcClient;
					GetDlgItem(IDC_IMAGESHOW)->GetWindowRect(&rcClient);
					ClipCursor(rcClient);
					//
					CClientDC dc(this);
					//CRect rcClient;
					//GetDlgItem(IDC_IMAGESHOW)->GetWindowRect(&rcClient);
					//ScreenToClient(&rcClient);
					//dc.FillSolidRect(rcClient, RGB(255, 255, 255));
					ShowImgToMFCFormMat(IDC_IMAGESHOW,m_colorImage);
					dc.SelectStockObject(NULL_BRUSH);
					CPen pen(PS_SOLID, 1, RGB(255, 255, 0));
					CRect rect(m_ptBegin, point);
					dc.SelectObject(&pen);
					dc.Rectangle(rect);
					m_bIsEffectMove=true;
					//Invalidate();
				}
				//			
				float fScale_x=(float)(m_grayImage.cols)/(m_ImgRegion.right-m_ImgRegion.left);
				float fScale_y=(float)(m_grayImage.rows)/(m_ImgRegion.bottom-m_ImgRegion.top);
				int iX=(point.x-m_ImgRegion.left)*fScale_x;
				int iY=(point.y-m_ImgRegion.top)*fScale_y;
				CString strPos;
				strPos.Format(L"X:%d-Y:%d",iX,iY);
				GetDlgItem(IDC_STATIC_IMG_POS)->SetWindowTextW(strPos);
			}
		}
	}
	///������

	CDialog::OnMouseMove(nFlags, point);
}

void DlgSampleCollection::OnLButtonUp(UINT nFlags, CPoint point)
{
	///����һ
	m_iMouseMode=0;
	ClipCursor(NULL);
	if(true==m_bIsEffectMove && m_grayImage.rows>0 && m_grayImage.cols>0)
	{
		if(m_ptBegin.x>m_ImgRegion.left && m_ptBegin.x<m_ImgRegion.right && m_ptBegin.y>m_ImgRegion.top && m_ptBegin.y<m_ImgRegion.bottom)
		{
			if(point.x>m_ImgRegion.left && point.x<m_ImgRegion.right && point.y>m_ImgRegion.top && point.y<m_ImgRegion.bottom)
			{
				m_ptEnd=point;
				if(abs(m_ptEnd.x-m_ptBegin.x)>10 && abs(m_ptEnd.y-m_ptBegin.y)>10)
				{
					CClientDC dc(this);
					CBrush *pBrush=CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
					CBrush *pOldBrush=dc.SelectObject(pBrush);
					CPen newPen;       // ���ڴ����»���    
					CPen *pOldPen;     // ���ڴ�žɻ���  
					newPen.CreatePen(PS_SOLID, 1, RGB(0,255,0)); // ����ʵ�Ļ��ʣ��ֶ�Ϊ1����ɫΪ��ɫ       
					pOldPen = dc.SelectObject(&newPen); // ѡ���»��ʣ������ɻ��ʵ�ָ�뱣�浽pOldPen 

					int iOldMode=dc.SetROP2(R2_NOTXORPEN);
					dc.Rectangle(&CRect(m_ptBegin,m_ptEnd));
					dc.SelectObject(pOldBrush);
					dc.SetROP2(iOldMode);

					dc.SelectObject(pOldBrush);      
					pBrush->DeleteObject();// ɾ���»�ˢ 
					dc.SelectObject(pOldPen); // �ָ��ɻ���  
					newPen.DeleteObject(); 

					float fScale_x=(float)(m_grayImage.cols)/(m_ImgRegion.right-m_ImgRegion.left);
					float fScale_y=(float)(m_grayImage.rows)/(m_ImgRegion.bottom-m_ImgRegion.top);
					int iLeft=min((m_ptBegin.x-m_ImgRegion.left),(m_ptEnd.x-m_ImgRegion.left))*fScale_x;
					int iRight=max((m_ptBegin.x-m_ImgRegion.left),(m_ptEnd.x-m_ImgRegion.left))*fScale_x;
					int iTop=min((m_ptBegin.y-m_ImgRegion.top),(m_ptEnd.y-m_ImgRegion.top))*fScale_y;
					int iBottom=max((m_ptBegin.y-m_ImgRegion.top),(m_ptEnd.y-m_ImgRegion.top))*fScale_y;
					//����ͼ���
					int n=m_sampleImageList.GetImageCount();
					cv::Rect rect(iLeft,iTop,iRight-iLeft,iBottom-iTop);
					cv::Mat small_img,small_norm;
					m_grayImage(rect).copyTo(small_img);
					cv::resize(small_img,small_norm,cv::Size(64,64));
					cv::Mat small_norm_color;
					cvtColor(small_norm, small_norm_color, CV_GRAY2RGBA);
					//��ɫͼ����ʾ��ͼ��ؼ�
					CBitmap bitmap;
					m_hbitmap=CreateBitmap( small_norm_color.cols,small_norm_color.rows,1,32,small_norm_color.data);
					bitmap.Attach(m_hbitmap);
					m_sampleImageList.Add(&bitmap,RGB(0,0,0));
					m_ListImgSample.SetImageList(&m_sampleImageList, LVSIL_NORMAL);
					CString strNum=L"";
					strNum.Format(L"%d",n);
					m_ListImgSample.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, n, strNum, LVIS_SELECTED , LVIS_SELECTED, n, 0);

					//string strnum=CStringA(strNum);
					//cv::rectangle(m_colorImage,cv::Point(rect.tl().x,rect.tl().y),cv::Point(rect.br().x,rect.br().y),cv::Scalar(0,255,255),2);
					//cv::putText(m_colorImage, strnum, cv::Point((rect.tl().x+rect.br().x)/2,(rect.tl().y+rect.br().y)/2), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 0, 255), 2);
					//��õ�ͼ���ŵ��ṹ��
					m_IndexToSitImgSet.Items[m_IndexToSitImgSet.iItemNum].iIndex=n;
					m_IndexToSitImgSet.Items[m_IndexToSitImgSet.iItemNum].rect=rect;
					m_IndexToSitImgSet.iItemNum++;
				}
			}
			else
			{
				//ShowImgToMFCFormMat(IDC_IMAGESHOW,m_colorImage);
			}
		}
		else
		{
			//ShowImgToMFCFormMat(IDC_IMAGESHOW,m_colorImage);
		}
	}
	///������

	CDialog::OnLButtonUp(nFlags, point);
}

BOOL DlgSampleCollection::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return CDialog::OnEraseBkgnd(pDC);
}
