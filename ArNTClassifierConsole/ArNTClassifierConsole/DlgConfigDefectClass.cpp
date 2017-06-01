// DlgConfigDefectClass.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgConfigDefectClass.h"
#include "DlgSampleCollection.h"
#include "ArNTClassifierConsoleDlg.h"

// DlgConfigDefectClass �Ի���

IMPLEMENT_DYNAMIC(DlgConfigDefectClass, CDialog)

DlgConfigDefectClass::DlgConfigDefectClass(CWnd* pParent /*=NULL*/)
	: CDialog(DlgConfigDefectClass::IDD, pParent)
{

}

DlgConfigDefectClass::~DlgConfigDefectClass()
{
}

void DlgConfigDefectClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLASS_FROM_CHOOSE, m_listBoxClassFrom);
	DDX_Control(pDX, IDC_LIST_CLASS_TO_CHOOSE, m_listBoxClassTo);
}


BEGIN_MESSAGE_MAP(DlgConfigDefectClass, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CLASS_ADD, &DlgConfigDefectClass::OnBnClickedButtonClassAdd)
	ON_BN_CLICKED(IDC_BUTTON_CLASS_SUB, &DlgConfigDefectClass::OnBnClickedButtonClassSub)
	ON_BN_CLICKED(IDC_BUTTON_OK, &DlgConfigDefectClass::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &DlgConfigDefectClass::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// DlgConfigDefectClass ��Ϣ�������
BOOL DlgConfigDefectClass::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//InitLoadConfig();
	bool bIsOK=InitLoadClassFileName();
	if(false==bIsOK)
	{
		OnCancel();
	}

	for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.iItemNum;i++)
	{
		CString strName;
		USES_CONVERSION;
		strName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[i].strClassName);
		int nAllSel=m_listBoxClassTo.GetCount();
		bool bIsEx=false;
		for(int j=0;j<nAllSel;j++)
		{
			CString strText; 
			m_listBoxClassTo.GetText(j, strText);
			if(strName.Compare(strText)==0)
			{
				bIsEx=true;
			}
		}
		if(!bIsEx)
		{
			m_listBoxClassFrom.AddString(strName);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void DlgConfigDefectClass::OnBnClickedButtonClassAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCurSel=m_listBoxClassFrom.GetCurSel();
	int nAllSel=m_listBoxClassFrom.GetCount();
	if(nCurSel<0)
	{
		if(nAllSel>0)
		{
			AfxMessageBox(TEXT("��ѡ�����"));
		}
		else
		{
			AfxMessageBox(TEXT("�����ڴ�ѡ���"));
		}
		return;
	}
	CString strText; 
	m_listBoxClassFrom.GetText(nCurSel, strText); 

	if(LB_ERR==m_listBoxClassTo.FindString(-1,strText))
	{
		m_listBoxClassTo.AddString(strText);
		m_listBoxClassFrom.DeleteString(nCurSel);
	}
	else
	{
		AfxMessageBox(TEXT("����Ŀ¼��"));
	}
}

void DlgConfigDefectClass::OnBnClickedButtonClassSub()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCurSel=m_listBoxClassTo.GetCurSel();
	int nAllSel=m_listBoxClassTo.GetCount();
	if(nCurSel<0)
	{
		if(nAllSel>0)
		{
			AfxMessageBox(TEXT("��ѡ�����"));
		}
		else
		{
			AfxMessageBox(TEXT("��������ѡ���"));
		}
		return;
	}
	UINT nRet=MessageBox(_T(">>������������Ӧ������ļ��н���ɾ����\n>>������ѡ��!"),_T("��ʾ��Ϣ"),MB_YESNO);
	if(nRet == IDYES)
	{
		CString strText; 
		m_listBoxClassTo.GetText(nCurSel, strText); 

		if(LB_ERR==m_listBoxClassFrom.FindString(-1,strText))
		{
			m_listBoxClassFrom.AddString(strText);
			m_listBoxClassTo.DeleteString(nCurSel);
		}
		else
		{
			AfxMessageBox(TEXT("����Ŀ¼��"));
		}
	}
}



