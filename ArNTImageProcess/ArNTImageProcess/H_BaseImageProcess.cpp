#include "H_BaseImageProcess.h"


cv::Mat getMyGabor(int width, int height, int U, int V, double Kmax, double f, double sigma, int ktype, const string& kernel_name)  
{  
    int half_width = width / 2;  
    int half_height = height / 2;  
    double Qu = PI*U/8;  
    double sqsigma = sigma*sigma;  
    double Kv = Kmax/pow(f,V);  
    double postmean = exp(-sqsigma/2);  
  
    cv::Mat kernel_re(width, height, ktype);  
    cv::Mat kernel_im(width, height, ktype);  
    cv::Mat kernel_mag(width, height, ktype);  
  
    double tmp1, tmp2, tmp3;  
    for(int j = -half_height; j <= half_height; j++){  
        for(int i = -half_width; i <= half_width; i++){  
            tmp1 = exp(-(Kv*Kv*(j*j+i*i))/(2*sqsigma));  
            tmp2 = cos(Kv*cos(Qu)*i + Kv*sin(Qu)*j) - postmean;  
            tmp3 = sin(Kv*cos(Qu)*i + Kv*sin(Qu)*j);  
  
            if(ktype == CV_32F)  
                kernel_re.at<float>(j+half_height, i+half_width) =   
                    (float)(Kv*Kv*tmp1*tmp2/sqsigma);  
            else  
                kernel_re.at<double>(j+half_height, i+half_width) =   
                    (double)(Kv*Kv*tmp1*tmp2/sqsigma);  
  
            if(ktype == CV_32F)  
                kernel_im.at<float>(j+half_height, i+half_width) =   
                    (float)(Kv*Kv*tmp1*tmp3/sqsigma);  
            else  
                kernel_im.at<double>(j+half_height, i+half_width) =   
                    (double)(Kv*Kv*tmp1*tmp3/sqsigma);  
        }  
    }   
    magnitude(kernel_re, kernel_im, kernel_mag);    
    if(kernel_name.compare("real") == 0)  
        return kernel_re;  
    else if(kernel_name.compare("imag") == 0)  
        return kernel_im;  
    else if(kernel_name.compare("mag") == 0)  
        return kernel_mag;   
}  

H_BaseImageProcess::H_BaseImageProcess(void)
{
}

H_BaseImageProcess::~H_BaseImageProcess(void)
{
}

void H_BaseImageProcess::Img_Gray_Change_Y(const cv::Mat& src,cv::Mat& dst,int baseGray,int offSet)  
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

void H_BaseImageProcess::Img_Gray_Change_X(const cv::Mat& src,cv::Mat& dst,int baseGray,int offSet,int iSelect)  
{  		
	if(1==iSelect)
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
	else if(2==iSelect)
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

void H_BaseImageProcess::Img_Gray_Change_Integral(const cv::Mat& src,cv::Mat& dst,int baseGray,int block_x,int block_y)  
{  
	//cv::Mat integral_img(src.rows+1,src.cols+1,CV_64FC1);
	//integral(src,integral_img,CV_64FC1);//积分图像
	//cv::Mat dst_zero=cv::Mat::zeros(src.rows,src.cols,src.type());
	//for(int i=block_y/2;i<integral_img.rows-block_y/2;i++)
	//{
	//	for(int j=block_x/2;j<integral_img.cols-block_x/2;j++)
	//	{
	//		float A=integral_img.at<double>(i+block_y/2,j+block_x/2);
	//		float B=integral_img.at<double>(i-block_y/2,j-block_x/2);
	//		float C=integral_img.at<double>(i-block_y/2,j+block_x/2);
	//		float D=integral_img.at<double>(i+block_y/2,j-block_x/2);
	//		float temp=A+B-C-D;
	//		float x=temp/(block_x*block_y);
	//		float _x=1/(x+0.000001);
	//		dst_zero.at<uchar>(i,j)=min((int)255,(int)(src.at<uchar>(i,j)*baseGray*_x));
	//	}
	//}
	//dst=dst_zero.clone();

	cv::Mat integral_img(src.rows+1,src.cols+1,CV_64FC1);
	integral(src,integral_img,CV_64FC1);//积分图像
	cv::Rect rect(0,0,src.cols,src.rows);
	cv::Mat integral_img_part;
	integral_img(rect).copyTo(integral_img_part);
	cv::Mat kern = (cv::Mat_<float>(5,5) <<  1,	0,  0,  0,  -1,  
											0,  0,  0,	0,	0,  
											0,	0,  0,	0,	0,
											0,	0,	0,	0,	0,
											-1,	0,	0,	0,	1); 
	cv::Mat dstF;  
	filter2D(integral_img_part,dstF,integral_img_part.depth(),kern); //,cv::Point(0,0) 
	cv::Mat src_64;
	src.convertTo(src_64,CV_64FC1);
	cv::Mat dst_64=cv::Mat::zeros(src.size(), CV_64FC1);
	dst_64=src_64*25*baseGray/dstF;
	cv::normalize(dst_64,dst,0,255,cv::NORM_MINMAX,CV_8UC1);
}

void H_BaseImageProcess::Img_Save_Image_OpenCV(const cv::Mat& src,int iStr) 
{
	stringstream ss;
	string str;
	ss<<iStr;
	ss>>str;
	const char* c_str = str.c_str();
	char* s=".bmp";
	str+=s;
	imwrite(str,src);
}

void H_BaseImageProcess::Img_Show_Image_OpenCV(const cv::Mat& src,int iStr) 
{
	stringstream ss;
	string str;
	ss<<iStr;
	ss>>str;
	const char* c_str = str.c_str();
	char* s=".bmp";
	str+=s;
	cv::namedWindow(str,0);
	cv::imshow(str,src);
}

void H_BaseImageProcess::Img_Otsu(const cv::Mat& src,cv::Mat& dst)
{
    long nl = src.rows; 
    long nc = src.cols;
    long NO = nc * nl;
    int h[256];
    double p[256];
	cv::Mat src0=src.clone();
	memset(h, 0, sizeof(h));
	memset(p, 0.0f, sizeof(p));

    for( int i = 0; i < nl; i++)
    {
        uchar * data = src0.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++)
		{
            h[data[j]]++; 
		}
    }
    int ThresoldVal = 0;
    double MG = 0.0f, P1, M ;
 
    for(int k = 0; k < 256; k++)
	{
        p[k] = h[k] / double(NO); 
		MG += k * p[k];
	}
 
    double fmax, fmax_max = -1;
    P1 =  M = 0.0f; // P1 = 0.0f, M = 0.0f;
    for(int k = 0; k < 256; k++)
    {
        P1 += p[k], M += k*p[k];
        fmax = ((MG * P1 - M) * (MG * P1 - M))/((1-P1) * P1 + .5 );
        if(fmax > fmax_max)
		{
			fmax_max = fmax;
			ThresoldVal = k;
		}
    }
	cv::threshold(src0,dst,ThresoldVal,255,3);
}

