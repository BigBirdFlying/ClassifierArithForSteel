#include "H_KHB_SanMingGangTie_1X_ImageProcess.h"
#include "H_BaseImageProcess.h"
#include "H_SteelImageProcess.h"

H_KHB_SanMingGangTie_1X_ImageProcess::H_KHB_SanMingGangTie_1X_ImageProcess(void)
{
}

H_KHB_SanMingGangTie_1X_ImageProcess::~H_KHB_SanMingGangTie_1X_ImageProcess(void)
{
}

int H_KHB_SanMingGangTie_1X_ImageProcess::K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug)
{
	//是否是调试模式
	m_bIsDebug=IsDebug;

	m_imgSrcWidth=iImgWidth;
	m_imgSrcHeight=iImgHeight;
	//原始图像
	cv::Mat cvSrcImg = cv::Mat(iImgHeight,iImgWidth,CV_8UC1,pImgData);
	cv::Mat cvSrcImg_P2D;
	cv::pyrDown(cvSrcImg,cvSrcImg_P2D,cv::Size(cvSrcImg.cols/2,cvSrcImg.rows/2));
	//矫正图像
	cv::Mat cvSrcImg_P2D_GCG;
	H_BaseImageProcess::Img_Gray_Change_Y(cvSrcImg_P2D,cvSrcImg_P2D_GCG,100,tConfigInfo_Segmentation.iEdgeThreshold);	
	//
	cv::Mat cvSrcImg_P4D,cvSrcImg_P4D_GCG;
	cv::pyrDown(cvSrcImg_P2D,cvSrcImg_P4D,cv::Size(cvSrcImg_P2D.cols/2,cvSrcImg_P2D.rows/2));
	cv::pyrDown(cvSrcImg_P2D_GCG,cvSrcImg_P4D_GCG,cv::Size(cvSrcImg_P2D_GCG.cols/2,cvSrcImg_P2D_GCG.rows/2));

	//边界判断
	int Left_Edge=m_imgSrcWidth-1;
	int Right_Edge=0;		
	Find_Edge_OpenCV_ForCommon(cvSrcImg_P4D_GCG,m_imgSrcWidth,m_imgSrcHeight,Left_Edge,Right_Edge,tConfigInfo_Segmentation.iEdgeThreshold,tConfigInfo_Segmentation.iEdgeNoDetect);	
	//强制边界限定
	if(tConfigInfo_Segmentation.tSteelInfo.iCamera>0)
	{
		if(Left_Edge<tConfigInfo_Segmentation.iLeftEdgeLimit[tConfigInfo_Segmentation.tSteelInfo.iCamera])
		{
			Left_Edge=tConfigInfo_Segmentation.iLeftEdgeLimit[tConfigInfo_Segmentation.tSteelInfo.iCamera];
		}
		if(Right_Edge>tConfigInfo_Segmentation.iRightEdgeLimit[tConfigInfo_Segmentation.tSteelInfo.iCamera])
		{
			Right_Edge=tConfigInfo_Segmentation.iRightEdgeLimit[tConfigInfo_Segmentation.tSteelInfo.iCamera];
		}
	}
	
	//
	if(true==m_bIsDebug)
	{
		cv::Mat cvSrcImgColor;
		cv::cvtColor(cvSrcImg,cvSrcImgColor,cv::COLOR_GRAY2RGB);
		cv::line(cvSrcImgColor,cv::Point(Left_Edge,0),cv::Point(Left_Edge,cvSrcImgColor.rows-1),cv::Scalar(255,0,0),2);
		cv::line(cvSrcImgColor,cv::Point(Right_Edge,0),cv::Point(Right_Edge,cvSrcImgColor.rows-1),cv::Scalar(255,0,0),2);
		cv::namedWindow("钢板边界",0);
		cv::imshow("钢板边界",cvSrcImgColor);
	}
	
	//检测缺陷
	if(Right_Edge-Left_Edge>m_imgSrcWidth*0.1)
	{
		Defects_HeatSteel1_OpenCV(cvSrcImg_P2D,cvSrcImg_P2D_GCG,Left_Edge/2,Right_Edge/2,2,tConfigInfo_Segmentation,pH_DefectSet);			
	}
	return pH_DefectSet->iItemNum;
}

