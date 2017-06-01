/////////////////////////////////////////////////
#pragma once
//#include "ArNTDefectDetectInterface.h"
#include "stdafx.h"
#include "ArNTGrayBmp.h"
#include "H_Common.h"
#include "H_DataTypeDef.h"

#define M_DefectListRows 128
#define M_DefectListCols 256
//类
class ArNTDefectArith_H	
{
public:
	ArNTDefectArith_H();
	~ArNTDefectArith_H(void);

public:
	ConfigInfo_Segmentation				m_ConfigInfo_Segmentation;//检出配置
	ClassifierObject					m_ClassifierObject;//分类器对象
	ConfigInfo_ClassifyRelation			m_ConfigInfo_ClassifyRelation;//分类器相关设计配置
	ConfigInfo_ClassConvertSet			m_ConfigInfo_ClassConvertSet;//内外类别转换

	int									m_imgSrcWidth;
	int									m_imgSrcHeight;
	static int							m_gArithSelection;
	int									m_iLastSegmentationThreshold;

	bool								m_bIsDebug;
	//周期缺陷
	cv::Mat m_DefectList;
	int m_DefectListIndex;
	bool m_IsPeriodDefect;
	int m_PeriodDefectSit[M_DefectListCols];
	int m_SideCountSit[M_DefectListCols];
	
	//int 				m_InterToExter[64];
public:
	//主检测函数
	int _DetectImgDefect( unsigned char* srcImg,int srcImgWidth,int srcImgHeight,PH_DefectSet pSet,int SteelNo=0,int ImgIndex=0,int CameralNo=0,unsigned char* dstImg=NULL);
	//初始化函数
	bool _Initial(int camaraIndex=0);
	//加日志函数
	void AddLog(StrPointer strLogInfo);
	//读检出配置文件
	bool Read_ConfigInfo_Segmentation();
	//读类别转换配置文件
	bool Read_ConfigInfo_ClassConvertSet();
	//读分类器相关配置文件
	bool Read_ConfigInfo_ClassifyRelation();
	//图像保存
	void Save_Image_OpenCV(cv::Mat src ,int iClass,int iSteel,int iCamera,int iImgIndex,int num); 
	//读Ini配置文件
	int Read_IniFile(CString strFileName,CString strSectionName,CString strKeyName,CString& strKeyValue);
	//缺陷图像分类
	void DefectImgClassify(cv::Mat src,int &preClass);
	//周期缺陷标识
	void Defects_Period_OpenCV(PH_DefectSet pSet,float factor,int interval,int PeriodClass,int ImgIndex);
};