void H_BaseImageProcess::Img_SSR(const cv::Mat& src,cv::Mat& dst,int scale)
{
	cv::Mat srcimg_gauss;
	GaussianBlur( src, srcimg_gauss, cv::Size( 15, 15 ), 0, 0 );
	cv::Mat srcimgf,srcimg_gaussf;
	src.convertTo(srcimgf,CV_32F,1.0,1.0);//类型转换
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
	temp.convertTo(dst,CV_8U);//类型转换
}

void H_BaseImageProcess::Img_Sobel(const cv::Mat& src,cv::Mat& dstx,cv::Mat& dsty,cv::Mat& dstxy,int scale,int delta,int ddepth)
{
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	Sobel( src, grad_x, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );
	Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );
	addWeighted( abs_grad_x, 0.5, abs_grad_x, 0.5, 0, dstx );
	addWeighted( abs_grad_y, 0.5, abs_grad_y, 0.5, 0, dsty );
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dstxy );
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

void H_BaseImageProcess::Img_Relief(const cv::Mat& src,cv::Mat& dst1,cv::Mat& dst2)
{  
	cv::Mat img0(src.size(),CV_8UC1);  
	cv::Mat img1(src.size(),CV_8UC1); 
	int color=1;
	cv::Mat src0=src.clone();
    for (int y=1; y<src0.rows-1; y++)  
    {  
        uchar *p0 = src0.ptr<uchar>(y);  
        uchar *p1 = src0.ptr<uchar>(y+1);  
  
        uchar *q0 = img0.ptr<uchar>(y);  
        uchar *q1 = img1.ptr<uchar>(y);  
        for (int x=1; x<src0.cols-1; x++)  
        {  
            for (int i=0; i<3; i++)  
            {  
                int tmp0 = p1[color*(x+1)+i]-p0[color*(x-1)+i]+128;//浮雕  
                if (tmp0<0)  
                    q0[color*x+i]=0;  
                else if(tmp0>255)  
                    q0[color*x+i]=255;  
                else  
                    q0[color*x+i]=tmp0;  
  
                int tmp1 = p0[color*(x-1)+i]-p1[color*(x+1)+i]+128;//雕刻  
                if (tmp1<0)  
                    q1[color*x+i]=0;  
                else if(tmp1>255)  
                    q1[color*x+i]=255;  
                else  
                    q1[color*x+i]=tmp1;  
            }  
        }  
    }  
	dst1=img0.clone();
	dst2=img1.clone();
} 

void H_BaseImageProcess::Img_Hist(const cv::Mat& src,cv::Mat& dst)
{   
    int bins = 256;    
    int hist_size[] = {bins};    
    float range[] = { 0, 256 };    
    const float* ranges[] = { range};    
  
    int channels[] = {0};    
	calcHist( &src, 1, channels, cv::Mat(),src, 1, hist_size, ranges,true,false);    
  
    //绘制直方图图像  
    int hist_height=256;    
    double max_val;  //直方图的最大值  
    int scale = 2;   //直方图的宽度  
    cv::minMaxLoc(src, 0, &max_val, 0, 0); //计算直方图最大值  
  
	dst = cv::Mat::zeros(hist_height,bins*scale, CV_8UC3); //创建一个直方图图像并初始化为0   
  
    //在直方图图像中写入直方图数据  
    for(int i=0;i<bins;i++)    
    {    
        float bin_val = src.at<float>(i); // 第i灰度级上的数      
        int intensity = cvRound(bin_val*hist_height/max_val);  //要绘制的高度    
        rectangle(dst,cv::Point(i*scale,hist_height-1),cv::Point((i+1)*scale - 1, hist_height - intensity),CV_RGB(255,255,255));    
    } 
} 

void H_BaseImageProcess::Img_Expansion_And_Extrusion(const cv::Mat& src,cv::Mat& dst1,cv::Mat& dst2)
{  
	int width = src.cols;  
    int heigh = src.rows;  
	cv::Point center(width/2,heigh/2);  
 
	//图像放大
	cv::Mat img1(src.size(),CV_8UC1); 
	src.copyTo(img1);
	int R1 = sqrtf(width*width+heigh*heigh)/2; //直接关系到放大的力度,与R1成正比;  
      
	for (int y=0; y<heigh; y++)  
	{  
		uchar *img1_p = img1.ptr<uchar>(y);  
		for (int x=0; x<width; x++)  
		{  
			int dis = cv::norm(cv::Point(x,y)-center);  
			if (dis<R1)  
			{  
				int newX = (x-center.x)*dis/R1+center.x;  
				int newY = (y-center.y)*dis/R1+center.y; 

				img1_p[x]=src.at<uchar>(newY,newX);  
			}  
		}  
	} 
	dst1=img1.clone();
	//图像缩小
	cv::Mat img2(src.size(),CV_8UC1);  
	src.copyTo(img2); 
	for (int y=0; y<heigh; y++)  
	{  
		uchar *img2_p = img2.ptr<uchar>(y);  
		for (int x=0; x<width; x++)  
		{  
			double theta = atan2((double)(y-center.y),(double)(x-center.x));//使用atan出现问题~  
			int R2 = sqrtf(cv::norm(cv::Point(x,y)-center))*8; //直接关系到挤压的力度，与R2成反比;                
			int newX = center.x+(int)(R2*cos(theta));   
			int newY = center.y+(int)(R2*sin(theta));  
  
			if(newX<0) newX=0;  
			else if(newX>=width) newX=width-1;  
			if(newY<0) newY=0;  
			else if(newY>=heigh) newY=heigh-1;  
                      
			img2_p[x]=src.at<uchar>(newY,newX);    
		}  
	} 
	dst2=img2.clone();
}  

