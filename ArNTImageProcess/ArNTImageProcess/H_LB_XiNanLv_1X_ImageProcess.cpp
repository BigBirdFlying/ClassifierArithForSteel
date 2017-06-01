#include "H_LB_XiNanLv_1X_ImageProcess.h"
#include "H_BaseImageProcess.h"
#include "H_SteelImageProcess.h"

H_LB_XiNanLv_1X_ImageProcess::H_LB_XiNanLv_1X_ImageProcess(void)
{
	//m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);
	//m_DefectListIndex=0;
	//memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
	//memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
}

H_LB_XiNanLv_1X_ImageProcess::~H_LB_XiNanLv_1X_ImageProcess(void)
{
}

int H_LB_XiNanLv_1X_ImageProcess::K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug)
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
	H_BaseImageProcess::Img_Gray_Change_Y(cvSrcImg_P2D,cvSrcImg_P2D_GCG,100,5);	
	//
	cv::Mat cvSrcImg_P4D,cvSrcImg_P4D_GCG;
	cv::pyrDown(cvSrcImg_P2D,cvSrcImg_P4D,cv::Size(cvSrcImg_P2D.cols/2,cvSrcImg_P2D.rows/2));
	cv::pyrDown(cvSrcImg_P2D_GCG,cvSrcImg_P4D_GCG,cv::Size(cvSrcImg_P2D_GCG.cols/2,cvSrcImg_P2D_GCG.rows/2));

	//边界判断
	int Left_Edge=m_imgSrcWidth-1;
	int Right_Edge=0;
		
	//if(tConfigInfo_Segmentation.tSteelInfo.iCamera==1 || tConfigInfo_Segmentation.tSteelInfo.iCamera==4)
	//{
	//	Find_Edge_OpenCV_ForSmooth(cvSrcImg_P4D,m_imgSrcWidth,m_imgSrcHeight,Left_Edge,Right_Edge,tConfigInfo_Segmentation.iEdgeThreshold,tConfigInfo_Segmentation.iEdgeNoDetect);
	//	if(tConfigInfo_Segmentation.tSteelInfo.iCamera==1)
	//	{
	//		if(Right_Edge<m_imgSrcWidth*0.9)
	//		{
	//			Left_Edge=Right_Edge;
	//			Right_Edge=m_imgSrcWidth-1;
	//		}
	//	}
	//	if(tConfigInfo_Segmentation.tSteelInfo.iCamera==4)
	//	{
	//		if(Left_Edge>m_imgSrcWidth*0.1)
	//		{
	//			Right_Edge=Left_Edge;
	//			Left_Edge=0;
	//		}
	//	}
	//}
	//else
	{
		Find_Edge_OpenCV_ForRough(cvSrcImg_P4D_GCG,m_imgSrcWidth,m_imgSrcHeight,Left_Edge,Right_Edge,tConfigInfo_Segmentation.iEdgeThreshold,tConfigInfo_Segmentation.iEdgeNoDetect);	
	}
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
		Defects_ColdSteel_OpenCV(cvSrcImg_P2D,cvSrcImg_P2D_GCG,Left_Edge/2,Right_Edge/2,2,tConfigInfo_Segmentation,pH_DefectSet);			
	}
	//Defects_Period_OpenCV(pH_DefectSet,0.65,100,22,tConfigInfo_Segmentation.tSteelInfo.iImgIndex);
	return pH_DefectSet->iItemNum;
}

