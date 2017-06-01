#pragma once
#include "H_Common.h"
#include "H_DataTypeDef.h"
//���ֳ����ƶ���
typedef enum 
{
	//ͨ��
	TY_Common_0X=000,
	//������
	BP_ChongGang_1X=101,
	BP_ChongGang_2X=102,
	BP_ChongGang_3X=103,
	BP_BeiHaiChengDe_1X=104,
	BP_BeiHaiChengDe_2X=105,
	BP_LiuGang_1X=106,
	//������
	KHB_NanGang_5000X=201,
	KHB_XiangTan_3800X=202,
	KHB_XiangTan_5000X=203,
	KHB_ChongGang_4100X=204,
	KHB_XingChengTeGang_1X=205,
	KHB_SanMingGangTie_1X=206,
	//������
	RZ_ChongGang_1780X=301,
	RZ_DingXin_1780X=302,
	RZ_DingXin_850X=303,
	RZ_LiuGang_1450X=304,
	//������
	GR_BeiHaiChengDe_1X=401,
	GR_BeiHaiChengDe_2X=402,
	GR_BeiHaiChengDe_3X=403,
	//������
	LZ_TianChengBuXiu_1X=501,
	//������
	LB_XiNanLv_1X=601,
	LB_XinJiangZhongHe_1X=602,
	//������
	BC_HuaiGangBangCai_1X=701,
	//��ֹ
	TY_Common_999X=999
}ProductLine;





///<func_info>
//������
//������ϵͳͼ����ӿ�
//������
//int iIDMark ָ���ֳ�
//unsigned char* pImgData ͼ������ָ��
//int iImgWidth ͼ����
//int iImgHeight ͼ��߶�
//PH_DefectSet pH_DefectSet ȱ�����ݼ�
//bool IsDebug=false �Ƿ��ǵ���ģʽ
//����ֵ��int ȱ������
extern "C"    int  __stdcall  H_DefectDetectInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug=false);
///</func_info>

///<func_info>
//������
//��ȡͼ������
//������
//const cv::Mat& src Դͼ��
//vector<float>& feat ��������
//char*strFeat ������
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_GetFeats(const cv::Mat& src,vector<float>& feat,int FeatNameNum,char*strFeat,...);
///</func_info>

///<func_info>
//������
//��ȡͼ������
//������
//const cv::Mat& src Դͼ��
//vector<float>& feat ��������
//char*strFeat ������
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_GetFeat(const cv::Mat& src,vector<float>& feat,char*strFeat);
///</func_info>

///<func_info>
//������
//������ѵ���ӿ�
//������
//char* strClassifierName ����������
//const cv::Mat& trainData ѵ������
//const cv::Mat& trainLabels ѵ����ǩ
//int iClassNum �������
//ClassifierParam tClassifierParam ����������
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_ClassifierTrain(char* strClassifierName,const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);
///</func_info>


///<func_info>
//������
//���������Խӿ�
//������
//char* strClassifierName ����������
//const cv::Mat& testData ��������
//const cv::Mat& testLabels ���Ա�ǩ
//cv::Mat& preLabels Ԥ���ǩ
//int iClassNum �������
//ClassifierParam tClassifierParam ����������
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_ClassifierPredict(char* strClassifierName,const cv::Mat& testData,const cv::Mat& testLabels,cv::Mat& preLabels,int iClassNum,ClassifierParam tClassifierParam);
///</func_info>


///<func_info>
//������
//������Ԥ������ⲿ���س�ʼ��
//������
//char* strClassifierName ����������
//char* cstrClassifierFileName �������ļ���
//ClassifierObject& tClassifierObject ����������ṹ��
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_ClassifierPredictFromExternal_Init(char* cstrClassifierName,char* cstrClassifierFileName,ClassifierObject& tClassifierObject);
///</func_info>

///<func_info>
//������
//������Ԥ������ⲿ����ʵ��
//������
//char* strClassifierName ����������
//const cv::Mat& testData ��������
//const cv::Mat& testLabels ���Ա�ǩ
//cv::Mat& preLabels Ԥ���ǩ
//int iClassNum �������
//ClassifierObject& tClassifierObject ����������ṹ��
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_ClassifierPredictFromExternal(char* strClassifierName,const cv::Mat& testData,const cv::Mat& testLabels,cv::Mat& preLabels,int iClassNum,const ClassifierObject& tClassifierObject);
///</func_info>


///<func_info>
//������
//�������ⲿ���عر�
//������
//char* cstrClassifierName ����������
//ClassifierObject& tClassifierObject ����������ṹ��
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_ClassifierPredictFromExternal_Close(char* cstrClassifierName,ClassifierObject& tClassifierObject);
///</func_info>

///<func_info>
//������
//����ͼ������
//������
//cv::Mat src Դͼ��
//char* cstrFuncName ��������
//����ֵ��
//int δ����
extern "C"    int  __stdcall  H_TestImageProcessFunc(cv::Mat src,char* cstrFuncName);
///</func_info>