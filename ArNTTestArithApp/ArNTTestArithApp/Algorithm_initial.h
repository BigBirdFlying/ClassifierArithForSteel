// Algorithm_initial.h : 配置文件初始化头文件
//

#pragma once
#include "stdafx.h"
#include "ArNTBaseDataStruct.h"
#include "CommonFunc.h"
#include "arntimage.h"
#include "ArNTDefectDetectInterface.h"
#include "ArNTCommonClass.h"

using namespace std;

//相机有效图像区域设置
typedef struct tagCamEffectArea
{
	UShortInt iHardEdge;
	UShortInt iSoftEdge;
}CamEffectArea, *PCamEffectArea;
typedef struct tagCamEffectAreaSet
{
	enum {MaxCamNum = 10};
	CamEffectArea	iCamEffectArea[MaxCamNum];
	UShortInt iCamNum;
}CamEffectAreaSet, *PCamEffectAreaSet;
//相机无效图像区域设置
typedef struct tagCamNoUseArea
{
	ArNT_BOOL bEnable;
	UShortInt iCol_Left;
	UShortInt iCol_Medium;
	UShortInt iCol_Right;
	UShortInt iTop_Lenght;
	UShortInt iBottom_Lenght;
	LongFloat fNoUse_threshold;
}CamNoUseArea, *PCamNoUseArea;
//判断头尾
typedef struct tagHeadTailJudge
{
	ArNT_BOOL bEnable;
	UShortInt iCross;
	UShortInt iLenght;
	LongFloat fThreshold;
}HeadTailJudge, *PHeadTailJudge;
//图像边界自动切除功能
typedef struct tagEdgeAutoCut
{
	ArNT_BOOL bEnable;
	UShortInt iCut_Lenght;
}EdgeAutoCut, *PEdgeAutoCut;
//钢板图像结疤检测
typedef struct tagDefects_Scarring
{
	ArNT_BOOL bEnable;
	UShortInt iNoUse_Width;
	UShortInt iEffect_Width;
	UShortInt iConf_Threshold;
}Defects_Scarring, *PDefects_Scarring;

class DefectsMergeXML
{
public:
		DefectsMergeXML();
	    ~DefectsMergeXML();

		LONGSTR m_strAppPath;
		LONGSTR m_strConfigFile;
		ArNTXMLOperator m_XMLOperator;

		int iItemNumFloor;
		int iItemNumCeiling;
		int XNeighbors;
		int YNeighbors;
		float iMaxDefectSize;
		float iMaxXYRatio;
		int MCF_width;
		int MCF_hight;
		UShortInt Useless_threshold;
		UShortInt HeadandTail_Removed;

		CamEffectAreaSet		m_CamEffectAreaSet;
		CamNoUseArea           m_CamNoUseArea;
		HeadTailJudge				m_HeadTailJudge;
		EdgeAutoCut              m_EdgeAutoCut;
		Defects_Scarring         m_Defects_Scarring;
};