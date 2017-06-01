#include "H_LB_XinJiangZhongHe_1X_ImageProcess.h"
#include "H_BaseImageProcess.h"
#include "H_SteelImageProcess.h"

H_LB_XinJiangZhongHe_1X_ImageProcess::H_LB_XinJiangZhongHe_1X_ImageProcess(void)
{
	m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);
	m_DefectListIndex=0;
	memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
	memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
}

H_LB_XinJiangZhongHe_1X_ImageProcess::~H_LB_XinJiangZhongHe_1X_ImageProcess(void)
{
}

int H_LB_XinJiangZhongHe_1X_ImageProcess::K_ImageProcessInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug)
{
	m_imgSrcWidth=iImgWidth;
	m_imgSrcHeight=iImgHeight;
	cv::Mat cvSrcImg = cv::Mat(iImgHeight,iImgWidth,CV_8UC1,pImgData);
	cv::Mat cvSrcImg_P2D;
	cv::pyrDown(cvSrcImg,cvSrcImg_P2D,cv::Size(cvSrcImg.cols/2,cvSrcImg.rows/2));

	cv::Mat cvSrcImg_P2D_GCG;
	H_BaseImageProcess::Img_Gray_Change_Y(cvSrcImg_P2D,cvSrcImg_P2D_GCG,100,25);	

	int Left_Edge=iImgWidth-1;
	int Right_Edge=0;
	int Left_Edge_P2D,Right_Edge_P2D;
	H_SteelImageProcess::Steel_FindEdge(cvSrcImg_P2D_GCG,Left_Edge_P2D,Right_Edge_P2D,100);
	Left_Edge=Left_Edge_P2D*2;
	Right_Edge=Right_Edge_P2D*2;

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

	if(Right_Edge-Left_Edge>iImgWidth*0.2)
	{
		//无效图像判定
		cv::Mat cvSrcImg_P2D_GCG_P2D;
		cv::pyrDown(cvSrcImg_P2D_GCG,cvSrcImg_P2D_GCG_P2D,cv::Size(cvSrcImg_P2D_GCG.cols/2,cvSrcImg_P2D_GCG.rows/2));
		
		cv::Mat cvSrcImg_P2D_GCG_P2D_VaildJudge;
		cv::Rect rect(Left_Edge/4,0,(Right_Edge-Left_Edge)/4,cvSrcImg_P2D_GCG_P2D.rows);
		cvSrcImg_P2D_GCG_P2D(rect).copyTo(cvSrcImg_P2D_GCG_P2D_VaildJudge);

		bool bIsValidJudge=IsValid(cvSrcImg_P2D_GCG_P2D_VaildJudge);
		if(!bIsValidJudge)
		{
			Defects_ColdSteel_OpenCV(cvSrcImg_P2D,cvSrcImg_P2D_GCG,Left_Edge/2,Right_Edge/2,2,tConfigInfo_Segmentation,pH_DefectSet);
		}		
	}

	Defects_Period_OpenCV(pH_DefectSet,0.25,100,9,tConfigInfo_Segmentation.tSteelInfo.iImgIndex);

	return 0;
}

