// ArNTClassifierConsoleDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "DlgClassifier.h"
#include "DlgDefectFromDBClassify.h"
#include "DlgSampleCollection.h"
#include "DlgHelp.h"
#include "HFile.h"
#include "DataDefCommon.h"

// CArNTClassifierConsoleDlg �Ի���
class CArNTClassifierConsoleDlg : public CDialog
{
// ����
public:
	CArNTClassifierConsoleDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ARNTCLASSIFIERCONSOLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
//�Ի������
public:
	DlgDefectFromDBClassify		m_DlgDefectClassify;
	DlgSampleCollection			m_DlgSampleCollection;
	DlgClassifier				m_DlgClassifier;
	DlgHelp						m_DlgHelp;

	HFile_log					m_LogOper;
	CString						m_strAppPath;

public:
	static ConfigInfo_Local					m_ConfigInfo_Local;
	static ConfigInfo_ClassConvertSet		m_ConfigInfo_ClassConvertSet;
	static ConfigInfo_Segmentation			m_ConfigInfo_Segmentation;
	void InitLoadConfig();

//����

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CTabCtrl m_MainTab;
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
};
