/////////////////////////////////////////////////
#pragma once
#include "ArNTDefectArithInterface.h"
#include "ArNTDefectArith_H.h"
//#include "Algorithm_initial.h"

class ArNTDefectArithmetic:public ArNTDefectArith
{

public:
	void		RegOnDetectDefectFunc(POnDetectDefectFuncDef pFunc, PObjPointer pContext);
	void		Detect(GrayBmpData& BmpData, DetectImgInfo& Info); 
	void		ChangePara(StrPointer strName, StrPointer strValue) {};
	void		GetPara(StrPointer strName, SHORTSTR& trValue) {};
	void        GetArithDesc(LONGSTR& strDesc);
	ArNT_BOOL	Init(void) {return ArNT_TRUE;};
	LongFloat   GetArithTime(void);
	LongInt		GetParaNum() {return 0;};
	ArNT_BOOL   GetParaInfo(LongInt iIndex, TINYSTR& strName){return ArNT_FALSE;};
	
	
public:
	ArNTDefectArithmetic(void);
	~ArNTDefectArithmetic(void);

private:
	POnDetectDefectFuncDef	m_pOnDetectDefect;
	PObjPointer				m_pContext;
	LongInt					m_iToalDefectNum;
	LongFloat				m_fArithTime;
	CGrayBmp				m_GrayBmp;

	enum	{MAX_CAMERAL_NUM = 10};
	ArNTDefectArith_H  m_Algrithm[MAX_CAMERAL_NUM];	
	void	AddLog(StrPointer strLogInfo);;
	///
	int iLastSquenceNo;

};