bool H_LB_XinJiangZhongHe_1X_ImageProcess::IsValid(const cv::Mat& src)
{
	///1、对于有黑横道的图像
	cv::Mat ss=cv::Mat::zeros(src.rows,1 , CV_16UC1); 
	cv::Mat temp;
	src.convertTo(temp,CV_16UC1);

	for(int i=0;i<src.cols;i++)
	{
		ss.col(0)+=temp.col(i);
	}
	float S_Judge1=0;
	for(int j=0;j<src.rows;j++)
	{
		float temp0=(float)ss.at<ushort>(j,0)/src.cols;
		if(temp0<50)
		{
			S_Judge1++;
		}
	}
	///2.对于钢板停止的图像和坏道图像
	int ddepth=CV_16S;
	int delta=0;
	int scale=1;
	cv::Mat dstimg1,dstimg2,dstimg3;
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	// 求 X方向梯度
	cv::Sobel( src, grad_x, ddepth, 0, 2, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x, abs_grad_x );
	// 求Y方向梯度
	cv::Sobel( src, grad_y, ddepth, 2, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_y, abs_grad_y );
	// 合并梯度(近似)
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_x, 0.5, 0, dstimg1 );
	cv::addWeighted( abs_grad_y, 0.5, abs_grad_y, 0.5, 0, dstimg2 );
	//cv::threshold(dstimg1,);
	//cv::namedWindow("dstimg1",0);
	//cv::imshow("dstimg1",dstimg1);
	//cv::namedWindow("dstimg2",0);
	//cv::imshow("dstimg2",dstimg2);
	//
	cv::Mat dstimg1_1,dstimg1_2;
	cv::Rect rect1_1(0,0,dstimg1.cols,dstimg1.rows/2);
	cv::Rect rect1_2(0,dstimg1.rows/2,dstimg1.cols,dstimg1.rows/2);
	dstimg1(rect1_1).copyTo(dstimg1_1);
	dstimg1(rect1_2).copyTo(dstimg1_2);
	cv::Mat Mean1_1,Dev1_1;
	cv::meanStdDev(dstimg1_1,Mean1_1,Dev1_1);
	cv::Mat Mean1_2,Dev1_2;
	cv::meanStdDev(dstimg1_2,Mean1_2,Dev1_2);
	float a1=Dev1_1.at<double>(0,0);
	float b1=Dev1_2.at<double>(0,0);
	//
	cv::Mat dstimg2_1,dstimg2_2;
	cv::Rect rect2_1(0,0,dstimg2.cols,dstimg2.rows/2);
	cv::Rect rect2_2(0,dstimg2.rows/2,dstimg2.cols,dstimg2.rows/2);
	dstimg2(rect2_1).copyTo(dstimg2_1);
	dstimg2(rect2_2).copyTo(dstimg2_2);
	cv::Mat Mean2_1,Dev2_1;
	cv::meanStdDev(dstimg2_1,Mean2_1,Dev2_1);
	cv::Mat Mean2_2,Dev2_2;
	cv::meanStdDev(dstimg2_2,Mean2_2,Dev2_2);
	float a2=Dev2_1.at<double>(0,0);
	float b2=Dev2_2.at<double>(0,0);
	//
	float e1=(a1/(b1+0.0001)>1 ? a1/(b1+0.0001): b1/(a1+0.0001));
	float e2=(a2/(b2+0.0001)>1 ? a2/(b2+0.0001): b2/(a2+0.0001));
	float e3=a2/(a1+0.0001);
	float e4=b2/(b1+0.0001);

	float S_Judge2=0;
	if((e1>2.5 && max(a1,b1)>30) || (e2>2.5 && max(a2,b2)>30) || e3>2 || e4>2)
	{
		S_Judge2=99;
	}
	//
	if(S_Judge1>2 || S_Judge2>0)
	{
		return true;
	}
	return false;
}

