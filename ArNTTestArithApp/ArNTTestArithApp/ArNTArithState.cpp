#include "ArNTArithState.h"


////////////////////////////////////////////
//
ArNTArithState::ArNTArithState(void)
{
	m_NetChannel.iSize = sizeof(m_NetChannel);
	m_NetChannel.Port = PORT_DEFECT_ARITH;
	m_NetChannel.ID = 0;
	m_NetChannel.iPermitRemoteNum = 0;
	CCommonFunc::SafeStringCpy(m_NetChannel.strChannelName, STR_LEN(m_NetChannel.strChannelName), L"ArithNetChannel");
	CCommonFunc::SafeStringCpy(m_NetChannel.IP, STR_LEN(m_NetChannel.IP), L"");

	m_DefectCenterService.iPort = 0;
	m_ConfigCenterService.iPort = 0;

	m_iDefectDBServiceNum = 0;
	m_pDefectDBService = NULL;

	m_iDefectImgServiceNum = 0;
	m_pDefectImgService = NULL;

	m_iImageSourceNum = 0;
	m_pImageSourceContext = NULL;

	m_pDefectClassContext = NULL;
	m_iDefectClassNum = 0;

	m_hEvtDefectCenter = CCommonFunc::CreateNullLimetEvent(TRUE, FALSE, NULL);
	m_hEvtDefectDBService = CCommonFunc::CreateNullLimetEvent(TRUE, FALSE, NULL);
	m_hEvtDefectImageService = CCommonFunc::CreateNullLimetEvent(TRUE, FALSE, NULL);
	m_hEvtImgSource = CCommonFunc::CreateNullLimetEvent(TRUE, FALSE, NULL);

	m_iCameralDefectOperNum = 0;
	m_pCameralDefectOper = NULL;

	
	m_pTopDetectHistImg = 0;
	m_iTopDetectHistImgNum = 0;

	m_pBottomDetectHistImg = 0;
	m_iBottomDetectHistImgNum = 0;


	m_iCurrDetectSteelNo = 0;
	CCommonFunc::SafeStringCpy(m_strCurrDetectID, STR_LEN(m_strCurrDetectID), L"");

	for(ShortInt i = 0; i < m_SteelHistImgPathCache.MAX_ITEM_NUM; i++)
	{
		m_SteelHistImgPathCache.Items[i].iSize = sizeof(m_SteelHistImgPathCache.Items[i]);
		m_SteelHistImgPathCache.Items[i].iCameralNo = 0;
		m_SteelHistImgPathCache.Items[i].iID = 0;
		m_SteelHistImgPathCache.Items[i].iSteelNo = 0;
	}
	m_SteelHistImgPathCache.iMaxID = 0;
	m_iCameralNum = 0;



}

ArNTArithState::~ArNTArithState(void)
{
	::CloseHandle(m_hEvtDefectCenter);
	::CloseHandle(m_hEvtDefectDBService);
	::CloseHandle(m_hEvtDefectImageService);
}

//////////////////////////////////////////////////////////////////////////
//接口函数

ArNT_BOOL  ArNTArithState::Init(void)
{
	LONGSTR strInfo = {0};
	//获取参数配置中心服务位置
	ShortInt iIndex = CCommonFunc::FindStrInRunPara(L"-ParaCenter");
	if(iIndex >= 0)
	{
		LONGSTR strPara = {0};
		if(CCommonFunc::GetStrInRunPara(iIndex+1, strPara))
		{
			CCommonFunc::SafeStringCpy(m_CFGIP, STR_LEN(m_CFGIP), strPara);
		}
		
	}else
	{
		CCommonFunc::ErrorBox(L"%s", L"未指定配置中心的IP地址,格式为:\" -ParaCenter IP地址\"");
		return ArNT_FALSE;
	}
	m_iCFGPort = PORT_CONFIG_CENTER;
	m_CFGCenterOper.Init(m_CFGIP, m_iCFGPort);

	m_ConfigCenterService.iSize = sizeof(m_ConfigCenterService);
	CCommonFunc::SafeStringCpy(m_ConfigCenterService.strIP, STR_LEN(m_ConfigCenterService.strIP), m_CFGIP);
	m_ConfigCenterService.iPort = m_iCFGPort;
	CCommonFunc::SafeStringCpy(m_ConfigCenterService.strDesc, STR_LEN(m_ConfigCenterService.strDesc), L"ConfigCenterService");

	
	iIndex = CCommonFunc::FindStrInRunPara(L"-trace");
	if(iIndex >= 0) 
	{
		m_bTrace = ArNT_TRUE;
	}else
	{
		m_bTrace = ArNT_FALSE;
	}

	if(m_NetCommandOper.FindNetIP(m_CFGIP, m_NetChannel.IP) ==  ArNT_FALSE)
	{
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"无法查找到与配置中心(IP:%s)同网段的IP地址", m_NetChannel.IP);
		CCommonFunc::ErrorBox(L"%s", strInfo);
		return ArNT_FALSE;
	}	

	if(INIT_COMMAND_OPER(m_NetCommandOper, m_NetChannel) == ArNT_FALSE)
	{
		CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"无法创建网络通道(IP:%s Port:%d)", m_NetChannel.IP, m_NetChannel.Port);
		CCommonFunc::ErrorBox(L"%s", strInfo);
		return ArNT_FALSE;
	}
	
	//询问当前的缺陷配置中心服务
	m_CFGCenterOper.AskDefectCenterService(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port);
	Sleep(200);
	//询问当前的图像源服务
	m_CFGCenterOper.AskImageSourceService(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port);

	return ArNT_TRUE;

}

