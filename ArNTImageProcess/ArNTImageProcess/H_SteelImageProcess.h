#pragma once
#include "h_baseimageprocess.h"

class H_SteelImageProcess :
	public H_BaseImageProcess
{
public:
	H_SteelImageProcess(void);
	~H_SteelImageProcess(void);

	public:
	///<func_info>
	//描述：得到区域的位置和大小保存到缺陷集
	//参数：
	//const cv::Mat& src	源图像数据
	//PH_DefectSet pH_DefectSet		缺陷集
	//int dilation_size				膨胀的尺寸 默认为5
	//int iScale_x					区域扩充 默认为5
	//int iScale_y					区域扩充 默认为10
	//int minAreaLimit				最小区域面积限制
	//返回值：空
	static void Steel_RegionToDefectSet(const cv::Mat& src_region,PH_DefectSet pH_DefectSet,int dilation_size=5,int iScale_x=5,int iScale_y=10,int minAreaLimit=500);
	///</func_info>

	public:
	///<func_info>
	//描述：得到钢板图像边界
	//参数：
	//const cv::Mat& src	源图像数据
	//int& iLeft			左边界
	//int& iRight			右边界
	//int offSet			边界缩进
	//返回值：空
	static void Steel_FindEdge(const cv::Mat& src,int& iLeft,int& iRight,int offset=0);
	///</func_info>

	public:
	///<func_info>
	//描述：切除钢板图像边界
	//参数：
	//const cv::Mat& src	源图像数据
	//const int& iLeft			左边界
	//const int& iRight			右边界
	//cv::Mat& dst			切除边界后的图像
	//返回值：空
	static void Steel_CutEdge(const cv::Mat& src,const int& iLeft,const int& iRight,cv::Mat& dst);
	///</func_info>
};
