// DlgImageShow.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgImageShow.h"
#include "CvvImage.h" 

// DlgImageShow 对话框

IMPLEMENT_DYNAMIC(DlgImageShow, CDialog)

DlgImageShow::DlgImageShow(CWnd* pParent /*=NULL*/)
	: CDialog(DlgImageShow::IDD, pParent)
{

}

DlgImageShow::~DlgImageShow()
{
}

void DlgImageShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgImageShow, CDialog)
END_MESSAGE_MAP()


// DlgImageShow 消息处理程序

BOOL DlgImageShow::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgImageShow::ShowImgToMFCFormMat(int ID,cv::Mat src)
{
	CRect rect_w; 
	GetWindowRect(&rect_w);
	
	CWnd *pWnd;
	pWnd = GetDlgItem( ID ); //获取控件指针，IDC_BUTTON1为控件ID号
	pWnd->SetWindowPos( NULL,0,0,rect_w.Width(),rect_w.Height(),SWP_NOZORDER | SWP_NOSIZE );
	
	
	CDC* pDC = GetDlgItem(ID)->GetDC();  
	HDC hDC = pDC->GetSafeHdc(); 
	IplImage img = src;  
	CvvImage cimg;  
	cimg.CopyOf( &img );  
	CRect rect;  
	//GetDlgItem(ID)->GetClientRect(&rect); 
	rect.top=0;
	rect.bottom=src.rows;
	rect.left=0;
	rect.right=src.cols;
	cimg.DrawToHDC(hDC, &rect);  
	ReleaseDC( pDC ); 
}