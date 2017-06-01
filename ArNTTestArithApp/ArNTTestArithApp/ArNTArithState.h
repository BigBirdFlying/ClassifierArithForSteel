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
	ULongInt					iCurrSteelNo;		//��ǰ���ڴ���ĸְ���ˮ��
	LONGSTR						strCurrSrcImgPath;	//��ǰ���ڴ���ĸְ�Դͼ��·��

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
	ArNT_BOOL           AskSaveDefectImgByFile( TinyInt		iCameralNo,		//ȱ��ͼ�����������
											ULongInt	iSteelNo,		//ȱ��ͼ�����ڸְ���ˮ��
											UShortInt	iImageIndex,	//ȱ��ͼ������ͼ��Ŀ¼�е�ͼ������
											StrPointer	strSrcImg,		//ȱ��Դͼ������·��		
											StrPointer strSendID = NULL);

	ArNT_BOOL			AskCurrDetectSteel(StrPointer strSendID = NULL); //ѯ�ʵ�ǰ��Ҫ���ĸְ���Ϣ
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
	//��Ϣ������
	//���յ��ظ���ȱ������������Ϣ
	ArNT_BOOL	ProcessAckDefectCenter(PCfgAckDefectCenterService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ��ظ���ȱ�����ݿ���Ϣ
	ArNT_BOOL	ProcessAckDefectDBService(PDfgAckDefectDBService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ��ظ���ȱ��ͼ����Ϣ
	ArNT_BOOL	ProcessAckDefectImageService(PDfgAckDefectImageService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ��ظ���ͼ��Դ����
	ArNT_BOOL	ProcessAckImageSource(PCfgAckImageSource pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ��ظ�����Ҫ����ȱ�ݼ��ĸְ���Ϣ
	ArNT_BOOL	ProcessAckCurrSteelInfo(PDfgAckCurrSteelInfo pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ�֪ͨ�ְ���״̬��Ϣ
	ArNT_BOOL	ProcessNtfSteelStatus(PDfgNtfSteelStatus pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ�֪ͨ��ȱ�������Ϣ
	ArNT_BOOL	ProcessAckDefectClassDefine(PDfgAckDefectClassDefine pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
	//���յ��ظ�����ʷͼ��·����Ϣ
	ArNT_BOOL	ProcessAckHistImgInfo(PISRAckHistImgInfo pPara,  ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID);
};
