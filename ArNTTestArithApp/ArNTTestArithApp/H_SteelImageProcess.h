#pragma once
#include "H_Common.h"
#include "H_DefectArithInterface.h"
//»±œ›–≈œ¢

class H_SteelImageProcess
{
public:
	H_SteelImageProcess(void);
	~H_SteelImageProcess(void);

//private:
public:
	static int	m_imgSrcWidth;
	static int	m_imgSrcHeight;
	//static HTuple m_hv_MLPHandle;

public:
	//static int SteelImageProcessFunction(const cv::Mat& cv_SrcImage_P2D,const HObject& ho_SrcImage_P2D,cv::Mat& cv_DstImage_P2D_GCG,PDefectSet pSet,int srcWidth,int srcHeight,int SteelNo,int ImgIndex,int CameralNo);
	static int SteelImageProcessFunction_OpenCV(const cv::Mat& cv_SrcImage_P2D,cv::Mat& cv_DstImage_P2D_GCG,PDefectSet pSet,int srcWidth,int srcHeight,int SteelNo,int ImgIndex,int CameralNo);
	static void Find_Edge(const cv::Mat& src,int &Left_Edge,int &Right_Edge,int offSet); 
	static void Find_Head(const cv::Mat& src,bool &isHead,int &Top_Edge,int &Bottom_Edge);
	//static void Cut_Edge_Halcon(HObject ho_srcimg,int Left_Edge,int Right_Edge,HObject *ho_dstimg);
	static void Cut_Edge_OpenCV(const cv::Mat& src,int Left_Edge,int Right_Edge,cv::Mat& dst);

	//static void Defects_Scratch_Halcon(HObject ho_Image,HObject ho_Image_src,int Base_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo);
	static void Defects_Scarring_OpenCV(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo);
	static void Defects_Scarring_OpenCV_New(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo);
	static void Defects_Scarring_OpenCV_New_P2D(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool& isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo); 
	static void Defects_BadSide_OpenCV(cv::Mat srcimg,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo);
	//static void Defects_BadSide_Halcon(HObject ho_Image,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo); 
	//static void Defects_ZhaPo_Halcon(HObject ho_Image,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo);  
	//static void Defects_Peeling_Halcon(HObject ho_Cutimg_GCG_P2D,HObject ho_Image,int Base_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo); 
	
	//static void Defects_To_Normal(HObject& ho_Images,HObject ho_Image,int Image_Width,int Image_Height, PDefectSet pSet,int Scale,int& Number);
	//static void Defects_To_Class(HObject ho_Images,PDefectSet pSet); 
	static void Defects_Common_OpenCV_P2D(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool& isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo); 
};