void H_LB_XiNanLv_1X_ImageProcess::Defects_ColdSteel_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet) 
{
	PH_DefectSet pSet_Class_All=new H_DefectSet;
	pSet_Class_All->iItemNum=0;
	PH_DefectSet pSet_Class_All_S=new H_DefectSet;
	pSet_Class_All_S->iItemNum=0;

	cv::Mat SrcImg_GCG_Cut;
	cv::Rect rect(Left_Edge,0,Right_Edge-Left_Edge,SrcImg_GCG.rows);
	SrcImg_GCG(rect).copyTo(SrcImg_GCG_Cut);

	cv::Mat mean1,mean2;
	cv::Mat dst1=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	cv::Mat dst1_1=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	cv::blur(SrcImg_GCG_Cut,mean1,cv::Size(15,15));
	cv::Mat dst2=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	cv::Mat dst2_2=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	cv::blur(SrcImg_GCG_Cut,mean2,cv::Size(25,25));
	cv::subtract(SrcImg_GCG_Cut, mean1, dst1);
	cv::subtract(SrcImg_GCG_Cut, mean2, dst2);
	cv::subtract(mean1, SrcImg_GCG_Cut, dst1_1);
	cv::subtract(mean2, SrcImg_GCG_Cut, dst2_2);
	cv::threshold(dst1,dst1,tConfigInfo_Segmentation.iSegmentationThreshold,255,0);
	cv::threshold(dst2,dst2,tConfigInfo_Segmentation.iSegmentationThreshold,255,0);
	cv::threshold(dst1_1,dst1_1,tConfigInfo_Segmentation.iSegmentationThreshold,255,0);
	cv::threshold(dst2_2,dst2_2,tConfigInfo_Segmentation.iSegmentationThreshold,255,0);
	cv::Mat dst3,dst3_3,dst;
	cv::addWeighted( dst1, 0.5, dst2, 0.5, 0, dst3 );
	cv::addWeighted( dst1_1, 0.5, dst2_2, 0.5, 0, dst3_3 );
	cv::addWeighted( dst3, 0.5, dst3_3, 0.5, 0, dst );

	cv::threshold(dst,dst,10,255,0);
	cv::medianBlur(dst,dst,tConfigInfo_Segmentation.iFilterThreshold*2+1);
	if(true==m_bIsDebug)
	{
		cv::namedWindow("dst",0);
		imshow("dst",dst);
	}
	///对缺陷区域进行腐蚀
	cv::Mat SrcImg_GCG_Cut_Dil;
	int dilation_size = 10;
	cv::Mat element = getStructuringElement( cv::MORPH_CROSS,cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),cv::Point( dilation_size, dilation_size ) );
	dilate( dst, SrcImg_GCG_Cut_Dil, element );

	cv::vector<cv::vector<cv::Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;
	cv::findContours( SrcImg_GCG_Cut_Dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	cv::vector<cv::vector<cv::Point> > contours_poly( contours.size() );
	cv::vector<cv::Rect> boundRect( contours.size() );
	cv::Mat drawing = cv::Mat::zeros( SrcImg_GCG_Cut_Dil.size(), CV_8UC1 );
	for( int i = 0; i < contours.size(); i++ )
	{ 
		cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
		boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), 255, -2, 8, 0 );
	}

	cv::vector<cv::vector<cv::Point> > contours_big;
	cv::vector<cv::Vec4i> hierarchy_big;
	cv::findContours( drawing, contours_big, hierarchy_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	cv::vector<cv::vector<cv::Point> > contours_poly_big( contours_big.size() );
	cv::vector<cv::Rect> boundRect_big( contours_big.size() );
	double area_sum=0;
	for( int i = 0; i < contours_big.size(); i++ )
	{ 
		approxPolyDP( cv::Mat(contours_big[i]), contours_poly_big[i], 3, true );
		boundRect_big[i] = cv::boundingRect( cv::Mat(contours_poly_big[i]) );
		float width=abs(boundRect_big[i].tl().x-boundRect_big[i].br().x);
		float height=abs(boundRect_big[i].tl().y- boundRect_big[i].br().y);
		if(abs(boundRect_big[i].tl().x-boundRect_big[i].br().x)<2 || abs(boundRect_big[i].tl().y- boundRect_big[i].br().y)<2)
		{
			continue;
		}
		float area=width*height;
		if(area<tConfigInfo_Segmentation.iAreaThreshold)//650
		{
			continue;
		}
		area_sum+=(width*height);
		///
		pSet_Class_All->Items[pSet_Class_All->iItemNum].fConfidence=0.5;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass=0;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iLeftInImage=(boundRect_big[i].tl().x+Left_Edge)*iZoom;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iTopInImage=boundRect_big[i].tl().y*iZoom;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iRightInImage=(boundRect_big[i].br().x+Left_Edge)*iZoom;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iBottomInImage=boundRect_big[i].br().y*iZoom;
		pSet_Class_All->iItemNum++;
	}

	///缺陷分类
	if(area_sum>((SrcImg_GCG_Cut.rows*SrcImg_GCG_Cut.cols)*0.25) || pSet_Class_All->iItemNum>tConfigInfo_Segmentation.iDefectMaxNum)//分类6：表面粗糙
	{
		pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].fConfidence=0.5;
		pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iDefectClass=29;
		pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iLeftInImage=Left_Edge*iZoom;
		pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iTopInImage=0;
		pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iRightInImage=Right_Edge*iZoom;
		pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iBottomInImage=m_imgSrcHeight-1;
		pSet_Class_All_S->iItemNum++;
	}
	else
	{
		//合并纵向缺陷
		bool IsOver=true;
		int Split_Range=100;//合并的最大距离
		int Width_Th=300;//待合并块的最大宽度
		while(true==IsOver)
		{
			IsOver=false;
			int ItemNum=pSet_Class_All->iItemNum;
			for(int i=0;i<pSet_Class_All->iItemNum;i++)
			{
				for(int j=i+1;j<pSet_Class_All->iItemNum;j++)
				{
					int Center_i_x=(pSet_Class_All->Items[i].iRightInImage+pSet_Class_All->Items[i].iLeftInImage)/2;
					int Center_j_x=(pSet_Class_All->Items[j].iRightInImage+pSet_Class_All->Items[j].iLeftInImage)/2;
					int Center_i_y=(pSet_Class_All->Items[i].iBottomInImage+pSet_Class_All->Items[i].iTopInImage)/2;
					int Center_j_y=(pSet_Class_All->Items[j].iBottomInImage+pSet_Class_All->Items[j].iTopInImage)/2;
					int iInterval=10000;
					if(Center_i_y>Center_j_y)
					{
						iInterval=max(pSet_Class_All->Items[i].iTopInImage-pSet_Class_All->Items[j].iBottomInImage,0);
					}
					else
					{
						iInterval=max(pSet_Class_All->Items[j].iTopInImage-pSet_Class_All->Items[i].iBottomInImage,0);
					}
					int Conf_i=pSet_Class_All->Items[i].fConfidence;
					int Conf_j=pSet_Class_All->Items[j].fConfidence;
					int Width_i=pSet_Class_All->Items[i].iRightInImage-pSet_Class_All->Items[i].iLeftInImage;
					int Width_j=pSet_Class_All->Items[j].iRightInImage-pSet_Class_All->Items[j].iLeftInImage;
					if(abs(Center_j_x-Center_i_x)<Split_Range && iInterval<Split_Range*1.5 && -1!=Conf_i && -1!=Conf_j && Width_i<Width_Th && Width_j<Width_Th)
					{
						int left=min(pSet_Class_All->Items[i].iLeftInImage,pSet_Class_All->Items[j].iLeftInImage);
						int right=max(pSet_Class_All->Items[i].iRightInImage,pSet_Class_All->Items[j].iRightInImage);
						int top=min(pSet_Class_All->Items[i].iTopInImage,pSet_Class_All->Items[j].iTopInImage);
						int bottom=max(pSet_Class_All->Items[i].iBottomInImage,pSet_Class_All->Items[j].iBottomInImage);
						pSet_Class_All->Items[pSet_Class_All->iItemNum].fConfidence=0.5;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass=2;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iLeftInImage=left;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iTopInImage=top;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iRightInImage=right;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iBottomInImage=bottom;
						pSet_Class_All->iItemNum++;
						//
						pSet_Class_All->Items[i].fConfidence=-1;
						pSet_Class_All->Items[j].fConfidence=-1;
						IsOver=false;
					}
				}
			}
		}
		//合并横向缺陷
		IsOver=true;
		Split_Range=100;
		int Height_Th=300;
		while(true==IsOver)
		{
			IsOver=false;
			for(int i=0;i<pSet_Class_All->iItemNum;i++)
			{
				for(int j=i+1;j<pSet_Class_All->iItemNum;j++)
				{
					int Center_i_x=(pSet_Class_All->Items[i].iRightInImage+pSet_Class_All->Items[i].iLeftInImage)/2;
					int Center_j_x=(pSet_Class_All->Items[j].iRightInImage+pSet_Class_All->Items[j].iLeftInImage)/2;
					int Center_i_y=(pSet_Class_All->Items[i].iBottomInImage+pSet_Class_All->Items[i].iTopInImage)/2;
					int Center_j_y=(pSet_Class_All->Items[j].iBottomInImage+pSet_Class_All->Items[j].iTopInImage)/2;
					int iInterval=10000;
					if(Center_i_x>Center_j_x)
					{
						iInterval=max(pSet_Class_All->Items[i].iLeftInImage-pSet_Class_All->Items[j].iRightInImage,0);
					}
					else
					{
						iInterval=max(pSet_Class_All->Items[j].iLeftInImage-pSet_Class_All->Items[i].iRightInImage,0);
					}
					int Conf_i=pSet_Class_All->Items[i].fConfidence;
					int Conf_j=pSet_Class_All->Items[j].fConfidence;
					int Height_i=pSet_Class_All->Items[i].iBottomInImage-pSet_Class_All->Items[i].iTopInImage;
					int Height_j=pSet_Class_All->Items[j].iBottomInImage-pSet_Class_All->Items[j].iTopInImage;
					if(abs(Center_j_y-Center_i_y)<Split_Range && iInterval<Split_Range*1.5 && -2!=Conf_i&&-1!=Conf_i && -2!=Conf_j&&-1!=Conf_j && Height_i<Height_Th && Height_j<Height_Th)
					{
						int left=min(pSet_Class_All->Items[i].iLeftInImage,pSet_Class_All->Items[j].iLeftInImage);
						int right=max(pSet_Class_All->Items[i].iRightInImage,pSet_Class_All->Items[j].iRightInImage);
						int top=min(pSet_Class_All->Items[i].iTopInImage,pSet_Class_All->Items[j].iTopInImage);
						int bottom=max(pSet_Class_All->Items[i].iBottomInImage,pSet_Class_All->Items[j].iBottomInImage);
						pSet_Class_All->Items[pSet_Class_All->iItemNum].fConfidence=0.5;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass=0;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iLeftInImage=left;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iTopInImage=top;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iRightInImage=right;
						pSet_Class_All->Items[pSet_Class_All->iItemNum].iBottomInImage=bottom;
						pSet_Class_All->iItemNum++;
						//
						pSet_Class_All->Items[i].fConfidence=-2;
						pSet_Class_All->Items[j].fConfidence=-2;
						IsOver=false;
					}
				}
			}
		}
		//所有合并后的缺陷复制给pSet_Class_All_S
		for(int i=0;i<pSet_Class_All->iItemNum;i++)
		{
			if(pSet_Class_All->Items[i].fConfidence!=-1 && pSet_Class_All->Items[i].fConfidence!=-2)
			{
				//合并的纵向缺陷块类别设为脱皮
				if(2==pSet_Class_All->Items[i].iDefectClass)//类别2：脱皮
				{				
					if(pSet_Class_All->Items[i].iBottomInImage-pSet_Class_All->Items[i].iTopInImage>m_imgSrcHeight/2)
					{
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].fConfidence=0.5;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iDefectClass=28;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iLeftInImage=pSet_Class_All->Items[i].iLeftInImage;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iTopInImage=pSet_Class_All->Items[i].iTopInImage;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iRightInImage=pSet_Class_All->Items[i].iRightInImage;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iBottomInImage=pSet_Class_All->Items[i].iBottomInImage;
						pSet_Class_All_S->iItemNum++;
					}
					else
					{
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].fConfidence=0.5;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iDefectClass=0;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iLeftInImage=pSet_Class_All->Items[i].iLeftInImage;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iTopInImage=pSet_Class_All->Items[i].iTopInImage;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iRightInImage=pSet_Class_All->Items[i].iRightInImage;
						pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iBottomInImage=pSet_Class_All->Items[i].iBottomInImage;
						pSet_Class_All_S->iItemNum++;
					}
				}
				else
				{
					pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].fConfidence=0.5;
					pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iDefectClass=pSet_Class_All->Items[i].iDefectClass;
					pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iLeftInImage=pSet_Class_All->Items[i].iLeftInImage;
					pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iTopInImage=pSet_Class_All->Items[i].iTopInImage;
					pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iRightInImage=pSet_Class_All->Items[i].iRightInImage;
					pSet_Class_All_S->Items[pSet_Class_All_S->iItemNum].iBottomInImage=pSet_Class_All->Items[i].iBottomInImage;
					pSet_Class_All_S->iItemNum++;
				}		
			}		
		}
	}

	//
	for(int n=0;n<pSet_Class_All_S->iItemNum;n++)
	{
		pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=pSet_Class_All_S->Items[n].fConfidence;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=pSet_Class_All_S->Items[n].iDefectClass;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=pSet_Class_All_S->Items[n].iLeftInImage;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=pSet_Class_All_S->Items[n].iTopInImage;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=pSet_Class_All_S->Items[n].iRightInImage;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=pSet_Class_All_S->Items[n].iBottomInImage;
		pH_DefectSet->iItemNum++;
	}

	if(pSet_Class_All!= NULL)
	{
		delete pSet_Class_All;
	}
	if(pSet_Class_All_S!= NULL)
	{
		delete pSet_Class_All_S;
	}
}

