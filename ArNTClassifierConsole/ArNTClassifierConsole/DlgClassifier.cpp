// DlgClassifier.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifier.h"
#include "H_DefectDetectInterface.h"
#include "ArNTClassifierConsoleDlg.h"

// DlgClassifier �Ի���
#define random(x) (rand()%x)

long g_lInsertIndex=0;//������־������
vector<float> svec(1,0);//����Ϊ�����ṹ���ʼ��
SampleImgSet DlgClassifier::m_TrainImgSet={0,0,cv::Mat::zeros(32,32,CV_8U),svec};
SampleImgSet DlgClassifier::m_TestImgSet={0,0,cv::Mat::zeros(32,32,CV_8U),svec};


//���̺߳���
UINT ThreadProcDefectClassify(LPVOID pParam)
{
	ThreadParam* pThreadParam = (ThreadParam*)pParam;
	while (!pThreadParam->bExit)
	{
		//CWnd m_Wnd;
		//m_Wnd.Attach(m_pParam->m_hWnd);
		::PostMessage(pThreadParam->hWnd, WM_THREAD_DEFECT_CLASSIFY, 0, 0);
		Sleep(5000);
		pThreadParam->bExit=true;
		//
	}
	::PostMessage(pThreadParam->hWnd, WM_THREAD_DEFECT_CLASSIFY_CLOSE, 0, 0);///Ӧ�üӻ���
	return 0;
}

IMPLEMENT_DYNAMIC(DlgClassifier, CDialog)

DlgClassifier::DlgClassifier(CWnd* pParent /*=NULL*/)
	: CDialog(DlgClassifier::IDD, pParent)
{
	DlgClassifier::m_TrainImgSet.iItemNum=0;
	DlgClassifier::m_TestImgSet.iItemNum=0;
	
	m_TrainSetImageList.Create(64, 64, ILC_COLOR32, 3, 3); //|ILC_MASK���ѵ��������ͼ��
	m_TestSetImageList.Create(64, 64, ILC_COLOR32, 3, 3); //|ILC_MASK��Ų��Լ�����ͼ��

	memset(m_iTrainClassNum,0,sizeof(m_iTrainClassNum));//��ʼ��
	memset(m_iTestClassNum,0,sizeof(m_iTestClassNum));
	memset(m_iExterToInter,0,sizeof(m_iExterToInter));
	
	m_ClassIndexNum=0;

	m_dlgImageShow=NULL;
}

DlgClassifier::~DlgClassifier()
{
}

void DlgClassifier::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IMGSIZE, m_ComboImgNormSize);
	DDX_Control(pDX, IDC_PROGRESS_LOADINGSAMPLEIMG, m_ProgressLoadingSampleImg);
	DDX_Control(pDX, IDC_CHECK_NORMIMGSIZE, m_checkNorm);
	DDX_Control(pDX, IDC_LIST_FEAT_FROM_CHOOSE, m_listBoxFeatFrom);
	DDX_Control(pDX, IDC_LIST_FEAT_TO_CHOOSE, m_listBoxFeatTo);
	DDX_Control(pDX, IDC_TAB_CLASSIFIER, m_tabCtrlClassifier);
	DDX_Control(pDX, IDC_CHECK_AUTO_GET_FEAT, m_checkBoxAutoGetFeat);
	DDX_Control(pDX, IDC_COMBO_TESTSET_PERCENT, m_camboTestSetPercent);
	DDX_Control(pDX, IDC_COMBO_TRAIN_CLASSNUM, m_comboTrainClassNum);
	DDX_Control(pDX, IDC_COMBO_TEST_CLASSNUM, m_comboTestClassNum);
	DDX_Control(pDX, IDC_LIST_TRAINIMGSHOW, m_listTrainImgShow);
	DDX_Control(pDX, IDC_LIST_TESTIMGSHOW, m_listTestImgShow);
	DDX_Control(pDX, IDC_LIST_SHOWINFO, m_ListShowInfo);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_staticResultShow);
	DDX_Control(pDX, IDC_PIC_RESULTSHOW, m_picResuleShow);
	DDX_Control(pDX, IDC_EDIT_RESULT_ACCURACY, m_editResultAccuracy);
}


BEGIN_MESSAGE_MAP(DlgClassifier, CDialog)
	ON_MESSAGE(WM_THREAD_DEFECT_CLASSIFY, OnMsgFuncDefectClassify)
	ON_MESSAGE(WM_THREAD_DEFECT_CLASSIFY_CLOSE, OnMsgFuncDefectClassifyClose)
	ON_BN_CLICKED(IDC_BUTTON_GET_IMGSAMPLEPATH, &DlgClassifier::OnBnClickedButtonGetImgsamplepath)
	ON_CBN_SELCHANGE(IDC_COMBO_IMGSIZE, &DlgClassifier::OnCbnSelchangeComboImgsize)
	ON_BN_CLICKED(IDC_BUTTON_LOADING_SAMPLEIMG, &DlgClassifier::OnBnClickedButtonLoadingSampleimg)
	ON_BN_CLICKED(IDC_BUTTON_FEAT_ADD, &DlgClassifier::OnBnClickedButtonFeatAdd)
	ON_BN_CLICKED(IDC_BUTTON_FEAT_SUB, &DlgClassifier::OnBnClickedButtonFeatSub)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CLASSIFIER, &DlgClassifier::OnTcnSelchangeTabClassifier)
	ON_BN_CLICKED(IDC_CHECK_AUTO_GET_FEAT, &DlgClassifier::OnBnClickedCheckAutoGetFeat)
	ON_BN_CLICKED(IDC_BUTTON_GET_FEAT, &DlgClassifier::OnBnClickedButtonGetFeat)
	ON_CBN_SELCHANGE(IDC_COMBO_TESTSET_PERCENT, &DlgClassifier::OnCbnSelchangeComboTestsetPercent)
	ON_CBN_SELCHANGE(IDC_COMBO_TRAIN_CLASSNUM, &DlgClassifier::OnCbnSelchangeComboTrainClassnum)
	ON_CBN_SELCHANGE(IDC_COMBO_TEST_CLASSNUM, &DlgClassifier::OnCbnSelchangeComboTestClassnum)
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_CLASSIFIER, &DlgClassifier::OnBnClickedButtonTrainClassifier)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CLASSIFIER, &DlgClassifier::OnBnClickedButtonTestClassifier)
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_AND_TEST_CLASSIFIER, &DlgClassifier::OnBnClickedButtonTrainAndTestClassifier)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_TRAINIMGSHOW, &DlgClassifier::OnLvnItemActivateListTrainimgshow)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_SHOWCLASSIFY_RESULT, &DlgClassifier::OnBnClickedButtonShowclassifyResult)
	
END_MESSAGE_MAP()


