#pragma once
#include "ArNTOnlineDetectHelper.h"
#include "ArNTDefectArithmetic.h"
#include "ArNTSimulDefectArithmetic.h"
#include <process.h>

typedef struct tagSimulImgItem
{
	LongInt iSize;
	LONGSTR	strImgPathName;
}SimulImgItem, *PSimulImgItem;


class ArNTTestArithState :
	public ArNTArithState
{
	DECL_SINGLE_STATE(ArNTTestArithState)

public:
	ArNT_BOOL	Init(LONGSTR& strError);
	void		OnNtfSteelStatus(ArNTSteelStatus& status, StrPointer strSenderID);
	void		OnAckImageSource(ShortInt iImageSourceNum, PArNTImageSourceContext pImageSource);
	void		OnNtfTopDetectHistImg(ShortInt DetectHistImgNum, PArNTInnerDetectHistImgInfo pInfo);
	void		OnNtfBottomDetectHistImg(ShortInt DetectHistImgNum, PArNTInnerDetectHistImgInfo pInfo);
	void		OnRcvDetectDefect(PArNTDefectInfo pDefetcArray, ShortInt iDefectNum);
	void		OnNtfCameralID(ShortInt iIDNum, PTinyInt pCameralID);
	void		OnRcvDefectImg(UShortInt	iCameralNo, 
								 ULongInt	iSteelNo,
								 UShortInt  iImageIndex,
								 ULongInt	iImageSize,
								 UShortInt	iImgWidth,
								 UShortInt	iImgHeight,
								 PImgDataType	pImgeBuffer,
								 StrPointer  strSender);
	void		OnRcvCropDefectImg(UShortInt	iCameralNo, 
									  ULongInt	iSteelNo,
									  UShortInt  iImageIndex,
									  ULongInt	iImageSize,
									  UShortInt	iImgWidth,
								      UShortInt	iImgHeight,
								      PImgDataType	pImgeBuffer,
									  StrPointer  strSender);

	void			OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum);

	void			StartDetectCameral(ShortInt iCameralNo);
	void			StopDetectCameral(ShortInt iCameralNo);

	void			JumpToMaxImage(ShortInt iCameralNo);
	void            JumpToMinImage(ShortInt iCameralNo);
	
	void			DetectCurrImg(void);
	ArNT_BOOL		DetectCurrCameralImg(LONGSTR& strImgFileName, ShortInt iCameralNo);
	void            DetectLastImg(void);

	ArNT_BOOL		GetCurrImgData(LONGSTR& strImgFileName);
	ArNT_BOOL		GetCurrCameralImgData(LONGSTR& strImgFileName, ShortInt iCamralNo);
	ArNT_BOOL		SearchSimulImg(StrPointer strSimulDir);

	ArNT_BOOL		GetSimulImgData(void);
	ArNT_BOOL		GetDefectClassDesc(ShortInt iClassNo, SHORTSTR& strName);




	ArNT_BOOL		MoveToNextSimulImg(void);
	ArNT_BOOL		MoveToPrieoSimulImg(void);
	ArNT_BOOL		MoveToFirstSimulImg(void);
	ArNT_BOOL		MoveToLastSimulImg(void);

	LongFloat		GetArithTime(void);

	void			GetImageNameByIndex(LONGSTR& strFileName, LongInt iIndex);
	void			JumpToIndex(LongInt iIndex);

	void			PostDelayDetect(ShortInt iDelayTime);	
	ArNT_BOOL       IsCameralEnableAutoDetect(ShortInt iCameralNo);
	void			AddLog(StrPointer strLogInfo);

public:
	ArNT_BOOL					m_ExitApp;
	ShortInt					m_iCurrSelCamerID;
	CGrayBmp					m_GrayBmp;
	GrayBmpData					m_SrcBmpData;
	DetectImgInfo				m_DetectImgInfo;

	ArNT_BOOL					m_bAutoStartDetect;
	long						m_iCamImgMaxNum;
	UShortInt					m_iMaxDefectNum;

	ArNTDefectArithmetic		m_DefectArithmetic;
	ArNTSimulDefectArithmetic   m_SimulDefectArithmetic;

	LongInt						m_iRcvDefectNum;
	ArNT_BOOL					m_bLocalMode;
	LONGSTR						m_strSimulDir;
	LongInt						m_iSimulImgNum;
	PSimulImgItem				m_pSimulImgItem;
	LongInt						m_iCurrSimulImgIndex;	
	LONGSTR						m_strCurrSimulImgPathName;
	DetectImgInfo				m_DetectSimulImgInfo;
	ArNTDefectArithmetic		m_SimulImgDefectArithmetic;
	CameralDefectOper			m_SimulCameralDefectOper;
	LongInt						m_iStartDetectLen;

	ULongInt					m_iCurrTopSteelNo;
	ULongInt					m_iCurrBottomSteelNo;
	//2016-05-24添加
	ULongInt					H_Detect_Process_m[10];
	ULongInt					H_Detect_Process_n[10];
	//2016-07-22添加
	ULongInt					H_Detect_IsNoEqual[10];
	//2016-09-25添加
	ArNT_BOOL					m_bAutoCloseApp;
	ULongInt					m_iAutoCloseInterval;
	ULongInt					m_iChannel;
	ULongInt					m_iOddOrEven;
private:
	ArNTTestArithState(void);
	virtual ~ArNTTestArithState(void);

private:
	LONGSTR				m_strConfigFile;
	ArNTXMLOperator		m_XMLOperator;
	LongFloat			m_fArithTime;
	enum {MAX_CAMERAL_NUM =  16};
	ArNT_BOOL			m_bEnableDetect[MAX_CAMERAL_NUM];
	ArNT_BOOL			m_bCameralIsDetect[MAX_CAMERAL_NUM];
	ArNT_BOOL			m_bEnableCameralAutoDetect[MAX_CAMERAL_NUM];
	LONGSTR				m_strLogPath;
	LONGSTR				m_strAppPath;
	CTaskCriticalSection	m_csOperLog;


///////////////////////////////////////////
//线程函数
	void  InitThreadPool(ShortInt iID);
	DECL_THREAD_POOL(ArNTTestArithState)
	DECL_THREAD_POOL_ENTRY(threadDetectCameralDefect)
	DECL_THREAD_POOL_ENTRY(threadDelyDetect)
	//DECL_THREAD_POOL_ENTRY(H_threadDetect_Process)

	void  threadDetectCameralDefect(PShortInt iCameralNo, LongInt iDataLen);
	void  threadDelyDetect(PShortInt pDelayTime, LongInt iDataLen);
	//void  H_threadDetect_Process(PShortInt pDelayTime, LongInt iDataLen);

	static void  threadDetectCameralDefect2(void* pPara);

};
