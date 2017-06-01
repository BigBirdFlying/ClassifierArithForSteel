#include "H_DefectDetectInterface.h"
#include "H_CommonImageProcess.h"
#include "H_BaseImageProcess.h"
#include "H_ClassifierRelation.h"
#include "H_RZ_DingXin_1780X_ImageProcess.h"
#include "H_LB_XinJiangZhongHe_1X_ImageProcess.h"
#include "H_LB_XiNanLv_1X_ImageProcess.h"
#include "H_KHB_SanMingGangTie_1X_ImageProcess.h"

//定义对象
H_CommonImageProcess							m_CommonImageProcess;
H_RZ_DingXin_1780X_ImageProcess					m_H_RZ_DingXin_1780X_ImageProcess;
H_LB_XinJiangZhongHe_1X_ImageProcess			m_H_LB_XinJiangZhongHe_1X_ImageProcess;
H_LB_XiNanLv_1X_ImageProcess					m_H_LB_XiNanLv_1X_ImageProcess;
H_KHB_SanMingGangTie_1X_ImageProcess			m_H_KHB_SanMingGangTie_1X_ImageProcess;

int  __stdcall  H_DefectDetectInterfaceFunc(int iIDMark,unsigned char* pImgData,int iImgWidth,int iImgHeight,ConfigInfo_Segmentation& tConfigInfo_Segmentation,PH_DefectSet pH_DefectSet,bool IsDebug)
{
	switch(iIDMark)
	{
		case TY_Common_0X:
			m_CommonImageProcess.K_ImageProcessInterfaceFunc(iIDMark,pImgData,iImgWidth,iImgHeight,tConfigInfo_Segmentation,pH_DefectSet,IsDebug);
			break;
		case BP_ChongGang_1X:
			break;
		case BP_ChongGang_2X:
			break;
		case BP_ChongGang_3X:
			break;
		case BP_BeiHaiChengDe_1X:
			break;
		case BP_BeiHaiChengDe_2X:
			break;
		case BP_LiuGang_1X:
			break;
		case KHB_NanGang_5000X:
			break;
		case KHB_XiangTan_3800X:
			break;
		case KHB_XiangTan_5000X:
			break;
		case KHB_ChongGang_4100X:
			break;
		case KHB_XingChengTeGang_1X:
			break;
		case KHB_SanMingGangTie_1X:
			m_H_KHB_SanMingGangTie_1X_ImageProcess.K_ImageProcessInterfaceFunc(iIDMark,pImgData,iImgWidth,iImgHeight,tConfigInfo_Segmentation,pH_DefectSet,IsDebug);
			break;
		case RZ_ChongGang_1780X:
			break;
		case RZ_DingXin_1780X:
			break;
		case RZ_DingXin_850X:
			m_H_RZ_DingXin_1780X_ImageProcess.K_ImageProcessInterfaceFunc(iIDMark,pImgData,iImgWidth,iImgHeight,tConfigInfo_Segmentation,pH_DefectSet);
			break;
		case RZ_LiuGang_1450X:
			break;
		case GR_BeiHaiChengDe_1X:
			break;
		case GR_BeiHaiChengDe_2X:
			break;
		case GR_BeiHaiChengDe_3X:
			break;
		case LZ_TianChengBuXiu_1X:
			break;
		case LB_XiNanLv_1X:
			m_H_LB_XiNanLv_1X_ImageProcess.K_ImageProcessInterfaceFunc(iIDMark,pImgData,iImgWidth,iImgHeight,tConfigInfo_Segmentation,pH_DefectSet,IsDebug);
			break;
		case LB_XinJiangZhongHe_1X:
			m_H_LB_XinJiangZhongHe_1X_ImageProcess.K_ImageProcessInterfaceFunc(iIDMark,pImgData,iImgWidth,iImgHeight,tConfigInfo_Segmentation,pH_DefectSet,IsDebug);
			break;
		case BC_HuaiGangBangCai_1X:
			break;
		default:
			break;
	}
	return 0;
}

