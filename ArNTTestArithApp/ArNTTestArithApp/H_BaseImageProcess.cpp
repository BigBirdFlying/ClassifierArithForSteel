#include "H_BaseImageProcess.h"

H_BaseImageProcess::H_BaseImageProcess(void)
{
}

H_BaseImageProcess::~H_BaseImageProcess(void)
{
}
/************************纵向灰度矫正************************/
void H_BaseImageProcess::Gray_Change_Y(const cv::Mat& src,cv::Mat& dst,int offSet,int baseGray)  
{  
	cv::Mat base_16=cv::Mat::zeros(src.size(), CV_16UC1);
	dst=cv::Mat::zeros(src.size(), CV_8UC1);
	cv::Mat src_16;
	src.convertTo(src_16,CV_16UC1);
	for(int i=0;i<src.rows;i++)
	{
		base_16.row(0)+=src_16.row(i);
	}
	
	base_16.row(0)/=src.rows;
	for(int j=0;j<base_16.cols;j++)
	{	
		if(base_16.at<ushort>(0,j)<offSet)
		{
			base_16.at<ushort>(0,j)=0;
		}
	}

	for(int i=0;i<base_16.rows;i++)
	{
		base_16.row(0).copyTo(base_16.row(i));
	}
	cv::Mat base;
	base_16.convertTo(base,CV_8UC1);
	dst=src*baseGray/base;
}
/************************横向灰度矫正************************/
void H_BaseImageProcess::Gray_Change_X(const cv::Mat& src,cv::Mat& dst,int offSet,int baseGray,bool bSelect)  
{  		
	if(true==bSelect)
	{
		dst=cv::Mat::zeros(src.size(), CV_8UC1); 
		float* Avg_Row=new float[src.rows];
		float* Avg_Row_Clone=new float[src.rows];
		memset(Avg_Row, 0, src.rows*sizeof(float));

		cv::Mat ss=cv::Mat::zeros(src.rows,1 , CV_16UC1); 
		cv::Mat temp;
		src.convertTo(temp,CV_16UC1);

		for(int i=0;i<src.cols;i++)
		{
			ss.col(0)+=temp.col(i);
		}

		for(int j=0;j<src.rows;j++)
		{
			float temp0=(float)ss.at<ushort>(j,0)/src.cols;
			Avg_Row[j]=1/temp0;
			Avg_Row_Clone[j]=Avg_Row[j];
		}
		 
		cv::Mat zz=cv::Mat::zeros(src.rows, 1, CV_8UC1);
		for(int i=0;i<src.rows;i++)
		{
			if(Avg_Row_Clone[i]<0.2)//0.2
			{
				dst.row(i)=Avg_Row_Clone[i]*100*src.row(i);//152
			}
		}
		delete[]Avg_Row_Clone;
		delete[]Avg_Row;
	}
	else
	{
		cv::Mat base_16=cv::Mat::zeros(src.size(), CV_16UC1);
		dst=cv::Mat::zeros(src.size(), CV_8UC1);
		cv::Mat src_16;
		src.convertTo(src_16,CV_16UC1);
		for(int i=0;i<src.cols;i++)
		{
			base_16.col(0)+=src_16.col(i);
		}
		
		base_16.col(0)/=src.cols;
		for(int j=0;j<base_16.rows;j++)
		{	
			if(base_16.at<ushort>(j,0)<offSet)
			{
				base_16.at<ushort>(j,0)=0;
			}
		}

		for(int i=0;i<base_16.cols;i++)
		{
			base_16.col(0).copyTo(base_16.col(i));
		}
		cv::Mat base;
		base_16.convertTo(base,CV_8UC1);
		dst=src*baseGray/base;
	}
}
/************************OpenCV到Halcon************************/
//void H_BaseImageProcess::OpencvToHalcon(cv::Mat& src,HalconCpp::HObject& ho_dst)
//{
//	int Height= src.rows; 
//	int Width = src.cols;
//	unsigned char* srcImg=new uchar[sizeof(BYTE)*Width*Height];
//	memcpy(srcImg,&src.at<uchar>(0,0),Width*Height*sizeof(BYTE));
//	HalconCpp::GenImage1(&ho_dst, "byte", Width, Height, (Hlong)srcImg);
//	delete[] srcImg;
//}
/************************指针到Halcon************************/
//void H_BaseImageProcess::Type_To_HObject(unsigned char* srcImg,HObject* ho_Image,int srcImgWidth,int srcImgHeight) 
//{
//	//HalconCpp::GenImage1Extern(ho_Image, "byte", srcImgWidth, srcImgHeight, (Hlong)srcImg, NULL);
//	HalconCpp::GenImage1(ho_Image, "byte", srcImgWidth, srcImgHeight, (Hlong)srcImg);
//}
/************************Halcon到指针************************/
//void H_BaseImageProcess::HObject_To_Type(HObject ho_Image,unsigned char* srcImg) 
//{
//	HTuple hv_Pointer,hv_Type,hv_Width,hv_Height;
//	GetImagePointer1(ho_Image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
//	BYTE *p=(BYTE *)hv_Pointer[0].L();
//	int height= hv_Height.L(); 
//	int width = hv_Width.L();
//	memcpy(srcImg,p,width*height*sizeof(BYTE));
//}
/************************显示图像************************/
//void H_BaseImageProcess::Show_Image_Halcon(HObject ho_Image,float zoom_factor,int Left_Edge,int Right_Edge) 
//{
//	HImage Image=ho_Image;
//	HTuple width;
//	HTuple height;
//	Image.GetImageSize(&width, &height);
//
//	HTuple h_width=width.D()*zoom_factor;
//	HTuple h_height=height.D()*zoom_factor;
//	int iwidth=h_width.D();
//	int iheight=h_height.D();
//	
//	HWindow w(0, 0, iwidth, iheight); 
//	HalconCpp::SetPart(w,0,0,height,width);
//	Image.DispObj(w);
//	if(Left_Edge>=0 && Right_Edge<width)
//	{
//		w.DispLine((HTuple)0,(HTuple)Left_Edge,(HTuple)height,(HTuple)Left_Edge);
//	}
//	if(Right_Edge>0 && Right_Edge<width)
//	{
//		w.DispLine((HTuple)0,(HTuple)Right_Edge,(HTuple)height,(HTuple)Right_Edge);
//	}
//	w.Click();
//	w.CloseWindow();
//}
/************************保存图像************************/
//void H_BaseImageProcess::Save_Image_Halcon(HObject ho_Image,int iStr) 
//{
//	HImage Image=ho_Image;
//	stringstream ss;
//	string str;
//	ss<<iStr;
//	ss>>str;
//	const char* c_str = str.c_str();
//	Image.WriteImage("bmp", 0, c_str);
//}
/************************保存图像************************/
void H_BaseImageProcess::Save_Image_OpenCV(cv::Mat srcimg,int iStr) 
{
	stringstream ss;
	string str;
	ss<<iStr;
	ss>>str;
	const char* c_str = str.c_str();
	char* s=".bmp";
	str+=s;
	imwrite(str,srcimg);
}
/************************获取Sobel特征************************/
//void H_BaseImageProcess::GenSobelFeatures (HObject ho_Image, HTuple hv_Features, HTuple *hv_FeaturesExtended)
//{
//	HObject  ho_EdgeAmplitude;
//
//	HTuple  hv_Energy, hv_Correlation, hv_Homogeneity;
//	HTuple  hv_Contrast, hv_AbsoluteHistoEdgeAmplitude;
//
//	CoocFeatureImage(ho_Image, ho_Image, 6, 0, &hv_Energy, &hv_Correlation, &hv_Homogeneity, &hv_Contrast);
//	CoocFeatureImage(ho_Image, ho_Image, 6, 90, &hv_Energy, &hv_Correlation, &hv_Homogeneity, &hv_Contrast);
//
//	SobelAmp(ho_Image, &ho_EdgeAmplitude, "sum_abs", 3);
//	GrayHistoAbs(ho_EdgeAmplitude, ho_EdgeAmplitude, 8, &hv_AbsoluteHistoEdgeAmplitude);
//
//	(*hv_FeaturesExtended).Clear();
//	(*hv_FeaturesExtended).Append(hv_Features);
//	(*hv_FeaturesExtended).Append(hv_Energy);
//	(*hv_FeaturesExtended).Append(hv_Correlation);
//	(*hv_FeaturesExtended).Append(hv_Homogeneity);
//	(*hv_FeaturesExtended).Append(hv_Contrast);
//	(*hv_FeaturesExtended) = (*hv_FeaturesExtended).TupleConcat(hv_AbsoluteHistoEdgeAmplitude);
//	return;
//}
/************************获取特征************************/
//void H_BaseImageProcess::GenFeatures (HObject ho_Image, HTuple *hv_FeatureVector)
//{
//	HObject  ho_Zoomed1, ho_Zoomed2;
//	(*hv_FeatureVector) = HTuple();
//	GenSobelFeatures(ho_Image, (*hv_FeatureVector), &(*hv_FeatureVector));
//	ZoomImageFactor(ho_Image, &ho_Zoomed1, 0.5, 0.5, "constant");
//	GenSobelFeatures(ho_Zoomed1, (*hv_FeatureVector), &(*hv_FeatureVector));
//	ZoomImageFactor(ho_Zoomed1, &ho_Zoomed2, 0.5, 0.5, "constant");
//	(*hv_FeatureVector) = (*hv_FeatureVector).TupleReal();
//	return;
//}
/************************SSR************************/
void H_BaseImageProcess::SSR(cv::Mat srcimg,int sigma,int scale,cv::Mat &dstimg)
{
	cv::Mat srcimg_gauss;
	GaussianBlur( srcimg, srcimg_gauss, cv::Size( 15, 15 ), 0, 0 );
	cv::Mat srcimgf,srcimg_gaussf;
	srcimg.convertTo(srcimgf,CV_32F,1.0,1.0);//类型转换
	srcimg_gauss.convertTo(srcimg_gaussf,CV_32F,1.0,1.0);//类型转换

	cv::log(srcimgf,srcimgf);
	cv::log(srcimg_gaussf,srcimg_gaussf);
	cv::Mat dstf;
	subtract(srcimgf,srcimg_gaussf,dstf);

	cv::Mat Mean;
	cv::Mat Dev;
	meanStdDev(dstf,Mean,Dev);
	float minVal=Mean.at<double>(0,0)-scale*Dev.at<double>(0,0);
	float maxVal=Mean.at<double>(0,0)+scale*Dev.at<double>(0,0);
	float Max_Min=maxVal-minVal;
	float _Max_Min=1/Max_Min;

	cv::Mat E=cv::Mat::ones(srcimgf.rows,srcimgf.cols,CV_32F);//对角矩阵
	cv::Mat S1=(255*_Max_Min)*dstf;
	int s2=255*_Max_Min*minVal;
	cv::Mat S2=s2*E;

	cv::Mat temp;
	subtract(S1,S2,temp);

	temp.convertTo(dstimg,CV_8U);//类型转换
}