//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::WaitInit(void)
{
	HANDLE hEvtWait[4] = {0};
	hEvtWait[0] = m_hEvtDefectCenter;
	hEvtWait[1] = m_hEvtDefectDBService;
	hEvtWait[2] = m_hEvtDefectImageService;
	hEvtWait[3] = m_hEvtImgSource;

	

	if(WAIT_TIMEOUT == ::WaitForMultipleObjects(4, hEvtWait, TRUE, 2000)) 
	{
		if(	::WaitForSingleObject(m_hEvtDefectCenter, 10) == WAIT_TIMEOUT)
		{
			CCommonFunc::ErrorBox(L"%s", L"请检查缺陷配置中心服务是否已经打开");
			return ArNT_FALSE;
		}
		if(	::WaitForSingleObject(m_hEvtDefectDBService, 10) == WAIT_TIMEOUT)
		{
			CCommonFunc::ErrorBox(L"%s", L"请检查缺陷数据库服务是否已经打开");
			return ArNT_FALSE;
		}
		if(	::WaitForSingleObject(m_hEvtDefectImageService, 10) == WAIT_TIMEOUT)
		{
			CCommonFunc::ErrorBox(L"%s", L"请检查缺陷图像服务是否已经打开");
			return ArNT_FALSE;
		}

		if(	::WaitForSingleObject(m_hEvtImgSource, 10) == WAIT_TIMEOUT)
		{
			CCommonFunc::ErrorBox(L"%s", L"请检查图像源服务是否已经打开");
			return ArNT_FALSE;
		}
		
	}

	if(ArNT_FALSE ==  InitDefectOper()) return ArNT_FALSE;

	 ShortInt iCameralIDNum = 0;	
	for(ShortInt i = 0; i < m_iImageSourceNum; i++)
	{
		iCameralIDNum += m_pImageSourceContext[i].iCameralNum;
	}
	if(iCameralIDNum > 0)
	{
		PTinyInt pCameralID = new TinyInt[iCameralIDNum];
		iCameralIDNum = 0;
		for(ShortInt i = 0; i < m_iImageSourceNum; i++)
		{
			for(ShortInt j = 0; j <  m_pImageSourceContext[i].iCameralNum; j++)
			{
				pCameralID[iCameralIDNum] = (TinyInt)m_pImageSourceContext[i].iCameralID[j];
				iCameralIDNum++;
			}
		}
		OnNtfCameralID(iCameralIDNum, pCameralID);
		delete[] pCameralID;
	}
	return ArNT_TRUE;
}
//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::AskAddDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum, ShortInt iUseCameral, StrPointer strSenderID)
{
	PCameralDefectOper pOper = FindCameralDefctOper(iUseCameral);
	if(pOper)
	{
		return  pOper->DefectDBOper.AskAddDefect(pOper->NetCommandOper, pOper->NetChannel.IP, pOper->NetChannel.Port, iDefectNum, pInfo, strSenderID);
	}else
	{
		return ArNT_FALSE;
	}

}
//----------------------------------------------------------------------
//
ArNT_BOOL  ArNTArithState::AskSaveDefectImgByFile( TinyInt		iCameralNo,		//缺陷图像所在相机号
													ULongInt	iSteelNo,		//缺陷图像所在钢板流水号
													UShortInt	iImageIndex,	//缺陷图像所在图像目录中的图像索引
													StrPointer	strSrcImg,		//缺陷源图像完整路径		
													StrPointer strSendID)
{
	PCameralDefectOper pOper = FindCameralDefctOper(iCameralNo);
	if(pOper)
	{
		return pOper->DefectImgOper.AskSaveImageByFile(pOper->NetCommandOper, 
												pOper->NetChannel.IP, 
												pOper->NetChannel.Port, 
												iCameralNo,
												iSteelNo,
												iImageIndex,
												strSrcImg,
												strSendID);

	}else
	{
		return ArNT_FALSE;
	}
}

//----------------------------------------------------------------------
//
ArNT_BOOL  ArNTArithState::AskSearchDefectImg(LongInt iSteelNo, TinyInt iCameralNo, UShortInt iImgIndex, StrPointer  strSender)
{
	PCameralDefectOper pOper = FindCameralDefctOper(iCameralNo);
	if(pOper)
	{
		return pOper->DefectImgOper.AskSearchDefectImg(pOper->NetCommandOper, 
														pOper->NetChannel.IP, 
														pOper->NetChannel.Port,
														iCameralNo, 
														iSteelNo,
														iImgIndex,
														strSender);
	}else
	{
		return ArNT_FALSE;
	}
}

//----------------------------------------------------------------------
//
ArNT_BOOL	 ArNTArithState::AskCropDefectImg(LongInt iSteelNo, TinyInt iCameralNo, UShortInt iImgIndex,
												UShortInt iLeft,
												UShortInt iRight,
												UShortInt iTop,
												UShortInt iBottom,
												StrPointer strSender)
{
	PCameralDefectOper pOper = FindCameralDefctOper(iCameralNo);
	if(pOper)
	{
		return pOper->DefectImgOper.AskCropDefectImg(pOper->NetCommandOper, 
														pOper->NetChannel.IP, 
														pOper->NetChannel.Port,
														iCameralNo, 
														iSteelNo,
														iImgIndex,
														iLeft,
														iRight,
														iTop,
														iBottom,
														strSender);
	}else
	{
		return ArNT_FALSE;
	}
}
//----------------------------------------------------------------------
//
ArNT_BOOL	 ArNTArithState::AskSearchDefect(LongInt iSteelNo, TinyInt iCameralNo)
{
	PCameralDefectOper pOper = FindCameralDefctOper(iCameralNo);
	if(pOper)
	{
		LONGSTR strCondtion = {0};
		CCommonFunc::SafeStringPrintf(strCondtion, STR_LEN(strCondtion), L"SteelNo = %d", iSteelNo);		
		return pOper->DefectDBOper.AskSearchDefect(pOper->NetCommandOper, 
													pOper->NetChannel.IP, 
													pOper->NetChannel.Port,
													pOper->iCameralID,
													strCondtion);

	}else
	{
		return ArNT_FALSE;
	}
}
//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::AskCurrProcessSteelHistImg(ULongInt iSteelNo,  StrPointer strSendID )
{
	
	HANDLE* hHandleArray = new HANDLE [m_iCameralDefectOperNum];
	
	for(ShortInt i = 0; i < m_iCameralDefectOperNum; i++)
	{ 
		ShortInt iCamralID = m_pCameralDefectOper[i].iCameralID;
		PArNTImageSourceContext pContext = FindImageSourceContext(iCamralID);
		if(pContext)
		{
			m_ImageSourceOper.Init(pContext->Info.strIP, pContext->Info.iPort);
			m_ImageSourceOper.AskHistImgInfo(m_pCameralDefectOper[i].NetCommandOper,
											 m_pCameralDefectOper[i].NetChannel.IP,
											 m_pCameralDefectOper[i].NetChannel.Port,
											iCamralID,
											iSteelNo,
											strSendID);									 

		}
	}

	return ArNT_TRUE;
}
//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::AskCurrDetectSteel(StrPointer strSendID)
{
	return (m_DefectCenterOper.AskCurrSteelInfo(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port, strSendID));

}

