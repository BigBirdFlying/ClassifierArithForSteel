#include "H_SteelImageProcess.h"
#include "H_BaseImageProcess.h"

//#define W_DEBUG
//#define uchar unsigned char

int	H_SteelImageProcess::m_imgSrcWidth=0;
int	H_SteelImageProcess::m_imgSrcHeight=0;
//HTuple H_SteelImageProcess::m_hv_MLPHandle=NULL;

H_SteelImageProcess::H_SteelImageProcess(void)
{
}

H_SteelImageProcess::~H_SteelImageProcess(void)
{
}

///主处理函数
int H_SteelImageProcess::SteelImageProcessFunction_OpenCV(const cv::Mat& cv_SrcImage_P2D,cv::Mat& cv_DstImage_P2D_GCG,PDefectSet pSet,int srcWidth,int srcHeight,int SteelNo,int ImgIndex,int CameralNo)
{
	PDefectSet pSet_Class_In_Common=new DefectSet;
	pSet_Class_In_Common->iItemNum=0;
	//获取原图灰度
	H_SteelImageProcess::m_imgSrcWidth=srcWidth;
	H_SteelImageProcess::m_imgSrcHeight=srcHeight;
	//下采样图灰度矫正
	cv::Mat cv_SrcImage_P2D_GCG;
	H_BaseImageProcess::Gray_Change_Y(cv_SrcImage_P2D,cv_SrcImage_P2D_GCG,5,100);
	cv_DstImage_P2D_GCG=cv_SrcImage_P2D_GCG.clone();
	//保存图片
	//H_BaseImageProcess::Save_Image_OpenCV(cv_SrcImage_P2D_GCG,ImgIndex);
	//去除没有钢板的图像，主要是有水存在
	cv::Mat cv_SrcImage_P2D_th;
	cv::threshold(cv_SrcImage_P2D,cv_SrcImage_P2D_th,10,255,0);
	float f_Sum=(float)cv::countNonZero(cv_SrcImage_P2D_th)/(cv_SrcImage_P2D_th.rows*cv_SrcImage_P2D_th.cols);
	//找边界
	int Left_Edge_P2D,Right_Edge_P2D;
	Find_Edge(cv_SrcImage_P2D_GCG,Left_Edge_P2D,Right_Edge_P2D,15);//50
	if(Right_Edge_P2D-Left_Edge_P2D<100 || Left_Edge_P2D<0 || f_Sum<0.25)
	{
		if(pSet_Class_In_Common!= NULL)
		{
			delete pSet_Class_In_Common;
		}
		return pSet->iItemNum;
	}
	//切边
	cv::Mat cv_SrcImage_P2D_GCG_Cut,cv_SrcImage_P2D_Cut;
	Cut_Edge_OpenCV(cv_SrcImage_P2D_GCG,Left_Edge_P2D,Right_Edge_P2D,cv_SrcImage_P2D_GCG_Cut);
	Cut_Edge_OpenCV(cv_SrcImage_P2D,Left_Edge_P2D,Right_Edge_P2D,cv_SrcImage_P2D_Cut);
	//是否为头部
	bool isHead=false;
	
	if(false==isHead)
	{
		//Defects_Common_OpenCV_P2D(cv_SrcImage_P2D_Cut,cv_SrcImage_P2D_GCG_Cut,Left_Edge_P2D,Right_Edge_P2D,isHead,pSet_Class_In_2,ImgIndex,CameralNo,SteelNo);
	}
	///测试代码
	//float gamma=0.4;
	//cv::Mat a,b,c;
	//cv::Mat img;
	//cv_SrcImage_P2D_Cut.convertTo(img,CV_32FC1);
	//cv::pow(img,gamma,a);
	//cv::normalize(a,b,0,255,cv::NORM_MINMAX,CV_8UC1);
	//cv::namedWindow("b",0);
	//cv::imshow("b",b);
	//double min,max;
	//cv::minMaxLoc(b,&min,&max);
	//float mu=255/(max-min);
	//float add=-mu*min;
	////cv::scaleAdd(b,mu,
	//cv::convertScaleAbs(b,c,mu,add);
	//cv::Mat gcg;
	//H_BaseImageProcess::Gray_Change_Y(c,gcg,5,100);
	//cv::namedWindow("gcg",0);
	//cv::imshow("gcg",gcg);
	cv::Mat integral_img(cv_SrcImage_P2D_Cut.rows+1,cv_SrcImage_P2D_Cut.cols+1,CV_64FC1);
	integral(cv_SrcImage_P2D_Cut,integral_img,CV_64FC1);//积分图像
	cv::Rect rect(0,0,cv_SrcImage_P2D_Cut.cols,cv_SrcImage_P2D_Cut.rows);
	cv::Mat integral_img_part;
	integral_img(rect).copyTo(integral_img_part);
	cv::Mat kern = (cv::Mat_<float>(5,5) <<  1,	0,  0,  0,  -1,  
											0,  0,  0,	0,	0,  
											0,	0,  0,	0,	0,
											0,	0,	0,	0,	0,
											-1,	0,	0,	0,	1); 
	cv::Mat dstF;  
	filter2D(integral_img_part,dstF,integral_img_part.depth(),kern); //,cv::Point(0,0) 
	cv::Mat cv_SrcImage_P2D_Cut_64;
	cv_SrcImage_P2D_Cut.convertTo(cv_SrcImage_P2D_Cut_64,CV_64FC1);
	cv::Mat dst_64=cv::Mat::zeros(cv_SrcImage_P2D_Cut.size(), CV_64FC1);
	dst_64=cv_SrcImage_P2D_Cut_64*2500/dstF;
	cv::Mat dst;
	cv::normalize(dst_64,dst,0,255,cv::NORM_MINMAX,CV_8UC1);
	cv::namedWindow("dst",0);
	cv::imshow("dst",dst);
	cv::Mat dst_gray;
	H_BaseImageProcess::Gray_Change_Y(dst,dst_gray,1,100);
	cv::namedWindow("dst_gray",0);
	cv::imshow("dst_gray",dst_gray);
	

	cv::Mat dst_sobel_cut;
	cv::Mat grad_x, grad_xx;
	cv::Mat abs_grad_x, abs_grad_xx;
	cv::Sobel( dst_gray, grad_x, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x, abs_grad_x );
	cv::Sobel( dst_gray, grad_xx, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_xx, abs_grad_xx );
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_xx, 0.5, 0, dst_sobel_cut );
	//H_BaseImageProcess::Save_Image_OpenCV(dst_sobel_cut,ImgIndex);
	int th_med=255;
	{
		int bins = 256; 
		int hist_height=256; 
		int hist_size[] = {bins};    
		float range[] = { 0, 256 };    
		const float* ranges[] = { range};    
		int channels[] = {0}; 
		cv::MatND hist;
		calcHist( &dst_sobel_cut, 1, channels, cv::Mat(), hist, 1, hist_size, ranges,true,false );    
		double max_val;  //直方图的最大值  
		int scale = 2;   //直方图的宽度  
		cv::minMaxLoc(hist, 0, &max_val, 0, 0); //计算直方图最大值  
		cv::Mat hist_img = cv::Mat::zeros(hist_height,bins*scale, CV_8UC3); //创建一个直方图图像并初始化为0   
		int his[256]={0};
		for(int i=0;i<bins;i++)    
		{    
			float bin_val = hist.at<float>(i); // 第i灰度级上的数      
			int intensity = cvRound(bin_val*hist_height/max_val);  //要绘制的高度  
			his[i]=intensity;
			rectangle(hist_img,cv::Point(i*scale,hist_height-1),cv::Point((i+1)*scale - 1, hist_height - intensity),CV_RGB(255,255,255));    
		}  
		cv::namedWindow("Gray Histogram",0);
		imshow( "Gray Histogram", hist_img ); 

		int his_med[256]={0};
		int temp[3]={0};
		int med=3;
		for(int i=0;i<256-med;i++)
		{
			for(int m=i,n=0;m<i+med;m++,n++)
			{
				temp[n]=his[m];
			}
			for(int j=0;j<med;j++)
			{
				for(int k=0;k<med-j-1;k++)
				{
					if(temp[k+1]<temp[k])
					{
						int t=temp[k+1];
						temp[k+1]=temp[k];
						temp[k]=t;
					}
				}
			}
			his_med[i]=temp[med/2];
		}
		cv::Mat Histogram_med = cv::Mat::zeros(hist_height,bins*scale, CV_8UC3);;
		for(int i=0;i<bins;i++)    
		{    
			rectangle(Histogram_med,cv::Point(i*scale,hist_height-1),cv::Point((i+1)*scale - 1, hist_height - his_med[i]),CV_RGB(255,255,255));    
		}  
		cv::namedWindow("Histogram_med",0);
		imshow( "Histogram_med", Histogram_med ); 
		for(int i=255;i>0;i--)
		{
			if(his_med[i]>5)
			{
				th_med=i;
				break;
			}
		}
	}
	th_med=min(th_med+30,255);

	cv::Mat dst_sobel_th;
	cv::threshold(dst_sobel_cut,dst_sobel_th,th_med,255,0);
	cv::namedWindow("dst_sobel_cut",0);
	cv::imshow("dst_sobel_cut",dst_sobel_cut);
	cv::namedWindow("dst_sobel_th",0);
	cv::imshow("dst_sobel_th",dst_sobel_th);

	//cv::Mat dst_sobel_th_mean;
	//cv::blur(dst_sobel_th,dst_sobel_th_mean,cv::Size(15,15));
	//cv::namedWindow("dst_sobel_th_mean",0);
	//cv::imshow("dst_sobel_th_mean",dst_sobel_th_mean);

	int width=15;
	int height=15;
	cv::Mat zeros1=cv::Mat::zeros(dst_sobel_cut.rows,dst_sobel_cut.cols,dst_sobel_cut.type());
	for(int i=0;i<dst_sobel_cut.cols/width-1;i++)
	{
		for(int j=0;j<dst_sobel_cut.rows/height-1;j++)
		{
			cv::Rect rect(i*width,j*height,width,height);
			cv::Mat tmp_img;
			dst_sobel_cut(rect).copyTo(tmp_img);
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
			if(entropy>4.5)
			{
				cv::Mat aa,am,ad;
				dst(rect).copyTo(aa);
				cv::meanStdDev(aa,am,ad);
				float mm=am.at<double>(0,0);
				if(mm<100)
				{
					tmp_img.copyTo(zeros1(rect));
				}
			}
		}
	}
	cv::namedWindow("zeros1",0);
	cv::imshow("zeros1",zeros1);
	//cv::Mat s1_lbp;
	//H_BaseImageProcess::SSR(s1,1,1,s1_lbp);
	//cv::namedWindow("s1_lbp",0);
	//cv::imshow("s1_lbp",s1_lbp);

	//cv::Mat ss=s1.clone();;
	//int tmp[8]={0};
	//int sum=0;
	//for(int i=1;i<s1.rows-1;i++)
	//{
	//	for(int j=1;j<s1.cols-1;j++)
	//	{
	//		if(s1.at<unsigned char>(i-1,j-1)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[0]=1;
	//		}
	//		else
	//		{
	//			tmp[0]=0;
	//		}
	//		if(s1.at<unsigned char>(i,j-1)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[1]=1;
	//		}
	//		else
	//		{
	//			tmp[1]=0;
	//		}
	//		if(s1.at<unsigned char>(i+1,j-1)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[2]=1;
	//		}
	//		else
	//		{
	//			tmp[2]=0;
	//		}
	//		if(s1.at<unsigned char>(i+1,j)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[3]=1;
	//		}
	//		else
	//		{
	//			tmp[3]=0;
	//		}
	//		if(s1.at<unsigned char>(i+1,j+1)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[4]=1;
	//		}
	//		else
	//		{
	//			tmp[4]=0;
	//		}
	//		if(s1.at<unsigned char>(i,j+1)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[5]=1;
	//		}
	//		else
	//		{
	//			tmp[5]=0;
	//		}
	//		if(s1.at<unsigned char>(i-1,j+1)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[6]=1;
	//		}
	//		else
	//		{
	//			tmp[6]=0;
	//		}
	//		if(s1.at<unsigned char>(i-1,j)>s1.at<unsigned char>(i,j))
	//		{
	//			tmp[7]=1;
	//		}
	//		else
	//		{
	//			tmp[7]=0;
	//		}
	//		for(int k=0;k<=7;k++)
	//		{
	//			sum+=abs(tmp[k]-tmp[k+1]);
	//		}
	//		sum=sum+abs(tmp[7]-tmp[0]);
	//		int s=0;
	//		if(sum<=5)
	//		{
	//			s=(tmp[0]*128+tmp[1]*64+tmp[2]*32+tmp[3]*16+tmp[4]*8+tmp[5]*4+tmp[6]*2+tmp[7]*1);
	//		}
	//		else
	//		{
	//			s=59;
	//		}
	//		ss.at<unsigned char>(i,j)=s;
	//	}
	//}
	//cv::namedWindow("ss",0);
	//cv::imshow("ss",ss);

	//int block_x1=8;
	//int block_y1=6;
	//int block_x2=5;
	//int block_y2=3;
	//int baseGray=100;
	//cv::Mat dst_zero1=cv::Mat::zeros(cv_SrcImage_P2D_Cut.rows,cv_SrcImage_P2D_Cut.cols,cv_SrcImage_P2D_Cut.type());
	//cv::Mat dst_zero2=cv::Mat::zeros(cv_SrcImage_P2D_Cut.rows,cv_SrcImage_P2D_Cut.cols,cv_SrcImage_P2D_Cut.type());
	//cv::Mat dst_zero_add=cv::Mat::zeros(cv_SrcImage_P2D_Cut.rows,cv_SrcImage_P2D_Cut.cols,cv_SrcImage_P2D_Cut.type());
	//for(int i=0;i<integral_img.rows-1;i++)
	//{
	//	for(int j=0;j<integral_img.cols-1;j++)
	//	{
	//		int begin_x1=max(0,j-block_x1/2);
	//		int end_x1=min(integral_img.cols-1,j+block_x1/2);
	//		int begin_y1=max(0,i-block_y1/2);
	//		int end_y1=min(integral_img.rows-1,i+block_y1/2);
	//		float A1=integral_img.at<double>(end_y1,end_x1);
	//		float B1=integral_img.at<double>(begin_y1,begin_x1);
	//		float C1=integral_img.at<double>(begin_y1,end_x1);
	//		float D1=integral_img.at<double>(end_y1,begin_x1);
	//		float temp1=A1+B1-C1-D1;
	//		float x1=temp1/(abs(end_x1-begin_x1)*abs(end_y1-begin_y1));
	//		float _x1=1/(x1+0.000001);		
	//		dst_zero1.at<uchar>(i,j)=min(255,cv_SrcImage_P2D_Cut.at<uchar>(i,j)*baseGray*_x1);

	//		int begin_x2=max(0,j-block_x2/2);
	//		int end_x2=min(integral_img.cols-1,j+block_x2/2);
	//		int begin_y2=max(0,i-block_y2/2);
	//		int end_y2=min(integral_img.rows-1,i+block_y2/2);
	//		float A2=integral_img.at<double>(end_y2,end_x2);
	//		float B2=integral_img.at<double>(begin_y2,begin_x2);
	//		float C2=integral_img.at<double>(begin_y2,end_x2);
	//		float D2=integral_img.at<double>(end_y2,begin_x2);
	//		float temp2=A2+B2-C2-D2;
	//		float x2=temp2/(abs(end_x2-begin_x2)*abs(end_y2-begin_y2));
	//		float _x2=1/(x2+0.000001);		
	//		dst_zero2.at<uchar>(i,j)=min(255,cv_SrcImage_P2D_Cut.at<uchar>(i,j)*baseGray*_x2);	

	//		float _x=2/(x1+x2+0.000001);
	//		dst_zero_add.at<uchar>(i,j)=min(255,cv_SrcImage_P2D_Cut.at<uchar>(i,j)*baseGray*_x);
	//	}
	//}
	//cv::namedWindow("dst_zero_add",0);
	//cv::imshow("dst_zero_add",dst_zero_add);


	///合并缺陷
	cv::Mat drawimg_Common=cv::Mat::zeros(srcHeight,srcWidth,cv_SrcImage_P2D.type());
	for(int n=0;n<pSet_Class_In_Common->iItemNum;n++)
	{
		cv::rectangle(drawimg_Common,cv::Point(pSet_Class_In_Common->Items[n].iLeft,pSet_Class_In_Common->Items[n].iTop),cv::Point(pSet_Class_In_Common->Items[n].iRight,pSet_Class_In_Common->Items[n].iBottom),255,-2);
	}

	cv::Mat drawimg;
	cv::addWeighted(drawimg_Common,0.5,drawimg_Common,0.5,0,drawimg);
	if(H_DefectArithInterface::m_ConfigurationInfo.DetectGrade==0)
	{
		cv::threshold(drawimg,drawimg,100,255,0);
	}
	else if(H_DefectArithInterface::m_ConfigurationInfo.DetectGrade==1)
	{
		cv::threshold(drawimg,drawimg,150,255,0);
	}
	cv::vector<cv::vector<cv::Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;
	findContours( drawimg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
	cv::vector<cv::Rect> boundRect( contours.size() );
	cv::vector<cv::RotatedRect> minRect( contours.size() ); 
	int minLeft=10000;
	int minTop=10000;
	int maxRight=0;
	int maxBottom=0;
	for( int k = 0; k< contours.size(); k++ )
	{
		approxPolyDP( cv::Mat(contours[k]), contours[k], 3, true );
		boundRect[k] = boundingRect( cv::Mat(contours[k]) );
		int d_left=max(0,boundRect[k].tl().x);
		int d_right=min(drawimg.cols-1,boundRect[k].br().x);
		int d_top=max(0,boundRect[k].tl().y);
		int d_bottom=min(drawimg.rows-1,boundRect[k].br().y);
		float width=d_right-d_left;
		float height=d_bottom-d_top;
		//
		if(d_left<minLeft)
		{
			minLeft=d_left;
		}
		if(d_top<minTop)
		{
			minTop=d_top;
		}
		if(d_right>maxRight)
		{
			maxRight=d_right;
		}
		if(d_bottom>maxBottom)
		{
			maxBottom=d_bottom;
		}
		//
		if(height/(width+0.000001)>1.6)
		{
			pSet->Items[pSet->iItemNum].iDefectClass=5;
		}
		else
		{
			pSet->Items[pSet->iItemNum].iDefectClass=2;
		}
		
		pSet->Items[pSet->iItemNum].iConfidence=0.5;
		pSet->Items[pSet->iItemNum].iLeft=d_left;
		pSet->Items[pSet->iItemNum].iTop=d_top;
		pSet->Items[pSet->iItemNum].iRight=d_right;
		pSet->Items[pSet->iItemNum].iBottom=d_bottom;
		pSet->iItemNum++;
	}
	if(pSet->iItemNum>=5)
	{
		pSet->iItemNum=0;
		pSet->Items[pSet->iItemNum].iDefectClass=2;	
		pSet->Items[pSet->iItemNum].iConfidence=0.5;	
		pSet->Items[pSet->iItemNum].iLeft=minLeft;
		pSet->Items[pSet->iItemNum].iTop=minTop;
		pSet->Items[pSet->iItemNum].iRight=maxRight;
		pSet->Items[pSet->iItemNum].iBottom=maxBottom;
		pSet->iItemNum++;
	}
	///
	if(pSet_Class_In_Common!= NULL)
	{
		delete pSet_Class_In_Common;
	}
	///
	return pSet->iItemNum;
}
/*****************************************************************************分函数部分**************************************************************************/
/************************找边界************************/
void H_SteelImageProcess::Find_Edge(const cv::Mat& src,int &Left_Edge,int &Right_Edge,int offSet) 
{
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
	Left_Edge=width-1;
	Right_Edge=0;
	for(int j=0;j<Num_Max;j+=2)
	{
		if(j!=-1)
		{
			if(abs(Set_Edge[j]-Set_Edge[j+1])>Max_Edge)
			{
				Max_Edge=abs(Set_Edge[j]-Set_Edge[j+1]);
				Left_Edge=Set_Edge[j];
				Right_Edge=Set_Edge[j+1];
			}
		}
		else
		{
			break;
		}
	}
	delete[] Set_Edge;
	delete[] fAverGray;

	Left_Edge=Left_Edge+offSet;
	Right_Edge=Right_Edge-offSet;
}
/************************切边界************************/
//void H_SteelImageProcess::Cut_Edge_Halcon(HObject ho_srcimg,int Left_Edge,int Right_Edge,HObject *ho_dstimg) 
//{
//	if(Right_Edge<=Left_Edge)
//	{
//		return;
//	}
//	HImage Image=ho_srcimg;
//	HTuple width,height;
//	Image.GetImageSize(&width,&height);
//	CropRectangle1(ho_srcimg, ho_dstimg, 0, Left_Edge, height-1, Right_Edge);
//	Image.Clear();
//	Image.GenEmptyObj();
//	return;
//}
/************************切边界************************/
void H_SteelImageProcess::Cut_Edge_OpenCV(const cv::Mat& src,int Left_Edge,int Right_Edge,cv::Mat& dst) 
{
	if(Right_Edge<=Left_Edge)
	{
		return;
	}
	cv::Rect rect(Left_Edge,0,Right_Edge-Left_Edge,src.rows);
	src(rect).copyTo(dst);
	return;
}
/************************找头尾************************/
void H_SteelImageProcess::Find_Head(const cv::Mat& src_Cut_GCG,bool &isHead,int &Top_Edge,int &Bottom_Edge) 
{
	cv::Rect rectTop(0,0,src_Cut_GCG.cols,5);
	cv::Rect rectBottom(0,src_Cut_GCG.rows-5,src_Cut_GCG.cols,5);
	cv::Mat top,bottom;
	src_Cut_GCG(rectTop).copyTo(top);
	src_Cut_GCG(rectBottom).copyTo(bottom);
	
	cv::Mat Mean_top,Mean_bottom,Dev_top,Dev_bottom;
	cv::meanStdDev(top,Mean_top,Dev_top);
	cv::meanStdDev(bottom,Mean_bottom,Dev_bottom);
	float fMean_top=Mean_top.at<double>(0,0);
	float fMean_bottom=Mean_bottom.at<double>(0,0);

	if(fMean_top>30 && fMean_bottom>30)
	{
		isHead=false;
	}
}
/************************检结疤************************/
void H_SteelImageProcess::Defects_Scarring_OpenCV(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo) 
{
	//尺寸偶和
	if(Left_Edge%2 != 0)
	{
		Left_Edge++;
	}
	if(Right_Edge%2 != 0)
	{
		Right_Edge--;
	}

	//第一次分解
	cv::Mat effect_gray_1=cv::Mat::zeros(srcimg_Cutimg_P2D.size(), CV_8UC1); 
	int n_ind=1;
	int m_1=50;
	for(int i=0;i<srcimg_Cutimg_P2D.rows/m_1;i++)
	{
		cv::Rect rectio(0,m_1*i,srcimg_Cutimg_P2D.cols,m_1);
		cv::Mat effectio;
		srcimg_Cutimg_P2D(rectio).copyTo(effectio);				
		cv::Mat dst=cv::Mat::zeros(effectio.rows,effectio.cols,effectio.type());
		H_BaseImageProcess::Gray_Change_Y(effectio,dst,5,100); 
		dst.copyTo(effect_gray_1(rectio));
	}
	//第二次分解
	cv::Mat effect_gray_2=cv::Mat::zeros(srcimg_Cutimg_P2D.size(), CV_8UC1);
	int m_2=25;
	int sx=1;
	for(int i=0;i<srcimg_Cutimg_P2D.rows/m_2;i++)
	{
		if(i>=1 && i<12)
		{
			sx=2;
		}

		cv::Rect rectio(0,m_2*i,srcimg_Cutimg_P2D.cols,m_2*sx);
		cv::Mat effectio;
		srcimg_Cutimg_P2D(rectio).copyTo(effectio);

		cv::Mat dst=cv::Mat::zeros(effectio.rows,effectio.cols,effectio.type());
		H_BaseImageProcess::Gray_Change_Y(effectio,dst,5,100); 
		dst.copyTo(effect_gray_2(rectio));

		sx=1;
		if(i>=1 && i<12)
		{
			i++;
		}
	}

	//namedWindow("effect_gray_1", 0 );
	//imshow( "effect_gray_1", effect_gray_1 );
	cv::Mat effect_gray_11;
	threshold( effect_gray_1, effect_gray_11, 35, 255,1);//50,40
	//namedWindow("effect_gray_11", 0 );
	//imshow( "effect_gray_11", effect_gray_11 );

	//namedWindow("effect_gray_2", 0 );
	//imshow( "effect_gray_2", effect_gray_2 );
	cv::Mat effect_gray_22;
	threshold( effect_gray_2, effect_gray_22, 35, 255,1);
	//namedWindow("effect_gray_22", 0 );
	//imshow( "effect_gray_22", effect_gray_22 );

	cv::Mat effect_gray=cv::Mat::zeros(srcimg_Cutimg_P2D.size(), CV_8UC1);
	addWeighted( effect_gray_11, 0.5, effect_gray_22, 0.5, 0, effect_gray );
	threshold( effect_gray, effect_gray, 150, 255,0);
	medianBlur ( effect_gray, effect_gray, 3);//7
	//cv::namedWindow("effect_gray", 0 );
	//imshow( "effect_gray", effect_gray );


	cv::Mat dstimg_Dilate_kongdong;
	int dilation_size_kongdong = 10;//15
	cv::Mat element_kongdong= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_kongdong + 1, 2*dilation_size_kongdong+1 ),cv::Point( dilation_size_kongdong, dilation_size_kongdong ) );
	dilate( effect_gray, dstimg_Dilate_kongdong, element_kongdong );
	cv::Mat image_kongdong=srcimg_Cutimg_P2D.clone();

	cv::vector<vector<cv::Point> > contours_kongdong;
	cv::vector<cv::Vec4i> hierarchy_kongdong;
	findContours( dstimg_Dilate_kongdong, contours_kongdong, hierarchy_kongdong, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
	cv::vector<cv::Rect> boundRect_kongdong( contours_kongdong.size() );
	double maxarea_kongdong=15000;  //10000
	double minarea_kongdong=350;  //350,600
		
	for( int i = 0; i< contours_kongdong.size(); i++ )
	{		
		double tmparea_kongdong=fabs(contourArea(contours_kongdong[i])); 
		double tmplen_kongdong=fabs(arcLength(contours_kongdong[i], true ));
		cv::Rect rect_kongdong = boundingRect(contours_kongdong[i]);  
		int tmparea=rect_kongdong.width*rect_kongdong.height;
		if(tmparea_kongdong<minarea_kongdong || tmparea_kongdong>maxarea_kongdong)
		{
			contours_kongdong[i].clear();continue;
		}
		//去除左右边
		if((abs(rect_kongdong.tl().x-0)<1 || abs(rect_kongdong.br().x-srcimg_Cutimg_P2D.cols)<1) && ((float)rect_kongdong.height/rect_kongdong.width)>1)//&& ((float)rect_kongdong.height/rect_kongdong.width)>1  //2016-11-21边界比5
		{
			contours_kongdong[i].clear();continue;
		}
		//去除上下边
		if((abs(rect_kongdong.tl().y-0)<5 || abs(rect_kongdong.br().y-srcimg_Cutimg_P2D.rows)<5) && (rect_kongdong.width/rect_kongdong.height)>3)
		{
			contours_kongdong[i].clear();continue;
		}
		//去除边角的伪图像
		if(abs(rect_kongdong.tl().x-0)<5 || abs(rect_kongdong.br().x-srcimg_Cutimg_P2D.cols)<5)
		{
			if(abs(rect_kongdong.tl().y-0)<5 || abs(rect_kongdong.br().y-srcimg_Cutimg_P2D.rows)<5)
			{
				contours_kongdong[i].clear();continue;
			}
		}
		//长宽比的限制
		if(rect_kongdong.width/(rect_kongdong.height+1)>5)
		{
			contours_kongdong[i].clear();continue;
		}
		//在原图上
		cv::Mat src_Part;
		srcimg_Cutimg_P2D(rect_kongdong).copyTo(src_Part);
		cv::Mat src_Part_Mean,src_Part_Dev;
		cv::meanStdDev(src_Part,src_Part_Mean,src_Part_Dev);
		float f_src_Part_Mean=src_Part_Mean.at<double>(0,0);
		if(f_src_Part_Mean<5)
		{
			contours_kongdong[i].clear();continue;
		}
		//在effect_gray_1上
		int T_Sa=0;
		double T_Sum1=0;
		double T_Sum2=0;
		int T_Sn=0;
		cv::Mat temp;
		effect_gray_1(rect_kongdong).copyTo(temp);
		cv::Mat temp0;
		effect_gray_2(rect_kongdong).copyTo(temp0);
		for(int i=0;i<temp.rows;i++)
		{
			for(int j=0;j<temp.cols;j++)
			{
				if(temp.at<uchar>(i,j)<5)
				{
					T_Sa++;
				}
				T_Sum1+=temp.at<uchar>(i,j);
				T_Sum2+=temp0.at<uchar>(i,j);
				T_Sn++;
			}
		}
		T_Sum1/=T_Sn;
		T_Sum2/=T_Sn;
		if(T_Sa>50 && abs(T_Sum1-T_Sum2)>15)//100
		{
			contours_kongdong[i].clear();continue;
		}
				
		//在灰度矫正下采样图像上
		cv::Mat temp1;
		srcimg_GCG_Cutimg_P2D(rect_kongdong).copyTo(temp1);
		cv::Mat temp11;
		Canny( temp1, temp11, 50, 120, 3 );//2016-11-21  100，200

		float Canny_num=0;
		int Canny_n=1;
		float Canny_top_num=0;
		int Canny_top_n=1;
		float Canny_bottom_num=0;
		int Canny_bottom_n=1;
		for(int i=0;i<temp11.rows;i++)
		{
			for(int j=0;j<temp11.cols;j++)
			{
				Canny_num+=temp11.at<uchar>(i,j);
				Canny_n++;
				if(i<temp11.rows/2)
				{
					Canny_top_num+=temp11.at<uchar>(i,j);
					Canny_top_n++;
				}
				else
				{
					Canny_bottom_num+=temp11.at<uchar>(i,j);
					Canny_bottom_n++;
				}
			}
		}
		Canny_num/=Canny_n;
		Canny_top_num/=Canny_n;
		Canny_bottom_num/=Canny_bottom_n;
		if(Canny_num<=0)
		{
			contours_kongdong[i].clear();continue;
		}
		if(Canny_top_num<=0 || Canny_bottom_num<=0)
		{
			contours_kongdong[i].clear();continue;
		}

		//去水滴
		cv::Mat temp2;
		effect_gray_11(rect_kongdong).copyTo(temp2);

		cv::vector<cv::vector<cv::Point> > contours;
		cv::vector<cv::Vec4i> hierarchy;
		findContours( temp2, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
		cv::vector<cv::Rect> boundRect( contours.size() );
		cv::vector<cv::RotatedRect> minRect( contours.size() );
		double maxarea=0;  
		int n=-1;//n为面积最大轮廓索引，m  
		for( int j = 0; j< contours.size(); j++ )
		{
			double tmparea=fabs(contourArea(contours[j]));
			if(tmparea>maxarea)
			{
				maxarea=tmparea;
				n=j;
			}
		}
		if(n>=0)
		{
			minRect[n] = minAreaRect( cv::Mat(contours[n]) );
			cv::Rect rect = boundingRect(contours[n]);
			cv::Point2f rect_points[4]; 
			minRect[n].points( rect_points );
			float min_roto=10000;
			float min_rect=10000;
			for( int j = 0; j < 4; j++ )
			{
				if(sqrt((rect_points[j].x-rect_points[(j+1)%4].x)*(rect_points[j].x-rect_points[(j+1)%4].x)+(rect_points[j].y-rect_points[(j+1)%4].y)*(rect_points[j].y-rect_points[(j+1)%4].y))<min_roto)
				{
					min_roto=sqrt((rect_points[j].x-rect_points[(j+1)%4].x)*(rect_points[j].x-rect_points[(j+1)%4].x)+(rect_points[j].y-rect_points[(j+1)%4].y)*(rect_points[j].y-rect_points[(j+1)%4].y));
				}
			}
			if(rect.width<min_rect)
			{
				min_rect=rect.width;
			}
			if(rect.height<min_rect)
			{
				min_rect=rect.height;
			}
			if(min_rect/(min_roto+0.01)>2.5)
			{
				contours_kongdong[i].clear();continue;
			}
			//去除波动
			if(abs(rect.width-temp2.cols)<5)
			{
				contours_kongdong[i].clear();continue;
			}
		}
		else
		{
			contours_kongdong[i].clear();continue;
		}

		//对钢板波动的限制
		cv::Mat temp3;
		effect_gray_11(rect_kongdong).copyTo(temp3);
		cv::Mat temp4;
		effect_gray_22(rect_kongdong).copyTo(temp4);
		int iGray1=1;//防止分母为0
		int iGray2=1;
		for(int i=0;i<temp3.rows;i++)
		{
			for(int j=0;j<temp3.cols;j++)
			{
				iGray1+=temp3.at<uchar>(i,j);
				iGray2+=temp4.at<uchar>(i,j);
			}
		}
		if(iGray1/iGray2>5 || iGray2/iGray1>5)
		{
			contours_kongdong[i].clear();continue;
		}
		//对小点状缺陷抑制
		float Canny_top_left_num=0;
		int Canny_top_left_n=1;
		float Canny_bottom_right_num=0;
		int Canny_bottom_right_n=1;
		if(tmparea_kongdong<700)
		{
			for(int i=0;i<temp11.rows;i++)
			{
				for(int j=0;j<temp11.cols;j++)
				{
					if((i+j)<temp11.rows/2)
					{
						Canny_top_left_num+=temp11.at<uchar>(i,j);
						Canny_top_left_n++;
					}
					if((i+j)>temp11.rows/2+temp11.cols)
					{
						Canny_bottom_right_num+=temp11.at<uchar>(i,j);
						Canny_bottom_right_n++;
					}
				}
			}
			if(Canny_top_left_num<=0 && Canny_bottom_right_num<=0)
			{
				contours_kongdong[i].clear();continue;
			}
			float ee=(Canny_top_left_num+Canny_bottom_right_num)/(Canny_top_left_n+Canny_bottom_right_n);
			if(ee<=20 )
			{
				contours_kongdong[i].clear();continue;
			}
		}

		//对波动限制2
		cv::Mat temp5;
		srcimg_GCG_Cutimg_P2D(rect_kongdong).copyTo(temp5);
		cv::Mat dst_temp5;

		H_BaseImageProcess::Gray_Change_X(temp5, dst_temp5,0,100,true);

		cv::Mat Mean5;
		cv::Mat Dev5;
		meanStdDev(dst_temp5,Mean5,Dev5);
		float Val5=Dev5.at<double>(0,0);
		if(Val5<20)
		{
			contours_kongdong[i].clear();continue;
		}


		//写缺陷
		approxPolyDP( cv::Mat(contours_kongdong[i]), contours_kongdong[i], 3, true );
		boundRect_kongdong[i] = boundingRect( cv::Mat(contours_kongdong[i]) );

		///2016-04-19日为限制缺陷而做
		cv::Mat temp_A;
		srcimg_GCG_Cutimg_P2D(rect_kongdong).copyTo(temp_A);
		//namedWindow("a", 0 );
		//imshow( "a", temp_A );
		int t_x=max(0,rect_kongdong.tl().x-rect_kongdong.width);
		int t_y=max(0,rect_kongdong.tl().y-rect_kongdong.height);
		int t_width=min(srcimg_GCG_Cutimg_P2D.cols-t_x,rect_kongdong.width*3);
		int t_height=min(srcimg_GCG_Cutimg_P2D.rows-t_y,rect_kongdong.height*3);
		cv::Rect rect_kongdong_big(t_x,t_y,t_width,t_height);
		cv::Mat temp_B;cv::Mat aa;
		srcimg_GCG_Cutimg_P2D(rect_kongdong_big).copyTo(aa);
		srcimg_GCG_Cutimg_P2D(rect_kongdong_big).copyTo(temp_B);
		//namedWindow("b", 0 );
		//imshow( "b", temp_B );
		cv::Mat Avg_1,Dev_1;
		meanStdDev(temp_A, Avg_1,Dev_1);
		float iAvg1=Avg_1.at<double>(0,0);
		float iDev1=Dev_1.at<double>(0,0);
		cv::Mat Avg_2,Dev_2;
		meanStdDev(temp_B, Avg_2,Dev_2);
		float iAvg2=Avg_2.at<double>(0,0);
		float iDev2=Dev_2.at<double>(0,0);

		int z_x;
		if(t_x==0)
		{
			z_x=0;
		}
		else
		{
			z_x=rect_kongdong_big.width/6;
		}
		//
		int z_y;
		if(t_y==0)
		{
			z_y=0;
		}
		else
		{
			z_y=rect_kongdong_big.height/6;
		}
		//
		int z_width;
		if(t_width==(srcimg_GCG_Cutimg_P2D.cols-t_x))
		{
			z_width=(srcimg_GCG_Cutimg_P2D.cols-t_x)-z_x;
		}
		else
		{
			z_width=rect_kongdong_big.width*4/6;
		}
		//
		int z_height;
		if(t_height==(srcimg_GCG_Cutimg_P2D.rows-t_y))
		{
			z_height=(srcimg_GCG_Cutimg_P2D.rows-t_y)-z_y;
		}
		else
		{
			z_height=rect_kongdong_big.height*4/6;
		}
		if(z_width>0 && z_height>0)
		{
			cv::Rect re(z_x,z_y,z_width,z_height);
			cv::Mat zero=cv::Mat::zeros(z_height,z_width,CV_8UC1);
			zero.copyTo(temp_B(re));
			//namedWindow("c", 0 );
			//imshow( "c", temp_B );
			cv::Mat Avg_3,Dev_3;
			meanStdDev(temp_B, Avg_3,Dev_3);
			float iAvg3=Avg_3.at<double>(0,0);
			float iDev3=Dev_3.at<double>(0,0);

			float sum1=0;
			float avr1=0;
			int n1=1;
			for(int i=0;i<temp_B.rows;i++)
			{
				for(int j=0;j<temp_B.cols;j++)
				{
					if(temp_B.at<uchar>(i,j)>0)
					{
						sum1+=temp_B.at<uchar>(i,j);
						n1++;
					}
				}
			}
			avr1=sum1/n1;
			float D1=1;
			for(int i=0;i<temp_B.rows;i++)
			{
				for(int j=0;j<temp_B.cols;j++)
				{
					if(temp_B.at<uchar>(i,j)>0)
					{
						D1+=(temp_B.at<uchar>(i,j)-avr1)*(temp_B.at<uchar>(i,j)-avr1);
					}
				}
			}
			D1=sqrt(D1)/n1;
			float ss=iDev1/D1;
			if(tmparea_kongdong<600 && ss<100)
			{
				contours_kongdong[i].clear();continue;
			}
			///2016-04-19日为限制波动
			int t_x1=max(0,rect_kongdong.tl().x-rect_kongdong.width/2);
			int t_y1=max(0,rect_kongdong.tl().y-rect_kongdong.height/2);
			int t_width1=min(srcimg_GCG_Cutimg_P2D.cols-t_x1,rect_kongdong.width*2);
			int t_height1=min(srcimg_GCG_Cutimg_P2D.rows-t_y1,rect_kongdong.height*2);
			cv::Rect rect_kongdong_big1(t_x1,t_y1,t_width1,t_height1);
			cv::Mat temp_D;
			srcimg_GCG_Cutimg_P2D(rect_kongdong_big1).copyTo(temp_D);
			//namedWindow("D", 0 );
			//imshow( "D", temp_D );
			double sum_t=0;
			int n_t=1;
			for(int i=0;i<temp_D.rows;i++)
			{
				for(int j=0;j<temp_D.cols;j++)
				{
					if(temp_D.at<uchar>(i,j)<50 && abs(i-temp_D.rows/2)<temp_D.rows/6 && abs(j-temp_D.cols/2)<temp_D.cols/6)
					{
						sum_t=sum_t+((temp_D.at<uchar>(i,j)+1)*(40-temp_D.at<uchar>(i,j)));
						n_t++;
					}
				}
			}
			float Val6=sum_t/(n_t);
			if(Val6<=0 && (float)t_width1/t_height1>1.05)
			{
				contours_kongdong[i].clear();continue;
			}
		}

		//2017-01-5日为限制头尾及边
		//3、斜边
		cv::Mat srcimg_Cutimg_P2D_part;
		srcimg_Cutimg_P2D(rect_kongdong).copyTo(srcimg_Cutimg_P2D_part);
		//cv::namedWindow("srcimg_Cutimg_P2D_part", 0 );
		//cv::imshow( "srcimg_Cutimg_P2D_part", srcimg_Cutimg_P2D_part );
		float part_left_top=0;
		float part_right_bottom=0;
		float part_left_bottom=0;			
		float part_right_top=0;
		float f_k=(float)srcimg_Cutimg_P2D_part.rows/srcimg_Cutimg_P2D_part.cols;
		for(int m=0;m<srcimg_Cutimg_P2D_part.rows;m++)
		{
			for(int n=0;n<srcimg_Cutimg_P2D_part.cols;n++)
			{
				if(m>(f_k*n+srcimg_Cutimg_P2D_part.rows*0.5))
				{
					part_left_top+=srcimg_Cutimg_P2D_part.at<uchar>(m,n);
				}
				else if(m<(f_k*n-srcimg_Cutimg_P2D_part.rows*0.5))
				{
					part_right_bottom+=srcimg_Cutimg_P2D_part.at<uchar>(m,n);
				}

				if(m<(srcimg_Cutimg_P2D_part.rows*0.5-f_k*n))
				{
					part_right_top+=srcimg_Cutimg_P2D_part.at<uchar>(m,n);
				}
				else if(m>(srcimg_Cutimg_P2D_part.rows*1.5-f_k*n))
				{
					part_left_bottom+=srcimg_Cutimg_P2D_part.at<uchar>(m,n);
				}
			}
		}
		float f_e_1=(part_left_top>part_right_bottom)?(part_right_bottom/(part_left_top+0.000001)):(part_left_top/(part_right_bottom+0.00001));
		float f_e_2=(part_right_top>part_left_bottom)?(part_left_bottom/(part_right_top+0.000001)):(part_right_top/(part_left_bottom+0.00001));
		if(f_e_1<0.15 || f_e_2<0.15)
		{
			continue;
		}
		//4、图像过黑
		cv::Mat srcimg_Cutimg_P2D_part_mean,srcimg_Cutimg_P2D_part_dev;
		cv::meanStdDev(srcimg_Cutimg_P2D_part,srcimg_Cutimg_P2D_part_mean,srcimg_Cutimg_P2D_part_dev);
		float f_srcimg_Cutimg_P2D_part_mean=srcimg_Cutimg_P2D_part_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P2D_part_dev=srcimg_Cutimg_P2D_part_dev.at<double>(0,0);
		if(f_srcimg_Cutimg_P2D_part_mean<10)
		{
			continue;
		}

		///

		pSet->Items[pSet->iItemNum].iConfidence=0.5;
		pSet->Items[pSet->iItemNum].iDefectClass=9;
		pSet->Items[pSet->iItemNum].iLeft=(boundRect_kongdong[i].tl().x+Left_Edge)*2;
		pSet->Items[pSet->iItemNum].iTop=boundRect_kongdong[i].tl().y*2;
		pSet->Items[pSet->iItemNum].iRight=(boundRect_kongdong[i].br().x+Left_Edge)*2;
		pSet->Items[pSet->iItemNum].iBottom=boundRect_kongdong[i].br().y*2;
		pSet->iItemNum++;
	}
}
void H_SteelImageProcess::Defects_Scarring_OpenCV_New(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo) 
{
	//尺寸偶和
	if(Left_Edge%2 != 0)
	{
		Left_Edge++;
	}
	if(Right_Edge%2 != 0)
	{
		Right_Edge--;
	}

	//去除水印
	if(Right_Edge-Left_Edge>100 && Left_Edge>=0)
	{
		cv::Mat srcimg_Cutimg_P2D_mean,srcimg_Cutimg_P2D_dev;
		cv::meanStdDev(srcimg_Cutimg_P2D,srcimg_Cutimg_P2D_mean,srcimg_Cutimg_P2D_dev);
		float f_srcimg_Cutimg_P2D_mean=srcimg_Cutimg_P2D_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P2D_dev=srcimg_Cutimg_P2D_dev.at<double>(0,0);
		int a=0;
		//cv::Mat imageLog(srcimg_GCG_Cutimg_P2D.size(),CV_32FC1);
		//for(int i=0;i<srcimg_GCG_Cutimg_P2D.rows;i++)
		//{
		//	for(int j=0;j<srcimg_GCG_Cutimg_P2D.cols;j++)
		//	{
		//		imageLog.at<float>(i,j)=log(1+(float)srcimg_GCG_Cutimg_P2D.at<uchar>(i,j));		
		//	}
		//}
		////归一化到0~255
		//normalize(imageLog,imageLog,0,255,CV_MINMAX);
		////转换成8bit图像显示
		//convertScaleAbs(imageLog,imageLog);
		//cv::namedWindow("imageLog",0);
		//imshow("imageLog",imageLog);
		//cv::Mat imageLog_x;
		//H_BaseImageProcess::Gray_Change_X(imageLog,imageLog_x,0,10,false);
		//cv::namedWindow("imageLog_x",0);
		//cv::imshow("imageLog_x",imageLog_x);
		////
		//cv::medianBlur(imageLog_x,imageLog_x,3);
		//cv::Mat mean1,mean2,dst1,dst2,dst3,dst4;
		//cv::GaussianBlur(imageLog_x,mean1,cv::Size(5,5),0);
		//cv::GaussianBlur(imageLog_x,mean2,cv::Size(11,11),0);
		//cv::subtract(imageLog_x,mean1,dst1);
		//cv::subtract(imageLog_x,mean2,dst2);
		//cv::threshold(dst1,dst1,2,255,0);
		//cv::threshold(dst2,dst2,2,255,0);
		//cv::addWeighted(dst1,0.5,dst2,0.5,0,dst3);
		//cv::threshold(dst3,dst3,100,255,0);
		//cv::Mat element_dst3 = getStructuringElement( cv::MORPH_ELLIPSE,cv::Size(15, 15),cv::Point( -1,-1 ) );
		//cv::morphologyEx(dst3,dst3,4,element_dst3);
		//cv::Canny(srcimg_GCG_Cutimg_P2D,dst4,80,150);
		//cv::morphologyEx(dst4,dst4,4,element_dst3);
		//cv::namedWindow("dst3",0);
		//cv::imshow("dst3",dst3);
		//cv::namedWindow("dst4",0);
		//cv::imshow("dst4",dst4);
		//cv::Mat dst5;
		//cv::addWeighted(dst3,0.5,dst4,0.5,0,dst5);
		//cv::threshold(dst5,dst5,150,255,0);
		//cv::namedWindow("dst5",0);
		//cv::imshow("dst5",dst5);
		///积分图
		cv::Mat integral_img(srcimg_GCG_Cutimg_P2D.rows+1,srcimg_GCG_Cutimg_P2D.cols+1,CV_64FC1);
		integral(srcimg_GCG_Cutimg_P2D,integral_img,CV_64FC1);//积分图像
		//cv::Mat integral_img_norm;
		//normalize(integral_img,integral_img_norm,0,255,CV_MINMAX);
		//convertScaleAbs(integral_img_norm,integral_img_norm);
		//#ifdef W_DEBUG
		//	cv::namedWindow("integral_img_norm",0);
		//	imshow("integral_img_norm",integral_img_norm);
		//#endif
		///计算矫正因子
		int block_x=10;
		int block_y=6;
		int baseGray=100;
		cv::Mat dst_zero=cv::Mat::zeros(srcimg_GCG_Cutimg_P2D.rows,srcimg_GCG_Cutimg_P2D.cols,srcimg_GCG_Cutimg_P2D.type());
		for(int i=block_y/2;i<integral_img.rows-block_y/2;i++)
		{
			for(int j=block_x/2;j<integral_img.cols-block_x/2;j++)
			{
				float A=integral_img.at<double>(i+block_y/2,j+block_x/2);
				float B=integral_img.at<double>(i-block_y/2,j-block_x/2);
				float C=integral_img.at<double>(i-block_y/2,j+block_x/2);
				float D=integral_img.at<double>(i+block_y/2,j-block_x/2);
				float temp=A+B-C-D;
				float x=temp/(block_x*block_y);
				float _x=1/(x+0.000001);
				dst_zero.at<uchar>(i,j)=min(255,srcimg_GCG_Cutimg_P2D.at<uchar>(i,j)*baseGray*_x);
			}
		}
		///再次纵向矫正
		//cv::Mat dst_zero_gcg;
		//H_BaseImageProcess::Gray_Change_Y(dst_zero,dst_zero_gcg,5,100);
		#ifdef W_DEBUG
			cv::namedWindow("dst_zero",0);
			cv::imshow("dst_zero",dst_zero);
			//cv::namedWindow("dst_zero_gcg",0);
			//cv::imshow("dst_zero_gcg",dst_zero_gcg);
		#endif
		//
		//cv::Mat mean1,mean2,dst1,dst2,dst3,dst4;
		//cv::GaussianBlur(dst_zero_gcg,mean1,cv::Size(15,15),0);
		//cv::GaussianBlur(dst_zero_gcg,mean2,cv::Size(25,25),0);
		//cv::subtract(dst_zero_gcg,mean1,dst1);
		//cv::subtract(dst_zero_gcg,mean2,dst2);
		//cv::threshold(dst_zero_gcg,dst1,60,255,1);
		//cv::threshold(dst2,dst2,30,255,0);
		//cv::namedWindow("dst1",0);
		//cv::imshow("dst1",dst1);
		//cv::namedWindow("dst2",0);
		//cv::imshow("dst2",dst2);
		//
		
		//截取
		cv::Mat dst_zero_th;
		cv::threshold(dst_zero,dst_zero_th,50,255,0);
		cv::Mat dst_zero_th_ero;
		cv::Mat element_dst_zero= getStructuringElement( cv::MORPH_RECT,cv::Size( 5, 5 ));
		cv::erode(dst_zero_th,dst_zero_th_ero,element_dst_zero);
		cv::Mat canny_judge;
		cv::Canny(dst_zero_th_ero,canny_judge,150,250);
		cv::Mat element_canny_judge= getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
		dilate( canny_judge, canny_judge, element_canny_judge );
		cv::Mat cdst = cv::Mat::zeros( canny_judge.size(), CV_8UC1 );	
		cv::vector<cv::Vec4i> lines;
		HoughLinesP(canny_judge, lines, 1, CV_PI/180, 50, canny_judge.cols/4, 0);//10,10,1
		int topLeft=10000;
		int topRight=0;
		int bottomLeft=10000;
		int bottomRight=0;
		for( size_t i = 0; i < lines.size(); i++ )
		{
			cv::Vec4i l = lines[i];
			float width=abs(l[2]-l[0]);
			float height=abs(l[3]- l[1]);
			if(width/(height+0.01)>canny_judge.cols/2)
			{
				line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
			}
			if(abs(l[3]+l[1])/2<block_y)
			{
				if(min(l[2],l[0])<topLeft)
				{
					topLeft=min(l[2],l[0]);
				}
				if(max(l[2],l[0])>topRight)
				{
					topRight=max(l[2],l[0]);
				}
			}
			if(abs(l[3]+l[1])/2>canny_judge.rows-block_y)
			{
				if(min(l[2],l[0])<bottomLeft)
				{
					bottomLeft=min(l[2],l[0]);
				}
				if(max(l[2],l[0])>bottomRight)
				{
					bottomRight=max(l[2],l[0]);
				}
			}
		}
		int maxLeft=max(topLeft,bottomLeft)+block_x;
		int minRight=min(topRight,bottomRight)-block_x;
		cv::line(srcimg_Cutimg_P2D,cv::Point(max(topLeft,bottomLeft),0),cv::Point(max(topLeft,bottomLeft),srcimg_Cutimg_P2D.rows),255);
		cv::line(srcimg_Cutimg_P2D,cv::Point(min(topRight,bottomRight),0),cv::Point(min(topRight,bottomRight),srcimg_Cutimg_P2D.rows),255);
		#ifdef W_DEBUG
			cv::namedWindow("dst_zero_th",0);
			cv::imshow("dst_zero_th",dst_zero_th);
			cv::namedWindow("canny_judge",0);
			cv::imshow("canny_judge",canny_judge);
			cv::namedWindow("cdst",0);
			cv::imshow("cdst",cdst);
		#endif
		
		cv::Mat dst_zero_cut;
		cv::Rect rect_in(block_x,block_y,srcimg_GCG_Cutimg_P2D.cols-2*block_x,srcimg_GCG_Cutimg_P2D.rows-2*block_y);
		dst_zero(rect_in).copyTo(dst_zero_cut);
		cv::Mat dst_zero_cut_mean,dst_zero_cut_dev;
		cv::meanStdDev(dst_zero_cut,dst_zero_cut_mean,dst_zero_cut_dev);
		float f_dst_zero_cut_dev=dst_zero_cut_dev.at<double>(0,0);
		float th=200;
		th=max(f_dst_zero_cut_dev*20,200);
		th=min(f_dst_zero_cut_dev*20,350);

		//矫正图上
		cv::Mat dst_canny,dst_canny_cut,dst_canny_cut_dil;
		cv::medianBlur(dst_zero,dst_zero,5);
		cv::Canny(dst_zero,dst_canny,th,th*1.25);
		dst_canny(rect_in).copyTo(dst_canny_cut);
		int dilation_size_jieba_dst = 6;
		cv::Mat element_jieba_dst= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_jieba_dst + 1, 2*dilation_size_jieba_dst+1 ));
		dilate( dst_canny_cut, dst_canny_cut_dil, element_jieba_dst );
		#ifdef W_DEBUG
			cv::namedWindow("dst_canny",0);
			cv::imshow("dst_canny",dst_canny);
			cv::namedWindow("dst_canny_cut_dil",0);
			cv::imshow("dst_canny_cut_dil",dst_canny_cut_dil);
		#endif
		//原图上
		cv::Mat src_canny,src_canny_cut,src_canny_cut_dil;
		cv::Canny(srcimg_Cutimg_P2D,src_canny,12,25,3);
		src_canny(rect_in).copyTo(src_canny_cut);
		int dilation_size_jieba_src = 3;
		cv::Mat element_jieba_src= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_jieba_src + 1, 2*dilation_size_jieba_src+1 ));
		dilate( src_canny_cut, src_canny_cut_dil, element_jieba_src );
		#ifdef W_DEBUG
			cv::namedWindow("src_canny",0);
			cv::imshow("src_canny",src_canny);
			cv::namedWindow("src_canny_cut_dil",0);
			cv::imshow("src_canny_cut_dil",src_canny_cut_dil);
		#endif
		//合并
		cv::Mat dst;
		cv::addWeighted(src_canny_cut_dil,0.5,dst_canny_cut_dil,0.5,0,dst);
		cv::threshold(dst,dst,150,255,0);
		#ifdef W_DEBUG
			cv::namedWindow("dst",0);
			cv::imshow("dst",dst);
		#endif
		//
		cv::Mat dst1,dst2,dst3;
		cv::threshold(dst_zero_cut,dst1,60,255,1);
		cv::threshold(dst_zero_cut,dst2,200,255,0);
		cv::addWeighted(dst1,0.5,dst2,0.5,0,dst3);
		cv::threshold(dst3,dst3,100,255,0);
		cv::addWeighted(dst,0.5,dst3,0.5,0,dst);
		cv::threshold(dst,dst,150,255,0);

		//膨胀
		cv::Mat dst_dil;
		int dilation_size_jieba = 5;
		cv::Mat element_jieba= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_jieba + 1, 2*dilation_size_jieba+1 ));
		dilate( dst, dst_dil, element_jieba );
		//画缺陷
		cv::vector<cv::vector<cv::Point> > contours;
		cv::vector<cv::Vec4i> hierarchy;
		findContours( dst_dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
		cv::vector<cv::Rect> boundRect( contours.size() );
		cv::vector<cv::RotatedRect> minRect( contours.size() ); 
		cv::Mat drawing = cv::Mat::zeros( dst_dil.rows,dst_dil.cols, CV_8UC1 );
		int iScale_x=5;
		int iScale_y=10;
		for( int i = 0; i< contours.size(); i++ )
		{
			approxPolyDP( cv::Mat(contours[i]), contours[i], 3, true );
			boundRect[i] = boundingRect( cv::Mat(contours[i]) );
			double tmparea=fabs(contourArea(contours[i])); 
			if(tmparea<100)
			{
				continue;
			}
			int left=max(0,boundRect[i].tl().x-iScale_x);
			int right=min(drawing.cols-1,boundRect[i].br().x+iScale_x);
			int top=max(0,boundRect[i].tl().y-iScale_y);
			int bottom=min(drawing.rows-1,boundRect[i].br().y+iScale_y);
			int xCenter=(left+right)/2;
			int yCenter=(top+bottom)/2;
			rectangle( drawing, cv::Point(left,top), cv::Point(right,bottom), 255, -2, 8, 0 );
			#ifdef W_DEBUG
				if(xCenter>maxLeft && xCenter<minRight)
				{
					cv::rectangle(srcimg_Cutimg_P2D,cv::Point(boundRect[i].tl().x+block_x,boundRect[i].tl().y+block_y),cv::Point(boundRect[i].br().x+block_x,boundRect[i].br().y+block_y),255,2);
				}
			#endif
		}
		#ifdef W_DEBUG
			cv::namedWindow("srcimg_Cutimg_P2D",0);
			cv::imshow("srcimg_Cutimg_P2D",srcimg_Cutimg_P2D);
		#endif

		//写缺陷
		cv::vector<cv::vector<cv::Point> > contours_big;
		cv::vector<cv::Vec4i> hierarchy_big;
		cv::findContours( drawing, contours_big, hierarchy_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::vector<cv::vector<cv::Point> > contours_poly_big( contours_big.size() );
		cv::vector<cv::Rect> boundRect_big( contours_big.size() );
		for( int k = 0; k < contours_big.size(); k++ )
		{ 
			approxPolyDP( cv::Mat(contours_big[k]), contours_poly_big[k], 3, true );
			boundRect_big[k] = cv::boundingRect( cv::Mat(contours_poly_big[k]) );
			//
			int d_left=max(0,boundRect_big[k].tl().x);
			int d_right=min(drawing.cols-1,boundRect_big[k].br().x);
			int d_top=max(0,boundRect_big[k].tl().y);
			int d_bottom=min(drawing.rows-1,boundRect_big[k].br().y);
			int xCenter=(d_left+d_right)/2;
			int yCenter=(d_top+d_bottom)/2;
			
			float width=abs(d_left-d_right);
			float height=abs(d_top- d_bottom);
			float area=width*height;
			if(area<500)
			{
				continue;
			}
			//
			if(xCenter>maxLeft && xCenter<minRight)
			{
				//保存图像
				cv::Rect rect_part(d_left,d_top,width,height);
				cv::Mat dst_part;
				dst_zero_cut(rect_part).copyTo(dst_part);
				H_BaseImageProcess::Save_Image_OpenCV(dst_part,ImgIndex*10000+pSet->iItemNum);
				//
				pSet->Items[pSet->iItemNum].iDefectClass=12;
				pSet->Items[pSet->iItemNum].iConfidence=0.5;
				pSet->Items[pSet->iItemNum].iLeft=(d_left+block_x+Left_Edge)*2;
				pSet->Items[pSet->iItemNum].iTop=(d_top+block_y)*2;
				pSet->Items[pSet->iItemNum].iRight=(d_right+block_x+Left_Edge)*2;
				pSet->Items[pSet->iItemNum].iBottom=(d_bottom+block_y)*2;
				pSet->iItemNum++;
			}
		}
	}
}
void H_SteelImageProcess::Defects_Scarring_OpenCV_New_P2D(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool& isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo) 
{
	//尺寸偶和
	if(Left_Edge%2 != 0)
	{
		Left_Edge++;
	}
	if(Right_Edge%2 != 0)
	{
		Right_Edge--;
	}
	int Left_Edge_4=Left_Edge/2;
	int Right_Edge_4=Right_Edge/2;

	//进入检测
	//下采样
	cv::Mat srcimg_GCG_Cutimg_P4D,srcimg_Cutimg_P4D;
	cv::pyrDown(srcimg_GCG_Cutimg_P2D,srcimg_GCG_Cutimg_P4D,cv::Size(srcimg_GCG_Cutimg_P2D.cols/2,srcimg_GCG_Cutimg_P2D.rows/2));
	cv::pyrDown(srcimg_Cutimg_P2D,srcimg_Cutimg_P4D,cv::Size(srcimg_Cutimg_P2D.cols/2,srcimg_Cutimg_P2D.rows/2));	
	//积分图
	cv::Mat integral_img(srcimg_GCG_Cutimg_P4D.rows+1,srcimg_GCG_Cutimg_P4D.cols+1,CV_64FC1);
	integral(srcimg_GCG_Cutimg_P4D,integral_img,CV_64FC1);//积分图像
	//计算矫正因子,得到矫正图
	int block_x=6;
	int block_y=4;
	int baseGray=100;
	cv::Mat dst_zero=cv::Mat::zeros(srcimg_GCG_Cutimg_P4D.rows,srcimg_GCG_Cutimg_P4D.cols,srcimg_GCG_Cutimg_P4D.type());
	for(int i=block_y/2;i<integral_img.rows-block_y/2;i++)
	{
		for(int j=block_x/2;j<integral_img.cols-block_x/2;j++)
		{
			float A=integral_img.at<double>(i+block_y/2,j+block_x/2);
			float B=integral_img.at<double>(i-block_y/2,j-block_x/2);
			float C=integral_img.at<double>(i-block_y/2,j+block_x/2);
			float D=integral_img.at<double>(i+block_y/2,j-block_x/2);
			float temp=A+B-C-D;
			float x=temp/(block_x*block_y);
			float _x=1/(x+0.000001);
			dst_zero.at<uchar>(i,j)=min(255,srcimg_GCG_Cutimg_P4D.at<uchar>(i,j)*baseGray*_x);
		}
	}
	#ifdef W_DEBUG
		cv::namedWindow("dst_zero",0);
		cv::imshow("dst_zero",dst_zero);
	#endif	
	//切黑边
	cv::Mat dst_zero_cut,srcimg_Cutimg_P4D_cut;
	cv::Rect rect_in(block_x,block_y,srcimg_GCG_Cutimg_P4D.cols-2*block_x,srcimg_GCG_Cutimg_P4D.rows-2*block_y);
	dst_zero(rect_in).copyTo(dst_zero_cut);
	srcimg_Cutimg_P4D(rect_in).copyTo(srcimg_Cutimg_P4D_cut);
	//
	
	//验证图像有效区域，并滤除无效图像（主要为有大量水的图像）
	cv::Mat dst_zero_th;
	cv::threshold(dst_zero,dst_zero_th,50,255,0);
	int NoZeroNum=cv::countNonZero(dst_zero_th);
	float e_num=(float)NoZeroNum/(dst_zero_th.cols*dst_zero_th.rows-dst_zero_th.cols*block_y-dst_zero_th.rows*block_x);
	bool isInvalid=(e_num<0.99)? true : false;
	//cv::Mat dst_zero_th_ero;
	//cv::Mat element_dst_zero= getStructuringElement( cv::MORPH_RECT,cv::Size( 5, 5 ));
	//cv::erode(dst_zero_th,dst_zero_th_ero,element_dst_zero);
	//cv::Mat canny_judge;
	//cv::Canny(dst_zero_th_ero,canny_judge,150,250);
	//cv::Mat element_canny_judge= getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
	//dilate( canny_judge, canny_judge, element_canny_judge );
	//cv::Mat cdst = cv::Mat::zeros( canny_judge.size(), CV_8UC1 );	
	//cv::vector<cv::Vec4i> lines;
	//HoughLinesP(canny_judge, lines, 1, CV_PI/180, 20, canny_judge.cols*0.15, 0);//10,10,1
	//int topLeft=10000;
	//int topRight=0;
	//int bottomLeft=10000;
	//int bottomRight=0;
	//for( size_t i = 0; i < lines.size(); i++ )
	//{
	//	cv::Vec4i l = lines[i];
	//	float width=abs(l[2]-l[0]);
	//	float height=abs(l[3]- l[1]);
	//	if(width/(height+0.01)>canny_judge.cols*0.15)
	//	{
	//		line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
	//	}
	//	if(abs(l[3]+l[1])/2<block_y*2)
	//	{
	//		if(min(l[2],l[0])<topLeft)
	//		{
	//			topLeft=min(l[2],l[0]);
	//		}
	//		if(max(l[2],l[0])>topRight)
	//		{
	//			topRight=max(l[2],l[0]);
	//		}
	//	}
	//	if(abs(l[3]+l[1])/2>canny_judge.rows-block_y*2)
	//	{
	//		if(min(l[2],l[0])<bottomLeft)
	//		{
	//			bottomLeft=min(l[2],l[0]);
	//		}
	//		if(max(l[2],l[0])>bottomRight)
	//		{
	//			bottomRight=max(l[2],l[0]);
	//		}
	//	}
	//}
	//int maxLeft=max(topLeft,bottomLeft)+block_x;
	//int minRight=min(topRight,bottomRight)-block_x;
	
	#ifdef W_DEBUG
		//cv::line(srcimg_Cutimg_P4D,cv::Point(max(topLeft,bottomLeft),0),cv::Point(max(topLeft,bottomLeft),srcimg_Cutimg_P4D.rows),255);
		//cv::line(srcimg_Cutimg_P4D,cv::Point(min(topRight,bottomRight),0),cv::Point(min(topRight,bottomRight),srcimg_Cutimg_P4D.rows),255);
		//cv::namedWindow("dst_zero_th",0);
		//cv::imshow("dst_zero_th",dst_zero_th);
		//cv::namedWindow("canny_judge",0);
		//cv::imshow("canny_judge",canny_judge);
		//cv::namedWindow("cdst",0);
		//cv::imshow("cdst",cdst);
	#endif
	
	//切完黑边后判断是否为头
	//int interval_x=6;
	//int interval_y=10;
	//bool isHead=false;
	//for(int i=0;i<dst_zero_cut.cols/interval_x;i++)
	//{
	//	cv::Rect rect_i_top(i*interval_x,0,interval_x,interval_y);
	//	cv::Mat temp_i_top;
	//	dst_zero_cut(rect_i_top).copyTo(temp_i_top);
	//	cv::Mat temp_i_top_mean,temp_i_top_dev;
	//	cv::meanStdDev(temp_i_top,temp_i_top_mean,temp_i_top_dev);
	//	float fdev_top=temp_i_top_dev.at<double>(0,0);
	//	if(fdev_top<0.000001)
	//	{
	//		for(int j=0;j<dst_zero_cut.rows/interval_y;j++)
	//		{
	//			cv::Rect rect_i_top_j(i*interval_x,j*interval_y,interval_x,interval_y);
	//			cv::Mat temp_i_top_j;
	//			dst_zero_cut(rect_i_top_j).copyTo(temp_i_top_j);
	//			cv::Mat temp_i_top_j_mean,temp_i_top_j_dev;
	//			cv::meanStdDev(temp_i_top_j,temp_i_top_j_mean,temp_i_top_j_dev);
	//			float fdev_top_j=temp_i_top_j_dev.at<double>(0,0);
	//			if(fdev_top_j>20)
	//			{
	//				isHead=true;
	//				break;
	//			}
	//		}
	//	}
	//	cv::Rect rect_i_bottom(i*interval_x,dst_zero_cut.rows-1-interval_y,interval_x,interval_y);
	//	cv::Mat temp_i_bottom;
	//	dst_zero_cut(rect_i_bottom).copyTo(temp_i_bottom);
	//	cv::Mat temp_i_bottom_mean,temp_i_bottom_dev;
	//	cv::meanStdDev(temp_i_bottom,temp_i_bottom_mean,temp_i_bottom_dev);
	//	float fdev_bottom=temp_i_bottom_dev.at<double>(0,0);
	//	if(fdev_bottom<0.000001)
	//	{
	//		for(int j=dst_zero_cut.rows/interval_y;j>1;j--)
	//		{
	//			cv::Rect rect_i_bottom_j(i*interval_x,(j-1)*interval_y,interval_x,interval_y);
	//			cv::Mat temp_i_bottom_j;
	//			dst_zero_cut(rect_i_bottom_j).copyTo(temp_i_bottom_j);
	//			cv::Mat temp_i_bottom_j_mean,temp_i_bottom_j_dev;
	//			cv::meanStdDev(temp_i_bottom_j,temp_i_bottom_j_mean,temp_i_bottom_j_dev);
	//			float fdev_bottom_j=temp_i_bottom_j_dev.at<double>(0,0);
	//			if(fdev_bottom_j>20)
	//			{
	//				isHead=true;
	//				break;
	//			}
	//		}
	//	}
	//}
	//进行缺陷阈值分割
	//1、sobel图
	int ddepth=CV_16S;
	int delta=0;
	int scale=1;
	cv::Mat dst_sobel_cut;
	cv::Mat grad_x, grad_xx;
	cv::Mat abs_grad_x, abs_grad_xx;
	cv::Sobel( dst_zero_cut, grad_x, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x, abs_grad_x );
	cv::Sobel( dst_zero_cut, grad_xx, ddepth, 0, 2, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_xx, abs_grad_xx );
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_xx, 0.5, 0, dst_sobel_cut );
	cv::Mat dst_sobel_cut_mean,dst_sobel_cut_dev;
	cv::meanStdDev(dst_sobel_cut,dst_sobel_cut_mean,dst_sobel_cut_dev);
	float f_dst_sobel_cut_mean=dst_sobel_cut_mean.at<double>(0,0);
	float th=200;
	th=max(f_dst_sobel_cut_mean*10,120);
	th=min(f_dst_sobel_cut_mean*10,225);		
	cv::Mat dst_sobel_th;
	cv::threshold(dst_sobel_cut,dst_sobel_th,th,255,0);
	cv::Mat dst_sobel_th_dil;
	int dilation_size_dst_sobel_th = 4;
	cv::Mat element_dst_sobel_th= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_dst_sobel_th + 1, 2*dilation_size_dst_sobel_th+1 ));
	dilate( dst_sobel_th, dst_sobel_th_dil, element_dst_sobel_th );		
	#ifdef W_DEBUG
		cv::namedWindow("dst_sobel_cut",0);
		cv::imshow("dst_sobel_cut",dst_sobel_cut);
		cv::namedWindow("dst_sobel_th",0);
		cv::imshow("dst_sobel_th",dst_sobel_th);
		cv::namedWindow("dst_sobel_th_dil",0);
		cv::imshow("dst_sobel_th_dil",dst_sobel_th_dil);
	#endif
	//2、原图上
	cv::Mat src_canny_cut,src_canny_cut_dil;
	cv::Canny(srcimg_Cutimg_P4D_cut,src_canny_cut,12,25,3);
	int dilation_size_src_canny_cut = 2;
	cv::Mat element_src_canny_cut= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_src_canny_cut + 1, 2*dilation_size_src_canny_cut+1 ));
	dilate( src_canny_cut, src_canny_cut_dil, element_src_canny_cut );
	#ifdef W_DEBUG
		cv::namedWindow("src_canny_cut",0);
		cv::imshow("src_canny_cut",src_canny_cut);
		cv::namedWindow("src_canny_cut_dil",0);
		cv::imshow("src_canny_cut_dil",src_canny_cut_dil);
	#endif
	//3、canny上
	//cv::Mat dst_zero_cut_mean,dst_zero_cut_dev;
	//cv::meanStdDev(dst_zero_cut,dst_zero_cut_mean,dst_zero_cut_dev);
	//float f_dst_zero_cut_dev=dst_zero_cut_dev.at<double>(0,0);
	//float th_canny=200;
	//th_canny=max(f_dst_zero_cut_dev*30,180);
	//th_canny=min(f_dst_zero_cut_dev*30,350);
	//cv::Mat dst_canny_cut,dst_canny_cut_dil;
	//cv::Canny(dst_zero_cut,dst_canny_cut,th_canny,th_canny*1.25);
	//#ifdef W_DEBUG
	//	cv::namedWindow("dst_canny_cut",0);
	//	cv::imshow("dst_canny_cut",dst_canny_cut);
	//#endif
	//合并
	cv::Mat dst;
	cv::addWeighted(src_canny_cut_dil,0.5,dst_sobel_th_dil,0.5,0,dst);
	cv::threshold(dst,dst,150,255,0);
	#ifdef W_DEBUG
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	#endif
	//
	cv::Mat dst1,dst2,_dst;
	cv::threshold(dst_zero_cut,dst1,75,255,1);
	cv::threshold(dst_zero_cut,dst2,200,255,0);
	cv::addWeighted(dst1,0.5,dst2,0.5,0,_dst);
	cv::threshold(_dst,_dst,100,255,0);
	#ifdef W_DEBUG
		cv::namedWindow("_dst",0);
		cv::imshow("_dst",_dst);
	#endif
	cv::addWeighted(dst,0.5,_dst,0.5,0,dst);
	cv::threshold(dst,dst,150,255,0);

	//测头尾
	isHead=false;
	cv::Mat cdst_head = cv::Mat::zeros( dst_sobel_th.size(), CV_8UC1 );	
	cv::vector<cv::Vec4i> lines_head;
	HoughLinesP(dst_sobel_th, lines_head, 1, CV_PI/180, 10, 100, 50);//10,100,50
	for( size_t i = 0; i < lines_head.size(); i++ )
	{
		cv::Vec4i l = lines_head[i];
		float width=abs(l[2]-l[0]);
		float height=abs(l[3]- l[1]);
		if(width/(height+0.000001)>10)
		{
			line( cdst_head, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
			isHead=true;
		}
	}
	#ifdef W_DEBUG
		cv::namedWindow("cdst_head",0);
		cv::imshow("cdst_head",cdst_head);
	#endif
	//膨胀
	cv::Mat dst_dil;
	int dilation_size_dst = 4;
	cv::Mat element_dst= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_dst + 1, 2*dilation_size_dst+1 ));
	dilate( dst, dst_dil, element_dst );
	//画缺陷
	cv::vector<cv::vector<cv::Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;
	findContours( dst_dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
	cv::vector<cv::Rect> boundRect( contours.size() );
	cv::vector<cv::RotatedRect> minRect( contours.size() ); 
	cv::Mat drawing = cv::Mat::zeros( dst_dil.rows,dst_dil.cols, CV_8UC1 );
	int iScale_x=2;
	int iScale_y=3;
	for( int i = 0; i< contours.size(); i++ )
	{
		approxPolyDP( cv::Mat(contours[i]), contours[i], 3, true );
		boundRect[i] = boundingRect( cv::Mat(contours[i]) );
		double tmparea=fabs(contourArea(contours[i])); 
		if(tmparea<100)
		{
			continue;
		}
		int left=max(0,boundRect[i].tl().x-iScale_x);
		int right=min(drawing.cols-1,boundRect[i].br().x+iScale_x);
		int top=max(0,boundRect[i].tl().y-iScale_y);
		int bottom=min(drawing.rows-1,boundRect[i].br().y+iScale_y);
		int xCenter=(left+right)/2;
		int yCenter=(top+bottom)/2;
		rectangle( drawing, cv::Point(left,top), cv::Point(right,bottom), 255, -2, 8, 0 );
		#ifdef W_DEBUG
			//if(xCenter>maxLeft && xCenter<minRight)
			//{
			//	cv::rectangle(srcimg_Cutimg_P4D,cv::Point(boundRect[i].tl().x+block_x,boundRect[i].tl().y+block_y),cv::Point(boundRect[i].br().x+block_x,boundRect[i].br().y+block_y),255,2);
			//}
		#endif
	}
	#ifdef W_DEBUG
		if(isInvalid)
		{
			cv::putText(srcimg_Cutimg_P4D,"Invalid",cv::Point(srcimg_Cutimg_P4D.cols*0.25,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		else
		{
			cv::putText(srcimg_Cutimg_P4D,"Valid",cv::Point(srcimg_Cutimg_P4D.cols*0.25,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		if(isHead)
		{
			cv::putText(srcimg_Cutimg_P4D,"Head",cv::Point(srcimg_Cutimg_P4D.cols*0.75,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		else
		{
			cv::putText(srcimg_Cutimg_P4D,"NoHead",cv::Point(srcimg_Cutimg_P4D.cols*0.75,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		cv::namedWindow("srcimg_Cutimg_P4D",0);
		cv::imshow("srcimg_Cutimg_P4D",srcimg_Cutimg_P4D);
	#endif

	//写缺陷
	cv::vector<cv::vector<cv::Point> > contours_big;
	cv::vector<cv::Vec4i> hierarchy_big;
	cv::findContours( drawing, contours_big, hierarchy_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	cv::vector<cv::vector<cv::Point> > contours_poly_big( contours_big.size() );
	cv::vector<cv::Rect> boundRect_big( contours_big.size() );
	for( int k = 0; k < contours_big.size(); k++ )
	{ 
		approxPolyDP( cv::Mat(contours_big[k]), contours_poly_big[k], 3, true );
		boundRect_big[k] = cv::boundingRect( cv::Mat(contours_poly_big[k]) );
		//
		int d_left=max(0,boundRect_big[k].tl().x);
		int d_right=min(drawing.cols-1,boundRect_big[k].br().x);
		int d_top=max(0,boundRect_big[k].tl().y);
		int d_bottom=min(drawing.rows-1,boundRect_big[k].br().y);
		int xCenter=(d_left+d_right)/2;
		int yCenter=(d_top+d_bottom)/2;
		
		float width=abs(d_left-d_right);
		float height=abs(d_top- d_bottom);
		float area=width*height;
		if(area<250 || true==isHead || true==isInvalid)
		{
			continue;
		}
		//去除残余边界
		//1、左右边
		cv::Mat srcimg_Cutimg_P4D_cut_part_left,srcimg_Cutimg_P4D_cut_part_right;
		cv::Rect rect_left(d_left,d_top,width*0.25,height);
		cv::Rect rect_right(d_left+width*0.75,d_top,width*0.25,height);
		srcimg_Cutimg_P4D_cut(rect_left).copyTo(srcimg_Cutimg_P4D_cut_part_left);
		srcimg_Cutimg_P4D_cut(rect_right).copyTo(srcimg_Cutimg_P4D_cut_part_right);
		#ifdef W_DEBUG
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_left",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_left",srcimg_Cutimg_P4D_cut_part_left);
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_right",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_right",srcimg_Cutimg_P4D_cut_part_right);
		#endif
		cv::Mat srcimg_Cutimg_P4D_cut_part_left_mean,srcimg_Cutimg_P4D_cut_part_left_dev;
		cv::Mat srcimg_Cutimg_P4D_cut_part_right_mean,srcimg_Cutimg_P4D_cut_part_right_dev;
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_left,srcimg_Cutimg_P4D_cut_part_left_mean,srcimg_Cutimg_P4D_cut_part_left_dev);
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_right,srcimg_Cutimg_P4D_cut_part_right_mean,srcimg_Cutimg_P4D_cut_part_right_dev);
		float f_srcimg_Cutimg_P4D_cut_part_left_mean=srcimg_Cutimg_P4D_cut_part_left_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P4D_cut_part_right_mean=srcimg_Cutimg_P4D_cut_part_right_mean.at<double>(0,0);
		float f_v_e=(f_srcimg_Cutimg_P4D_cut_part_left_mean>f_srcimg_Cutimg_P4D_cut_part_right_mean)?(f_srcimg_Cutimg_P4D_cut_part_right_mean/(f_srcimg_Cutimg_P4D_cut_part_left_mean+0.000001)):(f_srcimg_Cutimg_P4D_cut_part_left_mean/(f_srcimg_Cutimg_P4D_cut_part_right_mean+0.00001));
		if(f_v_e<0.3)
		{
			continue;
		}
		//2、上下边
		cv::Mat srcimg_Cutimg_P4D_cut_part_top,srcimg_Cutimg_P4D_cut_part_bottom;
		cv::Rect rect_top(d_left,d_top,width,height*0.25);
		cv::Rect rect_bottom(d_left,d_top+height*0.75,width,height*0.25);
		srcimg_Cutimg_P4D_cut(rect_top).copyTo(srcimg_Cutimg_P4D_cut_part_top);
		srcimg_Cutimg_P4D_cut(rect_bottom).copyTo(srcimg_Cutimg_P4D_cut_part_bottom);
		#ifdef W_DEBUG
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_top",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_top",srcimg_Cutimg_P4D_cut_part_top);
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_bottom",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_bottom",srcimg_Cutimg_P4D_cut_part_bottom);
		#endif
		cv::Mat srcimg_Cutimg_P4D_cut_part_top_mean,srcimg_Cutimg_P4D_cut_part_top_dev;
		cv::Mat srcimg_Cutimg_P4D_cut_part_bottom_mean,srcimg_Cutimg_P4D_cut_part_bottom_dev;
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_top,srcimg_Cutimg_P4D_cut_part_top_mean,srcimg_Cutimg_P4D_cut_part_top_dev);
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_bottom,srcimg_Cutimg_P4D_cut_part_bottom_mean,srcimg_Cutimg_P4D_cut_part_bottom_dev);
		float f_srcimg_Cutimg_P4D_cut_part_top_mean=srcimg_Cutimg_P4D_cut_part_top_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P4D_cut_part_bottom_mean=srcimg_Cutimg_P4D_cut_part_bottom_mean.at<double>(0,0);
		float f_h_e=(f_srcimg_Cutimg_P4D_cut_part_top_mean>f_srcimg_Cutimg_P4D_cut_part_bottom_mean)?(f_srcimg_Cutimg_P4D_cut_part_bottom_mean/(f_srcimg_Cutimg_P4D_cut_part_top_mean+0.000001)):(f_srcimg_Cutimg_P4D_cut_part_top_mean/(f_srcimg_Cutimg_P4D_cut_part_bottom_mean+0.00001));
		if(f_h_e<0.15)
		{
			continue;
		}
		//3、斜边
		cv::Mat srcimg_Cutimg_P4D_cut_part;
		cv::Rect rect(d_left,d_top,width,height);
		srcimg_Cutimg_P4D_cut(rect).copyTo(srcimg_Cutimg_P4D_cut_part);
		float part_left_top=0;
		float part_right_bottom=0;
		float part_left_bottom=0;			
		float part_right_top=0;
		float f_k=(float)srcimg_Cutimg_P4D_cut_part.rows/srcimg_Cutimg_P4D_cut_part.cols;
		for(int m=0;m<srcimg_Cutimg_P4D_cut_part.rows;m++)
		{
			for(int n=0;n<srcimg_Cutimg_P4D_cut_part.cols;n++)
			{
				if(m>(f_k*n+srcimg_Cutimg_P4D_cut_part.rows*0.5))
				{
					part_left_top+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}
				else if(m<(f_k*n-srcimg_Cutimg_P4D_cut_part.rows*0.5))
				{
					part_right_bottom+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}

				if(m<(srcimg_Cutimg_P4D_cut_part.rows*0.5-f_k*n))
				{
					part_right_top+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}
				else if(m>(srcimg_Cutimg_P4D_cut_part.rows*1.5-f_k*n))
				{
					part_left_bottom+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}
			}
		}
		float f_e_1=(part_left_top>part_right_bottom)?(part_right_bottom/(part_left_top+0.000001)):(part_left_top/(part_right_bottom+0.00001));
		float f_e_2=(part_right_top>part_left_bottom)?(part_left_bottom/(part_right_top+0.000001)):(part_right_top/(part_left_bottom+0.00001));
		if(f_e_1<0.15 || f_e_2<0.15)
		{
			continue;
		}
		//4、图像过黑
		cv::Mat srcimg_Cutimg_P4D_cut_part_mean,srcimg_Cutimg_P4D_cut_part_dev;
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part,srcimg_Cutimg_P4D_cut_part_mean,srcimg_Cutimg_P4D_cut_part_dev);
		float f_srcimg_Cutimg_P4D_cut_part_mean=srcimg_Cutimg_P4D_cut_part_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P4D_cut_part_dev=srcimg_Cutimg_P4D_cut_part_dev.at<double>(0,0);
		if(f_srcimg_Cutimg_P4D_cut_part_mean<10)
		{
			continue;
		}
		//
		//if(xCenter>maxLeft && xCenter<minRight)
		{
			//保存图像
			//cv::Rect rect_part(d_left,d_top,width,height);
			//cv::Mat dst_part;
			//dst_zero_cut(rect_part).copyTo(dst_part);
			//H_BaseImageProcess::Save_Image_OpenCV(dst_part,ImgIndex*10000+pSet->iItemNum);
			//
			pSet->Items[pSet->iItemNum].iDefectClass=9;
			pSet->Items[pSet->iItemNum].iConfidence=0.5;
			pSet->Items[pSet->iItemNum].iLeft=(d_left+block_x+Left_Edge_4)*4;
			pSet->Items[pSet->iItemNum].iTop=(d_top+block_y)*4;
			pSet->Items[pSet->iItemNum].iRight=(d_right+block_x+Left_Edge_4)*4;
			pSet->Items[pSet->iItemNum].iBottom=(d_bottom+block_y)*4;
			pSet->iItemNum++;
		}
	}
}

/************************检烂边************************/
void H_SteelImageProcess::Defects_BadSide_OpenCV(cv::Mat srcimg,int Left_Edge,int Right_Edge,bool isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo) 
{
	if(Left_Edge<200 || Right_Edge>srcimg.cols-200)
	{
		return;
	}
	cv::Mat edge_effect;
	cv::Mat img_cut_edge;
	GaussianBlur( srcimg, img_cut_edge, cv::Size( 3, 3), 0, 0 );
	int Avr_Col_Min_edge=255;
	int *Avr_Col_edge=new int[img_cut_edge.cols]; 
	for(int i=0;i<img_cut_edge.cols;i++)
	{
		Avr_Col_edge[i]=img_cut_edge.at<uchar>(img_cut_edge.rows/4,i)+img_cut_edge.at<uchar>(img_cut_edge.rows/2,i)+img_cut_edge.at<uchar>(img_cut_edge.rows*3/4,i);
		Avr_Col_edge[i]=(float)Avr_Col_edge[i]/3;
		if(Avr_Col_edge[i]<Avr_Col_Min_edge)
		{
			Avr_Col_Min_edge=Avr_Col_edge[i];
		}
	}
	int Cut_Line_edge=0;
	int Edge_Line_edge=0;
	bool IsLeft_edge=true;
	cv::Mat effect_cut_edge;
	Avr_Col_Min_edge=Avr_Col_Min_edge+15;
	cv::Mat kern_y;// = (Mat_<char>(1,5) <<-1,0,0,0,1);  
	if(Avr_Col_Min_edge<80)
	{
		int Index_min_edge=img_cut_edge.cols;
		int Index_max_edge=0;
		int Statistical_edge=0;
		for(int i=0;i<img_cut_edge.cols-16;i++)
		{
			if(Avr_Col_edge[i]>Avr_Col_Min_edge)
			{
				if(i<Index_min_edge&&Avr_Col_edge[i+16]>Avr_Col_Min_edge)
				{
					Index_min_edge=i;
				}
				if(i>Index_max_edge&&Avr_Col_edge[i+16]>Avr_Col_Min_edge)
				{
					Index_max_edge=i;
				}
			}
		}
			
		if(abs(Index_min_edge-0) > abs(img_cut_edge.cols-Index_max_edge))//左边界
		{
			IsLeft_edge=true;
			kern_y = (cv::Mat_<char>(3,3) <<-1,0,1,
										-1,0,1,
										-1,0,1);  
			Cut_Line_edge=Index_min_edge;
			Edge_Line_edge=Cut_Line_edge;
			if(Cut_Line_edge>=srcimg.cols-200)//防止溢出 
			{
				delete[] Avr_Col_edge;
				return;
			}
			cv::Rect rect_edge(Cut_Line_edge-64,0,64+64,srcimg.rows); 
			srcimg(rect_edge).copyTo(edge_effect);
		}
		else if(abs(Index_min_edge-0) < abs(img_cut_edge.cols-Index_max_edge))
		{
			IsLeft_edge=false;
			kern_y = (cv::Mat_<char>(3,3) <<1,0,-1,
										1,0,-1,
										1,0,-1); 
			Cut_Line_edge=Index_max_edge;
			Edge_Line_edge=Cut_Line_edge;
			if(Cut_Line_edge<=200)
			{
				delete[] Avr_Col_edge;
				return;
			}
			cv::Rect rect_edge(Cut_Line_edge-64,0,64+64,srcimg.rows); 
			srcimg(rect_edge).copyTo(edge_effect);
		}
		else
		{
			delete[] Avr_Col_edge;
			return;
		}
	}
	delete[] Avr_Col_edge;

	//namedWindow( "edge_effect", 0 );
	//imshow("edge_effect", edge_effect); 
	cv::Mat edge_effect0;
	GaussianBlur( edge_effect, edge_effect0, cv::Size(7, 7), 0, 0 );
	cv::Mat edge_canny;
	Canny( edge_effect0, edge_canny, 10, 20, 3 );//6,15
	//namedWindow( "edge_canny", 0 );
	//imshow("edge_canny", edge_canny); 
	cv::Mat dstimg_Dilate_edge;
	int dilation_size_edge = 2;
	cv::Mat element_edge= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_edge + 1, 2*dilation_size_edge+1 ),cv::Point( dilation_size_edge, dilation_size_edge ) );
	dilate( edge_canny, dstimg_Dilate_edge, element_edge );
	//namedWindow( "dstimg_Dilate_edge", 0 );
	//imshow("dstimg_Dilate_edge", dstimg_Dilate_edge); 
	cv::Mat dstimg_Erode_edge;
	int erodtion_size_edge = 2;
	cv::Mat element_Erode_edge= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*erodtion_size_edge + 1, 2*erodtion_size_edge+1 ),cv::Point( erodtion_size_edge, erodtion_size_edge ) );
	erode( dstimg_Dilate_edge, dstimg_Erode_edge, element_Erode_edge );
	dstimg_Dilate_edge=dstimg_Erode_edge.clone();
	//namedWindow( "dstimg_Erode_edge", 0 );
	//imshow("dstimg_Erode_edge", dstimg_Dilate_edge);

	int n_edge=0;
	int *Avr_Row_edge=new int[dstimg_Dilate_edge.rows]; 
	float Total_edge=0;
	float Avr_edge=0;
	if(IsLeft_edge)
	{
		for(int i=0;i<dstimg_Dilate_edge.rows;i++)
		{
			for(int j=0;j<dstimg_Dilate_edge.cols;j++)
			{
				if(dstimg_Dilate_edge.at<uchar>(i,j)>128)
				{
					Avr_Row_edge[i]=j;break;
				}
			}			
			Total_edge+=Avr_Row_edge[i];
			n_edge++;
		}
	}
	else
	{
		for(int i=0;i<dstimg_Dilate_edge.rows;i++)
		{
			for(int j=dstimg_Dilate_edge.cols-1;j>=0;j--)
			{
				if(dstimg_Dilate_edge.at<uchar>(i,j)>128)
				{
					Avr_Row_edge[i]=dstimg_Dilate_edge.cols-1-j;break;
				}
			}			
			Total_edge+=Avr_Row_edge[i];
			n_edge++;
		}
	}
	Avr_edge=Total_edge/n_edge;
	float e_edge=0;
	float Max_edge=0;
	float Min_edge=1024;
	float Max_temp_edge=0;
	float Min_temp_edge=1024;
	for(int i=0;i<n_edge;i++)
	{
		if(Avr_Row_edge[i]>Max_temp_edge)
		{
			Max_temp_edge=Avr_Row_edge[i];
		}
		if(Avr_Row_edge[i]<Min_temp_edge)
		{
			Min_temp_edge=Avr_Row_edge[i];
		}
		if(abs(Avr_Row_edge[i]-Avr_edge)>8)
		{
			Avr_Row_edge[i]=Avr_edge;
		}
		e_edge+=(Avr_Row_edge[i]-Avr_edge)*(Avr_Row_edge[i]-Avr_edge);
		if(Avr_Row_edge[i]>Max_edge)
		{
			Max_edge=Avr_Row_edge[i];
		}
		if(Avr_Row_edge[i]<Min_edge)
		{
			Min_edge=Avr_Row_edge[i];
		}
		//去除小点的情况
		if(Avr_Row_edge[i]<Avr_edge-3)
		{
			Avr_Row_edge[i]=Avr_edge;
		}
	}
	e_edge/=n_edge;

	int Range_edge=4;
	int temp_edge=0;
	int nEdgeCrack=0;
	for(int i=0;i<n_edge-Range_edge;i++)
	{
		temp_edge=Avr_Row_edge[i];
		if(abs(temp_edge-Avr_Row_edge[i+Range_edge])>=4)
		{
			nEdgeCrack++;
		}
	}

	delete[] Avr_Row_edge;
	if(Max_edge>45&&Max_edge<60&&Max_edge-Min_edge>3&&Max_temp_edge-Min_temp_edge<16&&e_edge<3&&nEdgeCrack>6)
	{
		pSet->Items[pSet->iItemNum].iConfidence=0;
		pSet->Items[pSet->iItemNum].iDefectClass=3;
		pSet->Items[pSet->iItemNum].iLeft=Edge_Line_edge-64;
		pSet->Items[pSet->iItemNum].iTop=0;
		pSet->Items[pSet->iItemNum].iRight=Edge_Line_edge+64;
		pSet->Items[pSet->iItemNum].iBottom=srcimg.rows;
		//LONGSTR strLogInfo={0};
		//CCommonFunc::SafeStringPrintf(strLogInfo, STR_LEN(strLogInfo), L"钢板%d检测到<烂边>置信度为%f的缺陷!", SteelNo,pSet->Items[pSet->iItemNum].iConfidence);
		//AddLog(strLogInfo);
		pSet->iItemNum++;
	
	}
}
void H_SteelImageProcess::Defects_Common_OpenCV_P2D(cv::Mat srcimg_Cutimg_P2D,cv::Mat srcimg_GCG_Cutimg_P2D,int Left_Edge,int Right_Edge,bool& isHead,PDefectSet pSet,int ImgIndex,int CameralNo,int SteelNo) 
{
	//尺寸偶和
	if(Left_Edge%2 != 0)
	{
		Left_Edge++;
	}
	if(Right_Edge%2 != 0)
	{
		Right_Edge--;
	}
	int Left_Edge_4=Left_Edge/2;
	int Right_Edge_4=Right_Edge/2;

	//进入检测
	//下采样
	cv::Mat srcimg_GCG_Cutimg_P4D,srcimg_Cutimg_P4D;
	cv::pyrDown(srcimg_GCG_Cutimg_P2D,srcimg_GCG_Cutimg_P4D,cv::Size(srcimg_GCG_Cutimg_P2D.cols/2,srcimg_GCG_Cutimg_P2D.rows/2));
	cv::pyrDown(srcimg_Cutimg_P2D,srcimg_Cutimg_P4D,cv::Size(srcimg_Cutimg_P2D.cols/2,srcimg_Cutimg_P2D.rows/2));	
	//积分图
	cv::Mat integral_img(srcimg_GCG_Cutimg_P4D.rows+1,srcimg_GCG_Cutimg_P4D.cols+1,CV_64FC1);
	integral(srcimg_GCG_Cutimg_P4D,integral_img,CV_64FC1);//积分图像
	//计算矫正因子,得到矫正图
	int block_x=6;
	int block_y=4;
	int baseGray=100;
	cv::Mat dst_zero=cv::Mat::zeros(srcimg_GCG_Cutimg_P4D.rows,srcimg_GCG_Cutimg_P4D.cols,srcimg_GCG_Cutimg_P4D.type());
	for(int i=block_y/2;i<integral_img.rows-block_y/2;i++)
	{
		for(int j=block_x/2;j<integral_img.cols-block_x/2;j++)
		{
			float A=integral_img.at<double>(i+block_y/2,j+block_x/2);
			float B=integral_img.at<double>(i-block_y/2,j-block_x/2);
			float C=integral_img.at<double>(i-block_y/2,j+block_x/2);
			float D=integral_img.at<double>(i+block_y/2,j-block_x/2);
			float temp=A+B-C-D;
			float x=temp/(block_x*block_y);
			float _x=1/(x+0.000001);
			dst_zero.at<uchar>(i,j)=min(255,srcimg_GCG_Cutimg_P4D.at<uchar>(i,j)*baseGray*_x);
		}
	}
	#ifdef W_DEBUG
		cv::namedWindow("dst_zero",0);
		cv::imshow("dst_zero",dst_zero);
	#endif	
	//切黑边
	cv::Mat dst_zero_cut,srcimg_Cutimg_P4D_cut;
	cv::Rect rect_in(block_x,block_y,srcimg_GCG_Cutimg_P4D.cols-2*block_x,srcimg_GCG_Cutimg_P4D.rows-2*block_y);
	dst_zero(rect_in).copyTo(dst_zero_cut);
	srcimg_Cutimg_P4D(rect_in).copyTo(srcimg_Cutimg_P4D_cut);
	//
	
	//验证图像有效区域，并滤除无效图像（主要为有大量水的图像）
	cv::Mat dst_zero_th;
	cv::threshold(dst_zero,dst_zero_th,50,255,0);
	int NoZeroNum=cv::countNonZero(dst_zero_th);
	float e_num=(float)NoZeroNum/(dst_zero_th.cols*dst_zero_th.rows-dst_zero_th.cols*block_y-dst_zero_th.rows*block_x);
	bool isInvalid=(e_num<0.99)? true : false;
	//cv::Mat dst_zero_th_ero;
	//cv::Mat element_dst_zero= getStructuringElement( cv::MORPH_RECT,cv::Size( 5, 5 ));
	//cv::erode(dst_zero_th,dst_zero_th_ero,element_dst_zero);
	//cv::Mat canny_judge;
	//cv::Canny(dst_zero_th_ero,canny_judge,150,250);
	//cv::Mat element_canny_judge= getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
	//dilate( canny_judge, canny_judge, element_canny_judge );
	//cv::Mat cdst = cv::Mat::zeros( canny_judge.size(), CV_8UC1 );	
	//cv::vector<cv::Vec4i> lines;
	//HoughLinesP(canny_judge, lines, 1, CV_PI/180, 20, canny_judge.cols*0.15, 0);//10,10,1
	//int topLeft=10000;
	//int topRight=0;
	//int bottomLeft=10000;
	//int bottomRight=0;
	//for( size_t i = 0; i < lines.size(); i++ )
	//{
	//	cv::Vec4i l = lines[i];
	//	float width=abs(l[2]-l[0]);
	//	float height=abs(l[3]- l[1]);
	//	if(width/(height+0.01)>canny_judge.cols*0.15)
	//	{
	//		line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
	//	}
	//	if(abs(l[3]+l[1])/2<block_y*2)
	//	{
	//		if(min(l[2],l[0])<topLeft)
	//		{
	//			topLeft=min(l[2],l[0]);
	//		}
	//		if(max(l[2],l[0])>topRight)
	//		{
	//			topRight=max(l[2],l[0]);
	//		}
	//	}
	//	if(abs(l[3]+l[1])/2>canny_judge.rows-block_y*2)
	//	{
	//		if(min(l[2],l[0])<bottomLeft)
	//		{
	//			bottomLeft=min(l[2],l[0]);
	//		}
	//		if(max(l[2],l[0])>bottomRight)
	//		{
	//			bottomRight=max(l[2],l[0]);
	//		}
	//	}
	//}
	//int maxLeft=max(topLeft,bottomLeft)+block_x;
	//int minRight=min(topRight,bottomRight)-block_x;
	
	#ifdef W_DEBUG
		//cv::line(srcimg_Cutimg_P4D,cv::Point(max(topLeft,bottomLeft),0),cv::Point(max(topLeft,bottomLeft),srcimg_Cutimg_P4D.rows),255);
		//cv::line(srcimg_Cutimg_P4D,cv::Point(min(topRight,bottomRight),0),cv::Point(min(topRight,bottomRight),srcimg_Cutimg_P4D.rows),255);
		//cv::namedWindow("dst_zero_th",0);
		//cv::imshow("dst_zero_th",dst_zero_th);
		//cv::namedWindow("canny_judge",0);
		//cv::imshow("canny_judge",canny_judge);
		//cv::namedWindow("cdst",0);
		//cv::imshow("cdst",cdst);
	#endif
	
	//切完黑边后判断是否为头
	//int interval_x=6;
	//int interval_y=10;
	//bool isHead=false;
	//for(int i=0;i<dst_zero_cut.cols/interval_x;i++)
	//{
	//	cv::Rect rect_i_top(i*interval_x,0,interval_x,interval_y);
	//	cv::Mat temp_i_top;
	//	dst_zero_cut(rect_i_top).copyTo(temp_i_top);
	//	cv::Mat temp_i_top_mean,temp_i_top_dev;
	//	cv::meanStdDev(temp_i_top,temp_i_top_mean,temp_i_top_dev);
	//	float fdev_top=temp_i_top_dev.at<double>(0,0);
	//	if(fdev_top<0.000001)
	//	{
	//		for(int j=0;j<dst_zero_cut.rows/interval_y;j++)
	//		{
	//			cv::Rect rect_i_top_j(i*interval_x,j*interval_y,interval_x,interval_y);
	//			cv::Mat temp_i_top_j;
	//			dst_zero_cut(rect_i_top_j).copyTo(temp_i_top_j);
	//			cv::Mat temp_i_top_j_mean,temp_i_top_j_dev;
	//			cv::meanStdDev(temp_i_top_j,temp_i_top_j_mean,temp_i_top_j_dev);
	//			float fdev_top_j=temp_i_top_j_dev.at<double>(0,0);
	//			if(fdev_top_j>20)
	//			{
	//				isHead=true;
	//				break;
	//			}
	//		}
	//	}
	//	cv::Rect rect_i_bottom(i*interval_x,dst_zero_cut.rows-1-interval_y,interval_x,interval_y);
	//	cv::Mat temp_i_bottom;
	//	dst_zero_cut(rect_i_bottom).copyTo(temp_i_bottom);
	//	cv::Mat temp_i_bottom_mean,temp_i_bottom_dev;
	//	cv::meanStdDev(temp_i_bottom,temp_i_bottom_mean,temp_i_bottom_dev);
	//	float fdev_bottom=temp_i_bottom_dev.at<double>(0,0);
	//	if(fdev_bottom<0.000001)
	//	{
	//		for(int j=dst_zero_cut.rows/interval_y;j>1;j--)
	//		{
	//			cv::Rect rect_i_bottom_j(i*interval_x,(j-1)*interval_y,interval_x,interval_y);
	//			cv::Mat temp_i_bottom_j;
	//			dst_zero_cut(rect_i_bottom_j).copyTo(temp_i_bottom_j);
	//			cv::Mat temp_i_bottom_j_mean,temp_i_bottom_j_dev;
	//			cv::meanStdDev(temp_i_bottom_j,temp_i_bottom_j_mean,temp_i_bottom_j_dev);
	//			float fdev_bottom_j=temp_i_bottom_j_dev.at<double>(0,0);
	//			if(fdev_bottom_j>20)
	//			{
	//				isHead=true;
	//				break;
	//			}
	//		}
	//	}
	//}
	//进行缺陷阈值分割
	//1、sobel图
	int ddepth=CV_16S;
	int delta=0;
	int scale=1;
	cv::Mat dst_sobel_cut;
	cv::Mat grad_x, grad_xx;
	cv::Mat abs_grad_x, abs_grad_xx;
	cv::Sobel( dst_zero_cut, grad_x, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x, abs_grad_x );
	cv::Sobel( dst_zero_cut, grad_xx, ddepth, 0, 2, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_xx, abs_grad_xx );
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_xx, 0.5, 0, dst_sobel_cut );
	cv::Mat dst_sobel_cut_mean,dst_sobel_cut_dev;
	cv::meanStdDev(dst_sobel_cut,dst_sobel_cut_mean,dst_sobel_cut_dev);
	float f_dst_sobel_cut_mean=dst_sobel_cut_mean.at<double>(0,0);
	float th=200;
	th=max(f_dst_sobel_cut_mean*10,120);
	th=min(f_dst_sobel_cut_mean*10,225);		
	cv::Mat dst_sobel_th;
	cv::threshold(dst_sobel_cut,dst_sobel_th,th,255,0);
	cv::Mat dst_sobel_th_dil;
	int dilation_size_dst_sobel_th = 4;
	cv::Mat element_dst_sobel_th= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_dst_sobel_th + 1, 2*dilation_size_dst_sobel_th+1 ));
	dilate( dst_sobel_th, dst_sobel_th_dil, element_dst_sobel_th );		
	#ifdef W_DEBUG
		cv::namedWindow("dst_sobel_cut",0);
		cv::imshow("dst_sobel_cut",dst_sobel_cut);
		cv::namedWindow("dst_sobel_th",0);
		cv::imshow("dst_sobel_th",dst_sobel_th);
		cv::namedWindow("dst_sobel_th_dil",0);
		cv::imshow("dst_sobel_th_dil",dst_sobel_th_dil);
	#endif
	//2、原图上
	cv::Mat src_canny_cut,src_canny_cut_dil;
	cv::Canny(srcimg_Cutimg_P4D_cut,src_canny_cut,12,25,3);
	int dilation_size_src_canny_cut = 2;
	cv::Mat element_src_canny_cut= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_src_canny_cut + 1, 2*dilation_size_src_canny_cut+1 ));
	dilate( src_canny_cut, src_canny_cut_dil, element_src_canny_cut );
	#ifdef W_DEBUG
		cv::namedWindow("src_canny_cut",0);
		cv::imshow("src_canny_cut",src_canny_cut);
		cv::namedWindow("src_canny_cut_dil",0);
		cv::imshow("src_canny_cut_dil",src_canny_cut_dil);
	#endif
	//3、canny上
	//cv::Mat dst_zero_cut_mean,dst_zero_cut_dev;
	//cv::meanStdDev(dst_zero_cut,dst_zero_cut_mean,dst_zero_cut_dev);
	//float f_dst_zero_cut_dev=dst_zero_cut_dev.at<double>(0,0);
	//float th_canny=200;
	//th_canny=max(f_dst_zero_cut_dev*30,180);
	//th_canny=min(f_dst_zero_cut_dev*30,350);
	//cv::Mat dst_canny_cut,dst_canny_cut_dil;
	//cv::Canny(dst_zero_cut,dst_canny_cut,th_canny,th_canny*1.25);
	//#ifdef W_DEBUG
	//	cv::namedWindow("dst_canny_cut",0);
	//	cv::imshow("dst_canny_cut",dst_canny_cut);
	//#endif
	//合并
	cv::Mat dst;
	cv::addWeighted(src_canny_cut_dil,0.5,dst_sobel_th_dil,0.5,0,dst);
	cv::threshold(dst,dst,150,255,0);
	#ifdef W_DEBUG
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	#endif
	//
	cv::Mat dst1,dst2,_dst;
	cv::threshold(dst_zero_cut,dst1,75,255,1);
	cv::threshold(dst_zero_cut,dst2,200,255,0);
	cv::addWeighted(dst1,0.5,dst2,0.5,0,_dst);
	cv::threshold(_dst,_dst,100,255,0);
	#ifdef W_DEBUG
		cv::namedWindow("_dst",0);
		cv::imshow("_dst",_dst);
	#endif
	cv::addWeighted(dst,0.5,_dst,0.5,0,dst);
	cv::threshold(dst,dst,150,255,0);

	//测头尾
	isHead=false;
	cv::Mat cdst_head = cv::Mat::zeros( dst_sobel_th.size(), CV_8UC1 );	
	cv::vector<cv::Vec4i> lines_head;
	HoughLinesP(dst_sobel_th, lines_head, 1, CV_PI/180, 10, 100, 50);//10,100,50
	for( size_t i = 0; i < lines_head.size(); i++ )
	{
		cv::Vec4i l = lines_head[i];
		float width=abs(l[2]-l[0]);
		float height=abs(l[3]- l[1]);
		if(width/(height+0.000001)>10)
		{
			line( cdst_head, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), 255, 2, CV_AA);
			isHead=true;
		}
	}
	#ifdef W_DEBUG
		cv::namedWindow("cdst_head",0);
		cv::imshow("cdst_head",cdst_head);
	#endif
	//膨胀
	cv::Mat dst_dil;
	int dilation_size_dst = 4;
	cv::Mat element_dst= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size_dst + 1, 2*dilation_size_dst+1 ));
	dilate( dst, dst_dil, element_dst );
	//画缺陷
	cv::vector<cv::vector<cv::Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;
	findContours( dst_dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
	cv::vector<cv::Rect> boundRect( contours.size() );
	cv::vector<cv::RotatedRect> minRect( contours.size() ); 
	cv::Mat drawing = cv::Mat::zeros( dst_dil.rows,dst_dil.cols, CV_8UC1 );
	int iScale_x=2;
	int iScale_y=3;
	for( int i = 0; i< contours.size(); i++ )
	{
		approxPolyDP( cv::Mat(contours[i]), contours[i], 3, true );
		boundRect[i] = boundingRect( cv::Mat(contours[i]) );
		double tmparea=fabs(contourArea(contours[i])); 
		if(tmparea<100)
		{
			continue;
		}
		int left=max(0,boundRect[i].tl().x-iScale_x);
		int right=min(drawing.cols-1,boundRect[i].br().x+iScale_x);
		int top=max(0,boundRect[i].tl().y-iScale_y);
		int bottom=min(drawing.rows-1,boundRect[i].br().y+iScale_y);
		int xCenter=(left+right)/2;
		int yCenter=(top+bottom)/2;
		rectangle( drawing, cv::Point(left,top), cv::Point(right,bottom), 255, -2, 8, 0 );
		#ifdef W_DEBUG
			//if(xCenter>maxLeft && xCenter<minRight)
			//{
			//	cv::rectangle(srcimg_Cutimg_P4D,cv::Point(boundRect[i].tl().x+block_x,boundRect[i].tl().y+block_y),cv::Point(boundRect[i].br().x+block_x,boundRect[i].br().y+block_y),255,2);
			//}
		#endif
	}
	#ifdef W_DEBUG
		if(isInvalid)
		{
			cv::putText(srcimg_Cutimg_P4D,"Invalid",cv::Point(srcimg_Cutimg_P4D.cols*0.25,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		else
		{
			cv::putText(srcimg_Cutimg_P4D,"Valid",cv::Point(srcimg_Cutimg_P4D.cols*0.25,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		if(isHead)
		{
			cv::putText(srcimg_Cutimg_P4D,"Head",cv::Point(srcimg_Cutimg_P4D.cols*0.75,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		else
		{
			cv::putText(srcimg_Cutimg_P4D,"NoHead",cv::Point(srcimg_Cutimg_P4D.cols*0.75,srcimg_Cutimg_P4D.rows*0.5),1,2,255);
		}
		cv::namedWindow("srcimg_Cutimg_P4D",0);
		cv::imshow("srcimg_Cutimg_P4D",srcimg_Cutimg_P4D);
	#endif

	//写缺陷
	cv::vector<cv::vector<cv::Point> > contours_big;
	cv::vector<cv::Vec4i> hierarchy_big;
	cv::findContours( drawing, contours_big, hierarchy_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	cv::vector<cv::vector<cv::Point> > contours_poly_big( contours_big.size() );
	cv::vector<cv::Rect> boundRect_big( contours_big.size() );
	for( int k = 0; k < contours_big.size(); k++ )
	{ 
		approxPolyDP( cv::Mat(contours_big[k]), contours_poly_big[k], 3, true );
		boundRect_big[k] = cv::boundingRect( cv::Mat(contours_poly_big[k]) );
		//
		int d_left=max(0,boundRect_big[k].tl().x);
		int d_right=min(drawing.cols-1,boundRect_big[k].br().x);
		int d_top=max(0,boundRect_big[k].tl().y);
		int d_bottom=min(drawing.rows-1,boundRect_big[k].br().y);
		int xCenter=(d_left+d_right)/2;
		int yCenter=(d_top+d_bottom)/2;
		
		float width=abs(d_left-d_right);
		float height=abs(d_top- d_bottom);
		float area=width*height;
		if(area<250 || true==isHead || true==isInvalid)
		{
			continue;
		}
		//去除残余边界
		//1、左右边
		cv::Mat srcimg_Cutimg_P4D_cut_part_left,srcimg_Cutimg_P4D_cut_part_right;
		cv::Rect rect_left(d_left,d_top,width*0.25,height);
		cv::Rect rect_right(d_left+width*0.75,d_top,width*0.25,height);
		srcimg_Cutimg_P4D_cut(rect_left).copyTo(srcimg_Cutimg_P4D_cut_part_left);
		srcimg_Cutimg_P4D_cut(rect_right).copyTo(srcimg_Cutimg_P4D_cut_part_right);
		#ifdef W_DEBUG
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_left",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_left",srcimg_Cutimg_P4D_cut_part_left);
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_right",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_right",srcimg_Cutimg_P4D_cut_part_right);
		#endif
		cv::Mat srcimg_Cutimg_P4D_cut_part_left_mean,srcimg_Cutimg_P4D_cut_part_left_dev;
		cv::Mat srcimg_Cutimg_P4D_cut_part_right_mean,srcimg_Cutimg_P4D_cut_part_right_dev;
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_left,srcimg_Cutimg_P4D_cut_part_left_mean,srcimg_Cutimg_P4D_cut_part_left_dev);
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_right,srcimg_Cutimg_P4D_cut_part_right_mean,srcimg_Cutimg_P4D_cut_part_right_dev);
		float f_srcimg_Cutimg_P4D_cut_part_left_mean=srcimg_Cutimg_P4D_cut_part_left_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P4D_cut_part_right_mean=srcimg_Cutimg_P4D_cut_part_right_mean.at<double>(0,0);
		float f_v_e=(f_srcimg_Cutimg_P4D_cut_part_left_mean>f_srcimg_Cutimg_P4D_cut_part_right_mean)?(f_srcimg_Cutimg_P4D_cut_part_right_mean/(f_srcimg_Cutimg_P4D_cut_part_left_mean+0.000001)):(f_srcimg_Cutimg_P4D_cut_part_left_mean/(f_srcimg_Cutimg_P4D_cut_part_right_mean+0.00001));
		if(f_v_e<0.3)
		{
			continue;
		}
		//2、上下边
		cv::Mat srcimg_Cutimg_P4D_cut_part_top,srcimg_Cutimg_P4D_cut_part_bottom;
		cv::Rect rect_top(d_left,d_top,width,height*0.25);
		cv::Rect rect_bottom(d_left,d_top+height*0.75,width,height*0.25);
		srcimg_Cutimg_P4D_cut(rect_top).copyTo(srcimg_Cutimg_P4D_cut_part_top);
		srcimg_Cutimg_P4D_cut(rect_bottom).copyTo(srcimg_Cutimg_P4D_cut_part_bottom);
		#ifdef W_DEBUG
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_top",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_top",srcimg_Cutimg_P4D_cut_part_top);
			cv::namedWindow("srcimg_Cutimg_P4D_cut_part_bottom",0);
			cv::imshow("srcimg_Cutimg_P4D_cut_part_bottom",srcimg_Cutimg_P4D_cut_part_bottom);
		#endif
		cv::Mat srcimg_Cutimg_P4D_cut_part_top_mean,srcimg_Cutimg_P4D_cut_part_top_dev;
		cv::Mat srcimg_Cutimg_P4D_cut_part_bottom_mean,srcimg_Cutimg_P4D_cut_part_bottom_dev;
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_top,srcimg_Cutimg_P4D_cut_part_top_mean,srcimg_Cutimg_P4D_cut_part_top_dev);
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part_bottom,srcimg_Cutimg_P4D_cut_part_bottom_mean,srcimg_Cutimg_P4D_cut_part_bottom_dev);
		float f_srcimg_Cutimg_P4D_cut_part_top_mean=srcimg_Cutimg_P4D_cut_part_top_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P4D_cut_part_bottom_mean=srcimg_Cutimg_P4D_cut_part_bottom_mean.at<double>(0,0);
		float f_h_e=(f_srcimg_Cutimg_P4D_cut_part_top_mean>f_srcimg_Cutimg_P4D_cut_part_bottom_mean)?(f_srcimg_Cutimg_P4D_cut_part_bottom_mean/(f_srcimg_Cutimg_P4D_cut_part_top_mean+0.000001)):(f_srcimg_Cutimg_P4D_cut_part_top_mean/(f_srcimg_Cutimg_P4D_cut_part_bottom_mean+0.00001));
		if(f_h_e<0.15)
		{
			continue;
		}
		//3、斜边
		cv::Mat srcimg_Cutimg_P4D_cut_part;
		cv::Rect rect(d_left,d_top,width,height);
		srcimg_Cutimg_P4D_cut(rect).copyTo(srcimg_Cutimg_P4D_cut_part);
		float part_left_top=0;
		float part_right_bottom=0;
		float part_left_bottom=0;			
		float part_right_top=0;
		float f_k=(float)srcimg_Cutimg_P4D_cut_part.rows/srcimg_Cutimg_P4D_cut_part.cols;
		for(int m=0;m<srcimg_Cutimg_P4D_cut_part.rows;m++)
		{
			for(int n=0;n<srcimg_Cutimg_P4D_cut_part.cols;n++)
			{
				if(m>(f_k*n+srcimg_Cutimg_P4D_cut_part.rows*0.5))
				{
					part_left_top+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}
				else if(m<(f_k*n-srcimg_Cutimg_P4D_cut_part.rows*0.5))
				{
					part_right_bottom+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}

				if(m<(srcimg_Cutimg_P4D_cut_part.rows*0.5-f_k*n))
				{
					part_right_top+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}
				else if(m>(srcimg_Cutimg_P4D_cut_part.rows*1.5-f_k*n))
				{
					part_left_bottom+=srcimg_Cutimg_P4D_cut_part.at<uchar>(m,n);
				}
			}
		}
		float f_e_1=(part_left_top>part_right_bottom)?(part_right_bottom/(part_left_top+0.000001)):(part_left_top/(part_right_bottom+0.00001));
		float f_e_2=(part_right_top>part_left_bottom)?(part_left_bottom/(part_right_top+0.000001)):(part_right_top/(part_left_bottom+0.00001));
		if(f_e_1<0.15 || f_e_2<0.15)
		{
			continue;
		}
		//4、图像过黑
		cv::Mat srcimg_Cutimg_P4D_cut_part_mean,srcimg_Cutimg_P4D_cut_part_dev;
		cv::meanStdDev(srcimg_Cutimg_P4D_cut_part,srcimg_Cutimg_P4D_cut_part_mean,srcimg_Cutimg_P4D_cut_part_dev);
		float f_srcimg_Cutimg_P4D_cut_part_mean=srcimg_Cutimg_P4D_cut_part_mean.at<double>(0,0);
		float f_srcimg_Cutimg_P4D_cut_part_dev=srcimg_Cutimg_P4D_cut_part_dev.at<double>(0,0);
		if(f_srcimg_Cutimg_P4D_cut_part_mean<10)
		{
			continue;
		}
		//
		//if(xCenter>maxLeft && xCenter<minRight)
		{
			//保存图像
			//cv::Rect rect_part(d_left,d_top,width,height);
			//cv::Mat dst_part;
			//dst_zero_cut(rect_part).copyTo(dst_part);
			//H_BaseImageProcess::Save_Image_OpenCV(dst_part,ImgIndex*10000+pSet->iItemNum);
			//
			pSet->Items[pSet->iItemNum].iDefectClass=9;
			pSet->Items[pSet->iItemNum].iConfidence=0.5;
			pSet->Items[pSet->iItemNum].iLeft=(d_left+block_x+Left_Edge_4)*4;
			pSet->Items[pSet->iItemNum].iTop=(d_top+block_y)*4;
			pSet->Items[pSet->iItemNum].iRight=(d_right+block_x+Left_Edge_4)*4;
			pSet->Items[pSet->iItemNum].iBottom=(d_bottom+block_y)*4;
			pSet->iItemNum++;
		}
	}
}