void H_KHB_SanMingGangTie_1X_ImageProcess::Defects_HeatSteel1_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet) 
{
	//
	if(Left_Edge%2 != 0)
	{
		Left_Edge++;
	}
	if(Right_Edge%2 != 0)
	{
		Right_Edge--;
	}
	//切除
	cv::Mat SrcImg_Cut,SrcImg_GCG_Cut;
	cv::Rect rect(Left_Edge,0,Right_Edge-Left_Edge,SrcImg.rows);
	SrcImg(rect).copyTo(SrcImg_Cut);
	SrcImg_GCG(rect).copyTo(SrcImg_GCG_Cut);
	//头尾判断
	int Top_Head=m_imgSrcHeight-1;
	int Bottom_Head=0;	
	m_MatMark=Find_Head_OpenCV_ForCommon(SrcImg_GCG_Cut,m_imgSrcWidth,m_imgSrcHeight,Top_Head,Bottom_Head,tConfigInfo_Segmentation.iEdgeThreshold,tConfigInfo_Segmentation.iEdgeNoDetect);

	cv::threshold(m_MatMark,m_MatMark,5,255,0);
	cv::Mat SRow=cv::Mat::zeros(m_MatMark.rows,1,CV_32SC1);
	cv::Mat MatMark_16;
	m_MatMark.convertTo(MatMark_16,CV_32SC1);
	for(int i=0;i<m_MatMark.cols;i++)
	{
		//cv::Mat a=m_MatMark.col(i);
		SRow.col(0)+=MatMark_16.col(i);
	}
	int height_row=256;    
    int scale_row = 1;   //直方图的宽度  
	int bins_row=SRow.rows;
	cv::Mat img_row = cv::Mat::zeros(height_row,bins_row*scale_row, CV_8UC3);
	float* Avg_Row=new float[SRow.rows];
	float Avg_Row_Min=10000;
	float Avg_Row_Max=0;
    for(int i=0;i<bins_row;i++)    
    {      
		Avg_Row[i]=SRow.at<int>(i,0)/m_MatMark.cols;
		if(Avg_Row[i]<Avg_Row_Min)
		{
			Avg_Row_Min=Avg_Row[i];
		}
		if(Avg_Row[i]>Avg_Row_Max)
		{
			Avg_Row_Max=Avg_Row[i];
		}
		if(true==m_bIsDebug)
		{
			rectangle(img_row,cv::Point(i*scale_row,img_row.rows-1-Avg_Row[i]), cv::Point((i+1)*scale_row,img_row.cols-1),CV_RGB(255,255,0)); 
		}
    } 
	int top=(Avg_Row[0]+Avg_Row[1]+Avg_Row[2]+Avg_Row[3]+Avg_Row[4])/5;
	int bottom=(Avg_Row[SRow.rows-1]+Avg_Row[SRow.rows-2]+Avg_Row[SRow.rows-3]+Avg_Row[SRow.rows-4]+Avg_Row[SRow.rows-5])/5;
	int th_head=(top+bottom)*0.5;//(Avg_Row_Max+Avg_Row_Min)*0.5;
	int offset_head=m_MatMark.rows*0.35;
	if(abs(bottom-top)<50)
	{
		Top_Head=0;
		Bottom_Head=SRow.rows-1;
	}
	else
	{
		if(top>th_head && bottom<th_head)
		{
			Top_Head=0;
			for(int j=Top_Head;j<SRow.rows-5;j++)
			{
				float f_avg=(Avg_Row[j]+Avg_Row[j+1]+Avg_Row[j+2]+Avg_Row[j+3]+Avg_Row[j+4])/5;
				if(f_avg<th_head)
				{
					Bottom_Head=j-offset_head;
					break;
				}
			}
		}
		else if(bottom>th_head && top<th_head)
		{
			Bottom_Head=SRow.rows-1;
			for(int j=Bottom_Head;j>5;j--)
			{
				float f_avg=(Avg_Row[j]+Avg_Row[j-1]+Avg_Row[j-2]+Avg_Row[j-3]+Avg_Row[j-4])/5;
				if(f_avg<th_head)
				{
					Top_Head=j+offset_head;
					break;
				}
			}
		}
		else if(bottom>th_head && top>th_head)
		{
			Top_Head=0;
			Bottom_Head=SRow.rows-1;
		}
	}

	delete[] Avg_Row;
	//if(true==m_bIsDebug)
	{
		cv::line(img_row,cv::Point(Top_Head,0),cv::Point(Top_Head,255),cv::Scalar(255,0,255),2);
		cv::line(img_row,cv::Point(Bottom_Head,0),cv::Point(Bottom_Head,255),cv::Scalar(255,0,255),2);
		cv::namedWindow("img_row",0);
		cv::imshow("img_row",img_row);
	}
	///进入检测，进行不同分辨率下的灰度矫正	
	cv::Mat integral_img(SrcImg_Cut.rows+1,SrcImg_Cut.cols+1,CV_64FC1);
	integral(SrcImg_Cut,integral_img,CV_64FC1);//积分图像

	int block_x=5;
	int block_y=5;
	cv::Mat kern=cv::Mat::zeros(block_y,block_x,CV_32FC1);
	kern.at<float>(0,0)=1;
	kern.at<float>(0,kern.cols-1)=-1;
	kern.at<float>(kern.rows-1,0)=-1;
	kern.at<float>(kern.rows-1,kern.cols-1)=1;
	cv::Mat integral_img_Expand;
	cv::copyMakeBorder(integral_img,integral_img_Expand,block_y,block_y,block_x,block_x,cv::BORDER_REPLICATE);
	//cv::copyMakeBorder(integral_img,integral_img_Expand,block_y,block_y,block_x,block_x,cv::BORDER_CONSTANT,cv::Scalar::all(0));
	//cv::Rect rect_I(block_x/2,block_y/2,SrcImg_Cut.cols-block_x/2,SrcImg_Cut.rows-block_y/2);
	//cv::Mat integral_img_part;
	//integral_img(rect_I).copyTo(integral_img_part);
	//cv::Mat kern = (cv::Mat_<float>(5,5) <<  1,	0,  0,  0,  -1,  
	//										0,  0,  0,	0,	0,  
	//										0,	0,  0,	0,	0,
	//										0,	0,	0,	0,	0,
	//										-1,	0,	0,	0,	1); 
	cv::Mat dstF,dstF_Expand;  
	filter2D(integral_img_Expand,dstF_Expand,integral_img_Expand.depth(),kern); //
	
	cv::Rect rect_part(block_x,block_y,SrcImg_Cut.cols,SrcImg_Cut.rows);
	dstF_Expand(rect_part).copyTo(dstF);

	cv::Mat SrcImg_Cut_64;
	SrcImg_Cut.convertTo(SrcImg_Cut_64,CV_64FC1);
	cv::Mat dst_64=cv::Mat::zeros(SrcImg_Cut.size(), CV_64FC1);
	dst_64=(SrcImg_Cut_64*block_x*block_y*100)/dstF; // (block_x*block_y)/dstF

	cv::Rect rect_2(block_x/2,block_y/2,SrcImg_Cut.cols-block_x,SrcImg_Cut.rows-block_y);
	cv::Mat SrcImg_Cut_1,dstF_1;
	SrcImg_Cut(rect_2).copyTo(SrcImg_Cut_1);
	dstF(rect_2).copyTo(dstF_1);
	cv::Mat SrcImg_Cut_1_64;
	SrcImg_Cut_1.convertTo(SrcImg_Cut_1_64,CV_64FC1);
	cv::Mat dst_64_cut=cv::Mat::zeros(SrcImg_Cut_1.size(), CV_64FC1);
	dst_64_cut=(SrcImg_Cut_1_64*block_x*block_y*100)/dstF_1; // (block_x*block_y)/dstF
	cv::Mat dst_cut;
	cv::normalize(dst_64_cut,dst_cut,0,255,cv::NORM_MINMAX,CV_8UC1);
	cv::namedWindow("dst_cut",0);
	cv::imshow("dst_cut",dst_cut);

	for(int i=0;i<SrcImg_Cut_64.rows;i++)
	{
		for(int j=0;j<SrcImg_Cut_64.cols;j++)
		{
			double aa=SrcImg_Cut_64.at<double>(i,j);
			double bb=dstF.at<double>(i,j);
			double cc=dst_64.at<double>(i,j);
			double dd=0;
			if(cc>255)
			{
				int a=1;
			}
		}
	}


	cv::Mat dst;
	cv::normalize(dst_64,dst,0,255,cv::NORM_MINMAX,CV_8UC1);
	cv::namedWindow("dst",0);
	cv::imshow("dst",dst);
	cv::Mat dst_equ;
	cv::equalizeHist(dst,dst_equ);
	cv::namedWindow("dst_equ",0);
	cv::imshow("dst_equ",dst_equ);
		
	cv::Mat dstimg1,dstimg2;
	cv::Mat grad_x1,grad_x2;
	cv::Mat abs_grad_x1,abs_grad_x2;
	cv::Sobel( dst, grad_x1, CV_16S, 0, 2, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x1, abs_grad_x1 );
	cv::addWeighted( abs_grad_x1, 0.5, abs_grad_x1, 0.5, 0, dstimg1);
	cv::medianBlur(dstimg1,dstimg1,3);
	cv::Sobel( dst, grad_x2, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x2, abs_grad_x2 );
	cv::addWeighted( abs_grad_x2, 0.5, abs_grad_x2, 0.5, 0, dstimg2);
	cv::medianBlur(dstimg2,dstimg2,3);
	{
		cv::namedWindow("dstimg1",0);
		cv::imshow("dstimg1",dstimg1);
		cv::namedWindow("dstimg2",0);
		cv::imshow("dstimg2",dstimg2);
	}
	cv::Mat mean1;
	cv::Mat dst1_1=cv::Mat::zeros(dst.rows,dst.cols,dst.type());
	cv::Mat dst1_2=cv::Mat::zeros(dst.rows,dst.cols,dst.type());
	cv::blur(dst,mean1,cv::Size(15,15));
	cv::subtract(dst, mean1, dst1_1);
	cv::subtract(mean1, dst, dst1_2);
	cv::threshold(dst1_1,dst1_1,25,255,0);
	cv::threshold(dst1_2,dst1_2,25,255,0);
	//cv::medianBlur(dst1_1,dst1_1,3);
	//cv::medianBlur(dst1_2,dst1_2,3);
	{
		cv::namedWindow("dst1_1",0);
		cv::imshow("dst1_1",dst1_1);
		cv::namedWindow("dst1_2",0);
		cv::imshow("dst1_2",dst1_2);
	}

	int width=32;
	int height=32;
	//
	//for(int i=0;i<dst1_2.cols/width;i++)
	//{
	//	for(int j=0;j<dst1_2.rows/height;j++)
	//	{
	//		cv::Rect rect(i*width,j*height,width,height);
	//		//dst_zero1上
	//		cv::Mat tmp_img1;
	//		dst1_2(rect).copyTo(tmp_img1);
	//		int s=cv::countNonZero(tmp_img1);
	//		if(s>20)
	//		{
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(rect.tl().x+Left_Edge)*2;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(rect.tl().y)*2;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(rect.br().x+Left_Edge)*2;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(rect.br().y)*2;
	//				pH_DefectSet->iItemNum++;
	//		}
	//	}
	//}
	//for(int i=0;i<dst1_2.cols/width-1;i++)
	//{
	//	for(int j=0;j<dst1_2.rows/height-1;j++)
	//	{
	//		cv::Rect rect(i*width+width/2,j*height+height/2,width,height);
	//		cv::Mat tmp_img1;
	//		dst1_2(rect).copyTo(tmp_img1);
	//		int s=cv::countNonZero(tmp_img1);
	//		if(s>20)
	//		{
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(rect.tl().x+Left_Edge)*2;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(rect.tl().y)*2;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(rect.br().x+Left_Edge)*2;
	//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(rect.br().y)*2;
	//				pH_DefectSet->iItemNum++;
	//		}
	//	}
	//}
	///
	if(Bottom_Head>Top_Head && Top_Head>=0)
	{
		cv::Mat dstimg1_cut,dstimg2_cut;
		cv::Rect rect_cut(0,Top_Head,dstimg1.cols,Bottom_Head-Top_Head);
		dstimg1(rect_cut).copyTo(dstimg1_cut);
		dstimg2(rect_cut).copyTo(dstimg2_cut);

		cv::Mat dst1_2_cut;
		dst1_2(rect_cut).copyTo(dst1_2_cut);

		cv::Mat zeros=cv::Mat::zeros(dstimg1_cut.rows,dstimg1_cut.cols,dstimg1_cut.type());
		for(int i=0;i<dstimg1_cut.cols/width;i++)
		{
			for(int j=0;j<dstimg1_cut.rows/height;j++)
			{
				cv::Rect rect(i*width,j*height,width,height);
				//dst_zero1上
				cv::Mat tmp_img1;
				dstimg1_cut(rect).copyTo(tmp_img1);
				int bins1 = 256;  
				int hist_size1[] = {bins1};  
				float range1[] = { 0, 256 };  
				const float* ranges1[] = {range1};  
				cv::MatND hist1;  
				int channels1[] = {0};  
				calcHist( &tmp_img1, 1, channels1, cv::Mat(),hist1, 1, hist_size1, ranges1,true, false );  
				float total_p1 = 0;    
				float entropy1 = 0;       
				for (int i=0;i<hist1.rows;i++)
				{
					total_p1 += hist1.at<float>(i,0);
				} 
				for (int i=0;i<hist1.rows;i++)    
				{      
					float percentage1 = hist1.at<float>(i,0)/total_p1;    
					if (percentage1 != 0)
					{
						entropy1 += -percentage1 * logf(percentage1);// 熵的定义公式；  
					}
				} 
				cv::Mat tmp_img_mean1,tmp_img_dev1;
				cv::meanStdDev(tmp_img1,tmp_img_mean1,tmp_img_dev1);
				float f_tmp_img_dev1=tmp_img_dev1.at<double>(0,0);
				//
				cv::Mat tmp_img2;
				dst1_2_cut(rect).copyTo(tmp_img2);
				int s=cv::countNonZero(tmp_img2);
				//
				float f_dd=1.5;
				if(entropy1>f_dd && f_tmp_img_dev1>f_dd*10)
				{
					if(s>20)
					{
						tmp_img1.copyTo(zeros(rect));
						//加缺陷 
						if(Bottom_Head-Top_Head>dst.rows*0.95)
						{
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(rect.tl().x+Left_Edge)*2;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(rect.tl().y+Top_Head)*2;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(rect.br().x+Left_Edge)*2;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(rect.br().y+Top_Head)*2;
							pH_DefectSet->iItemNum++;
						}
					}
				}
			}
		}
		for(int i=0;i<dstimg2_cut.cols/width-1;i++)
		{
			for(int j=0;j<dstimg1_cut.rows/height-1;j++)
			{
				cv::Rect rect(i*width+width/2,j*height+height/2,width,height);
				//dst_zero1上
				cv::Mat tmp_img1;
				dstimg1_cut(rect).copyTo(tmp_img1);
				int bins1 = 256;  
				int hist_size1[] = {bins1};  
				float range1[] = { 0, 256 };  
				const float* ranges1[] = {range1};  
				cv::MatND hist1;  
				int channels1[] = {0};  
				calcHist( &tmp_img1, 1, channels1, cv::Mat(),hist1, 1, hist_size1, ranges1,true, false );  
				float total_p1 = 0;    
				float entropy1 = 0;       
				for (int i=0;i<hist1.rows;i++)
				{
					total_p1 += hist1.at<float>(i,0);
				} 
				for (int i=0;i<hist1.rows;i++)    
				{      
					float percentage1 = hist1.at<float>(i,0)/total_p1;    
					if (percentage1 != 0)
					{
						entropy1 += -percentage1 * logf(percentage1);// 熵的定义公式；  
					}
				} 
				cv::Mat tmp_img_mean1,tmp_img_dev1;
				cv::meanStdDev(tmp_img1,tmp_img_mean1,tmp_img_dev1);
				float f_tmp_img_dev1=tmp_img_dev1.at<double>(0,0);
				//dst_zero2上
				cv::Mat tmp_img2;
				dst1_2_cut(rect).copyTo(tmp_img2);
				int s=cv::countNonZero(tmp_img2);
				//
				float f_dd=1.5;
				if(entropy1>f_dd && f_tmp_img_dev1>f_dd*10)
				{
					if(s>20)
					{
						tmp_img1.copyTo(zeros(rect));
						//加缺陷 
						if(Bottom_Head-Top_Head>dst.rows*0.95)
						{
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(rect.tl().x+Left_Edge)*2;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(rect.tl().y+Top_Head)*2;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(rect.br().x+Left_Edge)*2;
							pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(rect.br().y+Top_Head)*2;
							pH_DefectSet->iItemNum++;
						}
					}
				}
			}
		}
	}

}