//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::AddDefectArith(ArNTDefectArith* pAirth, ShortInt iCameralNo)
{
	PCameralDefectOper pDefectOper = 	FindCameralDefctOper(iCameralNo);
	if(NULL == pDefectOper) return ArNT_FALSE;
	pDefectOper->pArithmetic = pAirth;
	pDefectOper->pArithmetic->RegOnDetectDefectFunc(OnDetectDefect, pDefectOper);
	return ArNT_TRUE;
}
//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::AddSteelHistImgInfo(LongInt iSteelNo, TinyInt iCameralNo, StrPointer strImgPath)
{
	//查询需要添加的图像目录是否已经存在
	LONGSTR strTempImgPath = {0};
	if(FindSteelHistImgInfo(iSteelNo, iCameralNo, strTempImgPath) == ArNT_TRUE) return ArNT_TRUE; 	
	
	m_csSteelHistImgPath.Enter();
	ULongInt iMinID = m_SteelHistImgPathCache.Items[0].iID;
	ShortInt iIndex = 0;
	for(ShortInt i = 1; i < m_SteelHistImgPathCache.MAX_ITEM_NUM; i++)
	{
		if(m_SteelHistImgPathCache.Items[i].iID < iMinID)
		{
			iIndex = i;
			iMinID = m_SteelHistImgPathCache.Items[i].iID;
		}
	}
	m_SteelHistImgPathCache.iMaxID++;
	m_SteelHistImgPathCache.Items[iIndex].iID = m_SteelHistImgPathCache.iMaxID;
	m_SteelHistImgPathCache.Items[iIndex].iCameralNo = iCameralNo;
	m_SteelHistImgPathCache.Items[iIndex].iSteelNo = iSteelNo;
	CCommonFunc::SafeStringCpy(m_SteelHistImgPathCache.Items[iIndex].strImgPath,
								STR_LEN(m_SteelHistImgPathCache.Items[iIndex].strImgPath),
								strImgPath);

	m_csSteelHistImgPath.Leave();

	return ArNT_TRUE;
}

//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::FindSteelHistImgInfo(LongInt iSteelNo, TinyInt iCameralNo, LONGSTR& strImgPath)
{
	ShortInt iIndex = -1;
	m_csSteelHistImgPath.Enter();	
	for(ShortInt i = 1; i < m_SteelHistImgPathCache.MAX_ITEM_NUM; i++)
	{
		if(m_SteelHistImgPathCache.Items[i].iID > 0)
		{
			if(iSteelNo == m_SteelHistImgPathCache.Items[i].iSteelNo &&
				iCameralNo == m_SteelHistImgPathCache.Items[i].iCameralNo)
			{
				iIndex = i;
				CCommonFunc::SafeStringCpy(strImgPath, STR_LEN(strImgPath), 
											m_SteelHistImgPathCache.Items[i].strImgPath);
				break;
			}
		}
	}
	m_csSteelHistImgPath.Leave();

	if(iIndex >= 0) return ArNT_TRUE;
	return ArNT_FALSE;

}


//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::GetCameralImgData(ShortInt iCameralNo, 
											GrayBmpData& SrcBmpData, 
											ArNTSteelImageInfo& ImageInfo ,
											UShortInt& iImgIndex,
											LONGSTR& strImgFileName)
{
	PArNTInnerDetectHistImgInfo pInfo = FindDetecHistImgInfo(iCameralNo);
	if(pInfo)
	{

		CCommonFunc::SafeStringCpy(m_strImgType, STR_LEN(m_strImgType), L"img");		
		CCommonFunc::SafeStringPrintf(strImgFileName, STR_LEN(strImgFileName), L"%s\\%04d.%s", 
									pInfo->Info.strImgPath,
									pInfo->iCurrImgIndex,
									m_strImgType);
	
		if(ArNT_FALSE == ReadSteelImageByBmpData(strImgFileName, ImageInfo,  SrcBmpData, pInfo->Info.iCameral))
		{
			CCommonFunc::SafeStringCpy(m_strImgType, STR_LEN(m_strImgType), L"cimg");		
			CCommonFunc::SafeStringPrintf(strImgFileName, STR_LEN(strImgFileName), L"%s\\%04d.%s", 
									pInfo->Info.strImgPath,
									pInfo->iCurrImgIndex,
									m_strImgType);
			ReadSteelImageByBmpData(strImgFileName, ImageInfo,  SrcBmpData, pInfo->Info.iCameral);
		}
		iImgIndex = pInfo->iCurrImgIndex;
		if(SrcBmpData.iImgSize > 0)
		{
			return ArNT_TRUE;
		}else
		{
			return ArNT_FALSE;
		}
	}else
	{
		return ArNT_FALSE;
	}
}