void H_LB_XiNanLv_1X_ImageProcess::Find_Edge_OpenCV_ForRough(cv::Mat src,int srcImgWidth,int srcImgHeight,int &Left_Edge,int &Right_Edge,int EdgeThreshold,int offSet)
{
	int E_scale=m_imgSrcWidth/src.cols;
	
	int ddepth1=CV_16S;
	int delta1=0;
	int scale1=1;
	cv::Mat dstimg11,dstimg22,dstimg33;
	cv::Mat grad_x1, grad_y1;
	cv::Mat abs_grad_x1, abs_grad_y1;
	// 求 X方向梯度
	cv::Sobel( src, grad_x1, ddepth1, 0, 1, 3, scale1, delta1, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x1, abs_grad_x1 );
	// 求Y方向梯度
	cv::Sobel( src, grad_y1, ddepth1, 1, 0, 3, scale1, delta1, cv::BORDER_DEFAULT );
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
	
	cv::Mat UseImage=dstimg22.clone();
	int *Avr_Row=new int[UseImage.cols];
	float Avr_Row_Max=0;
	float Avr_Row_Min=65535;
	for(int i=0;i<UseImage.cols;i++)
	{
		int Tatal_Col=0;
		for(int j=0;j<UseImage.rows;j++)
		{
			Tatal_Col+=UseImage.at<uchar>(j,i);
		}
		Avr_Row[i]=Tatal_Col/UseImage.rows;
		if(Avr_Row[i]>Avr_Row_Max)
		{
			Avr_Row_Max=Avr_Row[i];
		}
		if(Avr_Row[i]<Avr_Row_Min)
		{
			Avr_Row_Min=Avr_Row[i];
		}
	}
	//直方图的宽度
	if(m_bIsDebug)
	{
		int height_row=256;    
		int scale_row = 1;     
		int bins_row=UseImage.cols;
		cv::Mat img_row = cv::Mat::zeros(height_row,bins_row*scale_row, CV_8UC3);
		for(int i=0;i<bins_row;i++)    
		{      
			cv::rectangle(img_row,cv::Point(i*scale_row,img_row.rows-1-Avr_Row[i]), cv::Point((i+1)*scale_row,img_row.cols-1),CV_RGB(255,255,0));    
		} 
		cv::namedWindow("img_row",0);
		cv::imshow("img_row",img_row);
	}

	int Left_Edge_4=UseImage.cols-1;
	int Right_Edge_4=0;
	int Num_Max=UseImage.cols/2;
	int *Set_Edge=new int[Num_Max];
	memset(Set_Edge, -1, Num_Max*sizeof(int));
	bool T_B_Edge=false;
	int th=Avr_Row_Min+EdgeThreshold;//55,EdgeThreshold=25
	//
	//Add_To_File("test.txt",Avr_Row_Max);
	//
	if(Avr_Row_Max<20)
	{
		Left_Edge_4=0;
		Right_Edge_4=UseImage.cols-1;
	}
	else
	{
		for(int i=0,j=0;i<UseImage.cols;i++)
		{
			if(Avr_Row[i]<th && (!T_B_Edge))
			{
				Set_Edge[j++]=i;
				T_B_Edge=true;
			}
			if(Avr_Row[i]>=th && T_B_Edge)
			{
				Set_Edge[j++]=i;
				T_B_Edge=false;
			}
			if(i==UseImage.cols-1 && j%2==1)
			{
				Set_Edge[j]=UseImage.cols-1;
			}
		}
		int Max_Edge=0;
		Left_Edge_4=UseImage.cols-1;
		Right_Edge_4=0;
		for(int j=0;j<Num_Max;j+=2)
		{
			if(j!=-1)
			{
				if(abs(Set_Edge[j]-Set_Edge[j+1])>Max_Edge)
				{
					Max_Edge=abs(Set_Edge[j]-Set_Edge[j+1]);
					Left_Edge_4=Set_Edge[j];
					Right_Edge_4=Set_Edge[j+1];
				}
			}
			else
			{
				break;
			}
		}
	}
	delete[] Set_Edge;
	delete[] Avr_Row;

	///原图边界
	Right_Edge=Right_Edge_4*E_scale-offSet;
	Left_Edge=Left_Edge_4*E_scale+offSet;
	

	///对边界进行二次确认，主要看非钢板区域的横纵比，以预防图像纵向拉伸的情况
	cv::Mat Nouse_x,Nouse_y;
	if((src.cols-Right_Edge_4>25 || Left_Edge_4-0>25)&&(Right_Edge_4-Left_Edge_4>50))
	{
		if(src.cols-Right_Edge_4>Left_Edge_4-0)
		{
			cv::Rect rect(Right_Edge_4,0,(src.cols-Right_Edge_4),src.rows);
			dstimg22(rect).copyTo(Nouse_x);
			dstimg33(rect).copyTo(Nouse_y);
		}
		else
		{
			cv::Rect rect(0,0,Left_Edge_4,src.rows);
			dstimg22(rect).copyTo(Nouse_x);
			dstimg33(rect).copyTo(Nouse_y);
		}
		cv::Scalar S1=cv::mean(Nouse_x);
		cv::Scalar S2=cv::mean(Nouse_y);
		float S_Judge1=S1[0];
		float S_Judge2=S2[0];
		float E=S_Judge2/(S_Judge1+0.00001);
		if(S_Judge1>30 && S_Judge2>30 && E>1.5)
		{
			Right_Edge=-1;
			Left_Edge=-1;
		}
	}
}

