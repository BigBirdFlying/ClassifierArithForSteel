/////////////////////////////////////////////////
#pragma once
#include "StdAfx.h"
#include "ArNTDefectArithmetic.h"
#include "CommonFunc.h"
#include "ArWatch.h"
#include <vector>
#include <cstdlib>
#include <math.h>
using namespace std;
#include "version-2.0-dll.h"
#include "H_DataTypeDef.h"
////////////////////////////////////////////////////////////

//extern int HeadTailJudgeNum;
ArNTDefectArithmetic::ArNTDefectArithmetic(void)
{
	m_pOnDetectDefect = NULL;
	m_iToalDefectNum = 0;
	//原版本声明无法详细初始化(初始化中设定相机编号)
	for (int i=0;i<MAX_CAMERAL_NUM;i++)
	{
		m_Algrithm[i]._Initial(i);
	}

	///
	iLastSquenceNo=-1;
	//m_pDefetcArray=new ArNTDefectInfo[1024];
	//m_DefetcArrayNum=0;
	//m_StartIndex=-1;
	//m_EndIndex=-1;
}

ArNTDefectArithmetic::~ArNTDefectArithmetic(void)
{
	for (int i=0;i<MAX_CAMERAL_NUM;i++)
	{
		m_Algrithm[i]._Initial(i);
	}
}
///////////////////////////////////////////////////////////
//接口函数
//-----------------------------------------------------------------
//
void	ArNTDefectArithmetic::RegOnDetectDefectFunc(POnDetectDefectFuncDef pFunc, PObjPointer pContext)
{
	m_pOnDetectDefect = pFunc;
	m_pContext = pContext;
}
//-----------------------------------------------------------------
//
void	ArNTDefectArithmetic::Detect(GrayBmpData& BmpData, DetectImgInfo& Info)
{

	ArNTDefectArith_H & Arith =  m_Algrithm[Info.iCameralNo];
	ShortInt iDefectNum = 0;

	////安全的方法见MantoDefectSet定义
	PH_DefectSet pDefectInfoSet=new H_DefectSet;
	m_fArithTime = 0;
	ArWatch Watch(&m_fArithTime);

	//
	if(iLastSquenceNo!=Info.iSteelNo)
	{
		LONGSTR strLogInfo={0};
		if(iLastSquenceNo>0)
		{
			CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"流水号[%d]的钢卷检测完成<<", iLastSquenceNo);
			AddLog(strLogInfo);
		}
		
		iLastSquenceNo=Info.iSteelNo;
			
		CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"流水号[%d]的钢卷检测开始>>", Info.iSteelNo);
		AddLog(strLogInfo);
	}

	///算法接口
	iDefectNum=Arith._DetectImgDefect(BmpData.pImgData, BmpData.iImgWidth,BmpData.iImgHeight,pDefectInfoSet,Info.iSteelNo,Info.iImgIndex,Info.iCameralNo);//主分类程序
	///
	if(iDefectNum <= 0) 
	{
		delete[] pDefectInfoSet;
		return;
	}
    
	PArNTDefectInfo pDefetcArray = new ArNTDefectInfo[iDefectNum];	

	int iToatalDefectNum = 0;

	/////添加
	//iDefectNum = pDefectInfoSet->iItemNum;
	for(ShortInt i = 0; i < iDefectNum; i++)
	{
		pDefetcArray[iToatalDefectNum].iSize = sizeof(ArNTDefectInfo);
		pDefetcArray[iToatalDefectNum].iClass = pDefectInfoSet->Items[i].iDefectClass;
		pDefetcArray[iToatalDefectNum].iGrade = pDefectInfoSet->Items[i].fConfidence*100;
		pDefetcArray[iToatalDefectNum].iCycle = 0;
		pDefetcArray[iToatalDefectNum].iDefectImgIndex = Info.iImgIndex;
		pDefetcArray[iToatalDefectNum].iCameralNo = Info.iCameralNo;
		pDefetcArray[iToatalDefectNum].iSteelNo = Info.iSteelNo;
		pDefetcArray[iToatalDefectNum].iImgIndex = Info.iImgIndex;
		pDefetcArray[iToatalDefectNum].iLeftInImage = pDefectInfoSet->Items[i].iLeftInImage;
		pDefetcArray[iToatalDefectNum].iRightInImage = pDefectInfoSet->Items[i].iRightInImage ;
		pDefetcArray[iToatalDefectNum].iTopInImage = pDefectInfoSet->Items[i].iTopInImage;
		pDefetcArray[iToatalDefectNum].iBottomInImage = pDefectInfoSet->Items[i].iBottomInImage;

		LongFloat fXRes = 1.0 * (Info.iRightPos - Info.iLeftPos) / Info.iImgWidth;
		LongFloat fYRes = 1.0 * Info.iSpeed / Info.iLineRate;		
		pDefetcArray[iToatalDefectNum].iLeftInSteel = Info.iLeftPos + pDefetcArray[iToatalDefectNum].iLeftInImage * fXRes;
		pDefetcArray[iToatalDefectNum].iRightInSteel = Info.iLeftPos +  pDefetcArray[iToatalDefectNum].iRightInImage * fXRes;
		pDefetcArray[iToatalDefectNum].iTopInSteel = Info.iDetectLen + pDefetcArray[iToatalDefectNum].iTopInImage * fYRes;
		pDefetcArray[iToatalDefectNum].iBottomInSteel = Info.iDetectLen + pDefetcArray[iToatalDefectNum].iBottomInImage * fYRes;
		pDefetcArray[iToatalDefectNum].iArea = abs(pDefectInfoSet->Items[i].iRightInImage-pDefectInfoSet->Items[i].iLeftInImage)*abs(pDefectInfoSet->Items[i].iBottomInImage-pDefectInfoSet->Items[i].iTopInImage);
		
		iToatalDefectNum++;
		m_iToalDefectNum++;
	}
	///集合缺陷
	//if(Info.iImgIndex<2)
	//{
	//	m_DefetcArrayNum=0;
	//	m_StartIndex=-1;
	//	m_EndIndex=-1;
	//}
	//for(ShortInt i = 0; i < iDefectNum; i++)
	//{
	//	m_pDefetcArray[m_DefetcArrayNum].iArea=pDefetcArray[i].iArea;
	//	m_pDefetcArray[m_DefetcArrayNum].iBottomInImage=pDefetcArray[i].iBottomInImage;
	//	m_pDefetcArray[m_DefetcArrayNum].iBottomInSteel=pDefetcArray[i].iBottomInSteel;
	//	m_pDefetcArray[m_DefetcArrayNum].iCameralNo=pDefetcArray[i].iCameralNo;
	//	m_pDefetcArray[m_DefetcArrayNum].iClass=pDefetcArray[i].iClass;
	//	m_pDefetcArray[m_DefetcArrayNum].iCycle=pDefetcArray[i].iCycle;
	//	m_pDefetcArray[m_DefetcArrayNum].iDefectImgIndex=pDefetcArray[i].iDefectImgIndex;
	//	m_pDefetcArray[m_DefetcArrayNum].iGrade=pDefetcArray[i].iGrade;
	//	m_pDefetcArray[m_DefetcArrayNum].iID=pDefetcArray[i].iID;
	//	m_pDefetcArray[m_DefetcArrayNum].iImgIndex=pDefetcArray[i].iImgIndex;
	//	m_pDefetcArray[m_DefetcArrayNum].iLeftInImage=pDefetcArray[i].iLeftInImage;
	//	m_pDefetcArray[m_DefetcArrayNum].iLeftInSteel=pDefetcArray[i].iLeftInSteel;
	//	m_pDefetcArray[m_DefetcArrayNum].iRightInImage=pDefetcArray[i].iRightInImage;
	//	m_pDefetcArray[m_DefetcArrayNum].iRightInSteel=pDefetcArray[i].iRightInSteel;
	//	m_pDefetcArray[m_DefetcArrayNum].iSize=pDefetcArray[i].iSize;
	//	m_pDefetcArray[m_DefetcArrayNum].iSteelNo=pDefetcArray[i].iSteelNo;
	//	m_pDefetcArray[m_DefetcArrayNum].iTopInImage=pDefetcArray[i].iTopInImage;
	//	m_pDefetcArray[m_DefetcArrayNum].iTopInSteel=pDefetcArray[i].iTopInSteel;
	//	m_DefetcArrayNum++;
	//	if(m_DefetcArrayNum>=1024)
	//	{
	//		break;
	//	}
	//}
	///
	if(m_pOnDetectDefect) 
	{
		m_pOnDetectDefect(pDefetcArray, iToatalDefectNum, m_pContext);
	}
	//Sleep(2000);
	delete[] pDefectInfoSet;
	delete[] pDefetcArray;
}
//-----------------------------------------------------------------
//
void    ArNTDefectArithmetic::GetArithDesc(LONGSTR& strDesc)
{
	CCommonFunc::SafeStringPrintf(strDesc, STR_LEN(strDesc), L"%s", L"划伤检测算法");
}
//
//-----------------------------------------------------------------
//
LongFloat    ArNTDefectArithmetic::GetArithTime(void)
{
	return m_fArithTime;
}

//加日志文件
void	ArNTDefectArithmetic::AddLog(StrPointer strLogInfo)
{
	LONGSTR strAppName = {0};
	LONGSTR m_strLogPath = {0};
	LONGSTR m_strAppPath = {0};
	CCommonFunc::GetAppPath(m_strAppPath, STR_LEN(m_strAppPath));
	CCommonFunc::GetAppName(strAppName, STR_LEN(strAppName));
	CCommonFunc::SafeStringPrintf(m_strLogPath, STR_LEN(m_strLogPath), L"%s\\Log\\%s", m_strAppPath, strAppName);
	
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
}