//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::SendCameralDefect(TinyInt iCameralNo, ShortInt& iSendDefectNum)
{
	iSendDefectNum = 0;
	PCameralDefectOper pCameralDefectOper = FindCameralDefctOper(iCameralNo);
	if(NULL == pCameralDefectOper) return ArNT_FALSE;

	ArNTDefectInfo pDefectInfo[tagCameralDefectOper::MAX_DEFECT_NUM];
	if(pCameralDefectOper->iRcvDefectNum <= 0) return ArNT_FALSE;
	ShortInt iRcvDefectNum = 0;
	pCameralDefectOper->csOperRcvDefect.Enter();
	iRcvDefectNum = pCameralDefectOper->iRcvDefectNum;
	memcpy(pDefectInfo, pCameralDefectOper->RcvDefectInfo, iRcvDefectNum * sizeof(ArNTDefectInfo));
	pCameralDefectOper->iRcvDefectNum = 0;
	pCameralDefectOper->csOperRcvDefect.Leave();
	
	ShortInt iIndex = -1;
	for(ShortInt i = 0; i < m_iDefectDBServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectDBService[i].iCameralNum; j++)
		{
			if(iCameralNo == m_pDefectDBService[i].CameralArray[j])
			{
				iIndex = i;
				break;
			}
		}
	}
	if(iIndex < 0) return ArNT_FALSE;

	if(ArNT_FALSE == pCameralDefectOper->DefectDBOper.AskAddDefect(pCameralDefectOper->NetCommandOper,
												  m_pDefectDBService[iIndex].loc.strIP,
												  m_pDefectDBService[iIndex].loc.iPort + iCameralNo * 10,
												  iRcvDefectNum,
												  pDefectInfo))
	{
		return ArNT_FALSE;
	}

	iSendDefectNum = iRcvDefectNum;

	//分析需要发送的图像信息
	iIndex = -1;
	for(ShortInt i = 0; i < m_iDefectImgServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectImgService[i].iCameralNum; j++)
		{
			if(iCameralNo == m_pDefectImgService[i].CameralArray[j])
			{
				iIndex = i;
				break;
			}
		}
	}
	if(iIndex < 0) return ArNT_FALSE;
	DefectImageInfo  ImageInfo[tagCameralDefectOper::MAX_DEFECT_NUM] = {0};
	ShortInt iImageInfoNum = 0;
	for(ShortInt i = 0; i < iRcvDefectNum; i++)
	{
		ShortInt iFindIndex = -1;
		for(ShortInt j = 0; j < iImageInfoNum; j++)
		{
			if(ImageInfo[j].iSteelNo ==  pDefectInfo[i].iSteelNo &&
			   ImageInfo[j].iCameralNo == pDefectInfo[i].iCameralNo &&
			   ImageInfo[j].iImgIndex == pDefectInfo[i].iImgIndex)
			{
				iFindIndex = j;
				break;
			}
		}
		if(iFindIndex < 0)
		{
			ImageInfo[iImageInfoNum].iSize = sizeof(ImageInfo[iImageInfoNum]);
			ImageInfo[iImageInfoNum].iSteelNo = pDefectInfo[i].iSteelNo;
			ImageInfo[iImageInfoNum].iCameralNo =  pDefectInfo[i].iCameralNo;
			ImageInfo[iImageInfoNum].iImgIndex = pDefectInfo[i].iImgIndex;
			iImageInfoNum++;

			//发送缺陷图像			
			LONGSTR strImgPath = {0};
			LONGSTR strImgFilePathName = {0};
			if(FindSteelHistImgInfo(pDefectInfo[i].iSteelNo, pDefectInfo[i].iCameralNo, strImgPath))
			{
				CCommonFunc::SafeStringPrintf(strImgFilePathName, STR_LEN(strImgFilePathName), L"%s\\%04d.%s",
											  strImgPath,
											   pDefectInfo[i].iImgIndex,
											  m_strImgType);				
				if(ArNT_FALSE == pCameralDefectOper->DefectImgOper.AskSaveImageByFile(pCameralDefectOper->NetCommandOper,
																	 m_pDefectImgService[iIndex].loc.strIP,
																	 m_pDefectImgService[iIndex].loc.iPort + iCameralNo * 10,
																	 pDefectInfo[i].iCameralNo,
																	 pDefectInfo[i].iSteelNo,
																	 pDefectInfo[i].iImgIndex,
																	 strImgFilePathName))
				{
					return ArNT_FALSE;
				}
			}
		}
	}
	return ArNT_TRUE;	
}
//----------------------------------------------------------------------
//
PCameralDefectOper	ArNTArithState::FindCameralDefctOper(ShortInt iCameralID)
{
	PCameralDefectOper pOper = NULL;

	ShortInt iIndex = -1;
	for(ShortInt i = 0; i < m_iCameralDefectOperNum; i++)
	{
		if(iCameralID == m_pCameralDefectOper[i].iCameralID)
		{
			pOper = &m_pCameralDefectOper[i];
			break;
		}
	}
	return pOper;
}
//----------------------------------------------------------------------
//
PArNTDefectImageServiceInfo ArNTArithState::FindDefectImageService(ShortInt iCameralID)
{
	PArNTDefectImageServiceInfo pServiceInfo = NULL;
	for(ShortInt i = 0; i < m_iDefectImgServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectImgService[i].iCameralNum; j++)
		{
			if(m_pDefectImgService[i].CameralArray[j] == iCameralID)
			{
				pServiceInfo = &m_pDefectImgService[i];
				break;
			}
		}

	}

	return pServiceInfo;
}
//----------------------------------------------------------------------
//
PArNTDefectDBServiceInfo	ArNTArithState::FindDefectDBService(ShortInt iCameralID)
{
	PArNTDefectDBServiceInfo pServiceInfo = NULL;
	for(ShortInt i = 0; i < m_iDefectDBServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectDBService[i].iCameralNum; j++)
		{
			if(m_pDefectDBService[i].CameralArray[j] == iCameralID)
			{
				pServiceInfo = &m_pDefectDBService[i];
				break;
			}
		}
	}
	return pServiceInfo;
}
//----------------------------------------------------------------------
//
PArNTImageSourceContext		ArNTArithState::FindImageSourceContext(ShortInt iCameralID)
{
	PArNTImageSourceContext pContext = NULL;
	for(ShortInt i = 0; i < m_iImageSourceNum; i++)
	{
		for(ShortInt j = 0; j < m_pImageSourceContext[i].iCameralNum; j++)
		{
			if(m_pImageSourceContext[i].iCameralID[j] == iCameralID)
			{
				pContext = &m_pImageSourceContext[i];
				break;
			}

		}
	}
	return pContext;
}