void H_BaseImageProcess::Img_Transform(const cv::Mat& src,cv::Mat& dst)
{
	cv::Mat img_warp,img_canny,img_down;

	cv::Point2f srcQuad[4], dstQuad[4];  
	cv::Mat warp_matrix( 3, 3, CV_32FC1 );

	img_warp=cv::Mat::zeros( src.rows, src.cols, src.type() );

	srcQuad[0].x = 0;           //src Top left  
	srcQuad[0].y = 0;  
	srcQuad[1].x = src.cols - 1;  //src Top right  
	srcQuad[1].y = 0;  
	srcQuad[2].x = 0;           //src Bottom left  
	srcQuad[2].y = src.rows - 1;  
	srcQuad[3].x = src.cols - 1;  //src Bot right  
	srcQuad[3].y = src.rows - 1;  

	dstQuad[0].x = src.cols*0.25;  //dst Top left  
	dstQuad[0].y = src.rows*0.33;  
	dstQuad[1].x = src.cols*0.9;  //dst Top right  
	dstQuad[1].y = src.rows*0.25;  
	dstQuad[2].x = src.cols*0.2;  //dst Bottom left  
	dstQuad[2].y = src.rows*0.7;        
	dstQuad[3].x = src.cols*0.7;  //dst Bot right  
	dstQuad[3].y = src.rows*0.5;  

	warp_matrix=getPerspectiveTransform(srcQuad,dstQuad);//由四对点计算透射变换   
	warpPerspective(src,img_warp,warp_matrix,img_warp.size());//对图像进行透视变换 dst.size()  

	dst=img_warp.clone();
}

void H_BaseImageProcess::Img_DWT(const cv::Mat& src,cv::Mat& dst,int nLayer)
{
   	IplImage pSrc1 = src;
	IplImage *pSrc=&pSrc1;
	CvSize size = cvGetSize(pSrc);
	if ((pSrc->width >> nLayer) << nLayer != pSrc->width)
	{
		size.width = ((pSrc->width >> nLayer) + 1) << nLayer;
	}
	if ((pSrc->height >> nLayer) << nLayer != pSrc->height)
	{
		size.height = ((pSrc->height >> nLayer) + 1) << nLayer;
	}
	IplImage *pWavelet = cvCreateImage(size, IPL_DEPTH_32F, pSrc->nChannels);
	if (pWavelet)
	{
		cvSetImageROI(pWavelet, cvRect(0, 0, pSrc->width, pSrc->height));
		cvConvertScale(pSrc, pWavelet, 1, -128);
		cvResetImageROI(pWavelet);
		IplImage *pImage = cvCreateImage(cvGetSize(pWavelet), IPL_DEPTH_32F, 1);
		if (pImage)
		{
			for (int i = 1; i <= pWavelet->nChannels; i++)
			{
				cvSetImageCOI(pWavelet, i);
				cvCopy(pWavelet, pImage, NULL);
				if (pImage->nChannels == 1 && pImage->depth == IPL_DEPTH_32F &&((pImage->width >> nLayer) << nLayer) == pImage->width &&((pImage->height >> nLayer) << nLayer) == pImage->height)
				{
					int x, y;
					float   fValue   = 0;
					float   fRadius  = sqrt(2.0f);
					int     nWidth   = pImage->width;
					int     nHeight  = pImage->height;
					int     nHalfW   = nWidth / 2;
					int     nHalfH   = nHeight / 2;
					float **pData    = new float*[pImage->height];
					float  *pRow     = new float[pImage->width];
					float  *pColumn  = new float[pImage->height];
					for (int i = 0; i < pImage->height; i++)
					{
						pData[i] = (float*) (pImage->imageData + pImage->widthStep * i);
					}
					// 多层小波变换
					for (int n = 0; n < nLayer; n++, nWidth /= 2, nHeight /= 2, nHalfW /= 2, nHalfH /= 2)
					{
						// 水平变换
						for (y = 0; y < nHeight; y++)
						{
							// 奇偶分离
							memcpy(pRow, pData[y], sizeof(float) * nWidth);
							for (int i = 0; i < nHalfW; i++)
							{
								x = i * 2;
								pData[y][i] = pRow[x];
								pData[y][nHalfW + i] = pRow[x + 1];
							}
							// 提升小波变换
							for (int i = 0; i < nHalfW - 1; i++)
							{
								fValue = (pData[y][i] + pData[y][i + 1]) / 2;
								pData[y][nHalfW + i] -= fValue;
							}
							fValue = (pData[y][nHalfW - 1] + pData[y][nHalfW - 2]) / 2;
							pData[y][nWidth - 1] -= fValue;
							fValue = (pData[y][nHalfW] + pData[y][nHalfW + 1]) / 4;
							pData[y][0] += fValue;
							for (int i = 1; i < nHalfW; i++)
							{
								fValue = (pData[y][nHalfW + i] + pData[y][nHalfW + i - 1]) / 4;
								pData[y][i] += fValue;
							}
							// 频带系数
							for (int i = 0; i < nHalfW; i++)
							{
								pData[y][i] *= fRadius;
								pData[y][nHalfW + i] /= fRadius;
							}
						}
						// 垂直变换
						for (x = 0; x < nWidth; x++)
						{
							// 奇偶分离
							for (int i = 0; i < nHalfH; i++)
							{
								y = i * 2;
								pColumn[i] = pData[y][x];
								pColumn[nHalfH + i] = pData[y + 1][x];
							}
							for (int i = 0; i < nHeight; i++)
							{
								pData[i][x] = pColumn[i];
							}
							// 提升小波变换
							for (int i = 0; i < nHalfH - 1; i++)
							{
								fValue = (pData[i][x] + pData[i + 1][x]) / 2;
								pData[nHalfH + i][x] -= fValue;
							}
							fValue = (pData[nHalfH - 1][x] + pData[nHalfH - 2][x]) / 2;
							pData[nHeight - 1][x] -= fValue;
							fValue = (pData[nHalfH][x] + pData[nHalfH + 1][x]) / 4;
							pData[0][x] += fValue;
							for (int i = 1; i < nHalfH; i++)
							{
								fValue = (pData[nHalfH + i][x] + pData[nHalfH + i - 1][x]) / 4;
								pData[i][x] += fValue;
							}
							// 频带系数
							for (int i = 0; i < nHalfH; i++)
							{
								pData[i][x] *= fRadius;
								pData[nHalfH + i][x] /= fRadius;
							}
						}
					}
					delete[] pData;
					delete[] pRow;
					delete[] pColumn;
				}
				cvCopy(pImage, pWavelet, NULL);
			}
			cvSetImageCOI(pWavelet, 0);
			cvReleaseImage(&pImage);
	   }
	   cvSetImageROI(pWavelet, cvRect(0, 0, pSrc->width, pSrc->height));
	   cvConvertScale(pWavelet, pSrc, 1, 128);
	   cvResetImageROI(pWavelet); 
	   cvReleaseImage(&pWavelet);
	}
	dst = pSrc;
}

