#pragma once
#include "H_CommonImageProcess.h"

class H_RZ_DingXin_1780X_ImageProcess :
	public H_CommonImageProcess
{
public:
	H_RZ_DingXin_1780X_ImageProcess(void);
	~H_RZ_DingXin_1780X_ImageProcess(void);
	
public:	
	int K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug=false);
};
