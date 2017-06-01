// DlgConfigDefectClass.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgConfigDefectClass.h"
#include "DlgSampleCollection.h"
#include "ArNTClassifierConsoleDlg.h"

// DlgConfigDefectClass 对话框

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


// DlgConfigDefectClass 消息处理程序
BOOL DlgConfigDefectClass::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
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
	// 异常: OCX 属性页应返回 FALSE
}

void DlgConfigDefectClass::OnBnClickedButtonClassAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCurSel=m_listBoxClassFrom.GetCurSel();
	int nAllSel=m_listBoxClassFrom.GetCount();
	if(nCurSel<0)
	{
		if(nAllSel>0)
		{
			AfxMessageBox(TEXT("请选择类别"));
		}
		else
		{
			AfxMessageBox(TEXT("不存在待选类别"));
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
		AfxMessageBox(TEXT("已在目录下"));
	}
}

void DlgConfigDefectClass::OnBnClickedButtonClassSub()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCurSel=m_listBoxClassTo.GetCurSel();
	int nAllSel=m_listBoxClassTo.GetCount();
	if(nCurSel<0)
	{
		if(nAllSel>0)
		{
			AfxMessageBox(TEXT("请选择类别"));
		}
		else
		{
			AfxMessageBox(TEXT("不存在已选类别"));
		}
		return;
	}
	UINT nRet=MessageBox(_T(">>撤销此类别后相应的类别文件夹将被删除！\n>>请慎重选择!"),_T("提示信息"),MB_YESNO);
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
			AfxMessageBox(TEXT("已在目录下"));
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
	//	hFileXml.Read(L"//缺陷类别表//缺陷类别//类别总数",strValue);
	//	int iClassTotal=_ttoi(strValue);
	//	for(int i=0;i<iClassTotal;i++)
	//	{
	//		CString strNode=L"";
	//		
	//		strNode.Format(L"//缺陷类别表//缺陷类别//类别%d//内部编号",i);
	//		hFileXml.Read(strNode,strValue);
	//		m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iInternalClassNo=_ttoi(strValue);
	//		
	//		strNode.Format(L"//缺陷类别表//缺陷类别//类别%d//名称",i);
	//		hFileXml.Read(strNode,strValue);
	//		int iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
	//		WideCharToMultiByte(CP_ACP, 0, strValue, -1, m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].strClassName, iLength, NULL, NULL);

	//		strNode.Format(L"//缺陷类别表//缺陷类别//类别%d//外部编号",i);
	//		hFileXml.Read(strNode,strValue);
	//		m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iExternalClassNo=_ttoi(strValue);

	//		m_ConfigInfo_ClassConvertSet.iItemNum++;
	//	}
	//}
}

bool DlgConfigDefectClass::InitLoadClassFileName()
{
	//类别写入
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
			//找到目录文件的路径
			CString strFilePath = finder.GetFilePath();
			//找到文件夹的名称			
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
				strWarn.Format(L">>类别(%s)为未配置类别!\n>>点击(是)该文件夹将被删除!\n>>点击(否)可返回重新配置!",strFileName);
				UINT nRet=MessageBox(strWarn,_T("提示信息"),MB_YESNO);
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
	// TODO: 在此添加控件通知处理程序代码
	int nAllSel=m_listBoxClassTo.GetCount();
	if(nAllSel<2)
	{
		AfxMessageBox(TEXT("请选择至少两个类别"));
		return;
	}
	if(m_strSampleImgPath==L"")
	{
		AfxMessageBox(TEXT("请选择样本集目录"));
		return;
	}

	UINT nRet=MessageBox(_T(">>修改后会删除未选类别目录!\n>>创建选中类别目录!\n>>是否确认修改!"),_T("提示信息"),MB_YESNO);
	if(nRet == IDYES)
	{
		//查找目录下所有文件，将未选中的类别文件删除
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
				//找到目录文件的路径
				CString strFilePath = finder.GetFilePath();
				//找到目录文件的名
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
			if (!DeleteTree(strPathName))//递归删除目录
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
	//删除空目录
	if (!::RemoveDirectory(strDirectory))
	{
		return FALSE;
	}
	return TRUE;
}

void DlgConfigDefectClass::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