void H_BaseImageProcess::Img_IDWT(const cv::Mat& src,cv::Mat& dst,int nLayer)
{
   	IplImage pSrc1 = src;
	IplImage *pSrc=&pSrc1;
	CvSize size = cvGetSize(pSrc);
	if ((pSrc->width >> nLayer) << nLayer != pSrc->width)
	{
		size.width = ((pSrc->width >> nLayer) + 1) << nLayer;
	}
	if ((pSrc->height >> nLayer) << nLayer != pSrc->height)
	{
		size.height = ((pSrc->height >> nLayer) + 1) << nLayer;
	}
	IplImage *pWavelet = cvCreateImage(size, IPL_DEPTH_32F, pSrc->nChannels);
	if (pWavelet)
	{
		cvSetImageROI(pWavelet, cvRect(0, 0, pSrc->width, pSrc->height));
		cvConvertScale(pSrc, pWavelet, 1, -128);
		cvResetImageROI(pWavelet);
		IplImage *pImage = cvCreateImage(cvGetSize(pWavelet), IPL_DEPTH_32F, 1);
		if (pImage)
		{
			for (int i = 1; i <= pWavelet->nChannels; i++)
			{
				cvSetImageCOI(pWavelet, i);
				cvCopy(pWavelet, pImage, NULL);
				if (pImage->nChannels == 1 &&
				pImage->depth == IPL_DEPTH_32F &&((pImage->width >> nLayer) << nLayer) == pImage->width &&((pImage->height >> nLayer) << nLayer) == pImage->height)
				{
					int     x, y;
					float   fValue   = 0;
					float   fRadius  = sqrt(2.0f);
					int     nWidth   = pImage->width >> (nLayer - 1);
					int     nHeight  = pImage->height >> (nLayer - 1);
					int     nHalfW   = nWidth / 2;
					int     nHalfH   = nHeight / 2;
					float **pData    = new float*[pImage->height];
					float  *pRow     = new float[pImage->width];
					float  *pColumn  = new float[pImage->height];
					for (int i = 0; i < pImage->height; i++)
					{
						pData[i] = (float*) (pImage->imageData + pImage->widthStep * i);
					}
					// 多层小波恢复
					for (int n = 0; n < nLayer; n++, nWidth *= 2, nHeight *= 2, nHalfW *= 2, nHalfH *= 2)
					{
						// 垂直恢复
						for (x = 0; x < nWidth; x++)
						{
							// 频带系数
							for (int i = 0; i < nHalfH; i++)
							{
								pData[i][x] /= fRadius;
								pData[nHalfH + i][x] *= fRadius;
							}
							// 提升小波恢复
							fValue = (pData[nHalfH][x] + pData[nHalfH + 1][x]) / 4;
							pData[0][x] -= fValue;
							for (int i = 1; i < nHalfH; i++)
							{
								fValue = (pData[nHalfH + i][x] + pData[nHalfH + i - 1][x]) / 4;
								pData[i][x] -= fValue;
							}
							for (int i = 0; i < nHalfH - 1; i++)
							{
								fValue = (pData[i][x] + pData[i + 1][x]) / 2;
								pData[nHalfH + i][x] += fValue;
							}
							fValue = (pData[nHalfH - 1][x] + pData[nHalfH - 2][x]) / 2;
							pData[nHeight - 1][x] += fValue;
							// 奇偶合并
							for (int i = 0; i < nHalfH; i++)
							{
								y = i * 2;
								pColumn[y] = pData[i][x];
								pColumn[y + 1] = pData[nHalfH + i][x];
							}
							for (int i = 0; i < nHeight; i++)
							{
								pData[i][x] = pColumn[i];
							}
						}
						// 水平恢复
						for (y = 0; y < nHeight; y++)
						{
							// 频带系数
							for (int i = 0; i < nHalfW; i++)
							{
								pData[y][i] /= fRadius;
								pData[y][nHalfW + i] *= fRadius;
							}
							// 提升小波恢复
							fValue = (pData[y][nHalfW] + pData[y][nHalfW + 1]) / 4;
							pData[y][0] -= fValue;
							for (int i = 1; i < nHalfW; i++)
							{
								fValue = (pData[y][nHalfW + i] + pData[y][nHalfW + i - 1]) / 4;
								pData[y][i] -= fValue;
							}
							for (int i = 0; i < nHalfW - 1; i++)
							{
								fValue = (pData[y][i] + pData[y][i + 1]) / 2;
								pData[y][nHalfW + i] += fValue;
							}
							fValue = (pData[y][nHalfW - 1] + pData[y][nHalfW - 2]) / 2;
							pData[y][nWidth - 1] += fValue;
							// 奇偶合并
							for (int i = 0; i < nHalfW; i++)
							{
								x = i * 2;
								pRow[x] = pData[y][i];
								pRow[x + 1] = pData[y][nHalfW + i];
							}
							memcpy(pData[y], pRow, sizeof(float) * nWidth);
						}
					 }
					 delete[] pData;
					 delete[] pRow;
					 delete[] pColumn;
				}
				cvCopy(pImage, pWavelet, NULL);
			}
			cvSetImageCOI(pWavelet, 0);
			cvReleaseImage(&pImage);
	   }
	   cvSetImageROI(pWavelet, cvRect(0, 0, pSrc->width, pSrc->height));
	   cvConvertScale(pWavelet, pSrc, 1, 128);
	   cvResetImageROI(pWavelet); 
	   cvReleaseImage(&pWavelet);
	}
	dst = pSrc;
}