// DlgClassifier ��Ϣ�������
BOOL DlgClassifier::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//����ͼ��淶����С
	m_ComboImgNormSize.AddString(L"16x16");
	m_ComboImgNormSize.AddString(L"32x16");
	m_ComboImgNormSize.AddString(L"16x32");
	m_ComboImgNormSize.AddString(L"32x32");
	m_ComboImgNormSize.AddString(L"32x64");
	m_ComboImgNormSize.AddString(L"64x32");
	m_ComboImgNormSize.AddString(L"64x64");
	m_ComboImgNormSize.AddString(L"64x128");
	m_ComboImgNormSize.AddString(L"128x64");
	m_ComboImgNormSize.AddString(L"128x128");
	m_ComboImgNormSize.AddString(L"256x128");
	m_ComboImgNormSize.AddString(L"128x256");
	m_ComboImgNormSize.AddString(L"256x256");
	m_ComboImgNormSize.SetCurSel(6);
	m_iNormWidth=64;
	m_iNormHeight=64;
	//����������ռ����
	m_camboTestSetPercent.AddString(L"10%");
	m_camboTestSetPercent.AddString(L"15%");
	m_camboTestSetPercent.AddString(L"20%");
	m_camboTestSetPercent.AddString(L"25%");
	m_camboTestSetPercent.AddString(L"30%");
	m_camboTestSetPercent.AddString(L"35%");
	m_camboTestSetPercent.AddString(L"40%");
	m_camboTestSetPercent.AddString(L"45%");
	m_camboTestSetPercent.AddString(L"50%");
	m_camboTestSetPercent.SetCurSel(4);
	m_fTestSetPercent=0.3;
	//����ͼ��Ľ���
	m_ProgressLoadingSampleImg.SetRange(0,100);
	m_ProgressLoadingSampleImg.SetStep(10);
	//�Ƿ�淶��
	m_checkNorm.SetCheck(1);
	//�����б�
	m_listBoxFeatFrom.AddString(L"Feat_LBP");
	m_listBoxFeatFrom.AddString(L"Feat_Sobel");
	m_listBoxFeatFrom.AddString(L"Feat_HOG");
	m_listBoxFeatFrom.AddString(L"Feat_Haar");
	m_listBoxFeatFrom.AddString(L"Feat_Moment1");
	m_listBoxFeatFrom.AddString(L"Feat_Moment2");
	m_listBoxFeatFrom.AddString(L"Feat_Moment3");
	m_listBoxFeatFrom.AddString(L"Feat_Original");
	m_listBoxFeatFrom.AddString(L"Feat_Histogram");
	m_listBoxFeatFrom.AddString(L"Feat_CLCM");
	m_listBoxFeatFrom.AddString(L"Feat_Suft");
	m_listBoxFeatFrom.AddString(L"Feat_Sift");
	m_listBoxFeatFrom.AddString(L"Feat_Tamura");
	m_listBoxFeatFrom.AddString(L"Feat_Spectrum");
	m_listBoxFeatFrom.AddString(L"Feat_Shape");
	//�������б�
	m_tabCtrlClassifier.InsertItem(0, _T("BP_Classifier"));  
	m_tabCtrlClassifier.InsertItem(1, _T("SVM_Classifier")); 
	m_tabCtrlClassifier.InsertItem(2, _T("TREE_Classifier")); 
	m_tabCtrlClassifier.InsertItem(3, _T("Boosting_Classifier")); 
	m_tabCtrlClassifier.InsertItem(4, _T("CNN_Classifier")); 
    m_dlgClassifierBP.Create(IDD_DIALOG_CLASSIFIER_BP,&m_tabCtrlClassifier);
	m_dlgClassifierSVM.Create(IDD_DIALOG_CLASSIFIER_SVM,&m_tabCtrlClassifier); 
	m_dlgClassifierTREE.Create(IDD_DIALOG_CLASSIFIER_TREE,&m_tabCtrlClassifier); 
	m_dlgClassifierBoosting.Create(IDD_DIALOG_CLASSIFIER_BOOSTING,&m_tabCtrlClassifier); 
	m_dlgClassifierCNN.Create(IDD_DIALOG_CLASSIFIER_CNN,&m_tabCtrlClassifier); 
	CRect classifierTabRect;   // ��ǩ�ؼ��ͻ�����λ�úʹ�С 
    m_tabCtrlClassifier.GetClientRect(&classifierTabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   
    classifierTabRect.left += 1;                  
    classifierTabRect.right -= 1;   
    classifierTabRect.top += 1;   
    classifierTabRect.bottom -= 20; 
	m_dlgClassifierBP.MoveWindow(&classifierTabRect);
	m_dlgClassifierSVM.MoveWindow(&classifierTabRect);
	m_dlgClassifierTREE.MoveWindow(&classifierTabRect);
	m_dlgClassifierBoosting.MoveWindow(&classifierTabRect);
	m_dlgClassifierCNN.MoveWindow(&classifierTabRect);
	m_dlgClassifierBP.ShowWindow(true);
	m_dlgClassifierSVM.ShowWindow(false);
	m_dlgClassifierTREE.ShowWindow(false);
	m_dlgClassifierBoosting.ShowWindow(false);
	m_dlgClassifierCNN.ShowWindow(false);

	//��ȡӦ�ó���·��
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	CString strFileName = szFileName;
	int nIndex = strFileName.ReverseFind('\\');
	m_strAppPath = strFileName.Left(nIndex);

	//�õ�ini�����ļ�·��
	m_FileIniOper.m_strFileName.Format(L"%s\\ClassifierInteraction_tmp.ini",m_strAppPath);
	::DeleteFile(m_FileIniOper.m_strFileName);
	m_strIniFileName.Format(L"%s\\ClassifierInteraction.ini",m_strAppPath);

	//�����õ�������ť���ص�
	GetDlgItem(IDC_BUTTON_TRAIN_CLASSIFIER)->ShowWindow(false);
	GetDlgItem(IDC_BUTTON_TEST_CLASSIFIER)->ShowWindow(false);
	//
	 
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
///���������ͼ��ť
void DlgClassifier::OnBnClickedButtonGetImgsamplepath()
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
		SetDlgItemText(IDC_EDIT_SAMPLEIMGPATH, m_SampleSetPathName);   
		m_ListShowInfo.InsertString(g_lInsertIndex++,m_SampleSetPathName);
    }  
    else 
	{
        AfxMessageBox(TEXT("��Ч��Ŀ¼��������ѡ��"));
		return;
	}
}
///�����������ͼ��ť
void DlgClassifier::OnBnClickedButtonLoadingSampleimg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ʼ���
	DlgClassifier::m_TrainImgSet.iItemNum=0;
	DlgClassifier::m_TestImgSet.iItemNum=0;
	m_ClassIndexNum=0;//�ڼ���ʱҪ��ʼΪ0
	m_iTestSetBeginIndex=random(10);
	m_iSampleSetAllImgNum=0;
	//����ļ��Ƿ����Ҫ��
	bool bIsOK=CheckSampleFilePath();
	if(true==bIsOK)
	{
		//����ͼ�����ݵ��ṹ��
		WriteSampleImageToTag(m_SampleSetPathName,-1);
		//��ʾͼ���б�
		ShowSampleImageToList();
		//��ʾ��Ϣ
		CString strInfo;
		if(1==m_checkNorm.GetCheck())
		{					
			strInfo.Format(L"�淶��������СΪ%dx%d",m_iNormWidth,m_iNormHeight);
			m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
		}
		else
		{
			m_ListShowInfo.InsertString(g_lInsertIndex++,L"��������ԭʼ��С");
		}
		strInfo.Format(L"����������ռ���أ�%f",m_fTestSetPercent);
		m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);	
	}
}
///��֤����ļ���
bool DlgClassifier::CheckSampleFilePath()
{
	//����Ŀ¼�������ļ����������ϵ�����ļ�ɾ��
	CString strWildcard;
	strWildcard.Format(L"%s\\*.*",m_SampleSetPathName);
	CFileFind finder;
	BOOL bFind = finder.FindFile(strWildcard);
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
			bool isEffect=false;
			for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.iItemNum;i++)
			{
				USES_CONVERSION;
				CString strClassName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[i].strClassName);
				if(strFileName.Compare(strClassName)==0)
				{
					isEffect=true;
				}
			}
			if(!isEffect)
			{
				CString strWarn;
				strWarn.Format(L">>���(%s)Ϊδ�������!\n>>���(��)���ļ��н���ɾ��!\n>>���(��)�ɷ�����������!",strFileName);
				UINT nRet=MessageBox(strWarn,_T("��ʾ��Ϣ"),MB_YESNO);
				if(nRet == IDYES)
				{
					DeleteTree(strFilePath);
				}
				else
				{
					return false;
				}
			}
		}
		CString strFilePath = finder.GetFilePath();
		::DeleteFile(strFilePath);
	} 
	finder.Close();
	return true;
}
bool DlgClassifier::DeleteTree(CString strDirectory)
{
	CString strWildcard = strDirectory;
	strWildcard += _T("\\*.*");

	CFileFind finder;
	BOOL bFind = finder.FindFile(strWildcard);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		CString strPathName = finder.GetFilePath();
		if (finder.IsDirectory())
		{		
			if (!DeleteTree(strPathName))//�ݹ�ɾ��Ŀ¼
			{
				return FALSE;
			}
		}
		else
		{
			if (!::DeleteFile(strPathName))
			{
				return FALSE;
			}		
		}
	}
	finder.Close();
	//ɾ����Ŀ¼
	if (!::RemoveDirectory(strDirectory))
	{
		return FALSE;
	}
	return TRUE;
}
///��������ͼ�񵽽ṹ��
void DlgClassifier::WriteSampleImageToTag(CString strPath,int iInClass)
{
	strPath += _T("\\*.*");
	CFileFind finder;
	BOOL bFind = finder.FindFile(strPath);
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
			CString strFilePath = finder.GetFilePath();
			CString strFileName = finder.GetFileName();
			for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.iItemNum;i++)
			{
				USES_CONVERSION;
				CString strClassName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[i].strClassName);
				if(strFileName.Compare(strClassName)==0)
				{
					iInClass=CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[i].iInternalClassNo;
				}
			}			
			WriteSampleImageToTag(strFilePath,iInClass);
			m_iExterToInter[m_ClassIndexNum]=iInClass;
			m_ClassIndexNum++;
			continue;
		}
		CString strFileName = finder.GetFileName();
		CString strFilePath = finder.GetFilePath();
		int n=strFileName.Find(L".");       //���ҵ�һ��","��λ��
		int m=strFileName.GetLength();
		CString strPostfix= strFileName.Right(m-n); 
		//�ж��Ƿ�Ϊbmp�ļ�
		if(strPostfix==L".bmp")
		{
			string strimagepath=CStringA(strFilePath);
			cv::Mat src=cv::imread(strimagepath,0);
			if(1==m_checkNorm.GetCheck())
			{			
				cv::resize(src,src,cv::Size(m_iNormWidth,m_iNormHeight));
			}
			//�����ֿ�Ϊѵ�����Ͳ��Լ�
			int iInterval=1/m_fTestSetPercent;
			int iTestImgIndex=(m_iSampleSetAllImgNum-m_iTestSetBeginIndex)%iInterval;
			if(0==iTestImgIndex)
			{
				DlgClassifier::m_TestImgSet.Items[DlgClassifier::m_TestImgSet.iItemNum].iExternalClassNo=m_ClassIndexNum;
				DlgClassifier::m_TestImgSet.Items[DlgClassifier::m_TestImgSet.iItemNum].iInternalClassNo=iInClass;
				DlgClassifier::m_TestImgSet.Items[DlgClassifier::m_TestImgSet.iItemNum].img=src.clone();
				DlgClassifier::m_TestImgSet.iItemNum++;
			}
			else
			{
				DlgClassifier::m_TrainImgSet.Items[DlgClassifier::m_TrainImgSet.iItemNum].iExternalClassNo=m_ClassIndexNum;
				DlgClassifier::m_TrainImgSet.Items[DlgClassifier::m_TrainImgSet.iItemNum].iInternalClassNo=iInClass;
				DlgClassifier::m_TrainImgSet.Items[DlgClassifier::m_TrainImgSet.iItemNum].img=src.clone();
				DlgClassifier::m_TrainImgSet.iItemNum++;
			}
			//�ܵ�����ͼ��
			m_iSampleSetAllImgNum++;
		}
	} 
	finder.Close();
}
///��ʾѵ�����Ͳ��Լ�����ͼ���б�
void DlgClassifier::ShowSampleImageToList()
{
	//��ʼ�����
	m_listTrainImgShow.DeleteAllItems();
	m_listTestImgShow.DeleteAllItems();

	m_comboTrainClassNum.ResetContent();
	m_comboTestClassNum.ResetContent();

	int m_train=m_TrainSetImageList.GetImageCount();
	int m_test=m_TestSetImageList.GetImageCount();
	while(m_train!=0)
	{
		m_TrainSetImageList.Remove(--m_train);
	}
	while(m_test!=0)
	{
		m_TestSetImageList.Remove(--m_test);
	}
	memset(m_iTrainClassNum,0,sizeof(m_iTrainClassNum));//��ʼ��
	memset(m_iTestClassNum,0,sizeof(m_iTestClassNum));
	//ѵ����
	float ini_train=0;
	for(int n=0;n<DlgClassifier::m_TrainImgSet.iItemNum;n++)
	{
		//��ʾͼ��ʱ����Ҫ��һ��������Ҫ��ɫͼƬ
		cv::Mat img_norm;
		cv::resize(DlgClassifier::m_TrainImgSet.Items[n].img,img_norm,cv::Size(64,64));
		cv::Mat img_norm_color;
		cvtColor(img_norm, img_norm_color, CV_GRAY2RGBA);
		CBitmap bitmap;
		m_hbitmap=CreateBitmap( img_norm_color.cols,img_norm_color.rows,1,32,img_norm_color.data);
		bitmap.Attach(m_hbitmap);
		m_TrainSetImageList.Add(&bitmap,RGB(0,0,0));
		m_listTrainImgShow.SetImageList(&m_TrainSetImageList, LVSIL_NORMAL);
		CString strNum=L"";
		strNum.Format(L"TrainClass%d",DlgClassifier::m_TrainImgSet.Items[n].iExternalClassNo);
		m_listTrainImgShow.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, n, strNum, LVIS_SELECTED , LVIS_SELECTED, n, 0);

		m_iTrainClassNum[DlgClassifier::m_TrainImgSet.Items[n].iExternalClassNo]++;
		//���ý�����
		float pro=(float)n/m_TrainImgSet.iItemNum;
		if(pro-ini_train>0.1)
		{
			ini_train=pro;
			m_ProgressLoadingSampleImg.StepIt();
		}
	}
	for(int i=0;i<m_ClassIndexNum;i++)
	{
		USES_CONVERSION;
		CString strClassName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[m_iExterToInter[i]].strClassName);
		CString strNum;
		strNum.Format(L"%d--%d@%s",i,m_iTrainClassNum[i],strClassName);
		m_comboTrainClassNum.InsertString(i,strNum);
	}
	CString strAll_train;
	strAll_train.Format(L"<%d>-<%d>@%s",m_ClassIndexNum,DlgClassifier::m_TrainImgSet.iItemNum,L"�������");
	m_comboTrainClassNum.InsertString(m_ClassIndexNum,strAll_train);
	m_comboTrainClassNum.SetCurSel(m_ClassIndexNum);
	//���Լ�
	float ini_test=0;
	for(int n=0;n<DlgClassifier::m_TestImgSet.iItemNum;n++)
	{
		//��ʾͼ��ʱ����Ҫ��һ��������Ҫ��ɫͼƬ
		cv::Mat img_norm;
		cv::resize(DlgClassifier::m_TestImgSet.Items[n].img,img_norm,cv::Size(64,64));
		cv::Mat img_norm_color;
		cvtColor(img_norm, img_norm_color, CV_GRAY2RGBA);
		CBitmap bitmap;
		m_hbitmap=CreateBitmap( img_norm_color.cols,img_norm_color.rows,1,32,img_norm_color.data);
		bitmap.Attach(m_hbitmap);
		m_TestSetImageList.Add(&bitmap,RGB(0,0,0));
		m_listTestImgShow.SetImageList(&m_TestSetImageList, LVSIL_NORMAL);
		CString strNum=L"";
		strNum.Format(L"TestClass%d",DlgClassifier::m_TestImgSet.Items[n].iExternalClassNo);
		m_listTestImgShow.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, n, strNum, LVIS_SELECTED , LVIS_SELECTED, n, 0);

		m_iTestClassNum[DlgClassifier::m_TestImgSet.Items[n].iExternalClassNo]++;
		//��ʾ������
		float pro=(float)n/m_TestImgSet.iItemNum;
		if(pro-ini_test>0.1)
		{
			ini_test=pro;
		}
	}
	m_ProgressLoadingSampleImg.StepIt();
	for(int i=0;i<m_ClassIndexNum;i++)
	{
		USES_CONVERSION;
		CString strClassName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[m_iExterToInter[i]].strClassName);
		CString strNum;
		strNum.Format(L"%d--%d@%s",i,m_iTestClassNum[i],strClassName);
		m_comboTestClassNum.InsertString(i,strNum);
	}
	CString strAll_test;
	strAll_test.Format(L"<%d>-<%d>@%s",m_ClassIndexNum,DlgClassifier::m_TestImgSet.iItemNum,L"�������");
	m_comboTestClassNum.InsertString(m_ClassIndexNum,strAll_test);
	m_comboTestClassNum.SetCurSel(m_ClassIndexNum);
	//
}
///ѵ�����л�ÿ���������ʱ
void DlgClassifier::OnCbnSelchangeComboTrainClassnum()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ʼ���
	m_listTrainImgShow.DeleteAllItems();
	int n=m_TrainSetImageList.GetImageCount();
	while(n!=0)
	{
		m_TrainSetImageList.Remove(--n);
	}
	//�õ���ǰ
	CString strSelect;   
    int nSel = m_comboTrainClassNum.GetCurSel();     
    m_comboTrainClassNum.GetLBText(nSel, strSelect);  
	for(int n=0,i=0;n<DlgClassifier::m_TrainImgSet.iItemNum;n++)
	{
		if(nSel==m_ClassIndexNum)
		{
			cv::Mat img_norm;
			cv::resize(DlgClassifier::m_TrainImgSet.Items[n].img,img_norm,cv::Size(64,64));
			cv::Mat img_norm_color;
			cvtColor(img_norm, img_norm_color, CV_GRAY2RGBA);
			CBitmap bitmap;
			m_hbitmap=CreateBitmap( img_norm_color.cols,img_norm_color.rows,1,32,img_norm_color.data);
			bitmap.Attach(m_hbitmap);
			m_TrainSetImageList.Add(&bitmap,RGB(0,0,0));
			m_listTrainImgShow.SetImageList(&m_TrainSetImageList, LVSIL_NORMAL);
			CString strNum=L"";
			strNum.Format(L"Class%d",DlgClassifier::m_TrainImgSet.Items[n].iExternalClassNo);
			m_listTrainImgShow.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, n, strNum, LVIS_SELECTED , LVIS_SELECTED, n, 0);
		}
		if(DlgClassifier::m_TrainImgSet.Items[n].iExternalClassNo==nSel)
		{
			cv::Mat img_norm;
			cv::resize(DlgClassifier::m_TrainImgSet.Items[n].img,img_norm,cv::Size(64,64));
			cv::Mat img_norm_color;
			cvtColor(img_norm, img_norm_color, CV_GRAY2RGBA);
			CBitmap bitmap;
			m_hbitmap=CreateBitmap( img_norm_color.cols,img_norm_color.rows,1,32,img_norm_color.data);
			bitmap.Attach(m_hbitmap);
			m_TrainSetImageList.Add(&bitmap,RGB(0,0,0));
			m_listTrainImgShow.SetImageList(&m_TrainSetImageList, LVSIL_NORMAL);
			CString strNum=L"";
			strNum.Format(L"Class%d",DlgClassifier::m_TrainImgSet.Items[n].iExternalClassNo);
			m_listTrainImgShow.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, i, strNum, LVIS_SELECTED , LVIS_SELECTED, i, 0);
			i++;
		}
	}
}
///���Լ��л�ÿ���������ʱ
void DlgClassifier::OnCbnSelchangeComboTestClassnum()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSelect;   
    int nSel = m_comboTestClassNum.GetCurSel();     
    m_comboTestClassNum.GetLBText(nSel, strSelect);  

	m_listTestImgShow.DeleteAllItems();
	int n=m_TestSetImageList.GetImageCount();
	while(n!=0)
	{
		m_TestSetImageList.Remove(--n);
	}
	for(int n=0,i=0;n<DlgClassifier::m_TestImgSet.iItemNum;n++)
	{
		if(nSel==m_ClassIndexNum)
		{
			cv::Mat img_norm;
			cv::resize(DlgClassifier::m_TestImgSet.Items[n].img,img_norm,cv::Size(64,64));
			cv::Mat img_norm_color;
			cvtColor(img_norm, img_norm_color, CV_GRAY2RGBA);
			CBitmap bitmap;
			m_hbitmap=CreateBitmap( img_norm_color.cols,img_norm_color.rows,1,32,img_norm_color.data);
			bitmap.Attach(m_hbitmap);
			m_TestSetImageList.Add(&bitmap,RGB(0,0,0));
			m_listTestImgShow.SetImageList(&m_TestSetImageList, LVSIL_NORMAL);
			CString strNum=L"";
			strNum.Format(L"Class%d",DlgClassifier::m_TestImgSet.Items[n].iExternalClassNo);
			m_listTestImgShow.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, n, strNum, LVIS_SELECTED , LVIS_SELECTED, n, 0);
		}
		if(DlgClassifier::m_TestImgSet.Items[n].iExternalClassNo==nSel)
		{
			cv::Mat img_norm;
			cv::resize(DlgClassifier::m_TestImgSet.Items[n].img,img_norm,cv::Size(64,64));
			cv::Mat img_norm_color;
			cvtColor(img_norm, img_norm_color, CV_GRAY2RGBA);
			CBitmap bitmap;
			m_hbitmap=CreateBitmap( img_norm_color.cols,img_norm_color.rows,1,32,img_norm_color.data);
			bitmap.Attach(m_hbitmap);
			m_TestSetImageList.Add(&bitmap,RGB(0,0,0));
			m_listTestImgShow.SetImageList(&m_TestSetImageList, LVSIL_NORMAL);
			CString strNum=L"";
			strNum.Format(L"Class%d",DlgClassifier::m_TestImgSet.Items[n].iExternalClassNo);
			m_listTestImgShow.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, i, strNum, LVIS_SELECTED , LVIS_SELECTED, i, 0);
			i++;
		}
	}
}
///ͼ��淶���ĳߴ�
void DlgClassifier::OnCbnSelchangeComboImgsize()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strImgNormSize;   
    int nSel= m_ComboImgNormSize.GetCurSel();    
    m_ComboImgNormSize.GetLBText(nSel, strImgNormSize);   
	int iPos=strImgNormSize.Find(L"x");
	int iLenght=strImgNormSize.GetLength();
	CString strWidth=strImgNormSize.Left(iPos);
	CString strHeight=strImgNormSize.Right(iLenght-iPos-1);
	m_iNormWidth=_ttoi(strWidth);
	m_iNormHeight=_ttoi(strHeight);
}
///ͼ����Լ���ռ�ı���
void DlgClassifier::OnCbnSelchangeComboTestsetPercent()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;   
    int nSel= m_camboTestSetPercent.GetCurSel();    
    m_camboTestSetPercent.GetLBText(nSel, str);   
	int iPos=str.Find(L"%");
	int iLenght=str.GetLength();
	CString strPer=str.Left(iPos);
	m_fTestSetPercent=(float)_ttoi(strPer)/100;
}
///�������
void DlgClassifier::OnBnClickedButtonFeatAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCurSel=m_listBoxFeatFrom.GetCurSel();
	int nAllSel=m_listBoxFeatFrom.GetCount();
	if(nCurSel<0)
	{
		if(nAllSel>0)
		{
			AfxMessageBox(TEXT("��ѡ������"));
		}
		else
		{
			AfxMessageBox(TEXT("�����ڴ�ѡ����"));
		}
		return;
	}
	CString strText; 
	m_listBoxFeatFrom.GetText(nCurSel, strText); 

	if(LB_ERR==m_listBoxFeatTo.FindString(-1,strText))
	{
		m_listBoxFeatTo.AddString(strText);
		m_listBoxFeatFrom.DeleteString(nCurSel);
	}
	else
	{
		AfxMessageBox(TEXT("����Ŀ¼��"));
	}
}
///ɾ������
void DlgClassifier::OnBnClickedButtonFeatSub()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCurSel=m_listBoxFeatTo.GetCurSel();
	int nAllSel=m_listBoxFeatTo.GetCount();
	if(nCurSel<0)
	{
		if(nAllSel>0)
		{
			AfxMessageBox(TEXT("��ѡ������"));
		}
		else
		{
			AfxMessageBox(TEXT("��������ѡ����"));
		}
		return;
	}
	CString strText; 
	m_listBoxFeatTo.GetText(nCurSel, strText); 

	if(LB_ERR==m_listBoxFeatFrom.FindString(-1,strText))
	{
		m_listBoxFeatFrom.AddString(strText);
		m_listBoxFeatTo.DeleteString(nCurSel);
	}
	else
	{
		AfxMessageBox(TEXT("����Ŀ¼��"));
	}
}
///�������л�ѡ��
void DlgClassifier::OnTcnSelchangeTabClassifier(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	CRect tabRect; 
    // ��ȡ��ǩ�ؼ��ͻ���Rect����������������ʺϷ��ñ�ǩҳ   
    m_tabCtrlClassifier.GetClientRect(&tabRect);   
    tabRect.left += 1;   
    tabRect.right -= 1;   
    tabRect.top += 1;   
    tabRect.bottom -= 20;  

	m_dlgClassifierBP.MoveWindow(&tabRect);
	m_dlgClassifierSVM.MoveWindow(&tabRect);
	m_dlgClassifierTREE.MoveWindow(&tabRect);
	m_dlgClassifierBoosting.MoveWindow(&tabRect);
	m_dlgClassifierCNN.MoveWindow(&tabRect);
	//�ֱ��������غ���ʾ
	
    switch (m_tabCtrlClassifier.GetCurSel())   
    {    
    case 0:   
		m_dlgClassifierBP.ShowWindow(true);
		m_dlgClassifierSVM.ShowWindow(false);
		m_dlgClassifierTREE.ShowWindow(false);
		m_dlgClassifierBoosting.ShowWindow(false);
		m_dlgClassifierCNN.ShowWindow(false);
		break;    
    case 1:   
        m_dlgClassifierBP.ShowWindow(false);
		m_dlgClassifierSVM.ShowWindow(true);
		m_dlgClassifierTREE.ShowWindow(false);
		m_dlgClassifierBoosting.ShowWindow(false);
		m_dlgClassifierCNN.ShowWindow(false);
		break;  
	case 2:   
        m_dlgClassifierBP.ShowWindow(false);
		m_dlgClassifierSVM.ShowWindow(false);
		m_dlgClassifierTREE.ShowWindow(true);
		m_dlgClassifierBoosting.ShowWindow(false);
		m_dlgClassifierCNN.ShowWindow(false);
		break;  
	case 3:   
        m_dlgClassifierBP.ShowWindow(false);
		m_dlgClassifierSVM.ShowWindow(false);
		m_dlgClassifierTREE.ShowWindow(false);
		m_dlgClassifierBoosting.ShowWindow(true);
		m_dlgClassifierCNN.ShowWindow(false);
		break; 
	case 4:   
        m_dlgClassifierBP.ShowWindow(false);
		m_dlgClassifierSVM.ShowWindow(false);
		m_dlgClassifierTREE.ShowWindow(false);
		m_dlgClassifierBoosting.ShowWindow(false);
		m_dlgClassifierCNN.ShowWindow(true);
		break;  
    default:   
        break;   
    }
}
///�Ƿ��Զ���ȡ����
void DlgClassifier::OnBnClickedCheckAutoGetFeat()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(1==m_checkBoxAutoGetFeat.GetCheck())
	{
		GetDlgItem(IDC_BUTTON_FEAT_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FEAT_SUB)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_FEAT_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FEAT_SUB)->EnableWindow(TRUE);
	}
}
///��ȡ����
void DlgClassifier::OnBnClickedButtonGetFeat()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_BUTTON_GET_FEAT)->EnableWindow(FALSE);
	int nAllSel=m_listBoxFeatTo.GetCount();
	if(nAllSel<=0)
	{
		AfxMessageBox(TEXT("δѡȡ����"));
		GetDlgItem(IDC_BUTTON_GET_FEAT)->EnableWindow(TRUE);
		return;
	}
	if(1==m_checkBoxAutoGetFeat.GetCheck())
	{
		AfxMessageBox(TEXT("��ѡ���Զ���ȡ����������Ҫ�ظ���ȡ"));
		return;
	}
	//��ʼ������
	for(int j=0;j<m_TrainImgSet.iItemNum;j++)
	{
		m_TrainImgSet.Items[j].feat.clear();
	}
	for(int j=0;j<m_TestImgSet.iItemNum;j++)
	{
		m_TestImgSet.Items[j].feat.clear();
	}

	///***���� ��ȡ����***///
	//
	//CString strInfo;
	//for(int i=0;i<nAllSel;i++)
	//{
	//	CString strText; 
	//	m_listBoxFeatTo.GetText(i, strText); 
	//	if(strText==L"Sobel����")
	//	{
	//		for(int j=0;j<m_TrainImgSet.iItemNum;j++)
	//		{
	//			cv::Mat src=m_TrainImgSet.Items[j].img.clone();
	//			ImgProcessPrimary::GetSobelFeat(src,m_TrainImgSet.Items[j].feat);
	//		}
	//		for(int j=0;j<m_TestImgSet.iItemNum;j++)
	//		{
	//			cv::Mat src=m_TestImgSet.Items[j].img.clone();
	//			ImgProcessPrimary::GetSobelFeat(src,m_TestImgSet.Items[j].feat);
	//		}
	//		m_ListShowInfo.InsertString(lInsertIndex++,L"���-Sobel����");
	//	}
	//	else if(strText==L"LBP����")
	//	{
	//		for(int j=0;j<m_TrainImgSet.iItemNum;j++)
	//		{
	//			cv::Mat src=m_TrainImgSet.Items[j].img.clone();
	//			ImgProcessPrimary::GetLBPFeat(src,m_TrainImgSet.Items[j].feat);
	//		}
	//		for(int j=0;j<m_TestImgSet.iItemNum;j++)
	//		{
	//			cv::Mat src=m_TestImgSet.Items[j].img.clone();
	//			ImgProcessPrimary::GetLBPFeat(src,m_TestImgSet.Items[j].feat);
	//		}
	//		m_ListShowInfo.InsertString(lInsertIndex++,L"���-LBP����");
	//	}
	//}
	///***DLL ��ȡ����***///
	CString strInfo;
	for(int i=0;i<nAllSel;i++)
	{
		//��ȡ������
		CString strText; 
		m_listBoxFeatTo.GetText(i, strText);
		int nLength = strText.GetLength();
		int nBytes = WideCharToMultiByte(CP_ACP,0,strText,nLength,NULL,0,NULL,NULL);
		char* chrFeatName = new char[ nBytes + 1];
		memset(chrFeatName,0,nLength + 1);
		WideCharToMultiByte(CP_OEMCP, 0, strText, nLength, chrFeatName, nBytes, NULL, NULL);
		chrFeatName[nBytes] = 0;
		//�ⲿdll�ļ���ȡ����
		for(int j=0;j<m_TrainImgSet.iItemNum;j++)
		{
			cv::Mat src=m_TrainImgSet.Items[j].img.clone();
			H_GetFeat(src,m_TrainImgSet.Items[j].feat,chrFeatName);
		}
		for(int j=0;j<m_TestImgSet.iItemNum;j++)
		{
			cv::Mat src=m_TestImgSet.Items[j].img.clone();
			H_GetFeat(src,m_TestImgSet.Items[j].feat,chrFeatName);
		}
		m_ListShowInfo.InsertString(g_lInsertIndex++,strText);
		
		delete[] chrFeatName;
		//����ȡ���������洢���ļ��Ա�������з���
		//CString strKey;
		//strKey.Format(L"Feat%d",i);
		//m_FileIniOper.Write(L"Feats",strKey,strText);
	}
	//
	m_iFeatsDim=m_TrainImgSet.Items[0].feat.size();		
	strInfo.Format(L"����ά��Ϊ%d",m_iFeatsDim);
	m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
	GetDlgItem(IDC_BUTTON_GET_FEAT)->EnableWindow(TRUE);
}