void H_LB_XiNanLv_1X_ImageProcess::Find_Edge_OpenCV_ForSmooth(cv::Mat src,int srcImgWidth,int srcImgHeight,int &Left_Edge,int &Right_Edge,int EdgeThreshold,int offSet)
{
	int E_scale=m_imgSrcWidth/src.cols;
	
	int ddepth1=CV_16S;
	int delta1=0;
	int scale1=1;
	cv::Mat dstimg11,dstimg22,dstimg33;
	cv::Mat grad_x1, grad_y1;
	cv::Mat abs_grad_x1, abs_grad_y1;
	// 求 X方向梯度
	cv::Sobel( src, grad_x1, ddepth1, 0, 1, 3, scale1, delta1, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x1, abs_grad_x1 );
	// 求Y方向梯度
	cv::Sobel( src, grad_y1, ddepth1, 1, 0, 3, scale1, delta1, cv::BORDER_DEFAULT );
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
	
	cv::Mat UseImage=dstimg33.clone();
	int *Avr_Row=new int[UseImage.cols];
	float Avr_Row_Max=0;
	float Avr_Row_Min=65535;
	for(int i=0;i<UseImage.cols;i++)
	{
		int Tatal_Col=0;
		for(int j=0;j<UseImage.rows;j++)
		{
			Tatal_Col+=UseImage.at<uchar>(j,i);
		}
		Avr_Row[i]=Tatal_Col/UseImage.rows;
		if(Avr_Row[i]>Avr_Row_Max)
		{
			Avr_Row_Max=Avr_Row[i];
		}
		if(Avr_Row[i]<Avr_Row_Min)
		{
			Avr_Row_Min=Avr_Row[i];
		}
	}
	//直方图的宽度
	if(m_bIsDebug)
	{
		int height_row=256;    
		int scale_row = 1;     
		int bins_row=UseImage.cols;
		cv::Mat img_row = cv::Mat::zeros(height_row,bins_row*scale_row, CV_8UC3);
		for(int i=0;i<bins_row;i++)    
		{      
			cv::rectangle(img_row,cv::Point(i*scale_row,img_row.rows-1-Avr_Row[i]), cv::Point((i+1)*scale_row,img_row.cols-1),CV_RGB(255,255,0));    
		} 
		cv::namedWindow("img_row",0);
		cv::imshow("img_row",img_row);
	}

	int Left_Edge_4=UseImage.cols-1;
	int Right_Edge_4=0;
	int Num_Max=UseImage.cols/2;
	int *Set_Edge=new int[Num_Max];
	memset(Set_Edge, -1, Num_Max*sizeof(int));
	bool T_B_Edge=false;
	int th=Avr_Row_Min+EdgeThreshold+15;//相比之前大理石的加了额外的20
	//
	//Add_To_File("test.txt",Avr_Row_Max);
	//
	if(Avr_Row_Max<20)
	{
		Left_Edge_4=0;
		Right_Edge_4=UseImage.cols-1;
	}
	else
	{
		for(int i=0,j=0;i<UseImage.cols;i++)
		{
			if(Avr_Row[i]<th && (!T_B_Edge))
			{
				Set_Edge[j++]=i;
				T_B_Edge=true;
			}
			if(Avr_Row[i]>=th && T_B_Edge)
			{
				Set_Edge[j++]=i;
				T_B_Edge=false;
			}
			if(i==UseImage.cols-1 && j%2==1)
			{
				Set_Edge[j]=UseImage.cols-1;
			}
		}
		int Max_Edge=0;
		Left_Edge_4=UseImage.cols-1;
		Right_Edge_4=0;
		for(int j=0;j<Num_Max;j+=2)
		{
			if(j!=-1)
			{
				if(abs(Set_Edge[j]-Set_Edge[j+1])>Max_Edge)
				{
					Max_Edge=abs(Set_Edge[j]-Set_Edge[j+1]);
					Left_Edge_4=Set_Edge[j];
					Right_Edge_4=Set_Edge[j+1];
				}
			}
			else
			{
				break;
			}
		}
	}
	delete[] Set_Edge;
	delete[] Avr_Row;

	///原图边界
	Right_Edge=Right_Edge_4*E_scale-offSet;
	Left_Edge=Left_Edge_4*E_scale+offSet;
	

	///对边界进行二次确认，主要看非钢板区域的横纵比，以预防图像纵向拉伸的情况
	cv::Mat Nouse_x,Nouse_y;
	if((src.cols-Right_Edge_4>25 || Left_Edge_4-0>25)&&(Right_Edge_4-Left_Edge_4>50))
	{
		if(src.cols-Right_Edge_4>Left_Edge_4-0)
		{
			cv::Rect rect(Right_Edge_4,0,(src.cols-Right_Edge_4),src.rows);
			dstimg22(rect).copyTo(Nouse_x);
			dstimg33(rect).copyTo(Nouse_y);
		}
		else
		{
			cv::Rect rect(0,0,Left_Edge_4,src.rows);
			dstimg22(rect).copyTo(Nouse_x);
			dstimg33(rect).copyTo(Nouse_y);
		}
		cv::Scalar S1=cv::mean(Nouse_x);
		cv::Scalar S2=cv::mean(Nouse_y);
		float S_Judge1=S1[0];
		float S_Judge2=S2[0];
		float E=S_Judge2/(S_Judge1+0.00001);
		if(S_Judge1>30 && S_Judge2>30 && E>1.5)
		{
			Right_Edge=-1;
			Left_Edge=-1;
		}
	}
}