void H_BaseImageProcess::Img_GrayStatistics(const cv::Mat& src,cv::Mat& dst_all,cv::Mat& dst_row,cv::Mat& dst_col)
{
	//总图像灰度曲线
	int Tatal_Col;
	int *Avr_Col=new int[src.cols]; 
	for(int i=0;i<src.cols;i++)
	{
		Tatal_Col=0;
		for(int j=0;j<src.rows;j++)
		{
			Tatal_Col+=src.at<uchar>(j,i);
		}
		Avr_Col[i]=Tatal_Col/src.rows;
	}

	//绘制图像  
    int height=256;    
    int scale = 1;   //直方图的宽度  
	int bins=src.cols;
  
    cv::Mat img = cv::Mat::zeros(height,bins*scale, CV_8UC3);
    for(int i=0;i<bins;i++)    
    {      
        rectangle(img,cv::Point(i*scale,img.rows-1-Avr_Col[i]), cv::Point((i+1)*scale,img.cols-1),CV_RGB(255,0,255));    
    } 
	dst_all=img.clone();
	//namedWindow("img",0);
	//imshow("img",img);
	//单行的灰度曲线
	int *Avr_Row_Only=new int[src.cols]; 
	int Row=511;
	for(int i=0;i<src.cols;i++)
	{
		Avr_Row_Only[i]=src.at<uchar>(Row,i);
	}
	int height_row=256;    
    int scale_row = 1;   //直方图的宽度  
	int bins_row=src.cols;
	cv::Mat img_row = cv::Mat::zeros(height_row,bins_row*scale_row, CV_8UC3);
    for(int i=0;i<bins_row;i++)    
    {      
        rectangle(img_row,cv::Point(i*scale_row,img_row.rows-1-Avr_Row_Only[i]), cv::Point((i+1)*scale_row,img_row.cols-1),CV_RGB(255,255,0));    
    } 
	dst_row=img_row.clone();
	//namedWindow("img_row",0);
	//imshow("img_row",img_row);
	//单列的灰度曲线
	int *Avr_Col_Only=new int[src.rows]; 
	int Col=50;
	for(int i=0;i<src.rows;i++)
	{
		Avr_Col_Only[i]=src.at<uchar>(i,Col);
	}
	int height_col=256;    
    int scale_col = 1;   //直方图的宽度  
	int bins_col=src.rows;
	cv::Mat img_col = cv::Mat::zeros(height_col,bins_col*scale_col, CV_8UC3);
    for(int i=0;i<bins_col;i++)    
    {      
        rectangle(img_col,cv::Point(i*scale_col,img_col.rows-1-Avr_Col_Only[i]), cv::Point((i+1)*scale_col,img_col.cols-1),CV_RGB(0,255,255));    
    } 
	dst_col=img_col.clone();
	//namedWindow("img_col",0);
	//imshow("img_col",img_col);
}

void H_BaseImageProcess::Img_Feather(const cv::Mat& src,cv::Mat& dst)
{
    float mSize = 0.5; 

	int width=src.cols;  
    int heigh=src.rows;  
    int centerX=width>>1;  
    int centerY=heigh>>1;  
      
    int maxV=centerX*centerX+centerY*centerY;  
    int minV=(int)(maxV*(1-mSize));  
    int diff= maxV -minV;  
    float ratio = width >heigh ? (float)heigh/(float)width : (float)width/(float)heigh;  
      
    cv::Mat img;  
    src.copyTo(img);  
  
	cv::Scalar avg=mean(src);  
	dst=cv::Mat::zeros(img.size(),CV_8UC1); 
    cv::Mat mask1u[3];   
    for (int y=0;y<heigh;y++)  
    {  
        uchar* imgP=img.ptr<uchar>(y);  
        uchar* dstP=dst.ptr<uchar>(y);  
        for (int x=0;x<width;x++)  
        {  
			int gray=imgP[x]; 
  
            float dx=centerX-x;  
            float dy=centerY-y;  
              
            if(width > heigh)  
                 dx= (dx*ratio);  
            else  
                dy = (dy*ratio);  
  
            int dstSq = dx*dx + dy*dy;  
  
            float v = ((float) dstSq / diff)*255;  

			gray = (int)(gray +v);
			gray = (gray>255 ? 255 : (gray<0? 0 : gray));  
			dstP[x] = (uchar)gray; 
        }  
    }
	//namedWindow("羽化",0);
 //   imshow("羽化",dst); 
}

