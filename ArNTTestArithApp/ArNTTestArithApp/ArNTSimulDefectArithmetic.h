#pragma once
#include "ArNTDefectArithInterface.h"

class ArNTSimulDefectArithmetic:public ArNTDefectArith
{

public:
	void		RegOnDetectDefectFunc(POnDetectDefectFuncDef pFunc, PObjPointer pContext);
	void		Detect(GrayBmpData& BmpData, DetectImgInfo& Info); 
	void		ChangePara(StrPointer strName, StrPointer strValue) {};
	void		GetPara(StrPointer strName, SHORTSTR& trValue) {};
	ArNT_BOOL	Init(void) {return ArNT_TRUE;};
	void        GetArithDesc(LONGSTR& strDesc);
	LongFloat   GetArithTime(void);
	LongInt		GetParaNum() {return 0;};
	ArNT_BOOL   GetParaInfo(LongInt iIndex, TINYSTR& strName){return ArNT_FALSE;};
	

public:
	ArNTSimulDefectArithmetic(void);
	~ArNTSimulDefectArithmetic(void);

private:
	POnDetectDefectFuncDef	m_pOnDetectDefect;
	PObjPointer				m_pContext;

	LongInt					m_iToalDefectNum;

	LongFloat				m_fArithTime;

	CGrayBmp				m_GrayBmp;

};
