#include "StdAfx.h"
#include "ArNTSimulDefectArithmetic.h"
#include "CommonFunc.h"
#include <vector>
using namespace std;
#include "version-2.0-dll.h"
////////////////////////////////////////////////////////////
//
ArNTSimulDefectArithmetic::ArNTSimulDefectArithmetic(void)
{
	m_pOnDetectDefect = NULL;
	m_iToalDefectNum = 0;
}

ArNTSimulDefectArithmetic::~ArNTSimulDefectArithmetic(void)
{
}
///////////////////////////////////////////////////////////
//接口函数
//-----------------------------------------------------------------
//
void	ArNTSimulDefectArithmetic::RegOnDetectDefectFunc(POnDetectDefectFuncDef pFunc, PObjPointer pContext)
{
	m_pOnDetectDefect = pFunc;
	m_pContext = pContext;
}
//-----------------------------------------------------------------
//
void	ArNTSimulDefectArithmetic::Detect(GrayBmpData& BmpData, DetectImgInfo& Info)
{
	ShortInt iDefectNum = 2;
	PArNTDefectInfo pDefetcArray = new ArNTDefectInfo[iDefectNum];	
	for(ShortInt i = 0; i < iDefectNum; i++)
	{
		TinyInt iClass = (m_iToalDefectNum) % 7;
		TinyInt iOffset = (m_iToalDefectNum) % 40;
		
		pDefetcArray[i].iSize = sizeof(ArNTDefectInfo);
		pDefetcArray[i].iClass = iClass;
		pDefetcArray[i].iGrade = 80;
		pDefetcArray[i].iCycle = 0;
		pDefetcArray[i].iDefectImgIndex = Info.iImgIndex;
		pDefetcArray[i].iCameralNo = Info.iCameralNo;
		pDefetcArray[i].iSteelNo = Info.iSteelNo;
		pDefetcArray[i].iImgIndex = Info.iImgIndex;
		pDefetcArray[i].iLeftInImage = iClass * 100 + iOffset * 10;
		pDefetcArray[i].iRightInImage = pDefetcArray[i].iLeftInImage + 80 * Info.iCameralNo;
		pDefetcArray[i].iTopInImage = iClass * 10 + iOffset * 5;
		pDefetcArray[i].iBottomInImage = pDefetcArray[i].iTopInImage + 40 * Info.iCameralNo;

		LongFloat fXRes = 1.0 * (Info.iRightPos - Info.iLeftPos) / Info.iImgWidth;
		LongFloat fYRes = 1.0 * Info.iSpeed / Info.iLineRate;
		
		pDefetcArray[i].iLeftInSteel = Info.iLeftPos + pDefetcArray[i].iLeftInImage * fXRes;
		pDefetcArray[i].iRightInSteel = Info.iLeftPos +  pDefetcArray[i].iRightInImage * fXRes;
		pDefetcArray[i].iTopInSteel = Info.iDetectLen + pDefetcArray[i].iTopInImage * fYRes;
		pDefetcArray[i].iBottomInSteel = Info.iDetectLen + pDefetcArray[i].iBottomInImage * fYRes;
		pDefetcArray[i].iArea = 30;

		m_iToalDefectNum++;
	}
	Sleep(200);
	if(m_pOnDetectDefect) m_pOnDetectDefect(pDefetcArray, iDefectNum, m_pContext);
	delete[]pDefetcArray;	

	
}
//-----------------------------------------------------------------
//
void    ArNTSimulDefectArithmetic::GetArithDesc(LONGSTR& strDesc)
{
	CCommonFunc::SafeStringPrintf(strDesc, STR_LEN(strDesc), L"%s", L"模拟检测算法");
}
//-----------------------------------------------------------------
//
LongFloat    ArNTSimulDefectArithmetic::GetArithTime(void)
{
	return m_fArithTime;
}