void DlgConfigDefectClass::InitLoadConfig()
{
	//HFile_xml hFileXml;
	//int iJudge=hFileXml.LoadFile(L"ArNTClassTable.xml");
	//if(1==iJudge)
	//{
	//	m_ConfigInfo_ClassConvertSet.iItemNum=0;
	//	CString strValue;
	//	hFileXml.Read(L"//ȱ������//ȱ�����//�������",strValue);
	//	int iClassTotal=_ttoi(strValue);
	//	for(int i=0;i<iClassTotal;i++)
	//	{
	//		CString strNode=L"";
	//		
	//		strNode.Format(L"//ȱ������//ȱ�����//���%d//�ڲ����",i);
	//		hFileXml.Read(strNode,strValue);
	//		m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iInternalClassNo=_ttoi(strValue);
	//		
	//		strNode.Format(L"//ȱ������//ȱ�����//���%d//����",i);
	//		hFileXml.Read(strNode,strValue);
	//		int iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
	//		WideCharToMultiByte(CP_ACP, 0, strValue, -1, m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].strClassName, iLength, NULL, NULL);

	//		strNode.Format(L"//ȱ������//ȱ�����//���%d//�ⲿ���",i);
	//		hFileXml.Read(strNode,strValue);
	//		m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iExternalClassNo=_ttoi(strValue);

	//		m_ConfigInfo_ClassConvertSet.iItemNum++;
	//	}
	//}
}

bool DlgConfigDefectClass::InitLoadClassFileName()
{
	//���д��
    CString strWildcard = m_strSampleImgPath;
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
		if (finder.IsDirectory())
		{
			//�ҵ�Ŀ¼�ļ���·��
			CString strFilePath = finder.GetFilePath();
			//�ҵ��ļ��е�����			
			CString strFileName = finder.GetFileName();	
			bool isAdd=false;
			for(int i=0;i<CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.iItemNum;i++)
			{
				CString strName;
				USES_CONVERSION;
				strName = A2T(CArNTClassifierConsoleDlg::m_ConfigInfo_ClassConvertSet.Items[i].strClassName);
				if(strFileName.Compare(strName)==0)
				{
					m_listBoxClassTo.AddString(strFileName);
					isAdd=true;
				}
			}
			if(!isAdd)
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
	} 
	finder.Close();
	return true;
}

void DlgConfigDefectClass::OnBnClickedButtonOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nAllSel=m_listBoxClassTo.GetCount();
	if(nAllSel<2)
	{
		AfxMessageBox(TEXT("��ѡ�������������"));
		return;
	}
	if(m_strSampleImgPath==L"")
	{
		AfxMessageBox(TEXT("��ѡ��������Ŀ¼"));
		return;
	}

	UINT nRet=MessageBox(_T(">>�޸ĺ��ɾ��δѡ���Ŀ¼!\n>>����ѡ�����Ŀ¼!\n>>�Ƿ�ȷ���޸�!"),_T("��ʾ��Ϣ"),MB_YESNO);
	if(nRet == IDYES)
	{
		//����Ŀ¼�������ļ�����δѡ�е�����ļ�ɾ��
		CString strWildcard;
		strWildcard.Format(L"%s\\*.*",m_strSampleImgPath);
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
				for(int i=0;i<nAllSel;i++)
				{
					CString strText; 
					m_listBoxClassTo.GetText(i, strText);
					if(strFileName.Compare(strText)==0)
					{
						isEffect=true;
					}
				}
				if(!isEffect)
				{
					DeleteTree(strFilePath);
				}
			}

			CString strFilePath = finder.GetFilePath();
			::DeleteFile(strFilePath);
		} 
		finder.Close();
		//
		for(int i=0;i<nAllSel;i++)
		{
			CString strText; 
			m_listBoxClassTo.GetText(i, strText);

			CString strClassPath=L"";
			strClassPath.Format(L"%s\\%s\\",m_strSampleImgPath,strText);

			int IsExist=PathFileExists(strClassPath);
			if(!IsExist)
			{
				CreateDirectory(strClassPath, NULL);
			}
		}
	}
	else
	{
		return;
	}
	OnOK(); 	
}

bool DlgConfigDefectClass::DeleteTree(CString strDirectory)
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

void DlgConfigDefectClass::OnBnClickedButtonCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}