void H_BaseImageProcess::Img_BrightLight(const cv::Mat& src,cv::Mat& dst)
{
	int R=11; 
	int width=src.cols;  
    int heigh=src.rows;  
	cv::Mat img;  
    src.copyTo(img);  
  
	dst=cv::Mat::zeros(img.size(),CV_8UC1);  
  
    float tmp,gray;  
    for (int y=0;y<heigh;y++)  
    {  
        uchar* imgP=img.ptr<uchar>(y);  
        uchar* dstP=dst.ptr<uchar>(y);  
        for (int x=0;x<width;x++)  
        {   
			gray = (float)imgP[x];
			if(gray>127.5)  
			{
				tmp = gray+(255-gray)*(gray-127.5)/127.5; 
			}
			else
			{
				tmp = gray*gray/127.5;  
			}
            tmp=tmp>255?255:tmp;  
            tmp=tmp<0?0:tmp;  
            dstP[x]=(uchar)(tmp);  
        }  
    }
	//namedWindow("强光",0);
	//imshow("强光",dst);
}

void H_BaseImageProcess::Img_HighContrast(const cv::Mat& src,cv::Mat& dst)
{
	int R=11; 
	int width=src.cols;  
    int heigh=src.rows;  
    cv::Mat img;  
    src.copyTo(img);  
    cv::Mat avg;   
    blur(img,avg,cv::Size(R,R));   
	dst=cv::Mat::zeros(img.rows,img.cols,CV_8UC1); 
    float tmp;  
    for (int y=0;y<heigh;y++)  
    {  
        uchar* imgP=img.ptr<uchar>(y);  
        uchar* avgP=avg.ptr<uchar>(y);  
        uchar* dstP=dst.ptr<uchar>(y);  
        for (int x=0;x<width;x++)  
        {  
			float gray = ((float)imgP[x]-(float)avgP[x]);  
            tmp = 128+abs(gray)*gray/(2*R);  
            tmp=tmp>255?255:tmp;  
            tmp=tmp<0?0:tmp;  
            dstP[x]=(uchar)(tmp);  
        }  
    }   
}

void H_BaseImageProcess::Img_Spread(const cv::Mat& src,cv::Mat& dst)
{
	int width=src.cols;  
    int heigh=src.rows; 
	cv::Mat src0=src.clone();
    cv::RNG rng;  
	cv::Mat img(src0.size(),CV_8UC1); 
    for (int y=1; y<heigh-1; y++)  
    {  
        uchar* P0  = src0.ptr<uchar>(y);  
        uchar* P1  = img.ptr<uchar>(y);  
        for (int x=1; x<width-1; x++)  
        {  
            int tmp=rng.uniform(0,9);  
			P1[x]=src0.at<uchar>(y-1+tmp/3,(x-1+tmp%3)); 
        }  
    }  
	dst=img.clone();
}

void H_BaseImageProcess::Img_Sketch(const cv::Mat& src,cv::Mat& dst)
{
	int width=src.cols;  
    int heigh=src.rows;  
    cv::Mat gray0,gray1; 

	cv::Rect rect(0,0,src.cols,src.rows);
	src(rect).copyTo(gray0);
	cv::Mat src_clone;
	src(rect).copyTo(src_clone);

    addWeighted(gray0,-1,NULL,0,255,gray1);  
	
    cv::GaussianBlur(gray1,gray1,cv::Size(7,7),0); //21 

	cv::Mat img0;
	img0.create(gray1.size(),CV_8UC1); 
    for (int y=0; y<heigh; y++)  
    {    
        uchar* P0  = gray0.ptr<uchar>(y);  
        uchar* P1  = gray1.ptr<uchar>(y);  
        uchar* P  = img0.ptr<uchar>(y);  
        for (int x=0; x<width; x++)  
        {  
            int tmp0=P0[x];  
            int tmp1=P1[x];  
            P[x] =(uchar) min((tmp0+(tmp0*tmp1)/(256-tmp1)),255);  
        }   
    }  
	int morph_size=7;
	cv::Mat element = cv::getStructuringElement(1, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );
	morphologyEx( src_clone, src_clone, 2, element );
	addWeighted( src_clone, 0.5, img0, 0.5, 0.0, dst);
	//namedWindow("素描",0);
	//imshow("素描",img);
}

void H_BaseImageProcess::Img_Wind(const cv::Mat& src,cv::Mat& dst)
{
	int num=10;//   num：风线密度  
	int num1=20;//  num1：风线长度 
  
    int width=src.cols;  
    int heigh=src.rows;  
    cv::Mat img;  
    src.copyTo(img);  
  
    cv::Point center(width/2,heigh/2);   
    cv::RNG rng;    
    for (int y=0; y<heigh; y++)  
    {    
        uchar *imgP  = img.ptr<uchar>(y);   
        for (int i=0;i<num;i++)      //  num：风线密度  
        {  
            int newX=rng.uniform(i*width/num,(i+1)*width/num);  
            int newY = y;  

            if(newX<0)newX=0;  
            if(newX>width-1)newX=width-1;   
			uchar tmp = src.at<uchar>(newY,newX); 
            for (int j=0; j<num1; j++)   //num1：风线长度  
            {  
                int tmpX=newX-j;//减：风向左；加：风向右  
                if(tmpX<0)tmpX=0;  
                if(tmpX>width-1)tmpX=width-1;  
				imgP[tmpX]=tmp;
            }  
        }    
    } 
	dst=img.clone(); 
}

