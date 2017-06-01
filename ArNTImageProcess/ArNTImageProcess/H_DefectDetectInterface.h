#pragma once
#include "H_Common.h"
#include "H_DataTypeDef.h"
//各现场名称定义
typedef enum 
{
	//通用
	TY_Common_0X=000,
	//板坯线
	BP_ChongGang_1X=101,
	BP_ChongGang_2X=102,
	BP_ChongGang_3X=103,
	BP_BeiHaiChengDe_1X=104,
	BP_BeiHaiChengDe_2X=105,
	BP_LiuGang_1X=106,
	//宽厚板线
	KHB_NanGang_5000X=201,
	KHB_XiangTan_3800X=202,
	KHB_XiangTan_5000X=203,
	KHB_ChongGang_4100X=204,
	KHB_XingChengTeGang_1X=205,
	KHB_SanMingGangTie_1X=206,
	//热轧线
	RZ_ChongGang_1780X=301,
	RZ_DingXin_1780X=302,
	RZ_DingXin_850X=303,
	RZ_LiuGang_1450X=304,
	//固溶线
	GR_BeiHaiChengDe_1X=401,
	GR_BeiHaiChengDe_2X=402,
	GR_BeiHaiChengDe_3X=403,
	//冷轧线
	LZ_TianChengBuXiu_1X=501,
	//铝箔线
	LB_XiNanLv_1X=601,
	LB_XinJiangZhongHe_1X=602,
	//棒材线
	BC_HuaiGangBangCai_1X=701,
	//终止
	TY_Common_999X=999
}ProductLine;





///<func_info>
//描述：
//表面检测系统图像处理接口
//参数：
//int iIDMark 指定现场
//unsigned char* pImgData 图像数据指针
//int iImgWidth 图像宽度
//int iImgHeight 图像高度
//PH_DefectSet pH_DefectSet 缺陷数据集
//bool IsDebug=false 是否是调试模式
//返回值：int 缺陷数量
extern "C"    int  __stdcall  H_DefectDetectInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug=false);
///</func_info>

///<func_info>
//描述：
//获取图像特征
//参数：
//const cv::Mat& src 源图像
//vector<float>& feat 特征向量
//char*strFeat 特征名
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_GetFeats(const cv::Mat& src,vector<float>& feat,int FeatNameNum,char*strFeat,...);
///</func_info>

///<func_info>
//描述：
//获取图像特征
//参数：
//const cv::Mat& src 源图像
//vector<float>& feat 特征向量
//char*strFeat 特征名
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_GetFeat(const cv::Mat& src,vector<float>& feat,char*strFeat);
///</func_info>

///<func_info>
//描述：
//分类器训练接口
//参数：
//char* strClassifierName 分类器名称
//const cv::Mat& trainData 训练数据
//const cv::Mat& trainLabels 训练标签
//int iClassNum 类别数量
//ClassifierParam tClassifierParam 分类器参数
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_ClassifierTrain(char* strClassifierName,const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);
///</func_info>


///<func_info>
//描述：
//分类器测试接口
//参数：
//char* strClassifierName 分类器名称
//const cv::Mat& testData 测试数据
//const cv::Mat& testLabels 测试标签
//cv::Mat& preLabels 预测标签
//int iClassNum 类别数量
//ClassifierParam tClassifierParam 分类器参数
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_ClassifierPredict(char* strClassifierName,const cv::Mat& testData,const cv::Mat& testLabels,cv::Mat& preLabels,int iClassNum,ClassifierParam tClassifierParam);
///</func_info>


///<func_info>
//描述：
//分类器预测类别外部加载初始化
//参数：
//char* strClassifierName 分类器名称
//char* cstrClassifierFileName 分类器文件名
//ClassifierObject& tClassifierObject 分类器对象结构体
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_ClassifierPredictFromExternal_Init(char* cstrClassifierName,char* cstrClassifierFileName,ClassifierObject& tClassifierObject);
///</func_info>

///<func_info>
//描述：
//分类器预测类别外部加载实例
//参数：
//char* strClassifierName 分类器名称
//const cv::Mat& testData 测试数据
//const cv::Mat& testLabels 测试标签
//cv::Mat& preLabels 预测标签
//int iClassNum 类别数量
//ClassifierObject& tClassifierObject 分类器对象结构体
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_ClassifierPredictFromExternal(char* strClassifierName,const cv::Mat& testData,const cv::Mat& testLabels,cv::Mat& preLabels,int iClassNum,const ClassifierObject& tClassifierObject);
///</func_info>


///<func_info>
//描述：
//分类器外部加载关闭
//参数：
//char* cstrClassifierName 分类器名称
//ClassifierObject& tClassifierObject 分类器对象结构体
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_ClassifierPredictFromExternal_Close(char* cstrClassifierName,ClassifierObject& tClassifierObject);
///</func_info>

///<func_info>
//描述：
//测试图像处理函数
//参数：
//cv::Mat src 源图像
//char* cstrFuncName 函数名称
//返回值：
//int 未定义
extern "C"    int  __stdcall  H_TestImageProcessFunc(cv::Mat src,char* cstrFuncName);
///</func_info>