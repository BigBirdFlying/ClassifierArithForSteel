#include "StdAfx.h"
#include "ImgProcessPrimary.h"

#include "afxwin.h"

ImgProcessPrimary::ImgProcessPrimary(void)
{
}

ImgProcessPrimary::~ImgProcessPrimary(void)
{
}

void ImgProcessPrimary::GetSobelFeat(cv::Mat srcimg,vector<float>& feat)
{
 	int ddepth=CV_16S;
	int delta=0;
	int scale=1;

	cv::Mat srcimg_down1;
	pyrDown(srcimg,srcimg_down1,cv::Size(srcimg.rows/2,srcimg.cols/2));
	cv::Mat dstimg2_1,dstimg2_2,dstimg2_3;
	cv::Mat grad_x2, grad_y2;
	cv::Mat abs_grad_x2, abs_grad_y2;
	Sobel( srcimg_down1, grad_x2, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_x2, abs_grad_x2 );
	Sobel( srcimg_down1, grad_y2, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_y2, abs_grad_y2 );
	addWeighted( abs_grad_x2, 0.5, abs_grad_y2, 0.5, 0, dstimg2_1 );
	addWeighted( abs_grad_x2, 0.5, abs_grad_x2, 0.5, 0, dstimg2_2 );
	addWeighted( abs_grad_y2, 0.5, abs_grad_y2, 0.5, 0, dstimg2_3 );
	
	double minVal2_1,maxVal2_1;
	cv::minMaxLoc(dstimg2_1,&minVal2_1,&maxVal2_1);
	double minVal2_2,maxVal2_2;
	cv::minMaxLoc(dstimg2_2,&minVal2_2,&maxVal2_2);
	double minVal2_3,maxVal2_3;
	cv::minMaxLoc(dstimg2_3,&minVal2_3,&maxVal2_3);
	for(int i=0;i<dstimg2_1.rows;i++)
	{
		for(int j=0;j<dstimg2_1.cols;j++)
		{
			float temp=0;
			temp=(dstimg2_1.at<uchar>(i,j)-minVal2_1)/(maxVal2_1-minVal2_1);
			feat.push_back(temp);
			temp=(dstimg2_2.at<uchar>(i,j)-minVal2_2)/(maxVal2_2-minVal2_2);
			feat.push_back(temp);
			temp=(dstimg2_3.at<uchar>(i,j)-minVal2_3)/(maxVal2_3-minVal2_3);
			feat.push_back(temp);
		}
	}

	cv::Mat srcimg_down2;
	pyrDown(srcimg_down1,srcimg_down2,cv::Size(srcimg_down1.rows/2,srcimg_down1.cols/2));
	cv::Mat dstimg3_1,dstimg3_2,dstimg3_3;
	cv::Mat grad_x3, grad_y3;
	cv::Mat abs_grad_x3, abs_grad_y3;
	Sobel( srcimg_down2, grad_x3, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_x3, abs_grad_x3 );
	Sobel( srcimg_down2, grad_y3, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_y3, abs_grad_y3 );
	addWeighted( abs_grad_x3, 0.5, abs_grad_y3, 0.5, 0, dstimg3_1 );
	addWeighted( abs_grad_x3, 0.5, abs_grad_x3, 0.5, 0, dstimg3_2 );
	addWeighted( abs_grad_y3, 0.5, abs_grad_y3, 0.5, 0, dstimg3_3 );
	
	double minVal3_1,maxVal3_1;
	cv::minMaxLoc(dstimg3_1,&minVal3_1,&maxVal3_1);
	double minVal3_2,maxVal3_2;
	cv::minMaxLoc(dstimg3_2,&minVal3_2,&maxVal3_2);
	double minVal3_3,maxVal3_3;
	cv::minMaxLoc(dstimg3_3,&minVal3_3,&maxVal3_3);
	
	for(int i=0;i<dstimg3_1.rows;i++)
	{
		for(int j=0;j<dstimg3_1.cols;j++)
		{
			float temp=0;
			temp=(dstimg3_1.at<uchar>(i,j)-minVal3_1)/(maxVal3_1-minVal3_1);
			feat.push_back(temp);
			temp=(dstimg3_2.at<uchar>(i,j)-minVal3_2)/(maxVal3_2-minVal3_2);
			feat.push_back(temp);
			temp=(dstimg3_3.at<uchar>(i,j)-minVal3_3)/(maxVal3_3-minVal3_3);
			feat.push_back(temp);
		}
	}

}

void ImgProcessPrimary::GetLBPFeat(cv::Mat srcimg,vector<float>& feat)
{
	cv::Mat src,dst;
	cv::Mat _dst;
	srcimg.copyTo(src);
    _dst.create(src.rows-2, src.cols-2, CV_8UC1);  
	_dst.copyTo(dst);  
    dst.setTo(0);  
  
    for(int i=1;i<src.rows-1;i++) {  

        for(int j=1;j<src.cols-1;j++) {  
              
            char center = src.at<char>(i,j);  

            unsigned char code = 0;  
            code |= (src.at<char>(i-1,j-1) >= center) << 7;  
            code |= (src.at<char>(i-1,j  ) >= center) << 6;  
            code |= (src.at<char>(i-1,j+1) >= center) << 5;  
            code |= (src.at<char>(i  ,j+1) >= center) << 4;  
            code |= (src.at<char>(i+1,j+1) >= center) << 3;  
            code |= (src.at<char>(i+1,j  ) >= center) << 2;  
            code |= (src.at<char>(i+1,j-1) >= center) << 1;  
            code |= (src.at<char>(i  ,j-1) >= center) << 0;  
  
            dst.at<unsigned char>(i-1,j-1) = code;  
        }  
    }
	double minVal,maxVal;
	cv::minMaxLoc(dst,&minVal,&maxVal);
	for(int i=0;i<dst.rows;i++)
	{
		for(int j=0;j<dst.cols;j++)
		{
			float temp=(dst.at<uchar>(i,j)-minVal)/(maxVal-minVal);
			feat.push_back(temp);
		}
	}	
}