//void H_KHB_SanMingGangTie_1X_ImageProcess::Defects_HeatSteel_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet) 
//{
//	//
//	if(Left_Edge%2 != 0)
//	{
//		Left_Edge++;
//	}
//	if(Right_Edge%2 != 0)
//	{
//		Right_Edge--;
//	}
//	//切除
//	cv::Mat SrcImg_Cut,SrcImg_GCG_Cut;
//	cv::Rect rect(Left_Edge,0,Right_Edge-Left_Edge,SrcImg.rows);
//	SrcImg(rect).copyTo(SrcImg_Cut);
//	SrcImg_GCG(rect).copyTo(SrcImg_GCG_Cut);
//	//头尾判断
//	int Top_Head=m_imgSrcHeight-1;
//	int Bottom_Head=0;	
//	m_MatMark=Find_Head_OpenCV_ForCommon(SrcImg_GCG_Cut,m_imgSrcWidth,m_imgSrcHeight,Top_Head,Bottom_Head,tConfigInfo_Segmentation.iEdgeThreshold,tConfigInfo_Segmentation.iEdgeNoDetect);
//
//	cv::threshold(m_MatMark,m_MatMark,5,255,0);
//	cv::Mat SRow=cv::Mat::zeros(m_MatMark.rows,1,CV_32SC1);
//	cv::Mat MatMark_16;
//	m_MatMark.convertTo(MatMark_16,CV_32SC1);
//	for(int i=0;i<m_MatMark.cols;i++)
//	{
//		//cv::Mat a=m_MatMark.col(i);
//		SRow.col(0)+=MatMark_16.col(i);
//	}
//	int height_row=256;    
//    int scale_row = 1;   //直方图的宽度  
//	int bins_row=SRow.rows;
//	cv::Mat img_row = cv::Mat::zeros(height_row,bins_row*scale_row, CV_8UC3);
//	float* Avg_Row=new float[SRow.rows];
//	float Avg_Row_Min=10000;
//	float Avg_Row_Max=0;
//    for(int i=0;i<bins_row;i++)    
//    {      
//		Avg_Row[i]=SRow.at<int>(i,0)/m_MatMark.cols;
//		if(Avg_Row[i]<Avg_Row_Min)
//		{
//			Avg_Row_Min=Avg_Row[i];
//		}
//		if(Avg_Row[i]>Avg_Row_Max)
//		{
//			Avg_Row_Max=Avg_Row[i];
//		}
//		if(true==m_bIsDebug)
//		{
//			rectangle(img_row,cv::Point(i*scale_row,img_row.rows-1-Avg_Row[i]), cv::Point((i+1)*scale_row,img_row.cols-1),CV_RGB(255,255,0)); 
//		}
//    } 
//	int top=(Avg_Row[0]+Avg_Row[1]+Avg_Row[2]+Avg_Row[3]+Avg_Row[4])/5;
//	int bottom=(Avg_Row[SRow.rows-1]+Avg_Row[SRow.rows-2]+Avg_Row[SRow.rows-3]+Avg_Row[SRow.rows-4]+Avg_Row[SRow.rows-5])/5;
//	int th_head=(top+bottom)*0.5;//(Avg_Row_Max+Avg_Row_Min)*0.5;
//	int offset_head=m_MatMark.rows*0.35;
//	if(abs(bottom-top)<50)
//	{
//		Top_Head=0;
//		Bottom_Head=SRow.rows-1;
//	}
//	else
//	{
//		if(top>th_head && bottom<th_head)
//		{
//			Top_Head=0;
//			for(int j=Top_Head;j<SRow.rows-5;j++)
//			{
//				float f_avg=(Avg_Row[j]+Avg_Row[j+1]+Avg_Row[j+2]+Avg_Row[j+3]+Avg_Row[j+4])/5;
//				if(f_avg<th_head)
//				{
//					Bottom_Head=j-offset_head;
//					break;
//				}
//			}
//		}
//		else if(bottom>th_head && top<th_head)
//		{
//			Bottom_Head=SRow.rows-1;
//			for(int j=Bottom_Head;j>5;j--)
//			{
//				float f_avg=(Avg_Row[j]+Avg_Row[j-1]+Avg_Row[j-2]+Avg_Row[j-3]+Avg_Row[j-4])/5;
//				if(f_avg<th_head)
//				{
//					Top_Head=j+offset_head;
//					break;
//				}
//			}
//		}
//		else if(bottom>th_head && top>th_head)
//		{
//			Top_Head=0;
//			Bottom_Head=SRow.rows-1;
//		}
//	}
//
//	delete[] Avg_Row;
//	if(true==m_bIsDebug)
//	{
//		cv::line(img_row,cv::Point(Top_Head,0),cv::Point(Top_Head,255),cv::Scalar(255,0,255),2);
//		cv::line(img_row,cv::Point(Bottom_Head,0),cv::Point(Bottom_Head,255),cv::Scalar(255,0,255),2);
//		cv::namedWindow("img_row",0);
//		cv::imshow("img_row",img_row);
//	}
//	///进入检测，进行不同分辨率下的灰度矫正	
//	cv::Mat integral_img(SrcImg_Cut.rows+1,SrcImg_Cut.cols+1,CV_64FC1);
//	integral(SrcImg_Cut,integral_img,CV_64FC1);//积分图像
//	int block_x1=32;
//	int block_y1=16;
//	int block_x2=8;
//	int block_y2=4;
//	int baseGray=100;
//	cv::Mat dst_zero1=cv::Mat::zeros(SrcImg_Cut.rows,SrcImg_Cut.cols,SrcImg_Cut.type());
//	cv::Mat dst_zero2=cv::Mat::zeros(SrcImg_Cut.rows,SrcImg_Cut.cols,SrcImg_Cut.type());
//	cv::Mat dst_zero_no=cv::Mat::zeros(SrcImg_Cut.rows,SrcImg_Cut.cols,SrcImg_Cut.type());
//	for(int i=0;i<integral_img.rows-1;i++)
//	{
//		for(int j=0;j<integral_img.cols-1;j++)
//		{
//			int begin_x1=max(0,j-block_x1/2);
//			int end_x1=min(integral_img.cols-1,j+block_x1/2);
//			int begin_y1=max(0,i-block_y1/2);
//			int end_y1=min(integral_img.rows-1,i+block_y1/2);
//			float A1=integral_img.at<double>(end_y1,end_x1);
//			float B1=integral_img.at<double>(begin_y1,begin_x1);
//			float C1=integral_img.at<double>(begin_y1,end_x1);
//			float D1=integral_img.at<double>(end_y1,begin_x1);
//			float temp1=A1+B1-C1-D1;
//			float x1=temp1/(abs(end_x1-begin_x1)*abs(end_y1-begin_y1));
//			float _x1=1/(x1+0.000001);		
//			dst_zero1.at<uchar>(i,j)=min(255,(int)(SrcImg_Cut.at<uchar>(i,j)*baseGray*_x1));
//
//			int begin_x2=max(0,j-block_x2/2);
//			int end_x2=min(integral_img.cols-1,j+block_x2/2);
//			int begin_y2=max(0,i-block_y2/2);
//			int end_y2=min(integral_img.rows-1,i+block_y2/2);
//			float A2=integral_img.at<double>(end_y2,end_x2);
//			float B2=integral_img.at<double>(begin_y2,begin_x2);
//			float C2=integral_img.at<double>(begin_y2,end_x2);
//			float D2=integral_img.at<double>(end_y2,begin_x2);
//			float temp2=A2+B2-C2-D2;
//			float x2=temp2/(abs(end_x2-begin_x2)*abs(end_y2-begin_y2));
//			float _x2=1/(x2+0.000001);		
//			dst_zero2.at<uchar>(i,j)=min(255,(int)(SrcImg_Cut.at<uchar>(i,j)*baseGray*_x2));		
//		}
//	}
//	if(true==m_bIsDebug)
//	{
//		cv::namedWindow("dst_zero1",0);
//		cv::imshow("dst_zero1",dst_zero1);
//		cv::namedWindow("dst_zero2",0);
//		cv::imshow("dst_zero2",dst_zero2);
//	}
//	///测试代码
//	//cv::Mat dstimg1,dstimg2;
//	//cv::Mat grad_x1,grad_x2;
//	//cv::Mat abs_grad_x1,abs_grad_x2;
//	//// 求 X方向梯度
//	//cv::Sobel( dst_zero1, grad_x1, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
//	//cv::convertScaleAbs( grad_x1, abs_grad_x1 );
//	//cv::addWeighted( abs_grad_x1, 0.5, abs_grad_x1, 0.5, 0, dstimg1);
//	//cv::medianBlur(dstimg1,dstimg1,3);
//	//cv::Sobel( dst_zero2, grad_x2, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
//	//cv::convertScaleAbs( grad_x2, abs_grad_x2 );
//	//cv::addWeighted( abs_grad_x2, 0.5, abs_grad_x2, 0.5, 0, dstimg2);
//	//cv::medianBlur(dstimg2,dstimg2,3);
//	//if(true==m_bIsDebug)
//	//{
//	//	cv::namedWindow("dstimg1",0);
//	//	cv::imshow("dstimg1",dstimg1);
//	//	cv::namedWindow("dstimg2",0);
//	//	cv::imshow("dstimg2",dstimg2);
//	//}
//	//int width=32;
//	//int height=32;
//	//if(Bottom_Head>Top_Head && Top_Head>=0)
//	//{
//	//	cv::Mat dstimg1_cut,dstimg2_cut;
//	//	cv::Rect rect_cut(0,Top_Head,dstimg1.cols,Bottom_Head-Top_Head);
//	//	dstimg1(rect_cut).copyTo(dstimg1_cut);
//	//	dstimg2(rect_cut).copyTo(dstimg2_cut);
//
//	//	cv::Mat zeros=cv::Mat::zeros(dstimg1_cut.rows,dstimg1_cut.cols,dstimg1_cut.type());
//	//	for(int i=0;i<dstimg1_cut.cols/width;i++)
//	//	{
//	//		for(int j=0;j<dstimg1_cut.rows/height;j++)
//	//		{
//	//			cv::Rect rect(i*width,j*height,width,height);
//	//			//dst_zero1上
//	//			cv::Mat tmp_img1;
//	//			dstimg1_cut(rect).copyTo(tmp_img1);
//	//			int bins1 = 256;  
//	//			int hist_size1[] = {bins1};  
//	//			float range1[] = { 0, 256 };  
//	//			const float* ranges1[] = {range1};  
//	//			cv::MatND hist1;  
//	//			int channels1[] = {0};  
//	//			calcHist( &tmp_img1, 1, channels1, cv::Mat(),hist1, 1, hist_size1, ranges1,true, false );  
//	//			float total_p1 = 0;    
//	//			float entropy1 = 0;       
//	//			for (int i=0;i<hist1.rows;i++)
//	//			{
//	//				total_p1 += hist1.at<float>(i,0);
//	//			} 
//	//			for (int i=0;i<hist1.rows;i++)    
//	//			{      
//	//				float percentage1 = hist1.at<float>(i,0)/total_p1;    
//	//				if (percentage1 != 0)
//	//				{
//	//					entropy1 += -percentage1 * logf(percentage1);// 熵的定义公式；  
//	//				}
//	//			} 
//	//			cv::Mat tmp_img_mean1,tmp_img_dev1;
//	//			cv::meanStdDev(tmp_img1,tmp_img_mean1,tmp_img_dev1);
//	//			float f_tmp_img_dev1=tmp_img_dev1.at<double>(0,0);
//	//			//dst_zero2上
//	//			cv::Mat tmp_img2;
//	//			dstimg2_cut(rect).copyTo(tmp_img2);
//	//			int bins2 = 256;  
//	//			int hist_size2[] = {bins2};  
//	//			float range2[] = { 0, 256 };  
//	//			const float* ranges2[] = {range2};  
//	//			cv::MatND hist2;  
//	//			int channels2[] = {0};  
//	//			calcHist( &tmp_img2, 1, channels2, cv::Mat(),hist2, 1, hist_size2, ranges2,true, false );  
//	//			float total_p2 = 0;    
//	//			float entropy2 = 0;       
//	//			for (int i=0;i<hist2.rows;i++)
//	//			{
//	//				total_p2 += hist2.at<float>(i,0);
//	//			} 
//	//			for (int i=0;i<hist2.rows;i++)    
//	//			{      
//	//				float percentage2 = hist2.at<float>(i,0)/total_p2;    
//	//				if (percentage2 != 0)
//	//				{
//	//					entropy2 += -percentage2 * logf(percentage2);// 熵的定义公式；  
//	//				}
//	//			} 
//	//			cv::Mat tmp_img_mean2,tmp_img_dev2;
//	//			cv::meanStdDev(tmp_img2,tmp_img_mean2,tmp_img_dev2);
//	//			float f_tmp_img_dev2=tmp_img_dev2.at<double>(0,0);
//	//			//
//	//			float f_dd=3.5;
//	//			if(entropy1>f_dd && f_tmp_img_dev1>f_dd*10)
//	//			{
//	//				if(entropy2>f_dd && f_tmp_img_dev2>f_dd*10)
//	//				{
//	//					tmp_img1.copyTo(zeros(rect));
//	//					//加缺陷 
//	//					if(Bottom_Head-Top_Head>dst_zero1.rows*0.95)
//	//					{
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(rect.tl().x+Left_Edge)*2;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(rect.tl().y+Top_Head)*2;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(rect.br().x+Left_Edge)*2;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(rect.br().y+Top_Head)*2;
//	//						pH_DefectSet->iItemNum++;
//	//					}
//	//				}
//	//			}
//	//		}
//	//	}
//	//	for(int i=0;i<dstimg2_cut.cols/width-1;i++)
//	//	{
//	//		for(int j=0;j<dstimg1_cut.rows/height-1;j++)
//	//		{
//	//			cv::Rect rect(i*width+width/2,j*height+height/2,width,height);
//	//			//dst_zero1上
//	//			cv::Mat tmp_img1;
//	//			dstimg1_cut(rect).copyTo(tmp_img1);
//	//			int bins1 = 256;  
//	//			int hist_size1[] = {bins1};  
//	//			float range1[] = { 0, 256 };  
//	//			const float* ranges1[] = {range1};  
//	//			cv::MatND hist1;  
//	//			int channels1[] = {0};  
//	//			calcHist( &tmp_img1, 1, channels1, cv::Mat(),hist1, 1, hist_size1, ranges1,true, false );  
//	//			float total_p1 = 0;    
//	//			float entropy1 = 0;       
//	//			for (int i=0;i<hist1.rows;i++)
//	//			{
//	//				total_p1 += hist1.at<float>(i,0);
//	//			} 
//	//			for (int i=0;i<hist1.rows;i++)    
//	//			{      
//	//				float percentage1 = hist1.at<float>(i,0)/total_p1;    
//	//				if (percentage1 != 0)
//	//				{
//	//					entropy1 += -percentage1 * logf(percentage1);// 熵的定义公式；  
//	//				}
//	//			} 
//	//			cv::Mat tmp_img_mean1,tmp_img_dev1;
//	//			cv::meanStdDev(tmp_img1,tmp_img_mean1,tmp_img_dev1);
//	//			float f_tmp_img_dev1=tmp_img_dev1.at<double>(0,0);
//	//			//dst_zero2上
//	//			cv::Mat tmp_img2;
//	//			dstimg2_cut(rect).copyTo(tmp_img2);
//	//			int bins2 = 256;  
//	//			int hist_size2[] = {bins2};  
//	//			float range2[] = { 0, 256 };  
//	//			const float* ranges2[] = {range2};  
//	//			cv::MatND hist2;  
//	//			int channels2[] = {0};  
//	//			calcHist( &tmp_img2, 1, channels2, cv::Mat(),hist2, 1, hist_size2, ranges2,true, false );  
//	//			float total_p2 = 0;    
//	//			float entropy2 = 0;       
//	//			for (int i=0;i<hist2.rows;i++)
//	//			{
//	//				total_p2 += hist2.at<float>(i,0);
//	//			} 
//	//			for (int i=0;i<hist2.rows;i++)    
//	//			{      
//	//				float percentage2 = hist2.at<float>(i,0)/total_p2;    
//	//				if (percentage2 != 0)
//	//				{
//	//					entropy2 += -percentage2 * logf(percentage2);// 熵的定义公式；  
//	//				}
//	//			} 
//	//			cv::Mat tmp_img_mean2,tmp_img_dev2;
//	//			cv::meanStdDev(tmp_img2,tmp_img_mean2,tmp_img_dev2);
//	//			float f_tmp_img_dev2=tmp_img_dev2.at<double>(0,0);
//	//			//
//	//			float f_dd=3.5;
//	//			if(entropy1>f_dd && f_tmp_img_dev1>f_dd*10)
//	//			{
//	//				if(entropy2>f_dd && f_tmp_img_dev2>f_dd*10)
//	//				{
//	//					tmp_img1.copyTo(zeros(rect));
//	//					//加缺陷 
//	//					if(Bottom_Head-Top_Head>dst_zero1.rows*0.95)
//	//					{
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(rect.tl().x+Left_Edge)*2;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(rect.tl().y+Top_Head)*2;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(rect.br().x+Left_Edge)*2;
//	//						pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(rect.br().y+Top_Head)*2;
//	//						pH_DefectSet->iItemNum++;
//	//					}
//	//				}
//	//			}
//	//		}
//	//	}
//	//	if(true==m_bIsDebug)
//	//	{
//	//		cv::namedWindow("zeros",0);
//	//		cv::imshow("zeros",zeros);
//	//	}
//	//}
//	///图像无效区域界定，主要在于将头尾那种燕尾型的区域去掉 
//	cv::Mat SrcImg_Cut_Med,SrcImg_Cut_Canny,SrcImg_Cut_Canny_Dil;
//	cv::medianBlur(SrcImg_Cut,SrcImg_Cut_Med,3);
//	cv::Canny(SrcImg_Cut_Med,SrcImg_Cut_Canny,20,30);
//	int dilation_size_canny = 2;
//	cv::Mat element_canny= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_canny + 1, 2*dilation_size_canny+1 ));
//	dilate( SrcImg_Cut_Canny, SrcImg_Cut_Canny_Dil, element_canny);
//	if(true==m_bIsDebug)
//	{
//		cv::namedWindow("SrcImg_Cut_Canny",0);
//		cv::imshow("SrcImg_Cut_Canny",SrcImg_Cut_Canny);
//		cv::namedWindow("SrcImg_Cut_Canny_Dil",0);
//		cv::imshow("SrcImg_Cut_Canny_Dil",SrcImg_Cut_Canny_Dil);
//	}
//	///裂纹检测
//	cv::Mat dst_zero1_mean,dst_zero1_dev;
//	cv::meanStdDev(dst_zero1,dst_zero1_mean,dst_zero1_dev);
//	float f_dst_zero1_dev=dst_zero1_dev.at<double>(0,0);
//	cv::Mat dst_zero2_mean,dst_zero2_dev;
//	cv::meanStdDev(dst_zero2,dst_zero2_mean,dst_zero2_dev);
//	float f_dst_zero2_dev=dst_zero2_dev.at<double>(0,0);
//
//	float th=(1/(f_dst_zero1_dev+f_dst_zero2_dev))*1000;
//	th=max(40,(int)(min(80,(int)th)));
//
//	cv::Mat dst_zero1_th,dst_zero2_th;
//	cv::threshold(dst_zero1,dst_zero1_th,th/1.05,255,1);
//	cv::threshold(dst_zero2,dst_zero2_th,th*1.05,255,1);
//	#ifdef W_DEBUG
//		cv::namedWindow("dst_zero1_th",0);
//		cv::imshow("dst_zero1_th",dst_zero1_th);
//		cv::namedWindow("dst_zero2_th",0);
//		cv::imshow("dst_zero2_th",dst_zero2_th);
//	#endif
//	cv::Mat cdst1=cv::Mat::zeros(dst_zero1_th.size(),CV_8UC1);
//	int num_line1=0;
//	cv::vector<cv::Vec4i>lines1;
//	HoughLinesP(dst_zero1_th,lines1,1,CV_PI/180,50,30,15);
//	for(int i=0;i<lines1.size();i++)
//	{
//		cv::Vec4i l=lines1[i];
//		float width=abs(l[2]-l[0]);
//		float height=abs(l[3]- l[1]);
//		if(height/(width+0.01)>7.5 )
//		{
//			line( cdst1, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
//			num_line1++;
//		}
//	}
//	cv::Mat cdst2=cv::Mat::zeros(dst_zero2_th.size(),CV_8UC1);
//	int num_line2=0;
//	cv::vector<cv::Vec4i>lines2;
//	HoughLinesP(dst_zero2_th,lines2,1,CV_PI/180,50,30,15);
//	for(int i=0;i<lines2.size();i++)
//	{
//		cv::Vec4i l=lines2[i];
//		float width=abs(l[2]-l[0]);
//		float height=abs(l[3]- l[1]);
//		if(height/(width+0.01)>7.5 )
//		{
//			line( cdst2, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
//			num_line2++;
//		}
//	}
//	cv::Mat dst_crack;
//	if(num_line2<=0)
//	{
//		dst_crack=cdst1.clone();
//	}
//	else
//	{
//		cv::addWeighted(cdst1,0.5,cdst1,0.5,0,dst_crack);
//	}
//	cv::threshold(dst_crack,dst_crack,150,255,0);
//	#ifdef W_DEBUG
//		cv::namedWindow("cdst1",0);
//		cv::imshow("cdst1",cdst1);
//		cv::namedWindow("cdst2",0);
//		cv::imshow("cdst2",cdst2);
//		cv::namedWindow("dst_crack",0);
//		cv::imshow("dst_crack",dst_crack);
//	#endif
//	///裂纹验证
//	cv::Mat dst_zero2_blur,dst_zero2_sub;
//	cv::blur(dst_zero2,dst_zero2_blur,cv::Size(11,11));
//	cv::subtract(dst_zero2_blur,dst_zero2,dst_zero2_sub);
//	cv::Mat dst_zero2_sub_th;
//	cv::threshold(dst_zero2_sub,dst_zero2_sub_th,20,255,0);
//	#ifdef W_DEBUG
//		cv::namedWindow("dst_zero2_sub",0);
//		cv::imshow("dst_zero2_sub",dst_zero2_sub);
//		cv::namedWindow("dst_zero2_sub_th",0);
//		cv::imshow("dst_zero2_sub_th",dst_zero2_sub_th);
//	#endif
//
//	cv::Mat dst_zero1_med;
//	cv::medianBlur(dst_zero1,dst_zero1_med,3);
//	cv::Mat dst_sobel_crack;
//	cv::Mat grad_y_crack,abs_grad_y_crack;
//	cv::Sobel( dst_zero1_med, grad_y_crack, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
//	cv::convertScaleAbs( grad_y_crack, abs_grad_y_crack );
//	cv::addWeighted( abs_grad_y_crack, 0.5, abs_grad_y_crack, 0.5, 0, dst_sobel_crack );
//	cv::threshold(dst_sobel_crack,dst_sobel_crack,150,255,0);
//	#ifdef W_DEBUG
//		cv::namedWindow("dst_sobel_crack",0);
//		cv::imshow("dst_sobel_crack",dst_sobel_crack);
//	#endif
//	///凹坑及翘皮检测
//	cv::Mat dst_zero3_th;
//	cv::threshold(dst_zero1,dst_zero3_th,40,255,1);//40
//	cv::Mat dst_aokeng;
//	cv::medianBlur(dst_zero3_th,dst_aokeng,5);
//	cv::Mat dst_zero_Add,dst_qiaopi;
//	cv::addWeighted(dst_zero1_th,0.5,dst_zero2_th,0.5,0,dst_zero_Add);
//	cv::threshold(dst_zero_Add,dst_qiaopi,150,255,0);
//	if(true==m_bIsDebug)
//	{
//		cv::namedWindow("dst_qiaopi",0);
//		cv::imshow("dst_qiaopi",dst_qiaopi);
//		cv::namedWindow("dst_aokeng",0);
//		cv::imshow("dst_aokeng",dst_aokeng);
//	}
//	cv::Mat dst_aokeng_and_qiaopi;
//	cv::addWeighted(dst_aokeng,0.5,dst_qiaopi,0.5,0,dst_aokeng_and_qiaopi);
//	cv::threshold(dst_aokeng_and_qiaopi,dst_aokeng_and_qiaopi,1,255,0);
//	if(true==m_bIsDebug)
//	{
//		cv::namedWindow("dst_aokeng_and_qiaopi",0);
//		cv::imshow("dst_aokeng_and_qiaopi",dst_aokeng_and_qiaopi);
//	}
//	normalize(dst_aokeng_and_qiaopi,dst_aokeng_and_qiaopi,1.0,0.0,cv::NORM_MINMAX);
//	cv::Mat integral_aokeng_and_qiaopi(dst_aokeng_and_qiaopi.rows+1,dst_aokeng_and_qiaopi.cols+1,CV_64FC1);
//	integral(dst_aokeng_and_qiaopi,integral_aokeng_and_qiaopi,CV_64FC1);//积分图像
//	int block_x3=25;
//	int block_y3=25;
//	cv::Mat dst_aoqiao=cv::Mat::zeros(dst_aokeng_and_qiaopi.rows,dst_aokeng_and_qiaopi.cols,dst_aokeng_and_qiaopi.type());
//	for(int i=0;i<integral_aokeng_and_qiaopi.rows-1;i++)
//	{
//		for(int j=0;j<integral_aokeng_and_qiaopi.cols-1;j++)
//		{
//			int begin_x3=max(0,j-block_x3/2);
//			int end_x3=min(integral_aokeng_and_qiaopi.cols-1,j+block_x3/2);
//			int begin_y3=max(0,i-block_y3/2);
//			int end_y3=min(integral_aokeng_and_qiaopi.rows-1,i+block_y3/2);
//			float A3=integral_aokeng_and_qiaopi.at<double>(end_y3,end_x3);
//			float B3=integral_aokeng_and_qiaopi.at<double>(begin_y3,begin_x3);
//			float C3=integral_aokeng_and_qiaopi.at<double>(begin_y3,end_x3);
//			float D3=integral_aokeng_and_qiaopi.at<double>(end_y3,begin_x3);
//			float temp3=A3+B3-C3-D3;
//			if(temp3>block_x3*block_y3*0.1)//0.1
//			{
//				rectangle( dst_aoqiao, cv::Point(max(0,j-2),max(0,i-2)), cv::Point(min(dst_aoqiao.cols,j+2),min(dst_aoqiao.rows,i+2)), 255, -2, 8, 0 );
//			}
//		}
//	}
//	if(true==m_bIsDebug)
//	{
//		cv::namedWindow("dst_aoqiao",0);
//		cv::imshow("dst_aoqiao",dst_aoqiao);
//	}
//	///翘皮验证
//	cv::Mat dst_zero2_med;
//	cv::medianBlur(dst_zero2,dst_zero2_med,3);
//	cv::Mat dst_sobel_qiaopi;
//	cv::Mat grad_x_qiaopi,abs_grad_x_qiaopi;
//	cv::Sobel( dst_zero2_med, grad_x_qiaopi, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
//	cv::convertScaleAbs( grad_x_qiaopi, abs_grad_x_qiaopi );
//	cv::addWeighted( abs_grad_x_qiaopi, 0.5, abs_grad_x_qiaopi, 0.5, 0, dst_sobel_qiaopi );
//	cv::threshold(dst_sobel_qiaopi,dst_sobel_qiaopi,100,255,0);
//	if(true==m_bIsDebug)
//	{
//		cv::namedWindow("dst_sobel_qiaopi",0);
//		cv::imshow("dst_sobel_qiaopi",dst_sobel_qiaopi);
//	}
//	///划伤
//	cv::Mat dst_zero2_mean_L;
//	cv::Mat dst_zero2_th_L;
//	cv::blur(dst_zero2,dst_zero2_mean_L,cv::Size(15,15));
//	cv::subtract(dst_zero2,dst_zero2_mean_L,dst_zero2_th_L);
//	cv::threshold(dst_zero2_th_L,dst_zero2_th_L,40,255,0);
//	#ifdef W_DEBUG
//		cv::namedWindow("dst_zero2_th_L",0);
//		cv::imshow("dst_zero2_th_L",dst_zero2_th_L);
//	#endif
//	cv::Mat cdst4=cv::Mat::zeros(dst_zero2_th_L.size(),CV_8UC1);
//	cv::vector<cv::Vec4i>lines4;
//	HoughLinesP(dst_zero2_th_L,lines4,1,CV_PI/180,120,150,10);
//	for(int i=0;i<lines4.size();i++)
//	{
//		cv::Vec4i l=lines4[i];
//		float width=abs(l[2]-l[0]);
//		float height=abs(l[3]- l[1]);
//		if(height/(width+0.01)>7.5 )
//		{
//			line( cdst4, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 5, CV_AA);
//		}
//	}
//	cv::Mat dst_scratch;
//	dst_scratch=cdst4.clone();
//	#ifdef W_DEBUG
//		cv::namedWindow("dst_scratch",0);
//		cv::imshow("dst_scratch",dst_scratch);
//	#endif	
//	///缺陷切割整合
//	cv::Mat dst;
//	cv::addWeighted(dst_crack,0.5,dst_aoqiao,0.5,0,dst);
//	cv::addWeighted(dst_scratch,0.5,dst,0.5,0,dst);
//	cv::threshold(dst,dst,1,255,0);
//
//	//膨胀
//	cv::Mat dst_dil;
//	int dilation_size_dst = 5;
//	cv::Mat element_dst= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_dst + 1, 2*dilation_size_dst+1 ));
//	dilate( dst, dst_dil, element_dst );
//	//画缺陷
//	cv::vector<cv::vector<cv::Point> > contours;
//	cv::vector<cv::Vec4i> hierarchy;
//	findContours( dst_dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
//	cv::vector<cv::Rect> boundRect( contours.size() );
//	cv::vector<cv::RotatedRect> minRect( contours.size() ); 
//	cv::Mat drawing = cv::Mat::zeros( dst_dil.rows,dst_dil.cols, CV_8UC1 );
//	int iScale_x=5;
//	int iScale_y=5;
//	for( int i = 0; i< contours.size(); i++ )
//	{
//		approxPolyDP( cv::Mat(contours[i]), contours[i], 3, true );
//		boundRect[i] = boundingRect( cv::Mat(contours[i]) );
//		double tmparea=fabs(contourArea(contours[i])); 
//		if(tmparea<100)
//		{
//			continue;
//		}
//		int left=max(0,boundRect[i].tl().x-iScale_x);
//		int right=min(drawing.cols-1,boundRect[i].br().x+iScale_x);
//		int top=max(0,boundRect[i].tl().y-iScale_y);
//		int bottom=min(drawing.rows-1,boundRect[i].br().y+iScale_y);
//		int xCenter=(left+right)/2;
//		int yCenter=(top+bottom)/2;
//		rectangle( drawing, cv::Point(left,top), cv::Point(right,bottom), 255, -2, 8, 0 );
//	}
//
//	//写缺陷
//	cv::vector<cv::vector<cv::Point> > contours_big;
//	cv::vector<cv::Vec4i> hierarchy_big;
//	cv::findContours( drawing, contours_big, hierarchy_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
//	cv::vector<cv::vector<cv::Point> > contours_poly_big( contours_big.size() );
//	cv::vector<cv::Rect> boundRect_big( contours_big.size() );
//	for( int k = 0; k < contours_big.size(); k++ )
//	{ 
//		approxPolyDP( cv::Mat(contours_big[k]), contours_poly_big[k], 3, true );
//		boundRect_big[k] = cv::boundingRect( cv::Mat(contours_poly_big[k]) );
//		//
//		int d_left=max(0,boundRect_big[k].tl().x);
//		int d_right=min(drawing.cols-1,boundRect_big[k].br().x);
//		int d_top=max(0,boundRect_big[k].tl().y);
//		int d_bottom=min(drawing.rows-1,boundRect_big[k].br().y);
//		int xCenter=(d_left+d_right)/2;
//		int yCenter=(d_top+d_bottom)/2;
//		
//		float width=abs(d_left-d_right);
//		float height=abs(d_top- d_bottom);
//		float area=width*height;
//		if(area<tConfigInfo_Segmentation.iAreaThreshold)//250
//		{
//			continue;
//		}
//		//
//		//去除钢板停下时竖条纹干扰
//		cv::Mat SrcImg_Cut_partbig;
//		cv::Rect rect_partbig(0,d_top,SrcImg_Cut.cols,height);
//		SrcImg_Cut(rect_partbig).copyTo(SrcImg_Cut_partbig);
//		cv::Mat grad_x, grad_y;
//		cv::Mat abs_grad_x, abs_grad_y;
//		cv::Sobel( SrcImg_Cut_partbig, grad_x, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
//		cv::convertScaleAbs( grad_x, abs_grad_x );
//		cv::Sobel( SrcImg_Cut_partbig, grad_y, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
//		cv::convertScaleAbs( grad_y, abs_grad_y );
//		cv::Mat abs_grad_x_mean,abs_grad_x_dev,abs_grad_y_mean,abs_grad_y_dev;
//		cv::meanStdDev(abs_grad_x,abs_grad_x_mean,abs_grad_x_dev);
//		cv::meanStdDev(abs_grad_y,abs_grad_y_mean,abs_grad_y_dev);
//		float f_abs_grad_x_mean=abs_grad_x_mean.at<double>(0,0);
//		float f_abs_grad_y_mean=abs_grad_y_mean.at<double>(0,0);
//		float f_e=f_abs_grad_y_mean/(f_abs_grad_x_mean+0.000001);
//		if(f_e>4)
//		{
//			continue;
//		}
//
//		//区分类别
//		cv::Mat dst_crack_part,dst_aoqiao_part,dst_scratch_part;
//		cv::Rect rect(d_left,d_top,width,height);
//		dst_crack(rect).copyTo(dst_crack_part);
//		dst_aoqiao(rect).copyTo(dst_aoqiao_part);
//		dst_scratch(rect).copyTo(dst_scratch_part);
//		int num_crack=cv::countNonZero(dst_crack_part);
//		int num_aoqiao=cv::countNonZero(dst_aoqiao_part);
//		int num_scratch=cv::countNonZero(dst_scratch_part);
//
//		//if(num_crack>=num_aoqiao && num_crack>=num_scratch)//纵裂
//		if(num_crack>0)
//		{
//			//		
//			cv::Mat dst_zero2_sub_th_part;
//			dst_zero2_sub_th(rect).copyTo(dst_zero2_sub_th_part);
//			float num_Invaild=cv::countNonZero(dst_zero2_sub_th_part);
//			float e_Invaild=num_Invaild/(dst_zero2_sub_th_part.cols*dst_zero2_sub_th_part.rows);
//			if(e_Invaild<0.005)
//			{
//				continue;
//			}
//			float f_dst_zero2_sub_th_part_left_num=0;
//			float f_dst_zero2_sub_th_part_med_num=0;
//			float f_dst_zero2_sub_th_part_right_num=0;
//			Defects_Left_Med_Right_Num(dst_zero2_sub_th_part,f_dst_zero2_sub_th_part_left_num,f_dst_zero2_sub_th_part_med_num,f_dst_zero2_sub_th_part_right_num);
//			float e_dst_zero2_sub_th_part=f_dst_zero2_sub_th_part_med_num/(f_dst_zero2_sub_th_part_left_num+f_dst_zero2_sub_th_part_right_num+0.00001);
//			if(e_dst_zero2_sub_th_part<1.25)
//			{
//				continue;
//			}
//			float E_r=(float)(f_dst_zero2_sub_th_part_left_num+f_dst_zero2_sub_th_part_right_num)/height;
//			if(E_r>0.5)
//			{
//				continue;
//			}
//
//			//
//			float fSum_x=0;
//			float fN_x=0.00001;
//			float fSum_y=0;
//			float fN_y=0.00001;
//			for(int i=0;i<dst_zero2_sub_th_part.cols;i++)
//			{
//				for(int j=0;j<dst_zero2_sub_th_part.rows;j++)
//				{
//					if(dst_zero2_sub_th_part.at<uchar>(j,i)>0)
//					{
//						fSum_x+=i;
//						fN_x++;
//						fSum_y+=j;
//						fN_y++;
//					}
//				}
//			}
//			float fAvr_x=fSum_x/fN_x;
//			float fAvr_y=fSum_y/fN_y;
//			float fSum_xy=0;
//			float fSum_xx=0;
//			float fSum_yy=0;
//			for(int i=0;i<dst_zero2_sub_th_part.cols;i++)
//			{
//				for(int j=0;j<dst_zero2_sub_th_part.rows;j++)
//				{
//					if(dst_zero2_sub_th_part.at<uchar>(j,i)>0)
//					{
//						fSum_xy+=((i-fAvr_x)*(j-fAvr_y));
//						fSum_xx+=((i-fAvr_x)*(i-fAvr_x));
//						fSum_yy+=((j-fAvr_y)*(j-fAvr_y));
//					}
//				}
//			}
//			float S=sqrt(fSum_xx)/(fN_x-1);
//			float fCorr=abs(fSum_xy/sqrt(fSum_xx*fSum_yy));
//			if((S>0.5))// 此处会滤掉一些比较严重的缺陷 fCorr<0.2 || 
//			{
//				continue;
//			}
//			//
//			if(height<80)
//			{
//				continue;
//			}
//			if(abs(d_left-0)<5 || abs(d_right-SrcImg_Cut.cols)<5)
//			{
//				continue;
//			}
//			//去边部的伪缺陷
//			cv::Mat SrcImg_Cut_part;		
//			SrcImg_Cut(rect).copyTo(SrcImg_Cut_part);
//			float f_SrcImg_Cut_part_left_mean=0;
//			float f_SrcImg_Cut_part_med_mean=0;
//			float f_SrcImg_Cut_part_right_mean=0;
//			Defects_Left_Med_Right(SrcImg_Cut_part,f_SrcImg_Cut_part_left_mean,f_SrcImg_Cut_part_med_mean,f_SrcImg_Cut_part_right_mean);
//			float e_SrcImg_Cut_part=(f_SrcImg_Cut_part_left_mean>f_SrcImg_Cut_part_right_mean) ? f_SrcImg_Cut_part_right_mean/(f_SrcImg_Cut_part_left_mean+0.00001) : f_SrcImg_Cut_part_left_mean/(f_SrcImg_Cut_part_right_mean+0.00001);
//			if(e_SrcImg_Cut_part<0.5)
//			{
//				continue;
//			}
//			//在阈值下检验点是否达到要求
//			int Y_num=0;
//			for(int i=0;i<dst_zero2_sub_th_part.rows;i++)
//			{
//				for(int j=0;j<dst_zero2_sub_th_part.cols;j++)
//				{
//					if(dst_zero2_sub_th_part.at<uchar>(i,j)>0)
//					{
//						Y_num++;
//						break;
//					}
//				}
//			}
//			float E_rido=(float)Y_num/height;
//			//表面相关
//			if(tConfigInfo_Segmentation.tSteelInfo.iCamera<4)
//			{
//				if(E_rido<0.3)
//				{
//					continue;
//				}
//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=1;
//			}
//			else
//			{
//				if(E_rido<0.5)
//				{
//					continue;
//				}
//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=5;
//			}
//		}
//		else if(num_aoqiao>=num_crack && num_aoqiao>=num_scratch)
//		{
//			//continue;
//			cv::Mat SrcImg_Cut_Canny_Dil_part;
//			SrcImg_Cut_Canny_Dil(rect).copyTo(SrcImg_Cut_Canny_Dil_part);
//			float num_Invaild=cv::countNonZero(SrcImg_Cut_Canny_Dil_part);
//			float e_Invaild=num_Invaild/(SrcImg_Cut_Canny_Dil_part.cols*SrcImg_Cut_Canny_Dil_part.rows);
//			if(e_Invaild<0.5)
//			{
//				continue;
//			}
//	
//			//2、上下边
//			cv::Mat SrcImg_Cut_part_top,SrcImg_Cut_part_bottom;
//			cv::Rect rect_top(d_left,d_top,width,height*0.25);
//			cv::Rect rect_bottom(d_left,d_top+height*0.75,width,height*0.25);
//			SrcImg_Cut(rect_top).copyTo(SrcImg_Cut_part_top);
//			SrcImg_Cut(rect_bottom).copyTo(SrcImg_Cut_part_bottom);
//			cv::Mat SrcImg_Cut_part_top_mean,SrcImg_Cut_part_top_dev;
//			cv::Mat SrcImg_Cut_part_bottom_mean,SrcImg_Cut_part_bottom_dev;
//			cv::meanStdDev(SrcImg_Cut_part_top,SrcImg_Cut_part_top_mean,SrcImg_Cut_part_top_dev);
//			cv::meanStdDev(SrcImg_Cut_part_bottom,SrcImg_Cut_part_bottom_mean,SrcImg_Cut_part_bottom_dev);
//			float f_SrcImg_Cut_part_top_mean=SrcImg_Cut_part_top_mean.at<double>(0,0);
//			float f_SrcImg_Cut_part_bottom_mean=SrcImg_Cut_part_bottom_mean.at<double>(0,0);
//			float f_h_e=(f_SrcImg_Cut_part_top_mean>f_SrcImg_Cut_part_bottom_mean)?(f_SrcImg_Cut_part_bottom_mean/(f_SrcImg_Cut_part_top_mean+0.000001)):(f_SrcImg_Cut_part_top_mean/(f_SrcImg_Cut_part_bottom_mean+0.00001));
//			if(f_h_e<0.5)
//			{
//				continue;
//			}
//
//			if(abs(d_left-0)<5|| abs(d_right-SrcImg_Cut.cols)<5)
//			{
//				continue;
//			}
//			if(abs(d_top-0)<5|| abs(d_bottom-SrcImg_Cut.rows)<5)
//			{
//				continue;
//			}
//			else
//			{
//				if(abs(d_top-0)>25 && abs(d_bottom-SrcImg_Cut.rows)>25)
//				{
//					int offset=25;
//					cv::Rect rect_top(d_left,d_top-offset,width,offset);
//					cv::Mat SrcImg_Cut_Canny_Dil_part_top;
//					SrcImg_Cut_Canny_Dil(rect_top).copyTo(SrcImg_Cut_Canny_Dil_part_top);
//					cv::Rect rect_bottom(d_left,d_bottom,width,offset);
//					cv::Mat SrcImg_Cut_Canny_Dil_part_bottom;
//					SrcImg_Cut_Canny_Dil(rect_bottom).copyTo(SrcImg_Cut_Canny_Dil_part_bottom);
//					float num_top=cv::countNonZero(SrcImg_Cut_Canny_Dil_part_top);
//					float num_bottom=cv::countNonZero(SrcImg_Cut_Canny_Dil_part_bottom);
//					float f_e_tb=(num_top<num_bottom)?(num_top/(num_bottom+0.00001)):(num_bottom/(num_top+0.00001));
//					if(f_e_tb<0.2)
//					{
//						continue;
//					}
//				}
//				else
//				{
//					int offset=5;
//					cv::Rect rect_top(d_left,d_top-offset,width,offset);
//					cv::Mat SrcImg_Cut_Canny_Dil_part_top;
//					SrcImg_Cut_Canny_Dil(rect_top).copyTo(SrcImg_Cut_Canny_Dil_part_top);
//					cv::Rect rect_bottom(d_left,d_bottom,width,offset);
//					cv::Mat SrcImg_Cut_Canny_Dil_part_bottom;
//					SrcImg_Cut_Canny_Dil(rect_bottom).copyTo(SrcImg_Cut_Canny_Dil_part_bottom);
//					float num_top=cv::countNonZero(SrcImg_Cut_Canny_Dil_part_top);
//					float num_bottom=cv::countNonZero(SrcImg_Cut_Canny_Dil_part_bottom);
//					float f_e_tb=(num_top<num_bottom)?(num_top/(num_bottom+0.00001)):(num_bottom/(num_top+0.00001));
//					if(f_e_tb<0.3)
//					{
//						continue;
//					}
//				}
//			}
//			//
//
//			cv::Mat dst_aokeng_and_qiaopi_partin;
//			cv::Rect rect_in(rect.x+0.25*rect.width,rect.y+0.25*rect.height,0.5*rect.width,0.5*rect.height);
//			dst_aokeng_and_qiaopi(rect_in).copyTo(dst_aokeng_and_qiaopi_partin);
//			float S_in=cv::countNonZero(dst_aokeng_and_qiaopi_partin);
//			float f_S=S_in/(dst_aokeng_and_qiaopi_partin.rows*dst_aokeng_and_qiaopi_partin.cols);
//			if(f_S>0.15)
//			{
//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=11;
//			}
//			else
//			{
//				cv::Mat dst_sobel_qiaopi_part;
//				dst_sobel_qiaopi(rect).copyTo(dst_sobel_qiaopi_part);
//				float n=cv::countNonZero(dst_sobel_qiaopi_part);
//				n=n/(dst_sobel_qiaopi_part.rows*dst_sobel_qiaopi_part.cols);
//				//if(n<0.05)
//				//{
//				//	continue;
//				//}
//				pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=30;
//			}
//		}
//		else if(num_scratch>=num_crack && num_scratch>=num_aoqiao)//划伤
//		{
//			//continue;
//			cv::Mat SrcImg_Cut_part;
//			cv::Rect rect_part(d_left,d_top,width,height);
//			SrcImg_Cut(rect_part).copyTo(SrcImg_Cut_part);
//			float f_SrcImg_Cut_part_left_mean=0;
//			float f_SrcImg_Cut_part_med_mean=0;
//			float f_SrcImg_Cut_part_right_mean=0;
//			Defects_Left_Med_Right(SrcImg_Cut_part,f_SrcImg_Cut_part_left_mean,f_SrcImg_Cut_part_med_mean,f_SrcImg_Cut_part_right_mean);
//			float e=(f_SrcImg_Cut_part_left_mean>f_SrcImg_Cut_part_right_mean) ? f_SrcImg_Cut_part_right_mean/(f_SrcImg_Cut_part_left_mean+0.00001) : f_SrcImg_Cut_part_left_mean/(f_SrcImg_Cut_part_right_mean+0.00001);
//			if(e<0.5)
//			{
//				continue;
//			}
//			if(abs(d_left-0)<5 || abs(d_right-SrcImg_Cut.cols)<5)
//			{
//				continue;
//			}
//			if((d_top+d_bottom)/2 > Bottom_Head || (d_top+d_bottom)/2 < Top_Head)
//			{
//				continue;
//			}
//			pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=5;
//		}	
//		//对所有缺陷进行sobel模板删除
//		cv::Rect rect_mark(d_left,d_top,d_right-d_left,d_bottom-d_top);
//		cv::Mat MatMarkCut;
//		m_MatMark(rect_mark).copyTo(MatMarkCut);
//		cv::Mat MatMarkCut_Mean,MatMarkCut_Dev;
//		cv::meanStdDev(MatMarkCut,MatMarkCut_Mean,MatMarkCut_Dev);
//		float f_MatMarkCut_Mean=MatMarkCut_Mean.at<double>(0,0);
//		//
//		pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
//		pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(d_left+Left_Edge)*2;
//		pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=(d_top)*2;
//		pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(d_right+Left_Edge)*2;
//		pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=(d_bottom)*2;
//		pH_DefectSet->iItemNum++;
//	}
//}