void H_LB_XiNanLv_1X_ImageProcess::Defects_Period_OpenCV(PH_DefectSet pSet,float factor,int interval,int PeriodClass,int ImgIndex)
{
	////针对横向较宽的缺陷的限制，主要应用于西铝头部大缺陷的情况
	//int LimitWidth=m_imgSrcWidth*0.2;
	//
	//cv::Mat RowZero=cv::Mat::zeros(1,M_DefectListCols,CV_8UC1);
	//if(ImgIndex<10)//索引小于10时进行数值的初始化
	//{
	//	m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);//缺陷列表图像
	//	m_DefectListIndex=0;//缺陷行索引
	//	memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
	//	memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
	//}
	//else
	//{
	//	int ReductionFactor=m_imgSrcWidth/M_DefectListCols;//画板与实际图像尺寸的比值
	//	//1、拼接画板
	//	if(pSet->iItemNum>0)
	//	{
	//		if(0==m_DefectListIndex)
	//		{
	//			if(m_bIsDebug)
	//			{
	//				cv::namedWindow("DefectList",0);
	//			}			
	//		}
	//		cv::Rect rect(0,m_DefectListIndex%M_DefectListRows,M_DefectListCols,1);
	//		RowZero.copyTo(m_DefectList(rect));//清空当前行
	//		for(int i=0;i<pSet->iItemNum;i++)
	//		{
	//			if(abs(pSet->Items[i].iRightInImage-pSet->Items[i].iLeftInImage)<LimitWidth)
	//			{
	//				int iLeft=pSet->Items[i].iLeftInImage/ReductionFactor;
	//				int iRight=pSet->Items[i].iRightInImage/ReductionFactor;
	//				for ( ;iLeft<iRight;iLeft++)
	//				{
	//					m_DefectList.at<uchar>(m_DefectListIndex%M_DefectListRows,iLeft)=255;
	//				}
	//			}
	//		}
	//		m_DefectListIndex++;
	//	}
	//	if(m_bIsDebug)
	//	{
	//		cv::imshow("DefectList",m_DefectList);
	//	}
	//	//2、画板拼接完成，验证是否存在周期性缺陷
	//	for(int j=0;j<M_DefectListCols;j++)
	//	{
	//		int Col_Count=0;
	//		for(int i=0;i<M_DefectListRows;i++)
	//		{
	//			if(m_DefectList.at<uchar>(i,j)>128)
	//			{
	//				Col_Count++;
	//			}
	//		}
	//		if(Col_Count>=M_DefectListRows*factor && -1!=m_PeriodDefectSit[j])//存在周期缺陷
	//		{
	//			m_PeriodDefectSit[j]=1;
	//		}					
	//	}
	//	//3、存在周期缺陷
	//	for(int i=0;i<pSet->iItemNum;i++)
	//	{			
	//		for(int j=0;j<M_DefectListCols;j++)
	//		{
	//			if(m_PeriodDefectSit[j]==1)
	//			{
	//				if(j>=pSet->Items[i].iLeftInImage/ReductionFactor && j<=pSet->Items[i].iRightInImage/ReductionFactor)
	//				{
	//					pSet->Items[i].iDefectClass=PeriodClass;
	//					m_PeriodDefectSit[j]=-1;
	//				}
	//			}
	//			if(m_PeriodDefectSit[j]==-1)
	//			{
	//				if(j>=pSet->Items[i].iLeftInImage/ReductionFactor && j<=pSet->Items[i].iRightInImage/ReductionFactor)
	//				{
	//					m_SideCountSit[j]++;
	//					if(m_SideCountSit[j]%interval!=0)
	//					{
	//						if(abs(pSet->Items[i].iRightInImage-pSet->Items[i].iLeftInImage)<LimitWidth)//if(6!=pSet->Items[i].iDefectClass)//针对西铝添加的
	//						{
	//							pSet->Items[i].iDefectClass=-1;
	//						}
	//					}
	//					else
	//					{
	//						pSet->Items[i].iDefectClass=PeriodClass;
	//					}

	//					//隐藏缺陷点个数超过某值时，打破周期重新计算
	//					if(m_SideCountSit[j]>interval*50)
	//					{
	//						m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);//缺陷列表图像
	//						m_DefectListIndex=0;//缺陷行索引
	//						memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
	//						memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
	//					}
	//				}
	//			}
	//		}
	//	}			
	//}
}