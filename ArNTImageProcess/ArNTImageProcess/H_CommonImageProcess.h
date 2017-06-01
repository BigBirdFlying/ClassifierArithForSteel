#pragma once
#include "H_Common.h"
#include "H_DataTypeDef.h"
#include "H_DefectDetectInterface.h"

class H_CommonImageProcess
{
public:
	H_CommonImageProcess(void);
	~H_CommonImageProcess(void);

public:
	int K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug=false);
};