void H_KHB_SanMingGangTie_1X_ImageProcess::Find_Edge_OpenCV_ForCommon(cv::Mat src,int srcImgWidth,int srcImgHeight,int &Left_Edge,int &Right_Edge,int EdgeThreshold,int offSet)
{
	int E_scale=m_imgSrcWidth/src.cols;

	float *fAverGray=new float[src.cols];
	cv::Mat aver=cv::Mat::zeros(1,src.cols, CV_8UC1);
	aver.row(0)=src.row(src.rows*1/32)+src.row(src.rows*16/32)+src.row(src.rows*31/32);
	for(int i=0;i<src.cols;i++)
	{
		fAverGray[i]=aver.at<uchar>(0,i)/3;
	}

	int width=src.cols;
	int height=src.rows;

	int Num_Max=width/2;
	int *Set_Edge=new int[Num_Max];
	memset(Set_Edge, -1, Num_Max*sizeof(int));
	bool T_B_Edge=false;
	for(int i=0,j=0;i<width;i++)
	{
		if(fAverGray[i]>0 && (!T_B_Edge))
		{
			Set_Edge[j++]=i;
			T_B_Edge=true;
		}
		if(fAverGray[i]==0 && T_B_Edge)
		{
			Set_Edge[j++]=i;
			T_B_Edge=false;
		}
		if(i==width-1 && j%2==1)
		{
			Set_Edge[j]=width-1;
		}
	}
	int Max_Edge=0;
	int left_edge=width-1;
	int right_edge=0;
	for(int j=0;j<Num_Max;j+=2)
	{
		if(j!=-1)
		{
			if(abs(Set_Edge[j]-Set_Edge[j+1])>Max_Edge)
			{
				Max_Edge=abs(Set_Edge[j]-Set_Edge[j+1]);
				left_edge=Set_Edge[j];
				right_edge=Set_Edge[j+1];
			}
		}
		else
		{
			break;
		}
	}
	delete[] Set_Edge;
	delete[] fAverGray;

	Right_Edge=right_edge*E_scale-offSet;
	Left_Edge=left_edge*E_scale+offSet;
}

