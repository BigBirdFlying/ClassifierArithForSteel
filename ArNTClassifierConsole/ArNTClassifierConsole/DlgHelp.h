#pragma once


// DlgHelp �Ի���

class DlgHelp : public CDialog
{
	DECLARE_DYNAMIC(DlgHelp)

public:
	DlgHelp(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgHelp();

public:
	int m_nTextX;   // ˮƽ�����ı�����ʼ���x����   
	int m_nTextY;   // ��ֱ�����ı�����ʼ���y����   
	CFont m_newFont;   // ������   
	CFont *m_pOldFont; // ѡ��������֮ǰ������  

// �Ի�������
	enum { IDD = IDD_DIALOG_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