void DlgClassifier::OnBnClickedButtonTrainClassifier()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CString strInfo;
	//int iClassifier=m_tabCtrlClassifier.GetCurSel();
	//if(iClassifier<0)
	//{
	//	AfxMessageBox(TEXT("��ѡ�������"));
	//}
	////��ʼ������
	//int iTrainSetNum=m_TrainImgSet.iItemNum;
	//int iFeatDims=m_TrainImgSet.Items[0].feat.size();
	//if(iTrainSetNum<=0)
	//{
	//	AfxMessageBox(TEXT("��ѡ��ѵ����"));
	//}
	//if(iFeatDims<=0)
	//{
	//	AfxMessageBox(TEXT("����ȡͼ������"));
	//}
	//cv::Mat trainData=cvCreateMat(iTrainSetNum,iFeatDims,CV_32FC1);
	//for(int i=0;i<iTrainSetNum;i++)
	//{
	//	Mat OneImgFeat(m_TrainImgSet.Items[i].feat);  
	//	Mat OneImgFeatT=OneImgFeat.reshape(0,1).clone();
	//	OneImgFeatT.copyTo(trainData.row(i)); 
	//}
	//Mat trainLabels= cvCreateMat(iTrainSetNum, 1, CV_32FC1); 
	//for(int i=0;i<trainLabels.rows;i++)
	//{
	//	trainLabels.at<float>(i,0)=m_TrainImgSet.Items[i].iExternalClassNo;
	//} 
	////ѵ��������
	//TCHAR strTemp[128];
	//int iLength ; 
	//
	//SetDlgItemText(IDC_BUTTON_TRAIN_CLASSIFIER,L"ѵ����..."); 
	//GetDlgItem(IDC_BUTTON_TRAIN_CLASSIFIER)->EnableWindow(false);
	//switch(iClassifier)
	//{
	//case 0:
	//	m_dlgClassifierBP.GetDlgItem(IDC_EDIT_HIDE_BODE_NUM)->GetWindowTextW(strTemp,128);
	//	m_ClassifierParam.iHideNode=_wtoi(strTemp);
	//	m_dlgClassifierBP.GetDlgItem(IDC_EDIT_ITER_NUM)->GetWindowTextW(strTemp,128);
	//	m_ClassifierParam.iMaxIter=_wtoi(strTemp);
	//	m_dlgClassifierBP.GetDlgItem(IDC_EDIT_EPSILON)->GetWindowTextW(strTemp,128);
	//	m_ClassifierParam.fEpsilon=_wtof(strTemp);
	//	m_dlgClassifierBP.GetDlgItem(IDC_EDIT_BP_CLASSIFIER_FILENAME)->GetWindowTextW(strTemp,128); 
	//	iLength = WideCharToMultiByte(CP_ACP, 0, strTemp, -1, NULL, 0, NULL, NULL);  
	//	WideCharToMultiByte(CP_ACP, 0, strTemp, -1, m_ClassifierParam.fileName, iLength, NULL, NULL);		
	//	ImgProcessClassifier::Classifier_BP(trainData,trainLabels,m_ClassIndexNum,m_ClassifierParam);
	//	strInfo.Format(L"BP�����������ز�Ϊ%d����������Ϊ%d���������Ϊ%f���洢�ļ���Ϊ%s",m_ClassifierParam.iHideNode,m_ClassifierParam.iMaxIter,m_ClassifierParam.fEpsilon,strTemp);
	//	m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
	//	break;
	//case 1:
	//	m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_ITER_NUM)->GetWindowTextW(strTemp,128);
	//	m_ClassifierParam.iMaxIter=_wtoi(strTemp);
	//	m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_EPSILON)->GetWindowTextW(strTemp,128);
	//	m_ClassifierParam.fEpsilon=_wtof(strTemp);
	//	m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_SVM_CLASSIFIER_FILENAME)->GetWindowTextW(strTemp,128); 
	//	iLength = WideCharToMultiByte(CP_ACP, 0, strTemp, -1, NULL, 0, NULL, NULL);  
	//	WideCharToMultiByte(CP_ACP, 0, strTemp, -1, m_ClassifierParam.fileName, iLength, NULL, NULL);
	//	ImgProcessClassifier::Classifier_SVM(trainData,trainLabels,m_ClassIndexNum,m_ClassifierParam);
	//	strInfo.Format(L"SVM����������������Ϊ%d���������Ϊ%f���洢�ļ���Ϊ%c",m_ClassifierParam.iMaxIter,m_ClassifierParam.fEpsilon,strTemp);
	//	m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
	//	break;
	//default:break;
	//}
	//SetDlgItemText(IDC_BUTTON_TRAIN_CLASSIFIER,L"ѵ��������."); 
	//GetDlgItem(IDC_BUTTON_TRAIN_CLASSIFIER)->EnableWindow(true);
}