cv::Mat H_KHB_SanMingGangTie_1X_ImageProcess::Find_Head_OpenCV_ForCommon(cv::Mat src,int srcImgWidth,int srcImgHeight,int &Top_Head,int &Bottom_Head,int HeadThreshold,int offSet)
{
	int E_scale=srcImgHeight/src.rows;

	cv::Mat dstimg11,dstimg22,dstimg33;
	cv::Mat grad_x1, grad_y1;
	cv::Mat abs_grad_x1, abs_grad_y1;
	// 求 X方向梯度
	cv::Sobel( src, grad_x1, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x1, abs_grad_x1 );
	// 求Y方向梯度
	cv::Sobel( src, grad_y1, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_y1, abs_grad_y1 );
	// 合并梯度(近似)
	//cv::addWeighted( abs_grad_x1, 0.5, abs_grad_y1, 0.5, 0, dstimg11 );
	cv::addWeighted( abs_grad_x1, 0.5, abs_grad_x1, 0.5, 0, dstimg22 );
	cv::addWeighted( abs_grad_y1, 0.5, abs_grad_y1, 0.5, 0, dstimg33 );
	//cv::namedWindow("dstimg11",0);
	//cv::imshow("dstimg11",dstimg11);
	if(m_bIsDebug)
	{
		cv::namedWindow("dstimg22",0);
		cv::imshow("dstimg22",dstimg22);
		cv::namedWindow("dstimg33",0);
		cv::imshow("dstimg33",dstimg33);	
	}
	return dstimg22;
	//cv::Mat dstimg22_th;
	//cv::threshold(dstimg22,dstimg22_th,5,255,0);
	//if(m_bIsDebug)
	//{
	//	cv::namedWindow("dstimg22_th",0);
	//	cv::imshow("dstimg22_th",dstimg22_th);
	//}
}