void H_BaseImageProcess::Img_Cartoons(const cv::Mat& src,cv::Mat& dst)
{
	cv::Mat img;  
    bilateralFilter(src,img,5,150,150);  
    bilateralFilter(img,src,5,150,150);   

    cv::Mat imgL;   
    Laplacian(src,imgL,-1,3,1);  
  
    cv::Mat imgC;  
    Canny(src,imgC,30,90);  
  
    cv::Mat imgS,imgSx,imgSy,imgS0;  
    Sobel(src,imgSx,-1,0,1);  
    Sobel(src,imgSx,-1,1,0);  
    imgS=imgSx+imgSy;  
    Sobel(src,imgS0,-1,1,1);  
      
    cv::Mat imgTotal;  
    imgTotal=imgC+imgS+imgL;  
    normalize(imgTotal,imgTotal,255,0,CV_MINMAX);  
	GaussianBlur(imgTotal,imgTotal,cv::Size(3,3),3);  
	threshold(imgTotal,imgTotal,100,255,cv::THRESH_BINARY_INV); 

	int morph_size=1;
	cv::Mat element = getStructuringElement(1, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );
	dilate( imgTotal, imgTotal, element );
	dst= imgTotal.clone();
}

void H_BaseImageProcess::Img_BrightStretching(const cv::Mat& src,cv::Mat& dst)
{
	dst=src.clone();
	int Avg=0;
	int* Wave=new int[src.rows];
	int Standard=100;
	memset(Wave, 0, src.rows);
	for(int j=0;j<src.cols;j++)
	{
		for(int i=0;i<src.rows;i++)
		{
			Avg+=src.at<uchar>(i,j);
		}
		Avg/=src.rows;
		int Wave_Total=0;
		for(int i=0;i<src.rows;i++)
		{
			Wave[i]=src.at<uchar>(i,j)-Avg;
			dst.at<uchar>(i,j)=Avg+(Standard-Avg)*Standard/(Avg+Standard)+Wave[i];
			Wave_Total=Wave_Total+(Wave[i]*Wave[i]);
		}
		int S=Wave_Total/src.rows;
	}
}

void H_BaseImageProcess::Img_Surf(const cv::Mat& src,cv::Mat& dst)
{
	cv::vector<cv::KeyPoint> keypoints;  
	cv::SurfFeatureDetector surf(2500);  
	surf.detect (src,keypoints);  
	cv::Scalar& color = cv::Scalar(0,0,255);
	cv::cvtColor(src,dst,CV_GRAY2BGR);
	drawKeypoints (src,keypoints,dst,color,cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);  
}

void H_BaseImageProcess::Img_Meanshift(const cv::Mat& src,cv::Mat& dst)
{
	int spatialRad=10,colorRad=10,maxPryLevel=1;
	cv::Mat src0;
	cv::cvtColor( src, src0, CV_GRAY2BGR );
	
	//可更改的三个参数
	spatialRad=10;
    colorRad=10;
    maxPryLevel=1;

    pyrMeanShiftFiltering(src0,dst,spatialRad,colorRad,maxPryLevel);
    cv::RNG rng=cv::theRNG();

    cv::Mat mask(dst.rows+2,dst.cols+2,CV_8UC1,cv::Scalar::all(0));
    for(int i=0;i<dst.rows;i++)    //opencv图像等矩阵也是基于0索引的
	{
        for(int j=0;j<dst.cols;j++)
		{
            if(mask.at<uchar>(i+1,j+1)==0)
            {
                cv::Scalar newcolor(rng(255),rng(255),rng(255));
                floodFill(dst,mask,cv::Point(j,i),newcolor,0,cv::Scalar::all(1),cv::Scalar::all(1));
            }
		}
	}
}

void H_BaseImageProcess::Img_Clustering(const cv::Mat& src,cv::Mat& dst)
{  
	cv::Mat src0;
	cv::cvtColor( src, src0, CV_GRAY2BGR );
	IplImage pSrc1 = src0;
	IplImage *imgA=&pSrc1;
  
	unsigned long int size;  
	size = imgA->width * imgA->height;//取得图片大小  
  
	CvMat *clusters;//分类后的矩阵  
	clusters = cvCreateMat (size, 1, CV_32SC1);//32位1通道的矩阵  
	CvMat *points;//分类前的样例浮点矩阵  
	points = cvCreateMat (size, 1, CV_32FC3); //32位3通道的矩阵  
    
	for (int i = 0; i < size; i++)   
	{  
		points->data.fl[i*3] = (unsigned char) imgA->imageData[i*3];  
		points->data.fl[i*3 + 1] = (unsigned char) imgA->imageData[i*3 + 1];  
		points->data.fl[i*3 + 2] = (unsigned char) imgA->imageData[i*3 + 2];   
	} //得到三通道图像的数据  
  
	cvKMeans2 (points, MAX_CLUSTERS, clusters,  cvTermCriteria (CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0));  //拆分为8类聚合，最大迭代次数是10，精度是1.0  	
  
	CvMat *color = cvCreateMat (MAX_CLUSTERS, 1, CV_32FC3);//8行1列的三通道浮点矩阵  
	CvMat *count = cvCreateMat (MAX_CLUSTERS, 1, CV_32SC1);//8行1列的单通道整数矩阵，用作计数  
	cvSetZero (color);  
	cvSetZero (count);  
  
	for (int i = 0; i < size; i++)  
	{  
		int idx = clusters->data.i[i];  
		int j = ++count->data.i[idx];  
		color->data.fl[idx * 3 ] = color->data.fl[idx * 3 ] * (j - 1) / j + points->data.fl[i * 3 ] / j;  
		color->data.fl[idx * 3 + 1] = color->data.fl[idx * 3 + 1] * (j - 1) / j + points->data.fl[i * 3 + 1] / j;  
		color->data.fl[idx * 3 + 2] = color->data.fl[idx * 3 + 2] * (j - 1) / j + points->data.fl[i * 3 + 2] / j;  
	}  
  
	//把处理过的数据打回imgA  
	for (int i = 0; i < size; i++)  
	{  
		int idx = clusters->data.i[i];  
		imgA->imageData[i * 3 ] = (char) color->data.fl[idx * 3 ];  
		imgA->imageData[i * 3 + 1] = (char) color->data.fl[idx * 3 + 1];  
		imgA->imageData[i * 3 + 2] = (char) color->data.fl[idx * 3 + 2];  
	}  

	dst = imgA;
}