void DlgClassifier::OnBnClickedButtonTestClassifier()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//int iClassifier=m_tabCtrlClassifier.GetCurSel();
	//if(iClassifier<0)
	//{
	//	AfxMessageBox(TEXT("��ѡ�������"));
	//}
	////��ʼ������
	//int iTestSetNum=m_TestImgSet.iItemNum;
	//int iFeatDims=m_TestImgSet.Items[0].feat.size();
	//if(iTestSetNum<=0)
	//{
	//	AfxMessageBox(TEXT("��ѡ��ѵ����"));
	//}
	//if(iFeatDims<=0)
	//{
	//	AfxMessageBox(TEXT("����ȡͼ������"));
	//}
	//cv::Mat testData=cvCreateMat(iTestSetNum,iFeatDims,CV_32FC1);
	//for(int i=0;i<iTestSetNum;i++)
	//{
	//	Mat OneImgFeat(m_TestImgSet.Items[i].feat);  
	//	Mat OneImgFeatT=OneImgFeat.reshape(0,1).clone();
	//	OneImgFeatT.copyTo(testData.row(i)); 
	//}
	//
	//Mat testLabels= cvCreateMat(iTestSetNum, 1, CV_32FC1); 
	//for(int i=0;i<testLabels.rows;i++)
	//{
	//	testLabels.at<float>(i,0)=m_TestImgSet.Items[i].iExternalClassNo;
	//} 
	////ѵ��������
	//Mat perLabels= cvCreateMat(iTestSetNum, 1, CV_32FC1); 
	//SetDlgItemText(IDC_BUTTON_TEST_CLASSIFIER,L"������..."); 
	//GetDlgItem(IDC_BUTTON_TEST_CLASSIFIER)->EnableWindow(false);
	//float fRido=0;
	//switch(iClassifier)
	//{
	//case 0:
	//	ImgProcessClassifier::Classifier_BP_Predict(testData,testLabels,m_ClassIndexNum,m_ClassifierParam,perLabels);
	//	break;
	//case 1:
	//	ImgProcessClassifier::Classifier_SVM_Predict(testData,testLabels,m_ClassIndexNum,m_ClassifierParam,perLabels);
	//	break;
	//default:break;
	//}
	//SetDlgItemText(IDC_BUTTON_TEST_CLASSIFIER,L"���Է�����."); 
	//GetDlgItem(IDC_BUTTON_TEST_CLASSIFIER)->EnableWindow(true);
	////CString str=L"";
	////str.Format(L"��ȷ��Ϊ��%f",fRido);
	////AfxMessageBox(str);
	//cv::Mat confusion_matrix=cv::Mat::zeros(m_ClassIndexNum,m_ClassIndexNum,CV_32SC1);
	//cv::Mat confusion_matrix_f=cv::Mat::zeros(m_ClassIndexNum,m_ClassIndexNum,CV_32FC1);
	//for(int i=0;i<testLabels.rows;i++)
	//{
	//	confusion_matrix.at<int>((int)testLabels.at<float>(i,0),(int)perLabels.at<float>(i,0))+=1;
	//	//confusion_matrix.at<int>((int)testLabels.at<float>(i,0),(int)perLabels.at<float>(i,0))=min(255,confusion_matrix.at<int>((int)testLabels.at<float>(i,0),(int)perLabels.at<float>(i,0))+1);
	//}
	////cv::namedWindow("confusion_matrix",0);
	////cv::imshow("confusion_matrix",confusion_matrix);

	//m_ListShowInfo.InsertString(g_lInsertIndex++,L"�����������£�");
	//CString strInfo=L"HH";
	//CString strTemp=L"";
	//for(int i=0;i<m_ClassIndexNum;i++)
	//{
	//	strTemp.Format(L"----%04d",i);
	//	strInfo=strInfo+strTemp;
	//}
	//strTemp.Format(L"----SSSS");
	//strInfo=strInfo+strTemp;
	//m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
	//for(int i=0;i<confusion_matrix.rows;i++)
	//{
	//	int iColTotal=0;
	//	strInfo.Format(L"%02d",i);
	//	for(int j=0;j<confusion_matrix.cols;j++)
	//	{
	//		strInfo.Format(L"%s----%04d",strInfo,confusion_matrix.at<int>(i,j));
	//		iColTotal+=confusion_matrix.at<int>(i,j);
	//	}
	//	strInfo.Format(L"%s----%04d",strInfo,iColTotal);
	//	m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
	//}
	//strInfo=L"SS";
	//for(int i=0;i<confusion_matrix.cols;i++)
	//{
	//	int iRowTotal=0;
	//	for(int j=0;j<confusion_matrix.rows;j++)
	//	{
	//		iRowTotal+=confusion_matrix.at<int>(i,j);
	//	}
	//	strInfo.Format(L"%s----%04d",strInfo,iRowTotal);
	//}	
	//m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
}

