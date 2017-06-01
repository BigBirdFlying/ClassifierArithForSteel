#pragma once

#include "CommonFunc.h"
#include "ArNtBaseDataStruct.h"
#include "ArNTCommonClass.h"
#include "ArNTOnlineDetectClass.h"
#include "ArNTDebugModule.h"
#include "ArNTDefectCenterOper.h"
#include "ArNTDefectDBOper.h"
#include "ArNTDefectImgOper.h"

typedef struct tagCameralDefectOper
{
	LongInt						iSize;
	ShortInt					iCameralID;
	ULongInt					iCurrSteelNo;		//当前正在处理的钢板流水号
	LONGSTR						strCurrSrcImgPath;	//当前正在处理的钢板源图像路径

	ArNTNetCommandOper			NetCommandOper;
	ArNTNetChannelInfo			NetChannel;
	ArNTDefectDBOper			DefectDBOper;
	ArNTDefectImgOper			DefectImgOper;
}CameralDefectOper, *PCameralDefectOper;

////////////////////////////////////////////
//
class ArNTArithState
{
	DECL_NET_COMMAND_HANDLER()
public:
	virtual ArNT_BOOL	Init(void);
	ArNT_BOOL			WaitInit(void);

	ArNT_BOOL			AskAddDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum, ShortInt iUseCameral, StrPointer strSenderID = NULL);
	ArNT_BOOL           AskSaveDefectImgByFile( TinyInt		iCameralNo,		//缺陷图像所在相机号
											ULongInt	iSteelNo,		//缺陷图像所在钢板流水号
											UShortInt	iImageIndex,	//缺陷图像所在图像目录中的图像索引
											StrPointer	strSrcImg,		//缺陷源图像完整路径		
											StrPointer strSendID = NULL);

	ArNT_BOOL			AskCurrDetectSteel(StrPointer strSendID = NULL); //询问当前需要检测的钢板信息
	ArNT_BOOL			AskCurrProcessSteelHistImg(ULongInt iSteelNo, StrPointer strSendID = NULL);

private:
	ArNT_BOOL			InitDefectOper(void);
	

public:
	virtual  void  OnAckCurrSteelInfo(ULongLongInt iSequceNo,StrPointer	strID,StrPointer strSenderID);
	virtual  void  OnNtfSteelStatus(ArNTSteelStatus& status, StrPointer strSenderID);

private:
	ArNTCFGCenterOper			m_CFGCenterOper;	
	ArNTDefectCenterOper		m_DefectCenterOper;
	ArNTImageSourceOper			m_ImageSourceOper;
public:
	ServiceLocation				m_DefectCenterService;
	ServiceLocation				m_ConfigCenterService;
public:
	ShortInt					m_iDefectDBServiceNum;
	PArNTDefectDBServiceInfo	m_pDefectDBService;

	ShortInt					m_iDefectImgServiceNum;
	PArNTDefectImageServiceInfo	m_pDefectImgService;

	ShortInt					m_iImageSourceNum;
	PArNTImageSourceContext		m_pImageSourceContext;

	ULongInt					m_iCurrDetectSteelNo;
	SHORTSTR					m_strCurrDetectID;
public:
	PSampleClassItem			m_pDefectClassContext;
	ShortInt					m_iDefectClassNum;


private:
	ArNTNetCommandOper			m_NetCommandOper;
	ArNTNetChannelInfo			m_NetChannel;		
	ArNT_BOOL					m_bTrace;	

	NetIP						m_CFGIP;
	UShortInt					m_iCFGPort;

	HANDLE						m_hEvtDefectCenter;
	HANDLE						m_hEvtDefectDBService;
	HANDLE						m_hEvtDefectImageService;
	HANDLE						m_hEvtImgSource;

public:
	PCameralDefectOper			m_pCameralDefectOper;
	ShortInt					m_iCameralDefectOperNum;

	PCameralDefectOper			FindCameralDefctOper(ShortInt iCameralID);
	PArNTDefectImageServiceInfo FindDefectImageService(ShortInt iCameralID);
	PArNTDefectDBServiceInfo	FindDefectDBService(ShortInt iCameralID);
	PArNTImageSourceContext		FindImageSourceContext(ShortInt iCameralID);

public:
	ArNTArithState(void);
	virtual ~ArNTArithState(void);

private:
	////////////////////////////////////////////////////////
	//消息处理函数
	//接收到回复的缺陷配置中心消息
	ArNT_BOOL	ProcessAckDefectCenter(PCfgAckDefectCenterService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到回复的缺陷数据库消息
	ArNT_BOOL	ProcessAckDefectDBService(PDfgAckDefectDBService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到回复的缺陷图像消息
	ArNT_BOOL	ProcessAckDefectImageService(PDfgAckDefectImageService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到回复的图像源服务
	ArNT_BOOL	ProcessAckImageSource(PCfgAckImageSource pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到回复的需要进行缺陷检测的钢板信息
	ArNT_BOOL	ProcessAckCurrSteelInfo(PDfgAckCurrSteelInfo pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到通知钢板检测状态信息
	ArNT_BOOL	ProcessNtfSteelStatus(PDfgNtfSteelStatus pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到通知的缺陷类别信息
	ArNT_BOOL	ProcessAckDefectClassDefine(PDfgAckDefectClassDefine pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//接收到回复的历史图像路径信息
	ArNT_BOOL	ProcessAckHistImgInfo(PISRAckHistImgInfo pPara,  ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
};
