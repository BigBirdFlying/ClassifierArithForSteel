#pragma once
#include "h_baseimageprocess.h"

class H_ClassifierRelation
{
public:
	H_ClassifierRelation(void);
	~H_ClassifierRelation(void);

	static void H_ClassifierRelation::CalcCLCM(cv::Mat imgSrc,int angleDirection, vector<float>& feat);

	static void GetSobelFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetLBPFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetHOGFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetHaarFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetMoment1Feat(cv::Mat srcimg,vector<float>& feat);
	static void GetMoment2Feat(cv::Mat srcimg,vector<float>& feat);
	static void GetMoment3Feat(cv::Mat srcimg,vector<float>& feat);
	static void GetOriginalFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetHistogramFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetCLCMFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetSuftFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetSiftFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetTamuraFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetSpectrumFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetShapeFeat(cv::Mat srcimg,vector<float>& feat);

	static void Classifier_BP(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_SVM(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_RTree(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_Boosting(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);

	static void Classifier_BP_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_SVM_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_RTree_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_Boosting_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam);
};