void DlgClassifier::OnBnClickedButtonTrainAndTestClassifier()
{
	//�����̺߳������з���
	//m_hDefectFeatAndClassify.m_ThreadParam.hWnd=m_hWnd;
	//m_hDefectFeatAndClassify.m_ThreadParam.nData=0;
	//m_hDefectFeatAndClassify.m_ThreadParam.index=0;
	//m_hDefectFeatAndClassify.m_ThreadParam.bExit=false;
	//m_hDefectFeatAndClassify.m_ThreadParam.pMutex = new CMutex();
	//m_hDefectFeatAndClassify.BeginThread(ThreadProcDefectClassify);
	//m_hDefectFeatAndClassify.ResumeThread();
	
	///
	//����������
	CString strInfo;
	int iClassifier=m_tabCtrlClassifier.GetCurSel();
	if(iClassifier<0)
	{
		AfxMessageBox(TEXT("��ѡ�������"));
		return;
	}
	//��ʼ������
	int iTrainSetNum=m_TrainImgSet.iItemNum;
	int iTestSetNum=m_TestImgSet.iItemNum;
	int iFeatDims=m_TrainImgSet.Items[0].feat.size();
	if(iTrainSetNum<=0)
	{
		AfxMessageBox(TEXT("��ѡ��ѵ����"));
		return;
	}
	if(iFeatDims<10)
	{
		AfxMessageBox(TEXT("����ȡͼ������"));
		return;
	}
	//
	GetDlgItem(IDC_BUTTON_TRAIN_AND_TEST_CLASSIFIER)->EnableWindow(FALSE);
	SetDlgItemText(IDC_BUTTON_TRAIN_AND_TEST_CLASSIFIER,L"ѵ����..."); 
	//���������ݹ淶��
	cv::Mat trainData=cvCreateMat(iTrainSetNum,iFeatDims,CV_32FC1);
	for(int i=0;i<iTrainSetNum;i++)
	{
		cv::Mat OneImgFeat(m_TrainImgSet.Items[i].feat);  
		cv::Mat OneImgFeatT=OneImgFeat.reshape(0,1).clone();
		OneImgFeatT.copyTo(trainData.row(i)); 
	}
	cv::Mat trainLabels= cvCreateMat(iTrainSetNum, 1, CV_32FC1); 
	for(int i=0;i<trainLabels.rows;i++)
	{
		trainLabels.at<float>(i,0)=m_TrainImgSet.Items[i].iExternalClassNo;
	} 
	cv::Mat testData=cvCreateMat(iTestSetNum,iFeatDims,CV_32FC1);
	for(int i=0;i<iTestSetNum;i++)
	{
		cv::Mat OneImgFeat(m_TestImgSet.Items[i].feat);  
		cv::Mat OneImgFeatT=OneImgFeat.reshape(0,1).clone();
		OneImgFeatT.copyTo(testData.row(i)); 
	}	
	cv::Mat testLabels= cvCreateMat(iTestSetNum, 1, CV_32FC1); 
	for(int i=0;i<testLabels.rows;i++)
	{
		testLabels.at<float>(i,0)=m_TestImgSet.Items[i].iExternalClassNo;
	} 
	cv::Mat perLabels= cvCreateMat(iTestSetNum, 1, CV_32FC1); 
	///**********************Dll�ļ�ѵ��������******************************************///
	//
	TCHAR strTemp[128];
	CString strClassifierFileName;
	CString strClassifierName;
	int iLength ;
	m_ListShowInfo.InsertString(g_lInsertIndex++,L"ѵ����...");

	switch(iClassifier)
	{
	case 0:
		//���ز�
		m_dlgClassifierBP.GetDlgItem(IDC_EDIT_HIDE_BODE_NUM)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_BP.iHideNode=_wtoi(strTemp);
		//����������
		m_dlgClassifierBP.GetDlgItem(IDC_EDIT_ITER_NUM)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_BP.iMaxIter=_wtoi(strTemp);
		//��С���
		m_dlgClassifierBP.GetDlgItem(IDC_EDIT_EPSILON)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_BP.fEpsilon=_wtof(strTemp);
		//�����ļ���
		m_dlgClassifierBP.GetDlgItem(IDC_EDIT_BP_CLASSIFIER_FILENAME)->GetWindowTextW(strTemp,128); 
		strClassifierFileName.Format(L"%s",strTemp);
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, m_ClassifierParam.tClassifierParam_BP.fileName, iLength, NULL, NULL);	
		//��������
		strClassifierName=L"BP_Classifier";
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, m_cstrClassifierName, iLength, NULL, NULL);	
		//������ѵ��
		H_ClassifierTrain(m_cstrClassifierName,trainData,trainLabels,m_ClassIndexNum,m_ClassifierParam);
		//��ʾ��Ϣ
		strInfo.Format(L"BP�����������ز�Ϊ%d����������Ϊ%d���������Ϊ%f���洢�ļ���Ϊ%s",m_ClassifierParam.tClassifierParam_BP.iHideNode,m_ClassifierParam.tClassifierParam_BP.iMaxIter,m_ClassifierParam.tClassifierParam_BP.fEpsilon,strClassifierFileName);
		m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
		m_ListShowInfo.InsertString(g_lInsertIndex++,L"������...");
		//����������
		H_ClassifierPredict(m_cstrClassifierName,testData,testLabels,perLabels,m_ClassIndexNum,m_ClassifierParam);
		break;
	case 1:
		//����������
		m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_ITER_NUM)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_SVM.iMaxIter=_wtoi(strTemp);
		//��С���
		m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_EPSILON)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_SVM.fEpsilon=_wtof(strTemp);
		//C
		m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_SVM_CLASSIFIER_C)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_SVM.fC=_wtof(strTemp);
		//GAMMA
		m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_SVM_CLASSIFIER_GAMMA)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_SVM.fGamma=_wtof(strTemp);
		//�˺�����ʽ     
