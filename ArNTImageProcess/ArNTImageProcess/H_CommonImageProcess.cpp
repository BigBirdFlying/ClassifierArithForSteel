#include "H_CommonImageProcess.h"
#include "H_BaseImageProcess.h"
#include "H_SteelImageProcess.h"

H_CommonImageProcess::H_CommonImageProcess(void)
{
}

H_CommonImageProcess::~H_CommonImageProcess(void)
{
}

int H_CommonImageProcess::K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug)
{
	cv::Mat cv_srcimg = cv::Mat(iImgHeight,iImgWidth,CV_8UC1,pImgData);
	//
	cv::Mat cv_srcimg_P2D;
	pyrDown( cv_srcimg, cv_srcimg_P2D, cv::Size(cv_srcimg.cols/2, cv_srcimg.rows/2 ));
	//
	cv::Mat cv_srcimg_P2D_GCG;
	H_BaseImageProcess::Img_Gray_Change_Y(cv_srcimg_P2D,cv_srcimg_P2D_GCG,100,10);
	//
	int Left_Edge_P2D,Right_Edge_P2D;
	H_SteelImageProcess::Steel_FindEdge(cv_srcimg_P2D_GCG,Left_Edge_P2D,Right_Edge_P2D,25);//50
	//
	cv::Mat cv_srcimg_P2D_cut;
	H_SteelImageProcess::Steel_CutEdge(cv_srcimg_P2D,Left_Edge_P2D,Right_Edge_P2D,cv_srcimg_P2D_cut);
	//
	cv::Mat dst_P2D_cut;
	H_BaseImageProcess::Img_Gray_Change_Integral(cv_srcimg_P2D_cut,dst_P2D_cut,100,5,5);
	cv::Mat dst_P2D_cut_Sobelxx,dst_P2D_cut_Sobelyy,dst_P2D_cut_Sobelxy;
	H_BaseImageProcess::Img_Sobel(dst_P2D_cut,dst_P2D_cut_Sobelxx,dst_P2D_cut_Sobelyy,dst_P2D_cut_Sobelxy);
	//
	int width=16;
	int height=16;
	cv::Mat zeros=cv::Mat::zeros(dst_P2D_cut.rows,dst_P2D_cut.cols,dst_P2D_cut.type());
	for(int i=0;i<dst_P2D_cut.cols/width;i++)
	{
		for(int j=0;j<dst_P2D_cut.rows/height;j++)
		{
			cv::Rect rect(i*width,j*height,width,height);
			cv::Mat tmp_img;
			dst_P2D_cut_Sobelxx(rect).copyTo(tmp_img);
			int bins = 256;  
			int hist_size[] = {bins};  
			float range[] = { 0, 256 };  
			const float* ranges[] = {range};  
			cv::MatND hist;  
			int channels[] = {0};  
			calcHist( &tmp_img, 1, channels, cv::Mat(),hist, 1, hist_size, ranges,true, false );  
			//求一维熵
			float total_p = 0;    
			float entropy = 0;       
			for (int i=0;i<hist.rows;i++)
			{
				total_p += hist.at<float>(i,0);
			} 
			for (int i=0;i<hist.rows;i++)    
			{      
				float percentage = hist.at<float>(i,0)/total_p;    
				if (percentage != 0)
				{
					entropy += -percentage * logf(percentage);// 熵的定义公式；  
				}
			} 
			if(entropy>4.0)
			{
				tmp_img.copyTo(zeros(rect));
			}
		}
	}
	//
	cv::Mat dst_P2D_cut_Sobelxx_th;
	cv::threshold(zeros,dst_P2D_cut_Sobelxx_th,5,255,0);
	H_SteelImageProcess::Steel_RegionToDefectSet(dst_P2D_cut_Sobelxx_th,pH_DefectSet);
	for(int n=0;n<pH_DefectSet->iItemNum;n++)
	{
		pH_DefectSet->Items[n].iLeftInImage=(pH_DefectSet->Items[n].iLeftInImage+Left_Edge_P2D)*2;
		pH_DefectSet->Items[n].iTopInImage=pH_DefectSet->Items[n].iTopInImage*2;
		pH_DefectSet->Items[n].iRightInImage=(pH_DefectSet->Items[n].iRightInImage+Left_Edge_P2D)*2;
		pH_DefectSet->Items[n].iBottomInImage=pH_DefectSet->Items[n].iBottomInImage*2;
	}
	if(IsDebug)
	{
		cv::namedWindow("cv_srcimg_P2D",0);
		cv::imshow("cv_srcimg_P2D",cv_srcimg_P2D);
		cv::namedWindow("cv_srcimg_P2D_cut",0);
		cv::imshow("cv_srcimg_P2D_cut",cv_srcimg_P2D_cut);
		cv::namedWindow("dst_P2D_cut_Sobelxx",0);
		cv::imshow("dst_P2D_cut_Sobelxx",dst_P2D_cut_Sobelxx);
		cv::namedWindow("zeros",0);
		cv::imshow("zeros",zeros);
	}
	return 0;
}