void H_KHB_SanMingGangTie_1X_ImageProcess::Defects_Left_Med_Right(cv::Mat src,float& Left,float& Med,float& Right)
{
	cv::Mat part_left,part_med,part_right;
	cv::Rect rect_left(0,0,src.cols*0.25,src.rows);
	cv::Rect rect_med(0+src.cols*0.25,0,src.cols*0.5,src.rows);
	cv::Rect rect_right(0+src.cols*0.75,0,src.cols*0.25,src.rows);
	src(rect_left).copyTo(part_left);
	src(rect_med).copyTo(part_med);
	src(rect_right).copyTo(part_right);

	cv::Mat part_left_mean,part_left_dev;
	cv::Mat part_med_mean,part_med_dev;
	cv::Mat part_right_mean,part_right_dev;
	cv::meanStdDev(part_left,part_left_mean,part_left_dev);
	cv::meanStdDev(part_med,part_med_mean,part_med_dev);
	cv::meanStdDev(part_right,part_right_mean,part_right_dev);
	float f_part_left_mean=part_left_mean.at<double>(0,0);
	float f_part_med_mean=part_med_mean.at<double>(0,0);
	float f_part_right_mean=part_right_mean.at<double>(0,0);
	Left=f_part_left_mean;
	Med=f_part_med_mean;
	Right=f_part_right_mean;
}

void H_KHB_SanMingGangTie_1X_ImageProcess::Defects_Left_Med_Right_Num(cv::Mat src,float& Left,float& Med,float& Right)
{
	cv::Mat part_left,part_med,part_right;
	cv::Rect rect_left(0,0,src.cols*0.25,src.rows);
	cv::Rect rect_med(0+src.cols*0.25,0,src.cols*0.5,src.rows);
	cv::Rect rect_right(0+src.cols*0.75,0,src.cols*0.25,src.rows);
	src(rect_left).copyTo(part_left);
	src(rect_med).copyTo(part_med);
	src(rect_right).copyTo(part_right);

	float f_part_left=cv::countNonZero(part_left);
	float f_part_med=cv::countNonZero(part_med);
	float f_part_right=cv::countNonZero(part_right);
	Left=f_part_left;
	Med=f_part_med;
	Right=f_part_right;
}