int  __stdcall  H_GetFeats(const cv::Mat& src,vector<float>& feat,int FeatNameNum,char*strFeat,...)
{
	va_list argp;													/* 定义保存函数参数的结构 */    												/* 纪录参数个数 */    
    char *para;														/* 存放取出的字符串参数 */                                            
    va_start( argp, FeatNameNum);									/* argp指向传入的第一个可选参数,msg是最后一个确定的参数 */          
    while (1)   
    {    
        para = va_arg( argp, char *);								/* 取出当前的参数，类型为char *. */    
		if(para != NULL)
		{															/* 采用空串指示参数输入结束 */    
            break; 
		}
        if (strcmp( para, "LBP") == 1)  
		{
			H_ClassifierRelation::GetLBPFeat(src,feat);
		}
		else if(strcmp( para, "Sobel") == 1) 
		{
			H_ClassifierRelation::GetSobelFeat(src,feat);
		}
		else
		{
			break;
		}
    }    
    va_end( argp );   
	return 0;
}

int  __stdcall  H_GetFeat(const cv::Mat& src,vector<float>& feat,char*strFeat)
{	
    if (strcmp( strFeat, "Feat_LBP") == 0)  
	{
		H_ClassifierRelation::GetLBPFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Sobel") == 0) 
	{
		H_ClassifierRelation::GetSobelFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_HOG") == 0) 
	{
		H_ClassifierRelation::GetHOGFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Haar") == 0) 
	{
		H_ClassifierRelation::GetHaarFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Moment1") == 0) 
	{
		H_ClassifierRelation::GetMoment1Feat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Moment2") == 0) 
	{
		H_ClassifierRelation::GetMoment2Feat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Moment3") == 0) 
	{
		H_ClassifierRelation::GetMoment3Feat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Original") == 0) 
	{
		H_ClassifierRelation::GetOriginalFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Histogram") == 0) 
	{
		H_ClassifierRelation::GetHistogramFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_CLCM") == 0) 
	{
		H_ClassifierRelation::GetCLCMFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Suft") == 0) 
	{
		H_ClassifierRelation::GetSuftFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Sift") == 0) 
	{
		H_ClassifierRelation::GetSiftFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Tamura") == 0) 
	{
		H_ClassifierRelation::GetTamuraFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Spectrum") == 0) 
	{
		H_ClassifierRelation::GetSpectrumFeat(src,feat);
	}
	else if(strcmp( strFeat, "Feat_Shape") == 0) 
	{
		H_ClassifierRelation::GetShapeFeat(src,feat);
	}
  
	return 0;
}

int  __stdcall  H_ClassifierTrain(char* strClassifierName,const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	if (strcmp( strClassifierName, "BP_Classifier") == 0)  
	{
		H_ClassifierRelation::Classifier_BP(trainData,trainLabels,iClassNum,tClassifierParam);
	}
	else if(strcmp( strClassifierName, "SVM_Classifier") == 0)
	{
		H_ClassifierRelation::Classifier_SVM(trainData,trainLabels,iClassNum,tClassifierParam);
	}
	else if(strcmp( strClassifierName, "RTree_Classifier") == 0)
	{
		H_ClassifierRelation::Classifier_RTree(trainData,trainLabels,iClassNum,tClassifierParam);
	}
	else if(strcmp( strClassifierName, "Boosting_Classifier") == 0)
	{
		H_ClassifierRelation::Classifier_Boosting(trainData,trainLabels,iClassNum,tClassifierParam);
	}
	return 0;
}

int  __stdcall  H_ClassifierPredict(char* strClassifierName,const cv::Mat& testData,const cv::Mat& testLabels,cv::Mat& preLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	if (strcmp( strClassifierName, "BP_Classifier") == 0)  
	{
		H_ClassifierRelation::Classifier_BP_Predict(testData,preLabels,iClassNum,tClassifierParam);
	}
	else if(strcmp( strClassifierName, "SVM_Classifier") == 0)
	{
		H_ClassifierRelation::Classifier_SVM_Predict(testData,preLabels,iClassNum,tClassifierParam);
	}
	else if(strcmp( strClassifierName, "RTree_Classifier") == 0)
	{
		H_ClassifierRelation::Classifier_RTree_Predict(testData,preLabels,iClassNum,tClassifierParam);
	}
	else if(strcmp( strClassifierName, "Boosting_Classifier") == 0)
	{
		H_ClassifierRelation::Classifier_Boosting_Predict(testData,preLabels,iClassNum,tClassifierParam);
	}
	return 0;
}

int  __stdcall  H_ClassifierPredictFromExternal_Init(char* cstrClassifierName,char* cstrClassifierFileName,ClassifierObject& tClassifierObject)
{	
	if(strcmp( cstrClassifierName, "BP_Classifier") == 0)
	{
		tClassifierObject.mlp.load(cstrClassifierFileName);
	}
	else if(strcmp( cstrClassifierName, "SVM_Classifier") == 0)
	{
		tClassifierObject.svm.load(cstrClassifierFileName);
	}
	else if(strcmp( cstrClassifierName, "RTree_Classifier") == 0)
	{
		tClassifierObject.rtree.load(cstrClassifierFileName);
	}
	else if(strcmp( cstrClassifierName, "Boosting_Classifier") == 0)
	{
		tClassifierObject.boost.load(cstrClassifierFileName);
	}
	return 1;
}

int  __stdcall  H_ClassifierPredictFromExternal(char* strClassifierName,const cv::Mat& testData,const cv::Mat& testLabels,cv::Mat& preLabels,int iClassNum,const ClassifierObject& tClassifierObject)
{
	int iDataNumCorrect=0;
	int iDataNum=testData.rows;
	if (strcmp( strClassifierName, "BP_Classifier") == 0)  
	{
		int iLabelNum=iClassNum;
		cv::Mat nearest(iDataNum, iLabelNum, CV_32FC1, cv::Scalar(0));
		tClassifierObject.mlp.predict(testData, nearest);
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
			preLabels.at<float>(i,0)=ipre;
		}
	}
	else if(strcmp( strClassifierName, "SVM_Classifier") == 0)
	{
		for(int i=0;i<testData.rows;i++)
		{
			float ipre = tClassifierObject.svm.predict(testData.row(i));
			preLabels.at<float>(i,0)=ipre;
		}
	}
	else if(strcmp( strClassifierName, "RTree_Classifier") == 0)
	{
		for(int i=0;i<testData.rows;i++)
		{
			float ipre = tClassifierObject.rtree.predict(testData.row(i));
			preLabels.at<float>(i,0)=ipre;
		}
	}
	else if(strcmp( strClassifierName, "Boosting_Classifier") == 0)
	{
		for(int i=0;i<testData.rows;i++)
		{
			float ipre = tClassifierObject.boost.predict(testData.row(i));
			preLabels.at<float>(i,0)=ipre;
		}
	}
	return 1;
}

int  __stdcall  H_ClassifierPredictFromExternal_Close(char* cstrClassifierName,ClassifierObject& tClassifierObject)
{	
	if(strcmp( cstrClassifierName, "BP_Classifier") == 0)
	{
		tClassifierObject.mlp.clear();
	}
	else if(strcmp( cstrClassifierName, "SVM_Classifier") == 0)
	{
		tClassifierObject.svm.clear();
	}
	else if(strcmp( cstrClassifierName, "RTree_Classifier") == 0)
	{
		tClassifierObject.rtree.clear();
	}
	else if(strcmp( cstrClassifierName, "Boosting_Classifier") == 0)
	{
		tClassifierObject.boost.clear();
	}
	return 1;
}

int  __stdcall  H_TestImageProcessFunc(cv::Mat src,char* cstrFuncName)
{	
	cv::Mat dst;
	if(strcmp( cstrFuncName, "Img_Gray_Change_Y") == 0)
	{
		H_BaseImageProcess::Img_Gray_Change_Y(src,dst,100,5);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Gray_Change_X") == 0)
	{
		H_BaseImageProcess::Img_Gray_Change_X(src,dst,100,5,1);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Gray_Change_Integral") == 0)
	{
		H_BaseImageProcess::Img_Gray_Change_Integral(src,dst,100,8,6);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Otsu") == 0)
	{
		H_BaseImageProcess::Img_Otsu(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_SSR") == 0)
	{
		H_BaseImageProcess::Img_SSR(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Sobel") == 0)
	{
		cv::Mat dst_xx,dst_yy,dst_xy;
		H_BaseImageProcess::Img_Sobel(src,dst_xx,dst_yy,dst_xy);
		cv::namedWindow("dst_xx",0);
		cv::imshow("dst_xx",dst_xx);
		cv::namedWindow("dst_yy",0);
		cv::imshow("dst_yy",dst_yy);
		cv::namedWindow("dst_xy",0);
		cv::imshow("dst_xy",dst_xy);
	}
	else if(strcmp( cstrFuncName, "Img_LBP") == 0)
	{
		H_BaseImageProcess::Img_LBP(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Relief") == 0)
	{
		cv::Mat dst1,dst2;
		H_BaseImageProcess::Img_Relief(src,dst1,dst2);
		cv::namedWindow("dst1",0);
		cv::imshow("dst1",dst1);
		cv::namedWindow("dst2",0);
		cv::imshow("dst2",dst2);
	}
	else if(strcmp( cstrFuncName, "Img_Hist") == 0)
	{
		H_BaseImageProcess::Img_Hist(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Expansion_And_Extrusion") == 0)
	{
		cv::Mat dst1,dst2;
		H_BaseImageProcess::Img_Expansion_And_Extrusion(src,dst1,dst2);
		cv::namedWindow("dst1",0);
		cv::imshow("dst1",dst);
		cv::namedWindow("dst2",0);
		cv::imshow("dst2",dst2);
	}
	else if(strcmp( cstrFuncName, "Img_Transform") == 0)
	{
		H_BaseImageProcess::Img_Transform(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_DWT") == 0)
	{
		H_BaseImageProcess::Img_DWT(src,dst,3);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_GrayStatistics") == 0)
	{
		cv::Mat dst_all,dst_row,dst_col;
		H_BaseImageProcess::Img_GrayStatistics(src,dst_all,dst_row,dst_col);
		cv::namedWindow("dst_all",0);
		cv::imshow("dst_all",dst_all);
		cv::namedWindow("dst_row",0);
		cv::imshow("dst_row",dst_row);
		cv::namedWindow("dst_col",0);
		cv::imshow("dst_col",dst_col);
	}
	else if(strcmp( cstrFuncName, "Img_Feather") == 0)
	{
		H_BaseImageProcess::Img_Feather(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_BrightLight") == 0)
	{
		H_BaseImageProcess::Img_BrightLight(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_HighContrast") == 0)
	{
		H_BaseImageProcess::Img_HighContrast(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Spread") == 0)
	{
		H_BaseImageProcess::Img_Spread(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Sketch") == 0)
	{
		H_BaseImageProcess::Img_Sketch(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Wind") == 0)
	{
		H_BaseImageProcess::Img_Wind(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Cartoons") == 0)
	{
		H_BaseImageProcess::Img_Cartoons(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_BrightStretching") == 0)
	{
		H_BaseImageProcess::Img_BrightStretching(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Surf") == 0)
	{
		H_BaseImageProcess::Img_Surf(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Meanshift") == 0)
	{
		H_BaseImageProcess::Img_Meanshift(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Clustering") == 0)
	{
		H_BaseImageProcess::Img_Clustering(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_Fold") == 0)
	{
		H_BaseImageProcess::Img_Fold(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_GaborFilter") == 0)
	{
		H_BaseImageProcess::Img_GaborFilter(src,dst);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	else if(strcmp( cstrFuncName, "Img_QuWu") == 0)
	{
		H_BaseImageProcess::Img_QuWu(src,dst,16);
		cv::namedWindow("dst",0);
		cv::imshow("dst",dst);
	}
	return 1;
}