;
		//m_dlgClassifierSVM.GetDlgItem(IDC_COMBO_SVM_CLASSIFIER_KEN)->GetLBText(nSel, strTemp);
		if(((CComboBox*)m_dlgClassifierSVM.GetDlgItem(IDC_COMBO_SVM_CLASSIFIER_KEN))->GetCurSel()==0)
		{
			m_ClassifierParam.tClassifierParam_SVM.iKernelType=cv::SVM::RBF;
		}
		else if(((CComboBox*)m_dlgClassifierSVM.GetDlgItem(IDC_COMBO_SVM_CLASSIFIER_KEN))->GetCurSel()==1)
		{
			m_ClassifierParam.tClassifierParam_SVM.iKernelType=cv::SVM::LINEAR;
		}
		//�����ļ���
		m_dlgClassifierSVM.GetDlgItem(IDC_EDIT_SVM_CLASSIFIER_FILENAME)->GetWindowTextW(strTemp,128); 
		strClassifierFileName.Format(L"%s",strTemp);
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, m_ClassifierParam.tClassifierParam_SVM.fileName, iLength, NULL, NULL);
		//�Ƿ��Ż�����
		if(BST_CHECKED == m_dlgClassifierSVM.IsDlgButtonChecked( IDC_CHECK_OPTIMIZE_PARAM ))
		{
			m_ClassifierParam.tClassifierParam_SVM.bIsOptimizeParam=true;
		}
		else
		{
			m_ClassifierParam.tClassifierParam_SVM.bIsOptimizeParam=false;
		}
		//m_dlgClassifierSVM.GetDlgItem(IDC_CHECK_OPTIMIZE_PARAM)->GetCheck();
		//��������
		strClassifierName="SVM_Classifier";
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, m_cstrClassifierName, iLength, NULL, NULL);	
		//������ѵ��
		H_ClassifierTrain(m_cstrClassifierName,trainData,trainLabels,m_ClassIndexNum,m_ClassifierParam);
		//��ʾ��Ϣ
		strInfo.Format(L"SVM����������������Ϊ%d���������Ϊ%f���洢�ļ���Ϊ%s",m_ClassifierParam.tClassifierParam_SVM.iMaxIter,m_ClassifierParam.tClassifierParam_SVM.fEpsilon,strClassifierFileName);
		m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
		m_ListShowInfo.InsertString(g_lInsertIndex++,L"������...");
		//����������
		H_ClassifierPredict(m_cstrClassifierName,testData,testLabels,perLabels,m_ClassIndexNum,m_ClassifierParam);
		break;
	case 2:
		//��������
		m_dlgClassifierTREE.GetDlgItem(IDC_EDIT_TREE_NUM)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_RTree.iTreeNum=_wtoi(strTemp);
		//��С���
		m_dlgClassifierTREE.GetDlgItem(IDC_EDIT_EPSILON)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_RTree.fEpsilon=_wtof(strTemp);
		//���
		m_ClassifierParam.tClassifierParam_RTree.iDepth=10;
		//Ȩ��
		if(BST_CHECKED == m_dlgClassifierTREE.IsDlgButtonChecked( IDC_CHECK_RTREE_CLASSIFIER_PROIOR ))
		{
			int iTotal=0;
			for(int k=0;k<m_ClassIndexNum;k++)
			{
				iTotal+=m_iTrainClassNum[k];
			}
			for(int k=0;k<m_ClassIndexNum;k++)
			{
				float fp=(float)m_iTrainClassNum[k]/iTotal;
				m_ClassifierParam.tClassifierParam_RTree.fPriors[k]=1-fp;
			}
		}
		else
		{
			for(int k=0;k<m_ClassIndexNum;k++)
			{
				m_ClassifierParam.tClassifierParam_RTree.fPriors[k]=1;
			}
		}
		
		//�����ļ���
		m_dlgClassifierTREE.GetDlgItem(IDC_EDIT_RTREE_CLASSIFIER_FILENAME)->GetWindowTextW(strTemp,128); 
		strClassifierFileName.Format(L"%s",strTemp);
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, m_ClassifierParam.tClassifierParam_RTree.fileName, iLength, NULL, NULL);
		//��������
		strClassifierName="RTree_Classifier";
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, m_cstrClassifierName, iLength, NULL, NULL);	
		//������ѵ��
		H_ClassifierTrain(m_cstrClassifierName,trainData,trainLabels,m_ClassIndexNum,m_ClassifierParam);
		//��ʾ��Ϣ
		strInfo.Format(L"���ɭ�ַ���������������Ϊ%d���������Ϊ%f���洢�ļ���Ϊ%s",m_ClassifierParam.tClassifierParam_RTree.iMaxIter,m_ClassifierParam.tClassifierParam_RTree.fEpsilon,strClassifierFileName);
		m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
		m_ListShowInfo.InsertString(g_lInsertIndex++,L"������...");
		//����������
		H_ClassifierPredict(m_cstrClassifierName,testData,testLabels,perLabels,m_ClassIndexNum,m_ClassifierParam);
		break;
	case 3:
		//����������
		m_dlgClassifierBoosting.GetDlgItem(IDC_EDIT_WEAK_CLASSIFIER_NUM)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_Boosting.iMaxIter=_wtoi(strTemp);
		//��С���
		m_dlgClassifierBoosting.GetDlgItem(IDC_EDIT_EPSILON)->GetWindowTextW(strTemp,128);
		m_ClassifierParam.tClassifierParam_Boosting.fEpsilon=_wtof(strTemp);
		//�����ļ���
		m_dlgClassifierBoosting.GetDlgItem(IDC_EDIT_BOOSTING_CLASSIFIER_FILENAME)->GetWindowTextW(strTemp,128); 
		strClassifierFileName.Format(L"%s",strTemp);
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierFileName, -1, m_ClassifierParam.tClassifierParam_Boosting.fileName, iLength, NULL, NULL);
		//��������
		strClassifierName="Boosting_Classifier";
		iLength = WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strClassifierName, -1, m_cstrClassifierName, iLength, NULL, NULL);	
		//������ѵ��
		H_ClassifierTrain(m_cstrClassifierName,trainData,trainLabels,m_ClassIndexNum,m_ClassifierParam);
		//��ʾ��Ϣ
		strInfo.Format(L"Boost����������������Ϊ%d���������Ϊ%f���洢�ļ���Ϊ%s",m_ClassifierParam.tClassifierParam_Boosting.iMaxIter,m_ClassifierParam.tClassifierParam_Boosting.fEpsilon,strClassifierFileName);
		m_ListShowInfo.InsertString(g_lInsertIndex++,strInfo);
		m_ListShowInfo.InsertString(g_lInsertIndex++,L"������...");
		//����������
		H_ClassifierPredict(m_cstrClassifierName,testData,testLabels,perLabels,m_ClassIndexNum,m_ClassifierParam);
		break;
	default:break;
	}
	///*****************************ѵ�����*******************************///
	//д���Լ���������Result
	WriteResultToFile(m_TestImgSet,testLabels,perLabels);
	//��������
	cv::Mat confusion_matrix=cv::Mat::zeros(m_ClassIndexNum,m_ClassIndexNum,CV_32SC1);
	cv::Mat confusion_matrix_f=cv::Mat::zeros(m_ClassIndexNum,m_ClassIndexNum,CV_32FC1);
	for(int i=0;i<testLabels.rows;i++)
	{
		confusion_matrix.at<int>((int)testLabels.at<float>(i,0),(int)perLabels.at<float>(i,0))+=1;
		//confusion_matrix.at<int>((int)testLabels.at<float>(i,0),(int)perLabels.at<float>(i,0))=min(255,confusion_matrix.at<int>((int)testLabels.at<float>(i,0),(int)perLabels.at<float>(i,0))+1);
	}
	ShowConfusionMatrix(confusion_matrix);
	//��׼ȷ��
	int iSum=0;
	int iOk=0;
	for(int i=0;i<confusion_matrix.rows;i++)
	{
		for(int j=0;j<confusion_matrix.cols;j++)
		{
			if(i==j)
			{
				iOk+=confusion_matrix.at<int>(i,j);
			}
			iSum+=confusion_matrix.at<int>(i,j);
		}
	}
	float fPer=(float)iOk/iSum;
	CString strPer;
	strPer.Format(L"%.4f",fPer);
	SetDlgItemText(IDC_EDIT_RESULT_ACCURACY, strPer);  
	//ѵ���ɹ������ô洢���ļ��Ա�������з���
	WriteClassifierParamToIniFile();
	//
	m_ListShowInfo.InsertString(g_lInsertIndex++,L"�������������");
	SetDlgItemText(IDC_BUTTON_TRAIN_AND_TEST_CLASSIFIER,L"ѵ�������Է�����"); 
	GetDlgItem(IDC_BUTTON_TRAIN_AND_TEST_CLASSIFIER)->EnableWindow(TRUE);
}
///����д��ini�ļ�
void DlgClassifier::WriteClassifierParamToIniFile()
{
	CString strValue;

	if(1==m_checkNorm.GetCheck())
	{
		strValue.Format(L"%d",1);
	}
	else
	{
		strValue.Format(L"%d",0);
	}
	m_FileIniOper.Write(L"ImgSize",L"Norm",strValue);

	strValue.Format(L"%d",m_iFeatsDim);
	m_FileIniOper.Write(L"Feats",L"FeatsDim",strValue);

	int nAllSel=m_listBoxFeatTo.GetCount();
	strValue.Format(L"%d",nAllSel);
	m_FileIniOper.Write(L"Feats",L"FeatsNum",strValue);
	for(int i=0;i<nAllSel;i++)
	{
		CString strText; 
		m_listBoxFeatTo.GetText(i, strText);
		CString strKey;
		strKey.Format(L"Feat%d",i);
		m_FileIniOper.Write(L"Feats",strKey,strText);
	}

	strValue.Format(L"%d",m_TrainImgSet.Items[0].img.rows);
	m_FileIniOper.Write(L"ImgSize",L"Height",strValue);

	strValue.Format(L"%d",m_TrainImgSet.Items[0].img.cols);
	m_FileIniOper.Write(L"ImgSize",L"Width",strValue);

	strValue.Format(L"%d",m_ClassIndexNum);
	m_FileIniOper.Write(L"Classifier",L"ClassNum",strValue);

	//strValue.Format(L"%d",m_ClassifierParam.iMaxIter);
	//m_FileIniOper.Write(L"Classifier",L"MaxIter",strValue);

	//strValue.Format(L"%f",m_ClassifierParam.fEpsilon);
	//m_FileIniOper.Write(L"Classifier",L"Epsilon",strValue);
	USES_CONVERSION;
	strValue = A2T(m_cstrClassifierName);
	m_FileIniOper.Write(L"Classifier",L"ClassifierName",strValue);
	if(strValue.Compare(L"BP_Classifier")==0)
	{	
		strValue = A2T(m_ClassifierParam.tClassifierParam_BP.fileName);
		m_FileIniOper.Write(L"Classifier",L"ClassifierFileName",strValue);
	}
	else if(strValue.Compare(L"SVM_Classifier")==0)
	{
		strValue = A2T(m_ClassifierParam.tClassifierParam_SVM.fileName);
		m_FileIniOper.Write(L"Classifier",L"ClassifierFileName",strValue);
	}
	else if(strValue.Compare(L"RTree_Classifier")==0)
	{
		strValue = A2T(m_ClassifierParam.tClassifierParam_RTree.fileName);
		m_FileIniOper.Write(L"Classifier",L"ClassifierFileName",strValue);
	}
	else if(strValue.Compare(L"Boosting_Classifier")==0)
	{
		strValue = A2T(m_ClassifierParam.tClassifierParam_Boosting.fileName);
		m_FileIniOper.Write(L"Classifier",L"ClassifierFileName",strValue);
	}
	
	for(int i=0;i<m_ClassIndexNum;i++)
	{
		CString strKey;
		strKey.Format(L"Class%d",i);
		strValue.Format(L"%d",m_iExterToInter[i]);
		m_FileIniOper.Write(L"ClassConversion",strKey,strValue);
	}
	::DeleteFile(m_strIniFileName);
	CFile::Rename(m_FileIniOper.m_strFileName, m_strIniFileName);
}
///д���Լ���������Result�ļ�
void DlgClassifier::WriteResultToFile(SampleImgSet ImgSet,cv::Mat iLabelClass,cv::Mat iPerClass)
{
	//�����ļ���
	CString strWildcard;
	strWildcard.Format(L"%s\\Result",m_strAppPath);
	int IsExist=PathFileExists(strWildcard);
	if(IsExist)
	{
		DeleteTree(strWildcard);
	}
	CreateDirectory(strWildcard, NULL);
	//д�ļ�
	if(ImgSet.iItemNum!=iPerClass.rows)
	{
		AfxMessageBox(TEXT("����������Ԥ�����������һ�£����飡"));
		return;
	}
	for(int i=0;i<ImgSet.iItemNum;i++)
	{
		int iExterClass=(int)iPerClass.at<float>(i,0);
		int iLabelrClass=(int)iLabelClass.at<float>(i,0);
		int iInterClass=m_iExterToInter[iExterClass];
		
		USES_CONVERSION;
		CString strClassName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[iInterClass].strClassName);
		CString strClassPath;
		strClassPath.Format(L"%s\\%s",strWildcard,strClassName);
		IsExist=PathFileExists(strClassPath);
		if(!IsExist)
		{
			CreateDirectory(strClassPath, NULL);
		}
		char classpath[256]={0};
		char imagepath[256]={0};
		int iLength = WideCharToMultiByte(CP_ACP, 0, strClassPath, -1, NULL, 0, NULL, NULL); 
		WideCharToMultiByte(CP_ACP, 0, strClassPath, -1, classpath, iLength, NULL, NULL);
		if(iLabelrClass==iExterClass)
		{
			sprintf(imagepath,"%s\\T_Index_%d--Label_%d--Perdict_%d.bmp",classpath,i,iLabelrClass,iExterClass);
		}
		else
		{
			sprintf(imagepath,"%s\\F_Index_%d--Label_%d--Perdict_%d.bmp",classpath,i,iLabelrClass,iExterClass);
		}
		cv::Mat src=ImgSet.Items[i].img;
		IplImage* image=&(IplImage)src;
		cvSaveImage(imagepath,image);
	}
}
///˫���б���ʾͼƬԭʼ��С
void DlgClassifier::OnLvnItemActivateListTrainimgshow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	if (NULL == m_dlgImageShow)   
    {   
        m_dlgImageShow = new DlgImageShow();   
        m_dlgImageShow->Create(IDD_DIALOG_SHOW_ORGINIMG, this);   
    } 

	POSITION pos = m_listTrainImgShow.GetFirstSelectedItemPosition();
	CPoint point;
	GetCursorPos(&point);

	//m_dlgImageShow->SetWindowPos(NULL,point.x,point.y,DlgClassifier::m_TrainImgSet.Items[(int)pos-1].img.cols*3,DlgClassifier::m_TrainImgSet.Items[(int)pos-1].img.rows*3,SWP_NOMOVE);
	m_dlgImageShow->MoveWindow(point.x,point.y,DlgClassifier::m_TrainImgSet.Items[(int)pos-1].img.cols*2,DlgClassifier::m_TrainImgSet.Items[(int)pos-1].img.rows*2);
	m_dlgImageShow->ShowImgToMFCFormMat(IDC_IMG_SHOW,DlgClassifier::m_TrainImgSet.Items[(int)pos-1].img);
	m_dlgImageShow->ShowWindow(SW_SHOW); 
}

