#include "H_RZ_DingXin_1780X_ImageProcess.h"
#include "H_BaseImageProcess.h"

H_RZ_DingXin_1780X_ImageProcess::H_RZ_DingXin_1780X_ImageProcess(void)
{
}

H_RZ_DingXin_1780X_ImageProcess::~H_RZ_DingXin_1780X_ImageProcess(void)
{
}

int H_RZ_DingXin_1780X_ImageProcess::K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug)
{
	cv::Mat cv_srcimg = cv::Mat(iImgHeight,iImgWidth,CV_8UC1,pImgData);
	cv::Mat dst;
	H_BaseImageProcess::Img_Sketch(cv_srcimg,dst);;
	cv::namedWindow("srcimg",0);
	cv::imshow("srcimg",cv_srcimg);
	cv::namedWindow("dst",0);
	cv::imshow("dst",dst);
	return 0;
}