//----------------------------------------------------------------------
//
PArNTInnerDetectHistImgInfo ArNTArithState::FindDetecHistImgInfo(ShortInt iCameralID)
{
	PArNTInnerDetectHistImgInfo pInfo = NULL;
	for(ShortInt i = 0; i < m_iTopDetectHistImgNum; i++)
	{
		if(iCameralID == m_pTopDetectHistImg[i].Info.iCameral)
		{
			pInfo = &m_pTopDetectHistImg[i];
			return pInfo;
		}
	}
	for(ShortInt i = 0; i < m_iBottomDetectHistImgNum; i++)
	{
		if(iCameralID == m_pBottomDetectHistImg[i].Info.iCameral)
		{
			pInfo = &m_pBottomDetectHistImg[i];
			return pInfo;
		}
	}
	return pInfo;
}
//----------------------------------------------------------------------
//
PTinyInt	ArNTArithState::GetCameralIDArray(ShortInt& iCameralNum)
{
	iCameralNum = m_iCameralNum;
	return m_pCameralID;
}
//----------------------------------------------------------------------
//
PArNTDefectDBServiceInfo	ArNTArithState::GetDefectDBService(ShortInt& iDBServiceNum)
{
	iDBServiceNum = m_iDefectDBServiceNum;
	return m_pDefectDBService;
}
//----------------------------------------------------------------------
//
PArNTDefectImageServiceInfo ArNTArithState::GetDefectImageSeriviceInfo(ShortInt& iServiceNum)
{
	iServiceNum = m_iDefectImgServiceNum;
	return m_pDefectImgService;
}
//----------------------------------------------------------------------
//
PArNTImageSourceContext		ArNTArithState::GetImageSourceContext(ShortInt& iServiceNum)
{
	iServiceNum = m_iImageSourceNum;
	return m_pImageSourceContext;
}
//----------------------------------------------------------------------
//
PSampleClassItem	ArNTArithState::GetSampleClassItem(ShortInt& iClassNum)
{
	iClassNum = m_iDefectClassNum;
	return m_pDefectClassContext;
}
//----------------------------------------------------------------------
//
ULongInt	ArNTArithState::GetCurrDetectSteelNo(void)
{
	return m_iCurrDetectSteelNo;	
}
//----------------------------------------------------------------------
//
void	ArNTArithState::GetCurrDetectID(SHORTSTR& strID)
{
	CCommonFunc::SafeStringCpy(strID, STR_LEN(strID), m_strCurrDetectID);
}

//-----------------------------------------------------------------------------
//
void	ArNTArithState::RegDBNtfApp(void)
{
	for(ShortInt i = 0; i < m_iDefectDBServiceNum; i++)
	{
		m_DefectDBOper.Init(m_pDefectDBService[i].loc.strIP, m_pDefectDBService[i].loc.iPort);
		LONGSTR strAppName = {0};
		CCommonFunc::GetAppName(strAppName, STR_LEN(strAppName));
		SHORTSTR strAppNameNoExt = {0};
		CCommonFunc::GetFileNoExt(strAppName, strAppNameNoExt, STR_LEN(strAppNameNoExt));
		m_DefectDBOper.AskRegNtfApp(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port,strAppNameNoExt);
	}
}

//----------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::InitDefectOper(void)
{
	ShortInt iCameralNum = 0;
	for(ShortInt i = 0; i <  m_iDefectDBServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectDBService[i].iCameralNum; j++)
		{
			iCameralNum++;	
		}
	}
	if(iCameralNum != m_iCameralDefectOperNum)
	{
		if(m_pCameralDefectOper) delete[] m_pCameralDefectOper;
		m_pCameralDefectOper = new CameralDefectOper[iCameralNum];
		m_iCameralDefectOperNum = iCameralNum;
		for(ShortInt i = 0; i < m_iCameralDefectOperNum; i++)
		{
			m_pCameralDefectOper[i].iSize = sizeof(m_pCameralDefectOper[i]);
			m_pCameralDefectOper[i].pParent = (PObjPointer)this;
			m_pCameralDefectOper[i].pArithmetic = NULL;
			m_pCameralDefectOper[i].iRcvDefectNum = 0;
		}
	}
	iCameralNum = 0;
	for(ShortInt i = 0; i <  m_iDefectDBServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectDBService[i].iCameralNum; j++)
		{
			m_pCameralDefectOper[iCameralNum].iSize = sizeof(m_pCameralDefectOper[iCameralNum]);
			ShortInt iCameralID = m_pDefectDBService[i].CameralArray[j];
			m_pCameralDefectOper[iCameralNum].iCameralID = iCameralID;
			memset(&m_pCameralDefectOper[iCameralNum].NetChannel, 0, sizeof(m_pCameralDefectOper[iCameralNum].NetChannel));
			m_pCameralDefectOper[iCameralNum].NetChannel.iSize = sizeof(m_pCameralDefectOper[iCameralNum].NetChannel);
			CCommonFunc::SafeStringCpy(m_pCameralDefectOper[iCameralNum].NetChannel.IP,
				STR_LEN(m_pCameralDefectOper[iCameralNum].NetChannel.IP), m_NetChannel.IP);
			m_pCameralDefectOper[iCameralNum].NetChannel.Port = PORT_DEFECT_ARITH + iCameralID * 10;

			if(INIT_COMMAND_OPER(m_pCameralDefectOper[iCameralNum].NetCommandOper, m_pCameralDefectOper[iCameralNum].NetChannel) == ArNT_FALSE)
			{
				LONGSTR strInfo = {0};
				CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"无法创建相机缺陷通道(IP:%s Port:%d)", 
											m_pCameralDefectOper[iCameralNum].NetChannel.IP,
											m_pCameralDefectOper[iCameralNum].NetChannel.Port);
				CCommonFunc::ErrorBox(L"%s", strInfo);
				return ArNT_FALSE;
			}
			
			ShortInt iPort =  m_pDefectDBService[i].loc.iPort + iCameralID * 10;
			m_pCameralDefectOper[iCameralNum].DefectDBOper.Init(m_pDefectDBService[i].loc.strIP, iPort);
			iCameralNum++;
		}
	}

	//设置缺陷图像服务通道
	for(ShortInt i = 0; i < m_iDefectImgServiceNum; i++)
	{
		for(ShortInt j = 0; j < m_pDefectImgService[i].iCameralNum; j++)
		{
			ShortInt iCameralNo = m_pDefectImgService[i].CameralArray[j];
			ShortInt iIndex = -1;
			for(ShortInt k = 0; k < m_iCameralDefectOperNum; k++)
			{
				if(m_pCameralDefectOper[k].iCameralID == iCameralNo)
				{
					iIndex = k;
					break;
				}
			}
			if(iIndex >= 0)
			{
				ShortInt iPort = m_pDefectImgService[i].loc.iPort + iCameralNo * 10;
				m_pCameralDefectOper[iIndex].DefectImgOper.Init(m_pDefectImgService[i].loc.strIP, iPort);
			}
		}
	}
	return ArNT_TRUE;
}

