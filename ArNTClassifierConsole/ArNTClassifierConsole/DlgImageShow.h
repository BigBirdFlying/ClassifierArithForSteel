#pragma once


// DlgImageShow �Ի���

class DlgImageShow : public CDialog
{
	DECLARE_DYNAMIC(DlgImageShow)

public:
	DlgImageShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgImageShow();

// �Ի�������
	enum { IDD = IDD_DIALOG_SHOW_ORGINIMG };

public:
	void ShowImgToMFCFormMat(int ID,cv::Mat src);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
