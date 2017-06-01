// DlgSearchDefect.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgSearchDefect.h"
#include "ArNTTestArithState.h"


// CDlgSearchDefect 对话框

IMPLEMENT_DYNAMIC(CDlgSearchDefect, CDialog)

CDlgSearchDefect::CDlgSearchDefect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSearchDefect::IDD, pParent)
{
	m_pDefectInfo = NULL;
	m_iDefectNum = 0;

}

CDlgSearchDefect::~CDlgSearchDefect()
{
}
///////////////////////////////////////////////////////////////////
//接口定义函数

//------------------------------------------------------------------
//
void  CDlgSearchDefect::OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum)
{
	if(m_pDefectInfo) delete[] m_pDefectInfo; 
	m_iDefectNum = iDefectNum;
	m_pDefectInfo = new ArNTDefectInfo[m_iDefectNum];

	m_lstDefect.ClearAllInfo();
	for(ShortInt i = 0; i < iDefectNum; i++)
	{
		m_lstDefect.AddInfo(pInfo[i]);
		memcpy(&m_pDefectInfo[i], &pInfo[i], sizeof(pInfo[i]));
	}
	SetDlgItemInt(IDC_EDTDTNUM, iDefectNum);

}
//------------------------------------------------------------------
//
void	CDlgSearchDefect::OnRcvCropDefectImg(UShortInt	iCameralNo, 
											ULongInt	iSteelNo,
											UShortInt  iImageIndex,
											ULongInt	iImageSize,
											UShortInt	iImgWidth,
											UShortInt	iImgHeight,
											PImgDataType	pImgeBuffer)
{
	m_DlgShowPanel.ShowImg(pImgeBuffer, iImgWidth, iImgHeight);
}

///////////////////////////////////////////////////////////////////
//消息处理函数
void CDlgSearchDefect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LSTDEFECT, m_lstDefect);
}


BEGIN_MESSAGE_MAP(CDlgSearchDefect, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSTDEFECT, &CDlgSearchDefect::OnLvnItemchangedLstdefect)
END_MESSAGE_MAP()


// CDlgSearchDefect 消息处理程序

BOOL CDlgSearchDefect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_DlgShowPanel.SetSimpleModel(ArNT_TRUE);
	m_DlgShowPanel.Create(IDD_SHOWPANEL, this);
	CWnd* pControl =  GetDlgItem(IDS_DEFECTIMG);
	if(pControl)
	{
		CRect rcControl;
		pControl->GetWindowRect(&rcControl);
		CWnd::GetDesktopWindow()->MapWindowPoints(this,&rcControl);
		rcControl.InflateRect(-5, -5);
		rcControl.top += 10;
		m_DlgShowPanel.MoveWindow(&rcControl);		
		m_DlgShowPanel.ShowWindow(SW_SHOW);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSearchDefect::OnLvnItemchangedLstdefect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int iSel = pNMLV->iItem;
	if(m_pDefectInfo && iSel >= 0)
	{
		LongInt iSteelNo =  m_pDefectInfo[iSel].iSteelNo;
		TinyInt iCameralNo = m_pDefectInfo[iSel].iCameralNo;
		UShortInt iImageIndex = m_pDefectInfo[iSel].iImgIndex;

		ArNTRectInfo DefectInSteel = {sizeof(DefectInSteel)};
		DefectInSteel.iLeft = m_pDefectInfo[iSel].iLeftInSteel;
		DefectInSteel.iRight = m_pDefectInfo[iSel].iRightInSteel;
		DefectInSteel.iTop =  m_pDefectInfo[iSel].iTopInSteel;
		DefectInSteel.iBottom = m_pDefectInfo[iSel].iBottomInSteel;
			
		ArNTRectInfo DefectRect = {sizeof(DefectRect)};
		DefectRect.iLeft = m_pDefectInfo[iSel].iLeftInImage;
		DefectRect.iRight = m_pDefectInfo[iSel].iRightInImage;
		DefectRect.iTop =  m_pDefectInfo[iSel].iTopInImage;
		DefectRect.iBottom = m_pDefectInfo[iSel].iBottomInImage;


		SetDlgItemInt(IDC_EDTDTID, m_pDefectInfo[iSel].iID);
		SetDlgItemInt(IDC_EDTCAMNO, iCameralNo);
		SetDlgItemInt(IDC_EDTDTSTEELNO, iSteelNo);
		SetDlgItemInt(IDC_EDTIMGNO, iImageIndex);

		SHORTSTR strClassDesc = {0};
		if(ArNTTestArithState::GetState()->GetDefectClassDesc(m_pDefectInfo[iSel].iClass, strClassDesc))
		{
			SetDlgItemText(IDC_EDTDTCLASS, strClassDesc);
		}else
		{
			SetDlgItemText(IDC_EDTDTCLASS, L"");
		}
		
		SHORTSTR strSender = {0};
		CCommonFunc::SafeStringPrintf(strSender, STR_LEN(strSender), L"%d", (LongInt)this);
		//ArNTTestArithState::GetState()->AskCropDefectImg(iSteelNo, 
		//												iCameralNo, 
		//												iImageIndex, 
		//												DefectInSteel,
		//												DefectRect,
		//												strSender);
		//ArNTTestArithState::GetState()->AskCropDefectImg(m_pDefectInfo[iSel],
		//												strSender);
	}

}