//----------------------------------------------------------------------
//
void  ArNTArithState::OnAckCurrSteelInfo(ULongLongInt iSequceNo,StrPointer	strID,StrPointer strSenderID)
{


}
//----------------------------------------------------------------------
//
 void ArNTArithState:: OnNtfSteelStatus(ArNTSteelStatus& status, StrPointer strSenderID)
 {


 }

//----------------------------------------------------------------------
//
 void  ArNTArithState::OnAckImageSource(ShortInt iImageSourceNum, PArNTImageSourceContext pImageSource)
 {


 }
//----------------------------------------------------------------------
//
 void  ArNTArithState::OnRcvDetectDefect(PArNTDefectInfo pDefetcArray, ShortInt iDefectNum)
 {


 }

//----------------------------------------------------------------------
//
 void  ArNTArithState::OnNtfTopDetectHistImg(ShortInt DetectHistImgNum, PArNTInnerDetectHistImgInfo pInfo)
 {


 }
//----------------------------------------------------------------------
//
 void  ArNTArithState::OnNtfBottomDetectHistImg(ShortInt DetectHistImgNum, PArNTInnerDetectHistImgInfo pInfo)
 {

 }

//----------------------------------------------------------------------
//
 void  ArNTArithState::OnNtfCameralID(ShortInt iIDNum, PTinyInt pCameralID)
 {
	m_iCameralNum = iIDNum;
	m_pCameralID = new TinyInt[m_iCameralNum];
	memcpy(m_pCameralID, pCameralID, sizeof(TinyInt) * m_iCameralNum);
 }

//----------------------------------------------------------------------
//
 void  ArNTArithState::OnRcvDefectImg(UShortInt	iCameralNo, 
								 ULongInt	iSteelNo,
								 UShortInt  iImageIndex,
								 ULongInt	iImageSize,
								 UShortInt	iImgWidth,
								 UShortInt	iImgHeight,
								 PImgDataType	pImgeBuffer,
								 StrPointer  strSender)
 {


 }

//----------------------------------------------------------------------
//
void  ArNTArithState::OnRcvCropDefectImg(UShortInt	iCameralNo, 
									  ULongInt	iSteelNo,
									  UShortInt  iImageIndex,
									  ULongInt	iImageSize,
									  UShortInt	iImgWidth,
								      UShortInt	iImgHeight,
								      PImgDataType	pImgeBuffer,
									  StrPointer  strSender)
{



 }

//----------------------------------------------------------------------
//
 void  ArNTArithState::OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum)
 {



 }


//////////////////////////////////////////////////////////////////////////////////
//消息处理函数

//实现网络命令消息的处理
BEGIN_NET_COMMAND(ArNTArithState)
COMMAND_ENTRY(CFG_ACK_DEFECTCENTER_SERVICE, ProcessAckDefectCenter, CfgAckDefectCenterService)
COMMAND_ENTRY(DFG_ACK_DEFECTDBSERVICE, ProcessAckDefectDBService, DfgAckDefectDBService)
COMMAND_ENTRY(DFG_ACK_DEFECTIMGSERVICE, ProcessAckDefectImageService, DfgAckDefectImageService)
COMMAND_ENTRY(CFG_ACK_IMAGESOURCE_SREVICE, ProcessAckImageSource, CfgAckImageSource)
COMMAND_ENTRY(DFG_ACK_CURRSTEELINFO, ProcessAckCurrSteelInfo, DfgAckCurrSteelInfo)
COMMAND_ENTRY(DFG_NTF_STEELSTATUS, ProcessNtfSteelStatus, DfgNtfSteelStatus)
COMMAND_ENTRY(DFG_NTF_TOP_DETECTHISTIMG, ProcessNtfTopDetectHistImg ,DfgNtfTopDetectHistImg)
COMMAND_ENTRY(DFG_NTF_BOTTOM_DETECTHISTIMG, ProcessNtfBottomDetectHistImg ,DfgNtfBottomDetectHistImg)
COMMAND_ENTRY(DFG_ACK_DEFECTCLASSDEFINE, ProcessAckDefectClassDefine,DfgAckDefectClassDefine) 
COMMAND_ENTRY(ISR_ACK_HISTIMGINFO, ProcessAckHistImgInfo, ISRAckHistImgInfo)
COMMAND_ENTRY(DTI_ACK_SEARCHDEFECTIMG, ProcessAckDefectImg, DTIAckSearchDefectImg)
COMMAND_ENTRY(DTI_ACK_CROPDEFECTIMG, ProcessAckCropDefectImg, DTIAckCropDefectImg)
COMMAND_ENTRY(DTB_ACK_SEARCHDEFECT, ProcessAckSearchDefect, DTBAckSearchDefect)
END_NET_COMMAND()

