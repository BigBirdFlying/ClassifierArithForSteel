#pragma once
#include "H_Common.h"
#include "H_DataTypeDef.h"

class H_BaseImageProcess
{
public:
	H_BaseImageProcess(void);
	~H_BaseImageProcess(void);

public:
	///<func_info>
	//描述：纵向灰度矫正算法
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据
	//int baseGray			基准灰度值
	//int offSet			最小矫正灰度限制
	//返回值：空
	static void Img_Gray_Change_Y(const cv::Mat& src,cv::Mat& dst,int baseGray,int offSet);
	///</func_info>

	///<func_info>
	//描述：横向灰度矫正算法
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据
	//int baseGray			基准灰度值
	//int offSet			最小矫正灰度限制
	//bool iSelect			选择矫正程序，为1时offSet有用，为2时offSet无用
	//返回值：空
	static void Img_Gray_Change_X(const cv::Mat& src,cv::Mat& dst,int baseGray,int offSet,int iSelect); 
	///<func_info>

	///<func_info>
	//描述：积分图灰度矫正算法
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据
	//int baseGray			基准灰度值
	//int block_x			X方向上无效边界,为偶数
	//int block_y			Y方向上无效边界,为偶数
	//返回值：空
	static void Img_Gray_Change_Integral(const cv::Mat& src,cv::Mat& dst,int baseGray,int block_x,int block_y); 
	///<func_info>

	///<func_info>
	//描述：保存图像
	//参数：
	//const cv::Mat& src	源图像数据
	//int iStr				图像数字标号
	//返回值：空
	static void Img_Save_Image_OpenCV(const cv::Mat& src,int iStr=0); 
	///<func_info>

	///<func_info>
	//描述：显示图像
	//参数：
	//const cv::Mat& src	源图像数据
	//int iStr				图像数字标号
	//返回值：空
	static void Img_Show_Image_OpenCV(const cv::Mat& src,int iStr=0); 
	///<func_info>

	///<func_info>
	//描述：Otsu分割图像
	//参数：
	//const cv::Mat& src	源图像数据
	//int iStr				图像数字标号
	//返回值：空
	static void Img_Otsu(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>
	 
	///<func_info>
	//描述：Ssr变换图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//int scale				均值在方差的两边的倍值
	//返回值：空
	static void Img_SSR(const cv::Mat& src,cv::Mat& dst,int scale=1); 
	///<func_info>

	///<func_info>
	//描述：Sobel图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//int scale				图像倍数
	//int delta				偏差
	//int ddepth			深度
	//返回值：空
	static void Img_Sobel(const cv::Mat& src,cv::Mat& dstx,cv::Mat& dsty,cv::Mat& dstxy,int scale=1,int delta=0,int ddepth=CV_16S); 
	///<func_info>

	///<func_info>
	//描述：LBP图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_LBP(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：浮雕图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Relief(const cv::Mat& src,cv::Mat& dst1,cv::Mat& dst2); 
	///<func_info>

	///<func_info>
	//描述：直方图图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Hist(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：局部放大缩小图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Expansion_And_Extrusion(const cv::Mat& src,cv::Mat& dst1,cv::Mat& dst2); 
	///<func_info>

	///<func_info>
	//描述：仿射变换图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Transform(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>
	
	///<func_info>
	//描述：小波分解图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_DWT(const cv::Mat& src,cv::Mat& dst,int nLayer); 
	///<func_info>

	///<func_info>
	//描述：小波合成图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_IDWT(const cv::Mat& src,cv::Mat& dst,int nLayer); 
	///<func_info>

	///<func_info>
	//描述：统计图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_GrayStatistics(const cv::Mat& src,cv::Mat& dst_all,cv::Mat& dst_row,cv::Mat& dst_col); 
	///<func_info>

	///<func_info>
	//描述：羽化图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Feather(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：高亮图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_BrightLight(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：高对比度图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_HighContrast(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：扩散图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Spread(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：素描图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Sketch(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：风特效图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Wind(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：卡通特效图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Cartoons(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：亮划伤图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_BrightStretching(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：Surf图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Surf(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：Meanshift图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Meanshift(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：Img_Clustering图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Clustering(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：Fold图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_Fold(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：Gabor图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_GaborFilter(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//描述：去雾图像
	//参数：
	//const cv::Mat& src	源图像数据
	//cv::Mat& dst			目标图像数据			
	//返回值：空
	static void Img_QuWu(const cv::Mat& src,cv::Mat& dst,int block,double w=8); 
	///<func_info>
};
