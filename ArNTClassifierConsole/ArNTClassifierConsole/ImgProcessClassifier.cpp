#include "StdAfx.h"
#include "ImgProcessClassifier.h"

ImgProcessClassifier::ImgProcessClassifier(void)
{
}

ImgProcessClassifier::~ImgProcessClassifier(void)
{
}

void ImgProcessClassifier::Classifier_BP(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	cv::Mat testLabels_BP = cv::Mat::zeros(trainData.rows, iClassNum, CV_32FC1); 
	for(int i=0;i<testLabels_BP.rows;i++)
	{
		for(int j=0;j<testLabels_BP.cols;j++)
		{
			if(j==(int)trainLabels.at<float>(i,0))
			{
				testLabels_BP.at<float>(i,j)=1;
			}
		}
	}
	int iFeatDims=trainData.cols;
	int iLabelNum=testLabels_BP.cols;
	cv::Mat layerSizes=(cv::Mat_<int>(1,3)<<iFeatDims,tClassifierParam.tClassifierParam_BP.iHideNode,iLabelNum);
	CvANN_MLP mlp;
	mlp.create(layerSizes);
	CvANN_MLP_TrainParams params;
	params.term_crit=cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,tClassifierParam.tClassifierParam_BP.iMaxIter,tClassifierParam.tClassifierParam_BP.fEpsilon);
	mlp.train(trainData, testLabels_BP, cv::Mat(),cv::Mat(), params);  
	mlp.save(tClassifierParam.tClassifierParam_BP.fileName);
}

void ImgProcessClassifier::Classifier_BP_Predict(const cv::Mat& testData,const cv::Mat& testLabels,int iClassNum,ClassifierParam tClassifierParam,cv::Mat& perLabels)
{
	int iDataNumCorrect=0;
	int iDataNum=testData.rows;
	int iLabelNum=iClassNum;
	cv::Mat nearest(iDataNum, iLabelNum, CV_32FC1, cv::Scalar(0));

	CvANN_MLP neuralNetwork;
	neuralNetwork.load(tClassifierParam.tClassifierParam_BP.fileName);
	neuralNetwork.predict(testData, nearest);
	for(int i=0;i<nearest.rows;i++)
	{
		float fConfMax=0;
		int ipre=0;
		for(int j=0;j<nearest.cols;j++)
		{
			if(nearest.at<float>(i,j)>fConfMax)
			{
				fConfMax=nearest.at<float>(i,j);
				ipre=j;
			}			
		}
		perLabels.at<float>(i,0)=ipre;
	}
}

void ImgProcessClassifier::Classifier_SVM(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	CvSVMParams Params;
    Params.svm_type    = cv::SVM::C_SVC;
    Params.C           = 1;//0.1
    Params.kernel_type = cv::SVM::LINEAR;
	Params.term_crit   = cv::TermCriteria(CV_TERMCRIT_ITER, tClassifierParam.tClassifierParam_SVM.iMaxIter, tClassifierParam.tClassifierParam_SVM.fEpsilon);

    CvSVM svm;
    svm.train(trainData, trainLabels, cv::Mat(), cv::Mat(), Params);
	svm.save(tClassifierParam.tClassifierParam_SVM.fileName);	
}

void ImgProcessClassifier::Classifier_SVM_Predict(const cv::Mat& testData,const cv::Mat& testLabels,int iClassNum,ClassifierParam tClassifierParam,cv::Mat& perLabels)
{
	int iDataNumCorrect=0;
	int iDataNum=testData.rows;
	CvSVM svm;
	svm.load(tClassifierParam.tClassifierParam_SVM.fileName);
	for(int i=0;i<testData.rows;i++)
	{
		float ipre = svm.predict(testData.row(i));
		perLabels.at<float>(i,0)=ipre;
	}
}