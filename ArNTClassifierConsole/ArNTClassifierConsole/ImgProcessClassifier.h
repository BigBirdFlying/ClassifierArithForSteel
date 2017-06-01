#pragma once
//#include "H_Common.h"
//#include "H_DataTypeDef.h"
//typedef struct tagClassifierParam
//{
//	int iMaxIter;
//	float fEpsilon;
//	int iHideNode;
//	char fileName[128];
//}ClassifierParam,PClassifierParam;

class ImgProcessClassifier
{
public:
	ImgProcessClassifier(void);
	~ImgProcessClassifier(void);

	static void Classifier_BP(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);
	static void Classifier_SVM(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam);

	static void Classifier_SVM_Predict(const cv::Mat& testData,const cv::Mat& testLabels,int iClassNum,ClassifierParam tClassifierParam,cv::Mat& perLabels);
	static void Classifier_BP_Predict(const cv::Mat& testData,const cv::Mat& testLabels,int iClassNum,ClassifierParam tClassifierParam,cv::Mat& perLabels);
};
