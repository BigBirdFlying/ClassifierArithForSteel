// ArNTTestArithAppDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "ArNTISDataStruct.h"
#include "DlgShowDlg.h"
#include "DlgSearchDefect.h"
#include "ArNTSteelImageOper.h"
#include "ArNTCommonClass.h"
#include "ArNTDefectArith_H.h"

#define  BTN_SELCAME_ID				100
#define  UM_NTF_CAMERAL_ARRAY		WM_APP + 20
#define  UM_NTF_DETECTDEFECT		WM_APP + 21

#define POINT_COUNT 100

typedef struct tagUM_NtfDetectDefect
{
	PArNTDefectInfo pDefetcArray;
	ShortInt iDefectNum;
}UM_NtfDetectDefect, *PUM_NtfDetectDefect;


typedef struct tagTestArithStateConfig
{
	ArNT_BOOL    bDelyProcess;
	UShortInt	iDelyTime;
}TestArithStateConfig, *PTestArithStateConfig;

// CArNTTestArithAppDlg 对话框
class CArNTTestArithAppDlg : public CDialog
{
public:
	void	OnNtfSteelStatus(ArNTSteelStatus& status);
	void	OnNewTopSteelIn(ULongInt iSequenceNo);

	void	OnRcvDefectImg(UShortInt	iCameralNo, 
								 ULongInt	iSteelNo,
								 UShortInt  iImageIndex,
								 ULongInt	iImageSize,
								 UShortInt	iImgWidth,
								 UShortInt	iImgHeight,
								 PImgDataType	pImgeBuffer);

	void	OnRcvCropDefectImg(UShortInt	iCameralNo, 
								ULongInt	iSteelNo,
								UShortInt  iImageIndex,
								ULongInt	iImageSize,
								UShortInt	iImgWidth,
								UShortInt	iImgHeight,
								PImgDataType	pImgeBuffer,
								StrPointer  strSender);

	void	OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum);

	void	OnNtfCameralID(ShortInt iIDNum, PTinyInt pCameralID);
	void	OnRcvDetectDefect(PArNTDefectInfo pDefetcArray, ShortInt iDefectNum);

	LongInt GetCurrStartDetectLen(void);
	void    AddErrorInfo(CString strError);
	void    DelayDetect(ShortInt iMaxDelyTime, ShortInt iCurrDelyTime);

	ArNTXMLOperator		m_XMLOperator;
	TestArithStateConfig m_TestArithStateConfig;
	void    ReadConfig();

// 构造
public:
	CArNTTestArithAppDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ARNTTESTARITHAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lstImgSrc;
	CListBox m_lstDefectDB;
	CListBox m_lstDefectImg;
	//CListBox m_lstCamDefect;

	CDlgShowDlg			m_DlgShowImg;
	CDlgShowDlg			m_DlgShowCropImg;
	CDlgSearchDefect	m_DlgSearchDefect;
	GrayBmpData			m_GrayBmpData;

	CButton*				m_pBtnChkSelCameral;
	ShortInt				m_iCameralNum;
	CFont					m_FontBtn;
	CTaskCriticalSection	m_csAddDefectInfo;

	LONGSTR					m_strDefectImgPath;

	UShortInt				m_iDelyTime;

	

	void  ShowCameralList(void);

private:
	void  SetImgSrcInfo(void);
	void  SetDefectDBInfo(void);
	void  SetDefectImgInfo(void);
	void  AdjustChkSelCameral(void);

	ArNT_BOOL  ReadFirstImgData(TinyInt iCameralNo, LONGSTR& strImgFileName);
	ArNT_BOOL  ReadNextImgData(TinyInt iCameralNo, LONGSTR& strImgFileName);
	ArNT_BOOL  ReadPrieoImgData(TinyInt iCameralNo, LONGSTR& strImgFileName);
	ArNT_BOOL  ReadLastImgData(TinyInt iCameralNo, LONGSTR& strImgFileName);
public:
	//2016-07-22添加
	int m_iTopSequeceNo;
	int m_iBottomSequeceNo;
	void AddLog(StrPointer strLogInfo);
	int m_iAlgoSelect;
public:	
	//线程函数
	void  InitThreadPool(ShortInt iID){};
	DECL_THREAD_POOL(CArNTTestArithAppDlg)
	DECL_THREAD_POOL_ENTRY(threadShowDefectNum)
	void  threadShowDefectNum(PShortInt pInterval,  LongInt iDataLen); 
	//void  SetCameralDefectInfo(void);
public:
	CListBox m_lstTopDetectHistImgInfo;
	CListBox m_lstBottomDetectHistImg;
	CListBox m_lstCameralList;
	afx_msg void OnLbnSelchangeListcamlist();
	afx_msg void OnBnClickedBtnshowimg();
	afx_msg void OnDeltaposSpinreview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtndetect();
	CListBox m_lstDefect;
	afx_msg void OnBnClickedChkautowritedb();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedGetdtimg();
	afx_msg void OnBnClickedBtncropdtimg();
	afx_msg void OnBnClickedBtnsearchdefect();
	CComboBox m_cmbSelCameral;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnsetdir();
	afx_msg void OnBnClickedBtncurrsimulimgpath();
	afx_msg void OnBnClickedBtnregapp();
	afx_msg LRESULT  OnNtfCameralArray(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheckdetect();
	afx_msg void OnBnClickedBtnfromcurrpos();
	afx_msg void OnClose();
	afx_msg void OnExitApp();
	afx_msg void OnEnChangeEdtimgnum();
	afx_msg void OnBnClickedBtnclear();
	afx_msg void OnBnClickedBtnjumpto();
	CListBox m_lstErrorInfo;
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLbnSelchangeListimgsrc();
	afx_msg void OnEnChangeEdtdelytime();

	CStatic m_picDraw;
	int   m_nzValues[POINT_COUNT];
	void DrawWave(CDC *pDC, CRect &rectPicture);   
	afx_msg void OnBnClickedButtonAddalgo();
	CComboBox m_SelectAlgo;
};
////////////////////////////////////////////////

extern CArNTTestArithAppDlg* pMainDlg;
