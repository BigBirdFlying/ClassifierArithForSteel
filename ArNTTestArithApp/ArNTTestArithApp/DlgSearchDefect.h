#pragma once

#include "LstDefectInfo.h"
#include "afxcmn.h"

#include "DlgShowPanel.h"

// CDlgSearchDefect �Ի���

class CDlgSearchDefect : public CDialog
{
	DECLARE_DYNAMIC(CDlgSearchDefect)
public:
	void OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum);

	void	OnRcvCropDefectImg(UShortInt	iCameralNo, 
								ULongInt	iSteelNo,
								UShortInt  iImageIndex,
								ULongInt	iImageSize,
								UShortInt	iImgWidth,
								UShortInt	iImgHeight,
								PImgDataType	pImgeBuffer);

public:
	CDlgSearchDefect(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSearchDefect();

// �Ի�������
	enum { IDD = IDD_SEARCHDEFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CLstDefectInfo m_lstDefect;

private:
	CDlgShowPanel		m_DlgShowPanel; 
	PArNTDefectInfo		m_pDefectInfo;
	ShortInt			m_iDefectNum;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedLstdefect(NMHDR *pNMHDR, LRESULT *pResult);
};