void H_LB_XinJiangZhongHe_1X_ImageProcess::Defects_ColdSteel_OpenCV(const cv::Mat& SrcImg,const cv::Mat& SrcImg_GCG,int Left_Edge,int Right_Edge,int iZoom,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet) 
{
	PH_DefectSet pSet_Class_All=new H_DefectSet;
	pSet_Class_All->iItemNum=0;
	
	cv::Mat SrcImg_Cut,SrcImg_GCG_Cut;
	cv::Rect rect(Left_Edge,0,Right_Edge-Left_Edge,SrcImg.rows);
	SrcImg(rect).copyTo(SrcImg_Cut);
	SrcImg_GCG(rect).copyTo(SrcImg_GCG_Cut);

	///1、原图上			
	cv::Mat DstImg_1=cv::Mat::zeros(SrcImg_Cut.rows,SrcImg_Cut.cols,SrcImg_Cut.type());
	cv::Mat DstImg_1_1=cv::Mat::zeros(SrcImg_Cut.rows,SrcImg_Cut.cols,SrcImg_Cut.type());
	cv::Mat DstImg_1_2=cv::Mat::zeros(SrcImg_Cut.rows,SrcImg_Cut.cols,SrcImg_Cut.type());
	//dst1
	cv::Mat SrcImg_Cut_Blur;
	cv::GaussianBlur(SrcImg_Cut,SrcImg_Cut_Blur,cv::Size(5,5),0);
	cv::Mat SrcImg_Cut_Blur_Mean_1,SrcImg_Cut_Blur_Mean_2;
	cv::blur(SrcImg_Cut_Blur,SrcImg_Cut_Blur_Mean_1,cv::Size(15,15));
	cv::blur(SrcImg_Cut_Blur,SrcImg_Cut_Blur_Mean_2,cv::Size(25,25));
	cv::subtract(SrcImg_Cut_Blur, SrcImg_Cut_Blur_Mean_1, DstImg_1_1);
	cv::subtract(SrcImg_Cut_Blur, SrcImg_Cut_Blur_Mean_2, DstImg_1_2);
	cv::threshold(DstImg_1_1,DstImg_1_1,25,255,0);
	cv::threshold(DstImg_1_2,DstImg_1_2,25,255,0);
	cv::addWeighted( DstImg_1_1, 0.5, DstImg_1_2, 0.5, 0, DstImg_1 );
	cv::threshold(DstImg_1,DstImg_1,5,255,0);
	cv::medianBlur(DstImg_1,DstImg_1,3);//5

	///2、矫正图上
	cv::Mat DstImg_2=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	cv::Mat DstImg_2_1=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	cv::Mat DstImg_2_2=cv::Mat::zeros(SrcImg_GCG_Cut.rows,SrcImg_GCG_Cut.cols,SrcImg_GCG_Cut.type());
	//dst2-1
	cv::GaussianBlur(SrcImg_GCG_Cut,SrcImg_GCG_Cut,cv::Size(3,3),0);
	cv::Mat DstImg_2_1_th,DstImg_2_1_canny;
	cv::threshold(SrcImg_GCG_Cut,DstImg_2_1_th,80,255,1);
	cv::Canny(SrcImg_GCG_Cut,DstImg_2_1_canny,60,100,3);
	cv::Mat DstImg_2_1_canny_Dil;
	int DstImg_2_1_canny_Size=3;
	cv::Mat DstImg_2_1_canny_Element = getStructuringElement( cv::MORPH_CROSS,cv::Size( 2*DstImg_2_1_canny_Size + 1, 2*DstImg_2_1_canny_Size+1 ),cv::Point( DstImg_2_1_canny_Size, DstImg_2_1_canny_Size ) );
	dilate( DstImg_2_1_canny, DstImg_2_1_canny_Dil, DstImg_2_1_canny_Element );
	cv::Mat DstImg_2_1_Add;
	cv::addWeighted( DstImg_2_1_th, 0.5, DstImg_2_1_canny_Dil, 0.5, 0, DstImg_2_1_Add);
	cv::threshold(DstImg_2_1_Add,DstImg_2_1_Add,150,255,0);

	cv::Mat DstImg_2_1_Add_Dil;
	int DstImg_2_1_Add_Size=1;
	cv::Mat DstImg_2_1_Add_Element = getStructuringElement( cv::MORPH_CROSS,cv::Size( 2*DstImg_2_1_Add_Size + 1, 2*DstImg_2_1_Add_Size+1 ),cv::Point( DstImg_2_1_Add_Size, DstImg_2_1_Add_Size ) );
	dilate( DstImg_2_1_Add, DstImg_2_1_Add_Dil, DstImg_2_1_Add_Element );		

	int left=50;
	int right=DstImg_2_1_Add_Dil.cols-50;
	cv::Mat DstImg_2_1_Add_Dil_Cut;
	cv::Rect rect_dst(left,0,right-left,DstImg_2_1_Add_Dil.rows);
	DstImg_2_1_Add_Dil(rect_dst).copyTo(DstImg_2_1_Add_Dil_Cut);


	cv::Mat cdst = cv::Mat::zeros( DstImg_2_1_Add_Dil_Cut.size(), CV_8UC1 );	
	cv::vector<cv::Vec4i> lines;
	HoughLinesP(DstImg_2_1_Add_Dil_Cut, lines, 1, CV_PI/180, 10, 15, 1);//10,10,1
	double line_lenght=0;
	for( size_t i = 0; i < lines.size(); i++ )
	{
		cv::Vec4i l = lines[i];
		float width=abs(l[2]-l[0]);
		float height=abs(l[3]- l[1]);
		if(height/(width+0.01)>7.5 )
		{
			line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 5, CV_AA);
			line_lenght=line_lenght+height;
		}		
	}
	if(line_lenght<cdst.rows*3)
	{
		cdst.copyTo(DstImg_2_1(rect_dst));
	}
	//dst2-2
	cv::threshold(SrcImg_GCG_Cut,DstImg_2_2,65,255,1);
	cv::Mat DstImg_2_2_Element = getStructuringElement( cv::MORPH_CROSS,cv::Size(3,3),cv::Point(-1,-1));
	dilate( DstImg_2_2, DstImg_2_2, DstImg_2_2_Element );
	//dst2
	cv::addWeighted( DstImg_2_1_Add, 0.5, DstImg_2_2, 0.5, 0, DstImg_2_2);
	cv::threshold(DstImg_2_2,DstImg_2_2,150,255,0);
	cv::addWeighted( DstImg_2_1, 0.5, DstImg_2_2, 0.5, 0, DstImg_2);
	///组合处理
	cv::Mat DstImg;
	cv::addWeighted( DstImg_1, 0.5, DstImg_2, 0.5, 0, DstImg);;
	cv::threshold(DstImg,DstImg,5,255,0);
	cv::medianBlur(DstImg,DstImg,5);

	cv::Mat DstImg_Dil;
	int dilation_size = 3;
	cv::Mat element = getStructuringElement( cv::MORPH_CROSS,cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),cv::Point( dilation_size, dilation_size ) );
	dilate( DstImg, DstImg_Dil, element );
	//一级组合
	cv::Mat drawing = cv::Mat::zeros( DstImg_Dil.size(), CV_8UC1 );
	cv::Mat drawing_last = cv::Mat::zeros( DstImg_Dil.size(), CV_8UC1 );
	cv::Mat mean,dev;	
	cv::meanStdDev(DstImg_Dil,mean,dev);
	double fMean=mean.at<double>(0,0);
	int C_Scale=10;
	int iScale=0;
	bool bScale=false;
	bool IsFirst=true;
	int NumCount=0;
	while(true)
	{
		if(++NumCount>15)
		{
			break;
		}
		cv::vector<cv::vector<cv::Point> > contours;
		cv::vector<cv::Vec4i> hierarchy;
		cv::findContours( DstImg_Dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::vector<cv::vector<cv::Point> > contours_poly( contours.size() );
		cv::vector<cv::Rect> boundRect( contours.size() );
		double lengthSum=0;
		for( int i = 0; i < contours.size(); i++ )
		{ 
			cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
			boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );

			lengthSum+=cv::arcLength(contours[i],true);
			int left=max(0,boundRect[i].tl().x-iScale);
			int right=min(drawing.cols-1,boundRect[i].br().x+iScale);
			int top=max(0,boundRect[i].tl().y-iScale);
			int bottom=min(drawing.rows-1,boundRect[i].br().y+iScale);
			rectangle( drawing, cv::Point(left,top), cv::Point(right,bottom), 255, -2, 8, 0 );
		}
		cv::Mat mean1,dev1,mean2,dev2;
		cv::meanStdDev(drawing,mean1,dev1);
		cv::meanStdDev(drawing_last,mean2,dev2);
		cv::Rect rect(0,0,drawing.cols,drawing.rows);
		drawing(rect).copyTo(DstImg_Dil);
		drawing(rect).copyTo(drawing_last);

		double fMean1=mean1.at<double>(0,0)-(lengthSum/(drawing.cols*drawing.rows));
		double fMean2=mean2.at<double>(0,0);
		if(abs(fMean1-fMean2)<1.5)
		{
			iScale=C_Scale;
			if(!IsFirst)
			{
				break;
			}
			IsFirst=false;
		}
		if(bScale)
		{
			iScale=0;
		}
		if(C_Scale==iScale)
		{
			bScale=true;
		}
	}	

	cv::vector<cv::vector<cv::Point> > contours_big;
	cv::vector<cv::Vec4i> hierarchy_big;
	cv::findContours( drawing, contours_big, hierarchy_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	cv::vector<cv::vector<cv::Point> > contours_poly_big( contours_big.size() );
	cv::vector<cv::Rect> boundRect_big( contours_big.size() );
	double area_sum=0;
	double maxWidth=0;
	int minLeft=10000;
	int minTop=10000;
	int maxRight=0;
	int maxBottom=0;
	for( int k = 0; k < contours_big.size(); k++ )
	{ 
		approxPolyDP( cv::Mat(contours_big[k]), contours_poly_big[k], 3, true );
		boundRect_big[k] = cv::boundingRect( cv::Mat(contours_poly_big[k]) );

		int d_left=boundRect_big[k].tl().x;
		int d_right=boundRect_big[k].br().x;
		int d_top=boundRect_big[k].tl().y;
		int d_bottom=boundRect_big[k].br().y;

		float width=abs(d_left-d_right);
		float height=abs(d_top- d_bottom);
		float area=width*height;

		//长宽限制
		if(abs(d_left-d_right)<2 || abs(d_top- d_bottom)<2)
		{
			continue;
		}
		//面积限制
		if(area<tConfigInfo_Segmentation.iAreaThreshold)//500
		{
			continue;
		}
		area_sum+=area;

		//占图像的区域大小
		if(minLeft>d_left)
		{
			minLeft=d_left;
		}
		if(minTop>d_top)
		{
			minTop=d_top;
		}
		if(maxRight<d_right)
		{
			maxRight=d_right;
		}
		if(maxBottom<d_bottom)
		{
			maxBottom=d_bottom;
		}
		//
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass=0;
		//
		cv::Rect rect(d_left,d_top,width,height);
		cv::Mat DstImg_Part;
		DstImg(rect).copyTo(DstImg_Part);
		cv::Mat DstImg_2_1_Part;
		DstImg_2_1(rect).copyTo(DstImg_2_1_Part);
		int DstImg_Part_Num=cv::countNonZero(DstImg_Part);
		int DstImg_2_1_Part_Num=cv::countNonZero(DstImg_2_1_Part);
		//黑道缺陷+线扩展
		if(0==pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass)
		{
			if(DstImg_2_1_Part_Num>10 && DstImg_Part_Num/(DstImg_2_1_Part_Num+1)<10)
			{
				cv::Rect rect(d_left,0,width,SrcImg_Cut.rows-1);
				cv::Mat SrcImg_Cut_Part;
				SrcImg_Cut(rect).copyTo(SrcImg_Cut_Part);

				cv::Mat mean;
				cv::Mat dst_Line=cv::Mat::zeros(SrcImg_Cut_Part.rows,SrcImg_Cut_Part.cols,SrcImg_Cut_Part.type());
				cv::blur(SrcImg_Cut_Part,mean,cv::Size(7,7));
				cv::subtract(mean, SrcImg_Cut_Part, dst_Line);
				cv::threshold(dst_Line,dst_Line,5,255,0);

				cv::Mat drawing_Line = cv::Mat::zeros( dst_Line.size(), CV_8UC1 );
				cv::vector<cv::Vec4i> lines_Line;
				HoughLinesP(dst_Line, lines_Line, 1, CV_PI/180, 50, 20, 2);//100,60,5
				int top_temp=99999;
				int bottom_temp=0;
				int num_H=0;
				int num_V=0;
				for( size_t i = 0; i < lines_Line.size(); i++ )
				{
					cv::Vec4i l = lines_Line[i];
					line( drawing_Line, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 5, CV_AA);//3
					float width=abs(l[2]-l[0]);
					float height=abs(l[3]-l[1]);
					float left_offset=abs(min(l[0],l[2])-0);
					float right_offset=abs(max(l[0],l[2])-dst_Line.cols);
					if(left_offset<2 || right_offset<2)
					{
						continue;
					}
					if(height/(width+0.001)>8)
					{
						num_V++;
					}
					else
					{
						num_H++;
					}
					if(min(l[1],l[3])<top_temp)
					{
						top_temp=min(l[1],l[3]);
					}
					if(max(l[1],l[3])>bottom_temp)
					{
						bottom_temp=max(l[1],l[3]);
					}
				}

				cv::Mat mean1,mean2,dev1,dev2;
				cv::meanStdDev(DstImg_2_1_Part,mean1,dev1);
				cv::meanStdDev(drawing_Line,mean2,dev2);
				float fmean1=mean1.at<double>(0,0);
				float fmean2=mean2.at<double>(0,0);
				if(num_V>0 && num_H==0 && fmean1<fmean2)
				{
					pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass=2;
					if(bottom_temp>top_temp)
					{
						d_top=top_temp;
						d_bottom=bottom_temp;
					}
				}
			}
		}
		//褶皱缺陷
		if(0==pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass && area>18000)//20000
		{
			pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass=4;
		}
		//排除图像较黑的图
		if(0==pSet_Class_All->Items[pSet_Class_All->iItemNum].iDefectClass)
		{
			cv::Mat SrcImg_Cut_Part;
			SrcImg_Cut(rect).copyTo(SrcImg_Cut_Part);
			cv::Mat mean,dev;
			cv::meanStdDev(SrcImg_Cut_Part,mean,dev);
			float fmean=mean.at<double>(0,0);
			float fdev=dev.at<double>(0,0);
			if(fmean<50 && fdev<10)
			{
				continue;
			}
		}

		//
		pSet_Class_All->Items[pSet_Class_All->iItemNum].fConfidence=0.5;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iLeftInImage=(d_left+Left_Edge)*iZoom;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iTopInImage=d_top*iZoom;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iRightInImage=(d_right+Left_Edge)*iZoom;
		pSet_Class_All->Items[pSet_Class_All->iItemNum].iBottomInImage=d_bottom*iZoom;
		pSet_Class_All->iItemNum++;
	}
	if(area_sum>((SrcImg_Cut.rows*SrcImg_Cut.cols)*0.35) || pSet_Class_All->iItemNum>10)//分类6：表面粗糙
	{
		pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=6;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=(Left_Edge+minLeft)*iZoom;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=minTop*iZoom;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=(Left_Edge+maxRight)*iZoom;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=maxBottom*iZoom;
		pH_DefectSet->iItemNum++;
	}
	else
	{
		for(int i=0;i<pSet_Class_All->iItemNum;i++)
		{
			pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=pSet_Class_All->Items[i].iDefectClass;
			pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;				
			pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=pSet_Class_All->Items[i].iLeftInImage;
			pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=pSet_Class_All->Items[i].iTopInImage;
			pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=pSet_Class_All->Items[i].iRightInImage;
			pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=pSet_Class_All->Items[i].iBottomInImage;
			pH_DefectSet->iItemNum++;
		}
	}

	if(pSet_Class_All!= NULL)
	{
		delete pSet_Class_All;
	}
}

