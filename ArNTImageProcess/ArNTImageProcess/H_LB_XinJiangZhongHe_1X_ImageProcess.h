#pragma once
#include "h_commonimageprocess.h"

#define M_DefectListRows 64
#define M_DefectListCols 128

class H_LB_XinJiangZhongHe_1X_ImageProcess :
	public H_CommonImageProcess
{
public:
	H_LB_XinJiangZhongHe_1X_ImageProcess(void);
	~H_LB_XinJiangZhongHe_1X_ImageProcess(void);

public:
	int m_imgSrcWidth;
	int m_imgSrcHeight;
	//ÖÜÆÚÈ±ÏÝ
	cv::Mat m_DefectList;
	int m_DefectListIndex;
	bool m_IsPeriodDefect;
	int m_PeriodDefectSit[M_DefectListCols];
	int m_SideCountSit[M_DefectListCols];

public:	
	int K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug=false);

	bool IsValid(const cv::Mat& src);
	void Defects_ColdSteel_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet);
	void Defects_Period_OpenCV(PH_DefectSet pSet,float factor,int interval,int PeriodClass,int ImgIndex);
};