void H_BaseImageProcess::Img_Fold(const cv::Mat& src,cv::Mat& dst)
{
	IplImage pSrc1 = src;
	IplImage *pSrc=&pSrc1;
	IplImage *image=cvCloneImage(pSrc);  
    int width=image->width;  
    int height=image->height;  
    int step=image->widthStep;  
    int channel=image->nChannels;  
    uchar* data=(uchar *)image->imageData;  
    int sign=-1;  
    for(int i=0;i<height;i++)  
    {     
        int cycle=10;  
        int margin=(i%cycle);  
        if((i/cycle)%2==0)  
        {  
            sign=-1;  
        }  
        else  
        {  
            sign=1;  
        }  
        if(sign==-1)  
        {     
            margin=cycle-margin;  
            for(int j=0;j<width-margin;j++)  
            {             
                for(int k=0;k<channel;k++)  
                {  
                    data[i*step+j*channel+k]=data[i*step+(j+margin)*channel+k];  
                }  
            }  
        }  
        else if(sign==1)  
        {         
            for(int j=0;j<width-margin;j++)  
            {  
                for(int k=0;k<channel;k++)  
                {  
                    data[i*step+j*channel+k]=data[i*step+(j+margin)*channel+k];  
                }  
            }  
        }     
    }  
	dst=image;
}

void H_BaseImageProcess::Img_GaborFilter(const cv::Mat& src,cv::Mat& dst) 
{  
    double Kmax = PI/2;  
    double f = sqrt(2.0);  
    double sigma = 2*PI;  
    int U = 7;  
    int V = 4;  
    int GaborH = 65;  
    int GaborW = 65;  
   
    cv::Mat kernel_re, kernel_im;  
    cv::Mat dst_re, dst_im, dst_mag;  
  
    cv::Point archor(-1,-1);  
    int ddepth = -1;  
    double delta = 0;  
   
    cv::Mat totalMat;  
    for(U = 0; U < 8; U++)
	{  
        cv::Mat colMat;  
        for(V = 0; V < 5; V++)
		{  
            kernel_re = getMyGabor(GaborW, GaborH, U, V, Kmax, f, sigma, CV_64F, "real");  
            kernel_im = getMyGabor(GaborW, GaborH, U, V, Kmax, f, sigma, CV_64F, "imag");  
  
            filter2D(src, dst_re, ddepth, kernel_re);  
            filter2D(src, dst_im, ddepth, kernel_im);  
  
            dst_mag.create(src.rows, src.cols, CV_32FC1);  
            magnitude(cv::Mat_<float>(dst_re),cv::Mat_<float>(dst_im),dst_mag);  
            normalize(dst_mag, dst_mag, 0, 1, CV_MINMAX);   
  
            if(V == 0)  
                colMat = dst_mag;  
            else  
                vconcat(colMat, dst_mag, colMat);  
        }  
        if(U == 0)  
            totalMat = colMat;  
        else  
            hconcat(totalMat, colMat, totalMat);  
    }  
    dst=totalMat.clone();  
} 

void H_BaseImageProcess::Img_QuWu(const cv::Mat& src,cv::Mat& dst,int block,double w)
{
	dst=cv::Mat::zeros( src.rows, src.cols, CV_8U );
	int w1=80;

	cv::Rect rect;
	rect.width=block;
	rect.height=block;
	rect.x=0;
	rect.y=0;

	cv::Mat imgroi=cv::Mat::zeros( block, block, CV_8U );
	cv::Mat dark=cv::Mat::zeros( src.rows, src.cols, CV_8U );
	
	double min=0;
	double max=0;
	for(int i=0;i<src.cols/block;i++)
	{
		for(int j=0;j<src.rows/block;j++)
		{
			src(rect).copyTo(imgroi);
			minMaxLoc(imgroi, &min, &max, 0, 0);
			cv::Mat roidark(block, block, CV_8U, min);
			roidark.copyTo(dark(rect));
			rect.x=block*i;
			rect.y=block*j;
		}
	}
	double min_dark;
	double max_dark;
	cv::Point min_loc;
	cv::Point max_loc;//max_loc是暗原色先验最亮一小块的原坐标
	minMaxLoc(dark, &min_dark, &max_dark, &min_loc, &max_loc);
	rect.x=max_loc.x;
	rect.y=max_loc.y;
	double A_dst;//定义大气光成分的估计值
	double dst_min;
	src(rect).copyTo(imgroi);
	minMaxLoc(imgroi, &dst_min, &A_dst, 0, 0);

	//求透射率
	int k,l,m,n;
	cv::Mat toushelv=cv::Mat::zeros( src.rows, src.cols, CV_8U );
	for(k=0;k<src.rows;k++)
	{
		for(l=0;l<src.cols;l++)
		{
			m=dark.at<uchar>(k,l);
			n=255-w*m;
			toushelv.at<uchar>(k,l)=n;
		}
	}
	//求无雾图像
	int p,q;
	double tx;
	double jj;
	int ix,jx;
	for(p=0;p<src.rows;p++)
	{
	   for(q=0;q<src.cols;q++)
	   {
			tx=toushelv.at<uchar>(p,q);
			tx=tx/255;
			if(tx<0.1)tx=0.1;
			ix=src.at<uchar>(p,q);            
			jj=(ix-A_dst)/tx+A_dst;//根据雾产生模型运算,还原出无雾图像
			int jx=jj;
			dst.at<uchar>(p,q)=jx;
	   }
	}
}