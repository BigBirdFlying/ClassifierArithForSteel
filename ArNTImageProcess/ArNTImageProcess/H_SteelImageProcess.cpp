#include "H_SteelImageProcess.h"
#include "H_Common.h"

H_SteelImageProcess::H_SteelImageProcess(void)
{
}

H_SteelImageProcess::~H_SteelImageProcess(void)
{
}


void H_SteelImageProcess::Steel_RegionToDefectSet(const cv::Mat& src_region,PH_DefectSet pH_DefectSet,int dilation_size,int iScale_x,int iScale_y,int minAreaLimit)
{
	cv::Mat src_region_dil;
	cv::Mat element= getStructuringElement( cv::MORPH_RECT,cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ));
	dilate( src_region, src_region_dil, element );
	//»­È±ÏÝ
	cv::vector<cv::vector<cv::Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;
	findContours( src_region_dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );	
	cv::vector<cv::Rect> boundRect( contours.size() );
	cv::vector<cv::RotatedRect> minRect( contours.size() ); 
	cv::Mat drawing = cv::Mat::zeros( src_region_dil.rows,src_region_dil.cols, CV_8UC1 );
	//int iScale_x=5;
	//int iScale_y=10;
	for( int i = 0; i< contours.size(); i++ )
	{
		approxPolyDP( cv::Mat(contours[i]), contours[i], 3, true );
		boundRect[i] = boundingRect( cv::Mat(contours[i]) );
		double tmparea=fabs(contourArea(contours[i])); 
		if(tmparea<minAreaLimit)
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
	}
	//Ð´È±ÏÝ
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
		//
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iDefectClass=0;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].fConfidence=0.5;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iLeftInImage=d_left;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iTopInImage=d_top;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iRightInImage=d_right;
		pH_DefectSet->Items[pH_DefectSet->iItemNum].iBottomInImage=d_bottom;
		pH_DefectSet->iItemNum++;
	}
}

void H_SteelImageProcess::Steel_FindEdge(const cv::Mat& src,int& iLeft,int& iRight,int offset)
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
	iLeft=width-1;
	iRight=0;
	for(int j=0;j<Num_Max;j+=2)
	{
		if(j!=-1)
		{
			if(abs(Set_Edge[j]-Set_Edge[j+1])>Max_Edge)
			{
				Max_Edge=abs(Set_Edge[j]-Set_Edge[j+1]);
				iLeft=Set_Edge[j];
				iRight=Set_Edge[j+1];
			}
		}
		else
		{
			break;
		}
	}
	delete[] Set_Edge;
	delete[] fAverGray;

	iLeft=iLeft+offset;
	iRight=iRight-offset;
}

void H_SteelImageProcess::Steel_CutEdge(const cv::Mat& src,const int& iLeft,const int& iRight,cv::Mat& dst)
{
	if(iRight>iLeft)
	{
		cv::Rect rect(iLeft,0,iRight-iLeft,src.rows);
		src(rect).copyTo(dst);
	}
}