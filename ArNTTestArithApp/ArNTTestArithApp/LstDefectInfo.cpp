// LstRcvInfo.cpp : implementation file
//

#include <afxwin.h>   
#include <afxcmn.h>    
#include "LstDefectInfo.h"
#include "CommonFunc.h"


// CLstRcvInfo

IMPLEMENT_DYNAMIC(CLstDefectInfo, CListCtrl)

CLstDefectInfo::CLstDefectInfo()
{

	m_iItemIndex = 0;
}

CLstDefectInfo::~CLstDefectInfo()
{
}


BEGIN_MESSAGE_MAP(CLstDefectInfo, CListCtrl)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CLstDefectInfo::PreSubclassWindow()
{
	LONG lStyle = SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE);
	lStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |	LVS_EX_HEADERDRAGDROP;
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0,	(LPARAM)lStyle);

	CRect rcClient;
	GetClientRect(&rcClient);
	int iWidth = rcClient.Width();

	
	//添加列信息
	LV_COLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH /*| LVCF_FMT*/;

	lvc.iSubItem = 0;
	lvc.pszText = L"缺陷号";
	lvc.cx = (int)(iWidth * 0.15);
	InsertColumn(lvc.iSubItem, &lvc);

	lvc.iSubItem = 1;
	lvc.pszText = L"相机号";
	lvc.cx = (int)(iWidth * 0.15);
	InsertColumn(lvc.iSubItem, &lvc);

	lvc.iSubItem = 2;
	lvc.pszText = L"类别";
	lvc.cx = (int)(iWidth * 0.1);
	InsertColumn(lvc.iSubItem, &lvc);

	lvc.iSubItem = 3;
	lvc.pszText = L"纵向位置";
	lvc.cx = (int)(iWidth * 0.2);
	InsertColumn(lvc.iSubItem, &lvc);

	lvc.iSubItem = 4;
	lvc.pszText = L"横向位置";
	lvc.cx = (int)(iWidth * 0.2);
	InsertColumn(lvc.iSubItem, &lvc);

	
	lvc.iSubItem = 5;
	lvc.pszText = L"宽度";
	lvc.cx = (int)(iWidth * 0.1);

	InsertColumn(lvc.iSubItem, &lvc);
	lvc.iSubItem = 6;
	lvc.pszText = L"高度";
	lvc.cx = (int)(iWidth * 0.1);
	
	InsertColumn(lvc.iSubItem,&lvc);
	CListCtrl::PreSubclassWindow();
}


void CLstDefectInfo::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);
	ChangeSize();

	// TODO: Add your message handler code here
	
}

void CLstDefectInfo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CListCtrl::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(bShow) ChangeSize();
}

//////////////////////////////////////////////////////////////////////////
//接口函数
//------------------------------------------------------------------------
//
void  CLstDefectInfo::ClearAllInfo()
{
	m_iItemIndex = 0;
	this->DeleteAllItems();
}
//------------------------------------------------------------------------
//
void  CLstDefectInfo::AddInfo(ArNTDefectInfo& Defect)
{

	if(m_iItemIndex > 2048) this->DeleteItem(0);
	
	LV_ITEM lvitem;
	lvitem.mask = LVIF_TEXT | LVIF_STATE;
	lvitem.state = 0;      
	lvitem.stateMask = 0; 
	lvitem.iItem = GetItemCount();
	int  iSubIndex  = 0;
	lvitem.iSubItem = iSubIndex++;	

	CString strID;
	strID.Format(L"%d", Defect.iID);
	lvitem.pszText = strID.GetBuffer(0);	
	int iIndex = InsertItem(&lvitem);

	lvitem.iSubItem =  iSubIndex++;	
	SHORTSTR strInfo = {0};
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d", Defect.iCameralNo);
	SetItemText(lvitem.iItem, lvitem.iSubItem, strInfo);


	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d", Defect.iClass);
	lvitem.iSubItem =  iSubIndex++;	
	SetItemText(lvitem.iItem, lvitem.iSubItem, strInfo);

	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d", Defect.iTopInSteel);
	lvitem.iSubItem =  iSubIndex++;	
	SetItemText(lvitem.iItem, lvitem.iSubItem, strInfo);
	
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d", Defect.iLeftInSteel);
	lvitem.iSubItem =  iSubIndex++;	
	SetItemText(lvitem.iItem, lvitem.iSubItem, strInfo);

	ShortInt iWidth = Defect.iRightInSteel - Defect.iLeftInSteel;
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d", iWidth);
	lvitem.iSubItem =  iSubIndex++;	
	SetItemText(lvitem.iItem, lvitem.iSubItem, strInfo);

	ShortInt iHeight = Defect.iBottomInSteel - Defect.iTopInSteel;
	CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d", iHeight);
	lvitem.iSubItem =  iSubIndex++;	
	SetItemText(lvitem.iItem, lvitem.iSubItem, strInfo);
}
//------------------------------------------------------------------------
//

void   CLstDefectInfo::ChangeSize(void)
{
	if(this->GetSafeHwnd())
	{
		CRect rcClient;
		this->GetClientRect(&rcClient);
		int iWidth = rcClient.Width();

		CHeaderCtrl* pHeader = GetHeaderCtrl();

		HDITEM hItem;
		int iIndex = 0;
		hItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH | HDI_STATE;
		hItem.fmt = HDF_CENTER | HDF_STRING;
		hItem.state = HDIS_FOCUSED;


		hItem.pszText = L"缺陷号";
		hItem.cxy = (int)(iWidth * 0.15);	
		pHeader->SetItem(iIndex++, &hItem);

		hItem.pszText = L"相机号";
		hItem.cxy = (int)(iWidth * 0.15);
		pHeader->SetItem(iIndex++, &hItem);

		hItem.pszText = L"类别";
		hItem.cxy =  (int)(iWidth * 0.15);
		pHeader->SetItem(iIndex++, &hItem);

		
		hItem.pszText = L"纵向位置";
		hItem.cxy =  (int)(iWidth * 0.15);
		pHeader->SetItem(iIndex++, &hItem);

		hItem.pszText = L"横向位置";
		hItem.cxy =  (int)(iWidth * 0.15);
		pHeader->SetItem(iIndex++, &hItem);

		hItem.pszText = L"宽度";
		hItem.cxy =  (int)(iWidth * 0.1);
		pHeader->SetItem(iIndex++, &hItem);

		hItem.pszText = L"高度";
		hItem.cxy =  (int)(iWidth * 0.1);
		pHeader->SetItem(iIndex++, &hItem);
	}

}



