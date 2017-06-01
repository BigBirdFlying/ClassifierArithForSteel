#pragma once
#include "afxwin.h"
#include "HFile.h"
#include "H_DataTypeDef.h"
// DlgConfigDefectClass �Ի���

class DlgConfigDefectClass : public CDialog
{
	DECLARE_DYNAMIC(DlgConfigDefectClass)

public:
	DlgConfigDefectClass(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgConfigDefectClass();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONFIG_DEFECTCLASS };

public:
	//ConfigInfo_ClassConvertSet		m_ConfigInfo_ClassConvertSet;
	CString				m_strSampleImgPath;

	void InitLoadConfig();
	bool InitLoadClassFileName();
	bool DeleteTree(CString strDirectory);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listBoxClassFrom;
	CListBox m_listBoxClassTo;
	afx_msg void OnBnClickedButtonClassAdd();
	afx_msg void OnBnClickedButtonClassSub();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