void H_LB_XinJiangZhongHe_1X_ImageProcess::Defects_Period_OpenCV(PH_DefectSet pSet,float factor,int interval,int PeriodClass,int ImgIndex)
{
	cv::Mat RowZero=cv::Mat::zeros(1,M_DefectListCols,CV_8UC1);
	if(ImgIndex<10)//索引小于10时进行数值的初始化
	{
		m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);//缺陷列表图像
		m_DefectListIndex=0;//缺陷行索引
		memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
		memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
	}
	else
	{
		int ReductionFactor=m_imgSrcWidth/M_DefectListCols;//画板与实际图像尺寸的比值
		//1、拼接画板
		if(pSet->iItemNum>0)
		{
			if(0==m_DefectListIndex)
			{
				//cv::namedWindow("DefectList",0);
			}
			cv::Rect rect(0,m_DefectListIndex%M_DefectListRows,M_DefectListCols,1);
			RowZero.copyTo(m_DefectList(rect));//清空当前行
			for(int i=0;i<pSet->iItemNum;i++)
			{
				int iLeft=pSet->Items[i].iLeftInImage/ReductionFactor;
				int iRight=pSet->Items[i].iRightInImage/ReductionFactor;

				for ( ;iLeft<iRight;iLeft++)
				{
					m_DefectList.at<uchar>(m_DefectListIndex%M_DefectListRows,iLeft)=255;
				}
			}
			m_DefectListIndex++;
		}
		//cv::imshow("DefectList",m_DefectList);
		//2、画板拼接完成，验证是否存在周期性缺陷
		for(int j=0;j<M_DefectListCols;j++)
		{
			int Col_Count=0;
			for(int i=0;i<M_DefectListRows;i++)
			{
				if(m_DefectList.at<uchar>(i,j)>128)
				{
					Col_Count++;
				}
			}
			if(Col_Count>=M_DefectListRows*factor && -1!=m_PeriodDefectSit[j])//存在周期缺陷
			{
				m_PeriodDefectSit[j]=1;
			}					
		}
		//3、存在周期缺陷
		for(int i=0;i<pSet->iItemNum;i++)
		{			
			for(int j=0;j<M_DefectListCols;j++)
			{
				if(m_PeriodDefectSit[j]==1)
				{
					if(j>=pSet->Items[i].iLeftInImage/ReductionFactor && j<=pSet->Items[i].iRightInImage/ReductionFactor)
					{
						pSet->Items[i].iDefectClass=PeriodClass;
						m_PeriodDefectSit[j]=-1;
					}
				}
				if(m_PeriodDefectSit[j]==-1)
				{
					if(j>=pSet->Items[i].iLeftInImage/ReductionFactor && j<=pSet->Items[i].iRightInImage/ReductionFactor)
					{
						m_SideCountSit[j]++;
						if(m_SideCountSit[j]%interval!=0)
						{
							//if(8==pSet->Items[i].iDefectClass || 0==pSet->Items[i].iDefectClass || 11==pSet->Items[i].iDefectClass)//针对众和添加的
							if(0==pSet->Items[i].iDefectClass)
							{
								pSet->Items[i].iDefectClass=-1;
							}
						}
						else
						{
							pSet->Items[i].iDefectClass=PeriodClass;
						}
					}
				}
			}
		}			
	}
}