void DlgClassifier::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_ptMouse.x=point.x;
	m_ptMouse.y=point.y;

	CDialog::OnMouseMove(nFlags, point);
}
///��ʾ��������
void DlgClassifier::ShowConfusionMatrix(const cv::Mat& confusion_matrix)
{
	CPen newPen;       // ���ڴ����»���   
    CPen *pOldPen;     // ���ڴ�žɻ���   
    CBrush newBrush;   // ���ڴ����»�ˢ   
    CBrush *pOldBrush; // ���ڴ�žɻ�ˢ 
	CRect rectResultShow;   // ��ǩ�ؼ��ͻ�����λ�úʹ�С 
    m_picResuleShow.GetClientRect(&rectResultShow);
	CDC *pDC=m_picResuleShow.GetDC();
    newBrush.CreateSolidBrush(RGB(0,0,0)); // ������ɫ�»�ˢ         
    pOldBrush = pDC->SelectObject(&newBrush); // ѡ���»�ˢ�������ɻ�ˢ��ָ�뱣�浽pOldBrush        
    pDC->Rectangle(rectResultShow); // �Ժ�ɫ��ˢΪ��ͼ�ؼ�����ɫ���γɺ�ɫ����
	pDC->SelectObject(pOldBrush);      
    newBrush.DeleteObject();// ɾ���»�ˢ 
      
	int iclass=m_ClassIndexNum+3; 
	float fT=0.25;
	int iOneWidth=rectResultShow.Width()/iclass;
	int iOneHeight=rectResultShow.Height()/iclass;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,255,0)); // ����ʵ�Ļ��ʣ��ֶ�Ϊ1����ɫΪ��ɫ       
    pOldPen = pDC->SelectObject(&newPen); // ѡ���»��ʣ������ɻ��ʵ�ָ�뱣�浽pOldPen         
	
	CFont newFont;
	LOGFONT logft;
	memset(&logft, 0, sizeof(LOGFONT));  
	CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT))->GetLogFont(&logft);
	logft.lfHeight = 10;
	newFont.CreateFontIndirect(&logft);
	CFont * pOldFont = pDC->SelectObject(&newFont);

	pDC->SetTextColor(RGB(255,255,0));
	pDC->SetBkMode(TRANSPARENT);
	for(int i=0;i<=iclass;i++)
	{
		if(i!=iclass)
		{
			pDC->MoveTo(rectResultShow.left+i*iOneWidth, rectResultShow.top); 
			pDC->LineTo(rectResultShow.left+i*iOneWidth, rectResultShow.bottom); 
		}
		else
		{
			pDC->MoveTo(rectResultShow.right, rectResultShow.top); 
			pDC->LineTo(rectResultShow.right, rectResultShow.bottom); 
		}
	} 
	for(int i=0;i<=iclass;i++)
	{
		if(i!=iclass)
		{
			pDC->MoveTo(rectResultShow.left, rectResultShow.top+i*iOneHeight); 
			pDC->LineTo(rectResultShow.right, rectResultShow.top+i*iOneHeight); 
		}
		else
		{
			pDC->MoveTo(rectResultShow.left, rectResultShow.bottom); 
			pDC->LineTo(rectResultShow.right, rectResultShow.bottom);
		}
	}
	//����������
	for(int i=0;i<iclass;i++)
	{
		if(i==0)
		{
			for(int j=0;j<iclass;j++)
			{
				if(j==0)
				{
					CString strNum;
					strNum.Format(_T("HH"));
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else if(j<=m_ClassIndexNum)
				{
					CString strNum;
					strNum.Format(_T("%d"),j-1);
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else
				{
					CString strNum;
					strNum.Format(_T("HH"));
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
			}
		}
		else if(i<=m_ClassIndexNum)
		{
			for(int j=0;j<iclass;j++)
			{
				if(j==0)
				{
					CString strNum;
					strNum.Format(_T("%d"),i-1);
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else if(j<=m_ClassIndexNum)
				{
					CString strNum;
					strNum.Format(_T("%d"),confusion_matrix.at<int>(j-1,i-1));
					pDC->SetTextColor(RGB(0,255,255));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else if(j<=m_ClassIndexNum+1)
				{
					cv::Scalar mSum;
					mSum=cv::sum(confusion_matrix.col(i-1));
					CString strNum;
					strNum.Format(_T("%d"),(int)mSum.val[0]);
					pDC->SetTextColor(RGB(0,255,255));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else
				{
					cv::Scalar mSum;
					mSum=cv::sum(confusion_matrix.col(i-1));
					CString strNum;
					strNum.Format(_T("%d%%"),(int)(((float)confusion_matrix.at<int>(i-1,i-1)/(mSum.val[0]+0.0001))*100));
					pDC->SetTextColor(RGB(0,255,255));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
			}
		}
		else if(i<=m_ClassIndexNum+1)
		{
			for(int j=0;j<iclass;j++)
			{
				if(j==0)
				{
					CString strNum;
					strNum.Format(_T("HH"));
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else if(j<=m_ClassIndexNum)
				{
					cv::Scalar mSum;
					mSum=cv::sum(confusion_matrix.row(j-1));
					CString strNum;
					strNum.Format(_T("%d"),(int)mSum.val[0]);
					pDC->SetTextColor(RGB(0,255,255));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else
				{
					CString strNum;
					strNum.Format(_T("HH"));
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
			}
		}
		else
		{
			for(int j=0;j<iclass;j++)
			{
				if(j==0)
				{
					CString strNum;
					strNum.Format(_T("HH"));
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else if(j<=m_ClassIndexNum)
				{
					cv::Scalar mSum;
					mSum=cv::sum(confusion_matrix.row(j-1));
					CString strNum;
					strNum.Format(_T("%d%%"),(int)(((float)confusion_matrix.at<int>(j-1,j-1)/(mSum.val[0]+0.0001))*100));
					pDC->SetTextColor(RGB(0,255,255));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
				else
				{
					CString strNum;
					strNum.Format(_T("HH"));
					pDC->SetTextColor(RGB(255,255,0));
					pDC->TextOut((i+fT)*iOneWidth,(j+fT)*iOneHeight,strNum);
				}
			}
		}

	}
    pDC->SelectObject(pOldPen); // �ָ��ɻ���  
	newPen.DeleteObject(); 
	pDC->SelectObject(pOldFont); // �ָ��ɻ���  
	newFont.DeleteObject(); 
	///
}
void DlgClassifier::OnBnClickedButtonShowclassifyResult()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strParam(_T("/select,"));
	strParam += m_strAppPath;
	ShellExecute(0,_T("open"),_T("Explorer.exe"),m_strAppPath,0,SW_NORMAL);
}

LRESULT DlgClassifier::OnMsgFuncDefectClassify(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//
	return 1;
}

LRESULT DlgClassifier::OnMsgFuncDefectClassifyClose(WPARAM wParam, LPARAM lParam)
{
	if (m_hDefectFeatAndClassify.pThread != NULL)
	{
		//�ȴ��߳̽���
		::WaitForSingleObject(m_hDefectFeatAndClassify.pThread, INFINITE);
		delete m_hDefectFeatAndClassify.pThread;
		m_hDefectFeatAndClassify.pThread = NULL;
	}
	if (m_hDefectFeatAndClassify.m_ThreadParam.pMutex != NULL)
	{
		delete m_hDefectFeatAndClassify.m_ThreadParam.pMutex;
		m_hDefectFeatAndClassify.m_ThreadParam.pMutex = NULL;
	}
	//
	return 1;
}