#pragma once
#include "h_commonimageprocess.h"

class H_KHB_SanMingGangTie_1X_ImageProcess :
	public H_CommonImageProcess
{
public:
	H_KHB_SanMingGangTie_1X_ImageProcess(void);
	~H_KHB_SanMingGangTie_1X_ImageProcess(void);

	bool m_bIsDebug;
	int m_imgSrcWidth;
	int m_imgSrcHeight;
	cv::Mat m_MatMark;

	static void Defects_Left_Med_Right(cv::Mat src,float& Left,float& Med,float& Right);
	static void Defects_Left_Med_Right_Num(cv::Mat src,float& Left,float& Med,float& Right);
	void Find_Edge_OpenCV_ForCommon(cv::Mat src,int srcImgWidth,int srcImgHeight,int &Left_Edge,int &Right_Edge,int EdgeThreshold,int offSet); 
	cv::Mat Find_Head_OpenCV_ForCommon(cv::Mat src,int srcImgWidth,int srcImgHeight,int &Top_Head,int &Bottom_Head,int HeadThreshold,int offSet);
	void Defects_HeatSteel_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet);
	void Defects_HeatSteel1_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet);

	int K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug=false);
};
