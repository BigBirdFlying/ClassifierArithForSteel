#include "StdAfx.h"
#include "ArNTTestArithState.h"
#include "ArNTTestArithAppDlg.h"

IMPL_SINGLE_STATE(ArNTTestArithState)

#define ARITHSTATE_NAME L"EvtArithStateOnlyOneApp"

//////////////////////////////////////////////////////////
//构造函数

ArNTTestArithState::ArNTTestArithState(void)
{
	for(ShortInt i = 0; i < 10; i++)
	{
		H_Detect_Process_m[i]=0;
		H_Detect_Process_n[i]=0;
		H_Detect_IsNoEqual[i]=0;
	}

	CCommonFunc::CreateNullLimetEvent(TRUE, FALSE, ARITHSTATE_NAME);
	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		CCommonFunc::ErrorBox(L"%s", L"已经有一个算法服务处于运行中,程序运行失败!");
		exit(0);
	}

	m_ExitApp = ArNT_FALSE;
	

	m_iCurrSelCamerID = 0;
	m_iRcvDefectNum = 0;

	m_pSimulImgItem = NULL;
	m_iSimulImgNum = 0;
	m_fArithTime = 0;

	for(ShortInt i = 0; i < MAX_CAMERAL_NUM; i++)
	{
		m_bEnableDetect[i] = ArNT_FALSE;
		m_bCameralIsDetect[i] = ArNT_FALSE;
		m_bEnableCameralAutoDetect[i] = ArNT_FALSE;
	}

	m_iStartDetectLen = 0;

	m_iCurrTopSteelNo = 0;
	m_iCurrBottomSteelNo = 0;
	m_bAutoStartDetect = ArNT_FALSE;
	m_iCamImgMaxNum=1000;
	m_iMaxDefectNum=30;
	m_bAutoCloseApp=ArNT_FALSE;
	m_iAutoCloseInterval=10;
	m_iChannel=1;
	m_iOddOrEven=0;

	CCommonFunc::GetAppPath(m_strAppPath, STR_LEN(m_strAppPath));
	CCommonFunc::SafeStringPrintf(m_strConfigFile, STR_LEN(m_strConfigFile), L"%s\\ArNTTestArithApp.xml", m_strAppPath);
	if(m_XMLOperator.LoadXML(m_strConfigFile))
	{
		LONGSTR strValue = {0};
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#自动检测设置#支持自动检测", strValue))
		{
			if(_wtoi(strValue) > 0) 
			{
				m_bAutoStartDetect = ArNT_TRUE;
			}
			else
			{
				m_bAutoStartDetect = ArNT_FALSE;
			}
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#自动检测设置#自动检测相机", strValue))
		{
			LongInt iValue = _wtoi(strValue);
			LongInt iCameralNum = 0;
			while(iValue > 0)
			{
				iCameralNum++;
				ShortInt iCameral = iValue % 10;
				m_bEnableCameralAutoDetect[iCameral] = ArNT_TRUE;
				iValue /= 10;
			}			
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#自动检测设置#是否启用自检功能", strValue))
		{
			if(_wtoi(strValue) > 0) 
			{
				m_bAutoCloseApp = ArNT_TRUE;
			}
			else
			{
				m_bAutoCloseApp = ArNT_FALSE;
			}
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#自动检测设置#自检功能间隔次数", strValue))
		{
			m_iAutoCloseInterval = _wtoi(strValue);
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#自动检测设置#选取通道", strValue))
		{
			m_iChannel = _wtoi(strValue);
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#自动检测设置#处理图像奇偶数", strValue))
		{
			m_iOddOrEven = _wtoi(strValue);
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#相机图像数量设置#单相机图像处理最大数量限制", strValue))
		{
			m_iCamImgMaxNum = _wtoi(strValue);
		}
		if(m_XMLOperator.GetValueByString(L"算法测试程序配置#显示缺陷数量设置#最大数量", strValue))
		{
			m_iMaxDefectNum = _wtoi(strValue);
		}
	}
	LONGSTR strAppName = {0};
	CCommonFunc::GetAppName(strAppName, STR_LEN(strAppName));
	CCommonFunc::SafeStringPrintf(m_strLogPath, STR_LEN(m_strLogPath), L"%s\\Log\\%s", m_strAppPath, strAppName);
	AddLog(L"程序开始运行");
}

ArNTTestArithState::~ArNTTestArithState(void)
{
}

///////////////////////////////////////////////////////////
//接口函数

//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::GetDefectClassDesc(ShortInt iClassNo, SHORTSTR& strName)
{
	ArNT_BOOL bFind = ArNT_FALSE;

	ShortInt iClassNum = 0;
	PSampleClassItem pClassItem = GetSampleClassItem(iClassNum);
	for(ShortInt i = 0; i < iClassNum; i++)
	{
		if(iClassNo == pClassItem[i].iClassNo)
		{
			bFind = ArNT_TRUE;
			CCommonFunc::SafeStringCpy(strName, STR_LEN(strName),  pClassItem[i].strName);
			break;
		}
	}
	return bFind;

}

//---------------------------------------------------------
//
ArNT_BOOL  ArNTTestArithState::Init(LONGSTR& strError)
{
	if(CCommonFunc::StrInRunPara(L"-local") ==  ArNT_TRUE)
	{
		m_bLocalMode = ArNT_TRUE;
		m_SimulCameralDefectOper.iCameralID = 1;
		m_SimulCameralDefectOper.iSize = sizeof(m_SimulCameralDefectOper);
		m_SimulCameralDefectOper.pParent = this;
		m_SimulCameralDefectOper.pArithmetic = &m_SimulImgDefectArithmetic;
		m_SimulCameralDefectOper.iRcvDefectNum = 0;
		CCommonFunc::SafeStringCpy(m_SimulCameralDefectOper.strCurrSrcImgPath,
			STR_LEN(m_SimulCameralDefectOper.strCurrSrcImgPath), L"");
		m_SimulImgDefectArithmetic.RegOnDetectDefectFunc(OnDetectDefect, &m_SimulCameralDefectOper);
		
		return ArNT_TRUE;
	}else
	{
		m_bLocalMode = ArNT_FALSE;
	}
	
	if(ArNTArithState::Init(strError) == ArNT_FALSE) return ArNT_FALSE;

	//创建线程池
	INIT_THREAD_POOL(16);
	Sleep(200);
	//UShortInt Voild;
	//THREAD_POOL_POST(threadDetect_Process, &Voild, sizeof(Voild));

	return ArNT_TRUE;
}
//---------------------------------------------------------
//
void  ArNTTestArithState::OnNtfSteelStatus(ArNTSteelStatus& status, StrPointer strSenderID)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	
	if(pMainDlg) pMainDlg->OnNtfSteelStatus(status);
	if(m_iCurrTopSteelNo != status.iTopSequeceNo)
	{
		m_iCurrTopSteelNo = status.iTopSequeceNo;
		if(pMainDlg) pMainDlg->OnNewTopSteelIn(m_iCurrTopSteelNo);
	}
	if(m_iCurrBottomSteelNo != status.iBottomSequeceNo)
	{
		m_iCurrBottomSteelNo = status.iBottomSequeceNo;
	}
}
//---------------------------------------------------------
//
void  ArNTTestArithState::OnAckImageSource(ShortInt iImageSourceNum, PArNTImageSourceContext pImageSource)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(pMainDlg) pMainDlg->ShowCameralList();
}

//---------------------------------------------------------
//
void	ArNTTestArithState::OnNtfTopDetectHistImg(ShortInt DetectHistImgNum, PArNTInnerDetectHistImgInfo pInfo)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(pMainDlg)
	{
		while(pMainDlg->m_lstTopDetectHistImgInfo.GetCount()) pMainDlg->m_lstTopDetectHistImgInfo.DeleteString(0);
		LONGSTR strInfo = {0};
		for(ShortInt i = 0; i < DetectHistImgNum; i++)
		{
			CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d号相机 %d流水号图像共%d幅(已处理%d幅)位于:%s", 
											pInfo[i].Info.iCameral, 
											pInfo[i].Info.iSeqNo, 
											pInfo[i].Info.iMaxImgIndex, 
											pInfo[i].iCurrImgIndex,
											pInfo[i].Info.strImgPath);
			pMainDlg->m_lstTopDetectHistImgInfo.AddString(strInfo);

			if(true==m_bAutoCloseApp)
			{
				//2016-05-24添加 2016-07-22补充添加
				if(1 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					//AddLog(L"1加了");
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表1进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							//CArNTTestArithAppDlg ms;
							//ms.OnExitApp();
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机1未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机1图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(2 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					//AddLog(L"2加了");
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表2进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机2未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机2图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(3 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					//AddLog(L"3加了");
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表3进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机3未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机3图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(4 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表4进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机4未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机4图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(5 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表5进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机5未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机5图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(6 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表6进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机6未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机6图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(7 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表6进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机7未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机7图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(8 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iTopSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"上表6进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机8未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机8图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
			}
			//LONGSTR strss = {0};
			//CCommonFunc::SafeStringPrintf(strss, STR_LEN(strss), L"共监督历史图像数量%d处理%d张",DetectHistImgNum,Detect_Process_Num);
			//AddLog(strss);
			//
		}
	}	
}

//---------------------------------------------------------
//
void	ArNTTestArithState::OnNtfBottomDetectHistImg(ShortInt DetectHistImgNum, PArNTInnerDetectHistImgInfo pInfo)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(pMainDlg)
	{
		while(pMainDlg->m_lstBottomDetectHistImg.GetCount() ) pMainDlg->m_lstBottomDetectHistImg.DeleteString(0);
		LONGSTR strInfo = {0};
		for(ShortInt i = 0; i < DetectHistImgNum; i++)
		{
			CCommonFunc::SafeStringPrintf(strInfo, STR_LEN(strInfo), L"%d号相机 %d流水号图像共%d幅(已处理%d幅) 位于:%s", 
											pInfo[i].Info.iCameral, 
											pInfo[i].Info.iSeqNo, 
											pInfo[i].Info.iMaxImgIndex, 
											pInfo[i].iCurrImgIndex,
											pInfo[i].Info.strImgPath);
			pMainDlg->m_lstBottomDetectHistImg.AddString(strInfo);

			if(true==m_bAutoCloseApp)
			{
				//2016-05-24添加
				if(1 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					//AddLog(L"1加了");
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表1进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							//CArNTTestArithAppDlg ms;
							//ms.OnExitApp();
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机1未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机1图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(2 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					//AddLog(L"2加了");
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表2进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机2未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机2图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(3 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					//AddLog(L"3加了");
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表3进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机3未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机3图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(4 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表4进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机4未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机4图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(5 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表5进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机5未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机5图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(6 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表6进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机6未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机6图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(7 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表6进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机7未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机7图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
				else if(8 == pInfo[i].Info.iCameral && ArNT_TRUE == m_bEnableCameralAutoDetect[pInfo[i].Info.iCameral])
				{
					H_Detect_Process_n[pInfo[i].Info.iCameral]++;
					if(pMainDlg->m_iBottomSequeceNo==pInfo[i].Info.iSeqNo)
					{
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]++;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 1)//余1的时候进行赋值
					{
						H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					}
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]%m_iAutoCloseInterval == 0)//余0的时候进行比较
					{
						//AddLog(L"下表6进来了");
						if(H_Detect_Process_m[pInfo[i].Info.iCameral] == pInfo[i].iCurrImgIndex && pInfo[i].Info.iMaxImgIndex != pInfo[i].iCurrImgIndex)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机8未检测准备关闭...当前图像索引：%d 最大图像索引：%d",pInfo[i].iCurrImgIndex, pInfo[i].Info.iMaxImgIndex);
							AddLog(strLogInfo);
							//pMainDlg->OnExitApp();
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						
						if(H_Detect_IsNoEqual[pInfo[i].Info.iCameral]<5)
						{
							LONGSTR strLogInfo = {0};
							CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"相机8图象流水号未同步准备关闭... 10次中有%d次流水号相同",H_Detect_IsNoEqual[pInfo[i].Info.iCameral]);
							AddLog(strLogInfo);
							HANDLE hself=GetCurrentProcess();
							TerminateProcess(hself,0);
						}
						H_Detect_IsNoEqual[pInfo[i].Info.iCameral]=0;
					}
					//H_Detect_Process_m[pInfo[i].Info.iCameral]=pInfo[i].iCurrImgIndex;
					if(H_Detect_Process_n[pInfo[i].Info.iCameral]>10000)
					{
						H_Detect_Process_n[pInfo[i].Info.iCameral]=0;
					}
				}
			}
			//LONGSTR strss = {0};
			//CCommonFunc::SafeStringPrintf(strss, STR_LEN(strss), L"共监督历史图像数量%d处理%d张",DetectHistImgNum,Detect_Process_Num);
			//AddLog(strss);
			//
		}
	}
}

//---------------------------------------------------------
//
void	ArNTTestArithState::OnRcvDetectDefect(PArNTDefectInfo pDefetcArray, ShortInt iDefectNum)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(m_bLocalMode == ArNT_FALSE)
	{
		m_iRcvDefectNum += iDefectNum;
	}else
	{
		m_iRcvDefectNum += iDefectNum;
		m_SimulCameralDefectOper.iRcvDefectNum += iDefectNum;

	}
	
	if(pMainDlg)
	{
		pMainDlg->OnRcvDetectDefect(pDefetcArray, iDefectNum);	
		pMainDlg->SetDlgItemInt(IDC_EDTDEFECTNUM, m_iRcvDefectNum);
		
	}


}
//---------------------------------------------------------
//
void	ArNTTestArithState::OnRcvDefectImg(UShortInt	iCameralNo, 
											ULongInt	iSteelNo,
											UShortInt	iImageIndex,
											ULongInt	iImageSize,
											UShortInt	iImgWidth,
											UShortInt	iImgHeight,
											PImgDataType	pImgeBuffer,
											StrPointer  strSender)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(pMainDlg)
	{
		pMainDlg->OnRcvDefectImg(iCameralNo, 
									iSteelNo,
									iImageIndex, 
									iImageSize,
									iImgWidth, 
									iImgHeight, 
									pImgeBuffer);
	}
}
//---------------------------------------------------------
//
void	ArNTTestArithState::OnRcvCropDefectImg(UShortInt	iCameralNo, 
									  ULongInt	iSteelNo,
									  UShortInt  iImageIndex,
									  ULongInt	iImageSize,
									  UShortInt	iImgWidth,
								      UShortInt	iImgHeight,
								      PImgDataType	pImgeBuffer,
									  StrPointer  strSender)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(pMainDlg)
	{
		pMainDlg->OnRcvCropDefectImg(iCameralNo, 
									iSteelNo,
									iImageIndex, 
									iImageSize,
									iImgWidth, 
									iImgHeight, 
									pImgeBuffer,
									strSender);
	}
}

//---------------------------------------------------------
//
void	ArNTTestArithState::OnRcvSearchDefect(PArNTDefectInfo pInfo, ShortInt iDefectNum)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	if(pMainDlg)
	{
		pMainDlg->OnRcvSearchDefect(pInfo, iDefectNum); 
	}
}

//---------------------------------------------------------
//
void	ArNTTestArithState::OnNtfCameralID(ShortInt iIDNum, PTinyInt pCameralID)
{
	if(ArNT_TRUE == m_ExitApp ) return;
	ArNTArithState::OnNtfCameralID(iIDNum, pCameralID);
	
	for(ShortInt i = 0; i< iIDNum; i++)
	{
		if(ArNT_FALSE == AddDefectArith(&m_DefectArithmetic, pCameralID[i]))
		//if(ArNT_FALSE == AddDefectArith(&m_SimulDefectArithmetic, pCameralID[i]))
		{
			CCommonFunc::ErrorBox(L"无法添加算法到%d号相机", pCameralID[i]);
		}
	}
	if(pMainDlg)
	{
		pMainDlg->OnNtfCameralID(iIDNum, pCameralID);
	}
}
//---------------------------------------------------------
//
void     ArNTTestArithState::DetectLastImg(void)
{
	if(m_bLocalMode == ArNT_FALSE)
	{
		PArNTInnerDetectHistImgInfo pInfo = FindDetecHistImgInfo(m_iCurrSelCamerID);
		if(NULL == pInfo)
		{
			return ;
		}
		if(pInfo->Info.iMaxImgIndex < 1) return;
		pInfo->iCurrImgIndex  = pInfo->Info.iMaxImgIndex -1;
		DetectCurrImg();
	}else
	{
		DetectCurrImg();
	}
}

//---------------------------------------------------------
//
void     ArNTTestArithState::JumpToMaxImage(ShortInt iCameralNo)
{
	PArNTInnerDetectHistImgInfo pInfo = FindDetecHistImgInfo(iCameralNo);
	if(NULL == pInfo)
	{
		return ;
	}
	if(pInfo->Info.iMaxImgIndex > 0)
	{
		pInfo->iCurrImgIndex = pInfo->Info.iMaxImgIndex -1;
	}else
	{
		pInfo->iCurrImgIndex = 0;
	}
}
//---------------------------------------------------------
//
void ArNTTestArithState::JumpToMinImage(ShortInt iCameralNo)
{
	PArNTInnerDetectHistImgInfo pInfo = FindDetecHistImgInfo(iCameralNo);
	if(NULL == pInfo)
	{
		return ;
	}
	pInfo->iCurrImgIndex = 0;
}

//---------------------------------------------------------
//
void  ArNTTestArithState::StartDetectCameral(ShortInt iCameralNo)
{
	ShortInt iDetectCamral = iCameralNo;
	m_bEnableDetect[iDetectCamral] =  ArNT_TRUE;
	//THREAD_POOL_POST(threadDetectCameralDefect, &iDetectCamral, sizeof(iDetectCamral));
	_beginthread(threadDetectCameralDefect2, 1024 * 1024 * 10, (void*)iDetectCamral);

}
//---------------------------------------------------------
//
void  ArNTTestArithState::StopDetectCameral(ShortInt iCameralNo)
{
	m_bEnableDetect[iCameralNo] =  ArNT_FALSE;

}

//---------------------------------------------------------
//
void  ArNTTestArithState::DetectCurrImg(void)
{
	if(this->m_bLocalMode ==  ArNT_FALSE)
	{
		LONGSTR strImgFileName = {0};
		if(GetCurrImgData(strImgFileName))
		{
			PCameralDefectOper pCameralDefectOper = FindCameralDefctOper(m_iCurrSelCamerID);
			if(pCameralDefectOper && pCameralDefectOper->pArithmetic)
			{
				pCameralDefectOper->pArithmetic->Detect(m_SrcBmpData, pCameralDefectOper->ImgInfo);
				ShortInt iSendDefectNum = 0;
				SendCameralDefect(m_iCurrSelCamerID, iSendDefectNum);
			}
		}
	}else
	{
		if(GetSimulImgData() == ArNT_TRUE)
		{
			m_SimulImgDefectArithmetic.Detect(m_SrcBmpData, m_DetectSimulImgInfo);	
			m_fArithTime = m_SimulImgDefectArithmetic.GetArithTime();
		}
	}
}
//---------------------------------------------------------
//
ArNT_BOOL	ArNTTestArithState::DetectCurrCameralImg(LONGSTR& strImgFileName, ShortInt iCameralNo)
{
	
	if(1==m_iChannel)
	{
		if(GetCurrCameralImgData(strImgFileName, iCameralNo) == ArNT_FALSE) return ArNT_FALSE;
		
		PArNTInnerDetectHistImgInfo pHistImgInfo = FindDetecHistImgInfo(iCameralNo);
		if(NULL == pHistImgInfo) return ArNT_FALSE;		
		PCameralDefectOper pCameralDefectOper = FindCameralDefctOper(iCameralNo);
		if(pCameralDefectOper && pCameralDefectOper->pArithmetic)
		{
			if(pCameralDefectOper->ImgInfo.iDetectLen >= m_iStartDetectLen)
			{
				pCameralDefectOper->pArithmetic->Detect(pHistImgInfo->SrcBmpData, pCameralDefectOper->ImgInfo);
				ShortInt iSendDefectNum = 0;
				SendCameralDefect(iCameralNo, iSendDefectNum);
			}
		}

		return ArNT_TRUE;
	}
	else if(2==m_iChannel)
	{
		///*****************************************2016-04-05直连网线测试-可行-多通道*********************///
		if(GetCurrCameralImgData(strImgFileName, iCameralNo) == ArNT_FALSE) return ArNT_FALSE;

		string s;
		wchar_t * wText = strImgFileName;
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,NULL,0,NULL,FALSE);// WideCharToMultiByte的运用
		char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
		psText = new char[dwNum];
		WideCharToMultiByte (CP_OEMCP,NULL,wText,-1,psText,dwNum,NULL,FALSE);// WideCharToMultiByte的再次运用
		s = psText;// std::string赋值
		delete []psText;// psText的清除
		int index=-1;
		//index=s.find(".100\\");
		//if(index>0)
		//{
		//	s.replace(10,1,"1");
		//}
		//index=s.find(".1\\");
		//if(index>0)
		//{
		//	s.replace(10,1,"2");
		//}
		//index=s.find(".2\\");
		//if(index>0)
		//{
		//	s.replace(10,1,"3");
		//}
		index=s.find(".img");
		if(index>0)
		{
			s = s.replace(index, 4,"_1.img");
		}

		std::string temp = s;
		int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, NULL,0); 
		wchar_t * wszUtf8 = new wchar_t[len+1]; 
		memset(wszUtf8, 0, len * 2 + 2); 
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, (LPWSTR)wszUtf8, len);
		wstring szDst = wszUtf8;
		std::wstring r = wszUtf8;
		delete[] wszUtf8;
		
		LONGSTR strFileNameNew={0};
		CCommonFunc::SafeStringPrintf(strFileNameNew, STR_LEN(strFileNameNew), L"%s", szDst.c_str());
		GrayBmpData			m_SrcBmpData;
		CGrayBmp			m_GrayBmp;
		ArNTSteelImageInfo ImageInfo = {sizeof(ImageInfo)};
		memset(&ImageInfo,0,sizeof(ImageInfo));
		ReadSteelImageByBmpData(strFileNameNew,ImageInfo,m_SrcBmpData,0);

		if(0 == m_SrcBmpData.iImgWidth || 0 == m_SrcBmpData.iImgHeight) return ArNT_FALSE;		
		PCameralDefectOper pCameralDefectOper = FindCameralDefctOper(iCameralNo);
		if(pCameralDefectOper && pCameralDefectOper->pArithmetic)
		{
			if(pCameralDefectOper->ImgInfo.iDetectLen >= m_iStartDetectLen)
			{
				pCameralDefectOper->pArithmetic->Detect(m_SrcBmpData, pCameralDefectOper->ImgInfo);
				ShortInt iSendDefectNum = 0;
				SendCameralDefect(iCameralNo, iSendDefectNum);
			}
		}

		return ArNT_TRUE;
	}
}

//---------------------------------------------------------
//
ArNT_BOOL	ArNTTestArithState::GetCurrImgData(LONGSTR& strImgFileName)
{
	
	ArNTSteelImageInfo Info = {sizeof(tagArNTSteelImageInfo)};	
	UShortInt iImgIndex = 0;
	if(ArNT_FALSE ==  GetCameralImgData(m_iCurrSelCamerID, m_SrcBmpData, Info, iImgIndex, strImgFileName)) return ArNT_FALSE;
	if(Info.iWidth <= 0) return ArNT_FALSE;
	if(Info.iHeight <= 0) return ArNT_FALSE;
	
	PCameralDefectOper		pCamralOper = FindCameralDefctOper(Info.iCameralNo);
	
	
	pCamralOper->ImgInfo.iSize = sizeof(m_DetectImgInfo);
	pCamralOper->ImgInfo.iCameralNo = Info.iCameralNo;
	pCamralOper->ImgInfo.iImgHeight = Info.iHeight;
	pCamralOper->ImgInfo.iImgWidth = Info.iWidth;
	pCamralOper->ImgInfo.iDetectLen = Info.iSteelLen;
	pCamralOper->ImgInfo.iLineRate = Info.iLineRate;
	pCamralOper->ImgInfo.iLeftPos = Info.iLeftPos;
	pCamralOper->ImgInfo.iRightPos = Info.iRightPos;
	pCamralOper->ImgInfo.iSpeed = Info.iSpeed;
	pCamralOper->ImgInfo.iSteelNo = Info.iSteelNo;
	pCamralOper->ImgInfo.iImgIndex = iImgIndex;

	return ArNT_TRUE;	
}
//---------------------------------------------------------
//
ArNT_BOOL	ArNTTestArithState::GetCurrCameralImgData(LONGSTR& strImgFileName, ShortInt iCamralNo)
{
	
	ArNTSteelImageInfo Info = {sizeof(tagArNTSteelImageInfo)};	
	UShortInt iImgIndex = 0;
	PArNTInnerDetectHistImgInfo pHistImgInfo = FindDetecHistImgInfo(iCamralNo);
	if(NULL == pHistImgInfo) return ArNT_FALSE;
	if(ArNT_FALSE ==  GetCameralImgData(iCamralNo, pHistImgInfo->SrcBmpData, Info, iImgIndex, strImgFileName)) return ArNT_FALSE;
	PCameralDefectOper		pCamralOper = FindCameralDefctOper(Info.iCameralNo);
	if(NULL == pCamralOper) return ArNT_FALSE;
	
	
	pCamralOper->ImgInfo.iSize = sizeof(m_DetectImgInfo);
	pCamralOper->ImgInfo.iCameralNo = Info.iCameralNo;
	pCamralOper->ImgInfo.iImgHeight = Info.iHeight;
	pCamralOper->ImgInfo.iImgWidth = Info.iWidth;
	pCamralOper->ImgInfo.iDetectLen = Info.iSteelLen;
	pCamralOper->ImgInfo.iLineRate = Info.iLineRate;
	pCamralOper->ImgInfo.iLeftPos = Info.iLeftPos;
	pCamralOper->ImgInfo.iRightPos = Info.iRightPos;
	pCamralOper->ImgInfo.iSpeed = Info.iSpeed;
	pCamralOper->ImgInfo.iSteelNo = Info.iSteelNo;
	pCamralOper->ImgInfo.iImgIndex = iImgIndex;

	return ArNT_TRUE;
}

//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::GetSimulImgData(void)
{
	ArNTSteelImageInfo ImageInfo = {sizeof(ImageInfo)};
	CCommonFunc::SafeStringCpy(m_strCurrSimulImgPathName, STR_LEN(m_strCurrSimulImgPathName),
		m_pSimulImgItem[m_iCurrSimulImgIndex].strImgPathName);

	if(ArNT_FALSE == ReadSteelImageByBmpData(m_strCurrSimulImgPathName, ImageInfo,  m_SrcBmpData, 1))
	{
		return ArNT_FALSE;
	}
	m_DetectSimulImgInfo.iSize = sizeof(m_DetectImgInfo);
	m_DetectSimulImgInfo.iImgHeight = m_SrcBmpData.iImgHeight;
	m_DetectSimulImgInfo.iImgWidth = m_SrcBmpData.iImgWidth;
	m_DetectSimulImgInfo.iImgIndex = m_iCurrSimulImgIndex;

	if( ImageInfo.iCameralNo > 0)
	{
		m_DetectSimulImgInfo.iCameralNo = ImageInfo.iCameralNo;	
		m_DetectSimulImgInfo.iDetectLen = ImageInfo.iSteelLen;
		m_DetectSimulImgInfo.iLineRate = ImageInfo.iLineRate;
		m_DetectSimulImgInfo.iLeftPos = ImageInfo.iLeftPos;
		m_DetectSimulImgInfo.iRightPos = ImageInfo.iRightPos;
		m_DetectSimulImgInfo.iSpeed = ImageInfo.iSpeed;
		m_DetectSimulImgInfo.iSteelNo = ImageInfo.iSteelNo;
	}else
	{
		m_DetectSimulImgInfo.iCameralNo = 1;	
		m_DetectSimulImgInfo.iDetectLen = 100000;
		m_DetectSimulImgInfo.iLineRate = 2048;
		m_DetectSimulImgInfo.iLeftPos = -200;
		m_DetectSimulImgInfo.iRightPos = 200;
		m_DetectSimulImgInfo.iSpeed = 500;
		m_DetectSimulImgInfo.iSteelNo = 1;	
	}
	return ArNT_TRUE;	
}

//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::SearchSimulImg(StrPointer strSimulDir)
{
	LongInt iTotalImgNum = 0;
	 
	LongInt iCImgNum = 0;
	CCommonFunc::SafeStringCpy(m_strSimulDir, STR_LEN(m_strSimulDir), strSimulDir);
	CCommonFunc::SearchFile(m_strSimulDir, L"CIMG", NULL, iCImgNum);
	
	LongInt iImgNum = 0;
	CCommonFunc::SearchFile(m_strSimulDir, L"IMG", NULL, iImgNum);

	LongInt iBmpNum = 0;
	CCommonFunc::SearchFile(m_strSimulDir, L"BMP", NULL, iBmpNum);

	iTotalImgNum = iCImgNum + iImgNum + iBmpNum;
	if(iTotalImgNum <= 0) return ArNT_FALSE;
	if(iTotalImgNum > m_iSimulImgNum)
	{
		if(m_pSimulImgItem) 
		{
			delete[] m_pSimulImgItem;
		}
		m_pSimulImgItem = new SimulImgItem[iTotalImgNum];
	}
	FileItemName* pFileItems = new FileItemName[iTotalImgNum];
	if(iCImgNum > 0) CCommonFunc::SearchFile(m_strSimulDir, L"CIMG", pFileItems, iCImgNum);
	if(iImgNum > 0) CCommonFunc::SearchFile(m_strSimulDir,  L"IMG",  pFileItems + iCImgNum, iImgNum);
	if(iBmpNum > 0) CCommonFunc::SearchFile(m_strSimulDir,  L"BMP",  pFileItems + iCImgNum + iImgNum, iBmpNum);
	for(LongInt i = 0; i < iTotalImgNum; i++)
	{
		CCommonFunc::SafeStringPrintf(m_pSimulImgItem[i].strImgPathName, STR_LEN(m_pSimulImgItem[i].strImgPathName),
									  L"%s\\%s", m_strSimulDir, pFileItems[i]);


	}
	m_iSimulImgNum = iTotalImgNum;
	m_iCurrSimulImgIndex = 0;
	delete[] pFileItems;
	return ArNT_TRUE;
}
//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::MoveToNextSimulImg(void)
{
	if(m_iSimulImgNum <= 0) return ArNT_FALSE;	
	if(m_iCurrSimulImgIndex + 1 >= m_iSimulImgNum) return ArNT_FALSE; 
	m_iCurrSimulImgIndex++;
	
	return ArNT_TRUE;
}
//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::MoveToPrieoSimulImg(void)
{
	if(m_iSimulImgNum <= 0) return ArNT_FALSE;
	if(m_iCurrSimulImgIndex <= 0) return ArNT_FALSE;
	m_iCurrSimulImgIndex--;
	return ArNT_TRUE;
}
//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::MoveToFirstSimulImg(void)
{
	if(m_iSimulImgNum <= 0) return ArNT_FALSE;
	m_iCurrSimulImgIndex = 0;
	return ArNT_TRUE;

}
//---------------------------------------------------------
//
ArNT_BOOL		ArNTTestArithState::MoveToLastSimulImg(void)
{
	if(m_iSimulImgNum <= 0) return ArNT_FALSE;
	m_iCurrSimulImgIndex = m_iSimulImgNum;
	return ArNT_TRUE;
}

//---------------------------------------------------------
//
LongFloat	ArNTTestArithState::GetArithTime(void)
{
	return m_fArithTime;
}
//---------------------------------------------------------
//
void	ArNTTestArithState::GetImageNameByIndex(LONGSTR& strFileName, LongInt iIndex)
{
	if(iIndex < 0) return;
	if(iIndex > m_iSimulImgNum - 1) return;
	CCommonFunc::SafeStringCpy(strFileName, STR_LEN(m_strCurrSimulImgPathName),
							m_pSimulImgItem[iIndex].strImgPathName);
}
//---------------------------------------------------------
//
void	ArNTTestArithState::JumpToIndex(LongInt iIndex)
{
	if(iIndex < 0) return;
	if(iIndex > m_iSimulImgNum - 1) return;
	m_iCurrSimulImgIndex = iIndex;
}

//---------------------------------------------------------
//
void	ArNTTestArithState::PostDelayDetect(ShortInt iDelayTime)
{
	THREAD_POOL_POST(threadDelyDetect, &iDelayTime, sizeof(iDelayTime));
}
//---------------------------------------------------------
//
ArNT_BOOL   ArNTTestArithState::IsCameralEnableAutoDetect(ShortInt iCameralNo)
{
	return m_bEnableCameralAutoDetect[iCameralNo];
}

//-------------------------------------------------------------------
//
void	ArNTTestArithState::AddLog(StrPointer strLogInfo)
{
	m_csOperLog.Enter();
	SYSTEMTIME NowTime;
	GetLocalTime(&NowTime);
	CCommonFunc::CreateDir(m_strLogPath);
	LONGSTR strLogFile = {0};
	CCommonFunc::SafeStringPrintf(strLogFile, STR_LEN(strLogFile), L"%s\\%d%02d%02d.txt", m_strLogPath, NowTime.wYear, NowTime.wMonth, NowTime.wDay);
	SHORTSTR strTime = {0};
	CCommonFunc::SafeStringPrintf(strTime, STR_LEN(strTime), L"%02d时%02d分%02d秒", NowTime.wHour, NowTime.wMinute, NowTime.wSecond);
	LONGSTR strTempLogInfo = {0};
	CCommonFunc::SafeStringPrintf(strTempLogInfo, STR_LEN(strTempLogInfo), L"[%s]:%s", strTime, strLogInfo);
	CCommonFunc::WriteStringToFile( strLogFile, strTempLogInfo);

	m_csOperLog.Leave();
}

//////////////////////////////////////////////////////////////////////
//线程函数
void  ArNTTestArithState::InitThreadPool(ShortInt iID)
{
	


}

//定义线程池函数
IMPL_THREAD_POOL(ArNTTestArithState)
//IMPL_THREAD_POOL_ENTRY(ArNTTestArithState, threadDetectCameralDefect, ShortInt)
IMPL_THREAD_POOL_ENTRY(ArNTTestArithState, threadDelyDetect, ShortInt)
//IMPL_THREAD_POOL_ENTRY(ArNTTestArithState, H_threadDetect_Process, ShortInt)
//----------------------------------------------------------------------------------------
//
void  ArNTTestArithState::threadDetectCameralDefect(PShortInt iCameralNo, LongInt iDataLen)
{
	ShortInt iTempCamraNo = *iCameralNo;
	if(iTempCamraNo < 0) return;
	if(iTempCamraNo > this->MAX_CAMERAL_NUM) return;

	if(ArNT_TRUE == m_bCameralIsDetect[iTempCamraNo]) return;
	m_bCameralIsDetect[iTempCamraNo] = ArNT_TRUE;	
	if(pMainDlg)
	{
		m_iStartDetectLen = pMainDlg->GetCurrStartDetectLen();
	}
	
	LONGSTR strImgFileName = {0};
	__try
	{
		while(m_bEnableDetect[iTempCamraNo] == ArNT_TRUE)
		{
		
			if(ArNT_TRUE == m_ExitApp) break;

			PArNTInnerDetectHistImgInfo pHistImgInfo = FindDetecHistImgInfo(iTempCamraNo);
			if(NULL == pHistImgInfo)
			{
				Sleep(100);//2016-9-21改，原return
				continue;
			}
			//2015-05-15添加用于限定单个相机处理最大图像数量
			if(pHistImgInfo->Info.iMaxImgIndex>m_iCamImgMaxNum)
			{
				pHistImgInfo->iCurrImgIndex=pHistImgInfo->Info.iMaxImgIndex;
			}

			//吴昆鹏-2015-11-09奇偶数时
			if(pHistImgInfo->iCurrImgIndex < pHistImgInfo->Info.iMaxImgIndex)
			{
				if(m_iOddOrEven==0)
				{
					bool bRet=DetectCurrCameralImg(strImgFileName, iTempCamraNo);
					pHistImgInfo->iCurrImgIndex++;
				}
				else if(m_iOddOrEven==1)
				{
					if(pHistImgInfo->iCurrImgIndex%2==1)
					{
						bool bRet=DetectCurrCameralImg(strImgFileName, iTempCamraNo);
						pHistImgInfo->iCurrImgIndex+=2;
						if(pHistImgInfo->iCurrImgIndex>pHistImgInfo->Info.iMaxImgIndex)
						{
							pHistImgInfo->iCurrImgIndex=pHistImgInfo->Info.iMaxImgIndex;
						}
					}
					else
					{
						pHistImgInfo->iCurrImgIndex++;
					}
				}
				else if(m_iOddOrEven==2)
				{
					if(pHistImgInfo->iCurrImgIndex%2==0)
					{
						bool bRet=DetectCurrCameralImg(strImgFileName, iTempCamraNo);
						pHistImgInfo->iCurrImgIndex+=2;
						if(pHistImgInfo->iCurrImgIndex>pHistImgInfo->Info.iMaxImgIndex)
						{
							pHistImgInfo->iCurrImgIndex=pHistImgInfo->Info.iMaxImgIndex;
						}
					}
					else
					{
						pHistImgInfo->iCurrImgIndex++;
					}
				}
			}
			else
			{
				Sleep(100);
			}
		}
		m_bCameralIsDetect[iTempCamraNo] = ArNT_FALSE;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		StartDetectCameral(iTempCamraNo);
	}
}

//------------------------------------------------------------------------------------
//
void  ArNTTestArithState::threadDelyDetect(PShortInt pDelayTime, LongInt iDataLen)
{
	ShortInt iMaxDelayTime = *pDelayTime;
	ShortInt iCurrDelayTime = 0;
	while(iCurrDelayTime < iMaxDelayTime)
	{
		Sleep(1000);
		iCurrDelayTime++;
		if(pMainDlg) pMainDlg->DelayDetect(iMaxDelayTime, iCurrDelayTime); 
	}
	if(pMainDlg) pMainDlg->DelayDetect(iMaxDelayTime, iCurrDelayTime); 
}

//------------------------------------------------------------------------------------
//
//void  ArNTTestArithState::H_threadDetect_Process(PShortInt pDelayTime, LongInt iDataLen)
//{
	//while(true)
	//{
	//	int Detect_Process_Sum=0;
	//	for(int i=0;i<10;i++)
	//	{
	//		Sleep(2000);
	//		Detect_Process_Sum+=Detect_Process_Num;
	//	}
	//	LONGSTR strss = {0};
	//	CCommonFunc::SafeStringPrintf(strss, STR_LEN(strss), L"10次处理%d张",Detect_Process_Sum);
	//	AddLog(strss);
	//	if(Detect_Process_Sum<1)
	//	{
	//		break;
	//	}
	//}
	//AddLog(L"挂");
	//CArNTTestArithAppDlg ms;
	//ms.OnExitApp();
	//AddLog(L"挂");
//}
//------------------------------------------------------------------------------------
//
void  ArNTTestArithState::threadDetectCameralDefect2(void* pPara)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	ShortInt  pCameralNo = (ShortInt)pPara;
	pState->threadDetectCameralDefect(&pCameralNo, sizeof(ShortInt));
}


	


