// DlgImageShow.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgImageShow.h"
#include "CvvImage.h" 

// DlgImageShow �Ի���

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


// DlgImageShow ��Ϣ�������

BOOL DlgImageShow::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void DlgImageShow::ShowImgToMFCFormMat(int ID,cv::Mat src)
{
	CRect rect_w; 
	GetWindowRect(&rect_w);
	
	CWnd *pWnd;
	pWnd = GetDlgItem( ID ); //��ȡ�ؼ�ָ�룬IDC_BUTTON1Ϊ�ؼ�ID��
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