int H_BaseImageProcess::Otsu(cv::Mat src) // must graylevel image
{
    long nl = src.rows; // the total lines of the image
    long nc = src.cols;
    long NO = nc * nl;
    int h[256];
    double p[256];
	#pragma omp parallel
    {
		#pragma omp sections
        {
			#pragma omp section
				memset(h, 0, sizeof(h));
			#pragma omp section
				memset(p, 0.0f, sizeof(p));
        } /*-- End of sections block --*/
    } 
    long i, j, k;
	#pragma omp parallel for  shared( h,src) private(i, j)
    for( i = 0; i < nl; i++)
    {
        uchar * data = src.ptr<uchar>(i);
        for(j = 0; j < nc; j ++)
		{
            h[data[j]]++; 
		}
    }
    int ThresoldVal = 0;
    double MG = 0.0f, P1, M ;
 
	#pragma omp parallel for  shared( p,h, NO) private(i)
    for(i = 0; i < 256; i++)
	{
        p[i] = h[i] / double(NO);  
	}
	#pragma omp parallel for reduction(+:MG) shared( p) private(k)
    for( k = 0; k < 256; k++)
	{
		MG += k * p[k];
	}
 
    double fmax, fmax_max = -1;
    P1 =  M = 0.0f; // P1 = 0.0f, M = 0.0f;
    for(k = 0; k < 256; k++)
    {
        P1 += p[k], M += k*p[k];
        fmax = ((MG * P1 - M) * (MG * P1 - M))/((1-P1) * P1 + .5 );
        if(fmax > fmax_max)
		{
			fmax_max = fmax;
			ThresoldVal = k;
		}
    }
    return ThresoldVal;
}

void H_BaseImageProcess::Img_LBP(const cv::Mat& src,cv::Mat& dst)
{ 
	cv::Mat _src;
	cv::Mat _dst;
	src.copyTo(_src);
    _dst.create(_src.rows-2, _src.cols-2, CV_8UC1);  
	_dst.copyTo(dst);  
    dst.setTo(0);  
  
    for(int i=1;i<_src.rows-1;i++) {  

        for(int j=1;j<_src.cols-1;j++) {  
              
            char center = _src.at<char>(i,j);  

            unsigned char code = 0;  
            code |= (_src.at<char>(i-1,j-1) >= center) << 7;  
            code |= (_src.at<char>(i-1,j  ) >= center) << 6;  
            code |= (_src.at<char>(i-1,j+1) >= center) << 5;  
            code |= (_src.at<char>(i  ,j+1) >= center) << 4;  
            code |= (_src.at<char>(i+1,j+1) >= center) << 3;  
            code |= (_src.at<char>(i+1,j  ) >= center) << 2;  
            code |= (_src.at<char>(i+1,j-1) >= center) << 1;  
            code |= (_src.at<char>(i  ,j-1) >= center) << 0;  
  
            dst.at<unsigned char>(i-1,j-1) = code;  
        }  
    }  
}