//-------------------------------------------------------------------------------
//
ArNT_BOOL	ArNTArithState::ProcessAckDefectCenter(PCfgAckDefectCenterService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	memcpy(&m_DefectCenterService, &pPara->Info, sizeof(pPara->Info));
	m_DefectCenterOper.Init(pPara->Info.strIP, pPara->Info.iPort);

	//询问缺陷数据库服务
	m_DefectCenterOper.AskDBService(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port);
	
	
	
	::SetEvent(m_hEvtDefectCenter);
	return  ArNT_TRUE;
}

//----------------------------------------------------------------------------------
//处理接收到的缺陷数据库服务
ArNT_BOOL	ArNTArithState::ProcessAckDefectDBService(PDfgAckDefectDBService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	if(pPara->iDBServiceNum <= 0) return ArNT_TRUE;
	
	if(pPara->iDBServiceNum != m_iDefectDBServiceNum)
	{
		if(m_pDefectDBService) delete[] m_pDefectDBService;
		m_pDefectDBService  = new ArNTDefectDBServiceInfo[pPara->iDBServiceNum];	
		m_iDefectDBServiceNum = pPara->iDBServiceNum;
	}
	for(ShortInt i = 0; i < m_iDefectDBServiceNum; i++)
	{
		memcpy(&m_pDefectDBService[i], &pPara->Info, sizeof(m_pDefectDBService[i]));
	}

	::SetEvent(m_hEvtDefectDBService);

	//询问缺陷图像服务
	m_DefectCenterOper.AskDefectImageService(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port);


	return ArNT_TRUE;
}
//----------------------------------------------------------------------------------
//接收到回复的缺陷图像消息
ArNT_BOOL	ArNTArithState::ProcessAckDefectImageService(PDfgAckDefectImageService pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	if(pPara->iServiceNum <= 0) return ArNT_TRUE;
	if(pPara->iServiceNum != m_iDefectImgServiceNum)
	{
		if(m_pDefectImgService) delete[] m_pDefectImgService;
		m_pDefectImgService  = new ArNTDefectImageServiceInfo[pPara->iServiceNum];	
		m_iDefectImgServiceNum = pPara->iServiceNum;
	}
	for(ShortInt i = 0; i < m_iDefectDBServiceNum; i++)
	{
		memcpy(&m_pDefectImgService[i], &pPara->Info, sizeof(m_pDefectImgService[i]));
	}

	::SetEvent(m_hEvtDefectImageService);

	//注册缺陷检测控制台
	m_DefectCenterOper.AskRegNtfService(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port, L"缺陷算法测试程序");

	//询问缺陷类别定义
	m_DefectCenterOper.AskDefectClassDefine(m_NetCommandOper, m_NetChannel.IP, m_NetChannel.Port);



	return ArNT_TRUE;
}

//--------------------------------------------------------------------------
//处理回复的数据源信息
ArNT_BOOL	ArNTArithState::ProcessAckImageSource(PCfgAckImageSource pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	if(pPara->iImageSourceNum <= 0) return ArNT_TRUE;
	if(m_iImageSourceNum != pPara->iImageSourceNum)
	{
		m_iImageSourceNum = pPara->iImageSourceNum;
		if(m_pImageSourceContext) delete m_pImageSourceContext;
		m_pImageSourceContext = new ArNTImageSourceContext[m_iImageSourceNum];
	}
	memcpy(m_pImageSourceContext, pPara->Info, pPara->iImageSourceNum * sizeof(ArNTImageSourceContext));
	::SetEvent(m_hEvtImgSource);
	OnAckImageSource(m_iImageSourceNum, m_pImageSourceContext);

	

	
	return ArNT_TRUE;
}
//--------------------------------------------------------------------------
//接收到回复的需要进行缺陷检测的钢板信息
ArNT_BOOL	ArNTArithState::ProcessAckCurrSteelInfo(PDfgAckCurrSteelInfo pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	
	m_iCurrDetectSteelNo = (ULongInt)pPara->iSequceNo;
	CCommonFunc::SafeStringCpy(m_strCurrDetectID, STR_LEN(m_strCurrDetectID), pPara->strID);
	OnAckCurrSteelInfo(m_iCurrDetectSteelNo, m_strCurrDetectID, strSenderID);	
	return ArNT_TRUE;
}

//--------------------------------------------------------------------------
//接收到通知钢板检测状态信息
ArNT_BOOL	ArNTArithState::ProcessNtfSteelStatus(PDfgNtfSteelStatus pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	OnNtfSteelStatus(pPara->status, strSenderID);	
	return ArNT_TRUE;
}
//----------------------------------------------------------------------------
//接收到通知的缺陷类别信息
ArNT_BOOL	ArNTArithState::ProcessAckDefectClassDefine(PDfgAckDefectClassDefine pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	if(pPara->iClassNum <= 0) return ArNT_TRUE;
	if(m_iDefectClassNum < pPara->iClassNum)
	{
		if(m_pDefectClassContext) delete m_pDefectClassContext;
		m_pDefectClassContext = new SampleClassItem[pPara->iClassNum];
	}
	m_iDefectClassNum =  pPara->iClassNum;
	for(ShortInt i = 0; i < m_iDefectClassNum; i++)
	{
		memcpy(&m_pDefectClassContext[i], &pPara->Items[i], sizeof(pPara->Items[i]));
	}
	return ArNT_TRUE;
}
//----------------------------------------------------------------------------
//接收到回复的历史图像路径信息
ArNT_BOOL	ArNTArithState::ProcessAckHistImgInfo(PISRAckHistImgInfo pPara,  ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	ShortInt iCameralID = pPara->iCameralNo;
	PCameralDefectOper pDefectOper = FindCameralDefctOper(iCameralID);
	if(pDefectOper)
	{
		pDefectOper->iCurrSteelNo = pPara->iSequceNo;
		CCommonFunc::SafeStringCpy(pDefectOper->strCurrSrcImgPath, STR_LEN(pDefectOper->strCurrSrcImgPath), pPara->strImgFullPath);
	}

	return ArNT_TRUE;
}
//------------------------------------------------------------------------------
//接收到回复的缺陷图像信息
ArNT_BOOL   ArNTArithState::ProcessAckDefectImg(PDTIAckSearchDefectImg pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	OnRcvDefectImg(pPara->iCameralNo, pPara->iSteelNo, pPara->iImageIndex, pPara->iImageSize, pPara->iImgWidth,
					pPara->iImgHeight, pPara->ImgeBuffer, strSenderID);

	
	return ArNT_TRUE;
}

