#pragma once
#include "H_Common.h"

class H_BaseImageProcess
{
public:
	H_BaseImageProcess(void);
	~H_BaseImageProcess(void);

public:
	static void Gray_Change_Y(const cv::Mat& src,cv::Mat& dst,int offSet,int baseGray);
	static void Gray_Change_X(const cv::Mat& src,cv::Mat& dst,int offSet,int baseGray,bool bSelect); 

	//static void OpencvToHalcon(cv::Mat& src,HalconCpp::HObject& ho_dst);
	//static void Type_To_HObject(unsigned char* srcImg,HObject* ho_Image,int srcImgWidth,int srcImgHeight);
	//static void HObject_To_Type(HObject ho_Image,unsigned char* srcImg); 

	//static void Show_Image_Halcon(HObject ho_Image,float zoom_factor,int Left_Edge,int Right_Edge);
	//static void Save_Image_Halcon(HObject ho_Image,int iStr); 
	static void Save_Image_OpenCV(cv::Mat srcimg,int iStr=0);
	 
	//static void GenFeatures (HObject ho_Image, HTuple *hv_FeatureVector);
	//static void GenSobelFeatures (HObject ho_Image, HTuple hv_Features, HTuple *hv_FeaturesExtended);
	
	static void SSR(cv::Mat srcimg,int sigma,int scale,cv::Mat &dstimg);
	static int Otsu(cv::Mat src);
	static void Img_LBP(const cv::Mat& src,cv::Mat& dst);
};