//----------------------------------------------------------------------------
//接收到回复的指定区域的缺陷图像
ArNT_BOOL   ArNTArithState::ProcessAckCropDefectImg(PDTIAckCropDefectImg pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	OnRcvCropDefectImg(pPara->iCameralNo, pPara->iSteelNo, pPara->iImageIndex, pPara->iImageSize, pPara->iImgWidth,
					pPara->iImgHeight, pPara->ImgeBuffer, strSenderID);
	return ArNT_TRUE;
}

//----------------------------------------------------------------------------
//接收到通知的上表历史图像信息
ArNT_BOOL	ArNTArithState::ProcessNtfTopDetectHistImg(PDfgNtfTopDetectHistImg pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	m_csRcvDetectHistImg.Enter();
	if(m_iTopDetectHistImgNum !=  pPara->iItemNum)
	{
		if(m_pTopDetectHistImg) delete[] m_pTopDetectHistImg;
		m_iTopDetectHistImgNum = pPara->iItemNum;
		m_pTopDetectHistImg = new ArNTInnerDetectHistImgInfo[m_iTopDetectHistImgNum];	
		for(ShortInt i = 0; i < pPara->iItemNum; i++)
		{
			m_pTopDetectHistImg[i].Info.iSeqNo = 0;
		}
	}
	for(ShortInt i = 0; i < pPara->iItemNum; i++)
	{
		if(pPara->Items[i].iSeqNo > 0 && m_pTopDetectHistImg[i].Info.iSeqNo != pPara->Items[i].iSeqNo)
		{
			m_pTopDetectHistImg[i].iCurrImgIndex = 0;
		}
		memcpy(&m_pTopDetectHistImg[i].Info, &pPara->Items[i], sizeof(m_pTopDetectHistImg[i].Info));

		AddSteelHistImgInfo(m_pTopDetectHistImg[i].Info.iSeqNo, 
							m_pTopDetectHistImg[i].Info.iCameral,
							m_pTopDetectHistImg[i].Info.strImgPath);
	}	
	OnNtfTopDetectHistImg(m_iTopDetectHistImgNum, m_pTopDetectHistImg);

	
	m_csRcvDetectHistImg.Leave();
	return ArNT_TRUE;
}


//----------------------------------------------------------------------------
//接收到通知的下表历史图像信息
ArNT_BOOL	ArNTArithState::ProcessNtfBottomDetectHistImg(PDfgNtfBottomDetectHistImg pPara, ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	m_csRcvDetectHistImg.Enter();
	if(m_iBottomDetectHistImgNum !=  pPara->iItemNum)
	{
		if(m_pBottomDetectHistImg) delete[] m_pBottomDetectHistImg;
		m_iBottomDetectHistImgNum = pPara->iItemNum;
		m_pBottomDetectHistImg = new ArNTInnerDetectHistImgInfo[m_iBottomDetectHistImgNum];	
		for(ShortInt i = 0; i < pPara->iItemNum; i++)
		{
			m_pBottomDetectHistImg[i].Info.iSeqNo = 0;
		}
	}
	for(ShortInt i = 0; i < pPara->iItemNum; i++)
	{
		if(m_pBottomDetectHistImg[i].Info.iSeqNo != pPara->Items[i].iSeqNo && pPara->Items[i].iSeqNo > 0)
		{
			m_pBottomDetectHistImg[i].iCurrImgIndex = 0;
		}
		memcpy(&m_pBottomDetectHistImg[i].Info, &pPara->Items[i], sizeof(m_pBottomDetectHistImg[i].Info));

		AddSteelHistImgInfo(m_pBottomDetectHistImg[i].Info.iSeqNo, 
							m_pBottomDetectHistImg[i].Info.iCameral,
							m_pBottomDetectHistImg[i].Info.strImgPath);
	}	

	OnNtfBottomDetectHistImg(m_iBottomDetectHistImgNum, m_pBottomDetectHistImg);
		
	m_csRcvDetectHistImg.Leave();
	return ArNT_TRUE;
}
//------------------------------------------------------------------------------
//接收到回复的缺陷信息
ArNT_BOOL	ArNTArithState::ProcessAckSearchDefect(PDTBAckSearchDefect pPara,  ArNTNetChannelInfo& FromChannel, StrPointer strLocalChannel, StrPointer strSenderID)
{
	OnRcvSearchDefect(pPara->Info, pPara->iDefectNum);
	return ArNT_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//回调函数
void __stdcall ArNTArithState::OnDetectDefect(PArNTDefectInfo pDefetcArray, ShortInt iDefectNum, PObjPointer pContext)
{
	PCameralDefectOper pDefectOper =  (PCameralDefectOper)pContext;
	ArNTArithState* pState = (ArNTArithState*)pDefectOper->pParent;
	
	for(ShortInt i = 0; i < iDefectNum; i++)
	{
		PCameralDefectOper pCameralDefectOper = pState->FindCameralDefctOper(pDefetcArray[i].iCameralNo);
		if(pCameralDefectOper)
		{
			pCameralDefectOper->csOperRcvDefect.Enter();
			if(pCameralDefectOper->iRcvDefectNum < pCameralDefectOper->MAX_DEFECT_NUM)
			{
				memcpy(&pCameralDefectOper->RcvDefectInfo[pCameralDefectOper->iRcvDefectNum],
						&pDefetcArray[i], 
						sizeof(pDefetcArray[i]));
				pCameralDefectOper->iRcvDefectNum++;
			}
			pCameralDefectOper->csOperRcvDefect.Leave();
		}
	}

	pState->OnRcvDetectDefect(pDefetcArray, iDefectNum);
}