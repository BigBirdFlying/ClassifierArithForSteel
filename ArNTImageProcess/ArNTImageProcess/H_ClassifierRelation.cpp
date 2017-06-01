#include "H_ClassifierRelation.h"

H_ClassifierRelation::H_ClassifierRelation(void)
{
}

H_ClassifierRelation::~H_ClassifierRelation(void)
{
}

void H_ClassifierRelation::CalcCLCM(cv::Mat imgSrc,int angleDirection, vector<float>& feat) 
{
	int glcm_class=16;//计算灰度共生矩阵的图像灰度值等级化  
	int glcm_dis=3;//灰度共生矩阵的统计距离  
    int width=imgSrc.cols;
	int height= imgSrc.rows;  
  
    int * glcm = new int[glcm_class * glcm_class];  
    int * histImage = new int[width * height];  
  
    if(NULL == glcm || NULL == histImage) 
	{
        return ;  
	}
    //灰度等级化---分GLCM_CLASS个等级   
    for(int i = 0;i < height;i++)
	{  
        for(int j = 0;j < width;j++)
		{  
			histImage[i * width + j] = (int)(imgSrc.at<uchar>(i,j)* glcm_class / 256);  //(int)(data[bWavelet->widthStep * i + j] * GLCM_CLASS / 256);  
        }  
    }  
    //初始化共生矩阵  
    for (int i = 0;i < glcm_class;i++) 
	{
        for (int j = 0;j < glcm_class;j++) 
		{
            glcm[i * glcm_class + j] = 0; 
		}
	}
  
    //计算灰度共生矩阵  
    int w,k,l;      
    if(angleDirection == 0) //水平方向  
    {  
        for (int i = 0;i < height;i++)  
        {  
            for (int j = 0;j < width;j++)  
            {  
                l = histImage[i * width + j];  
                if(j + glcm_dis >= 0 && j + glcm_dis < width)  
                {  
                    k = histImage[i * width + j + glcm_dis];  
                    glcm[l * glcm_class + k]++;  
                }  
                if(j - glcm_dis >= 0 && j - glcm_dis < width)  
                {  
                    k = histImage[i * width + j - glcm_dis];  
                    glcm[l * glcm_class + k]++;  
                }  
            }  
        }  
    }     
    else if(angleDirection == 1) //垂直方向   
    {  
        for (int i = 0;i < height;i++)  
        {  
            for (int j = 0;j < width;j++)  
            {  
                l = histImage[i * width + j];  
                if(i + glcm_dis >= 0 && i + glcm_dis < height)   
                {  
                    k = histImage[(i + glcm_dis) * width + j];  
                    glcm[l * glcm_class + k]++;  
                }  
                if(i - glcm_dis >= 0 && i - glcm_dis < height)   
                {  
                    k = histImage[(i - glcm_dis) * width + j];  
                    glcm[l * glcm_class + k]++;  
                }  
            }  
        }  
    }      
    else if(angleDirection == 2) //对角方向   
    {  
        for (int i = 0;i < height;i++)  
        {  
            for (int j = 0;j < width;j++)  
            {  
                l = histImage[i * width + j];  
  
                if(j + glcm_dis >= 0 && j + glcm_dis < width && i + glcm_dis >= 0 && i + glcm_dis < height)  
                {  
                    k = histImage[(i + glcm_dis) * width + j + glcm_dis];  
                    glcm[l * glcm_class + k]++;  
                }  
                if(j - glcm_dis >= 0 && j - glcm_dis < width && i - glcm_dis >= 0 && i - glcm_dis < height)  
                {  
                    k = histImage[(i - glcm_dis) * width + j - glcm_dis];  
                    glcm[l * glcm_class + k]++;  
                }  
            }  
        }  
    }  
  
    //计算特征值  
    double entropy = 0,energy = 0,contrast = 0,homogenity = 0;  
    for (int i = 0;i < glcm_class;i++)  
    {  
        for (int j = 0;j < glcm_class;j++)  
        {            
            if(glcm[i * glcm_class + j] > 0)  
			{
                entropy -= glcm[i * glcm_class + j] * log10(double(glcm[i * glcm_class + j])); //熵 
			}  
            energy += glcm[i * glcm_class + j] * glcm[i * glcm_class + j];   //能量             
            contrast += (i - j) * (i - j) * glcm[i * glcm_class + j];  //对比度          
            homogenity += 1.0 / (1 + (i - j) * (i - j)) * glcm[i * glcm_class + j];    //一致性  
        }  
    }  
    //返回特征值  
	feat.push_back(entropy);
	feat.push_back(energy);
	feat.push_back(contrast);
	feat.push_back(homogenity);
  
    delete[] glcm;  
    delete[] histImage;  
}

void H_ClassifierRelation::GetSobelFeat(cv::Mat srcimg,vector<float>& feat)
{
 	int ddepth=CV_16S;
	int delta=0;
	int scale=1;

	cv::Mat srcimg_down1;
	pyrDown(srcimg,srcimg_down1,cv::Size(srcimg.rows/2,srcimg.cols/2));
	cv::Mat dstimg2_1,dstimg2_2,dstimg2_3;
	cv::Mat grad_x2, grad_y2;
	cv::Mat abs_grad_x2, abs_grad_y2;
	Sobel( srcimg_down1, grad_x2, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_x2, abs_grad_x2 );
	Sobel( srcimg_down1, grad_y2, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_y2, abs_grad_y2 );
	addWeighted( abs_grad_x2, 0.5, abs_grad_y2, 0.5, 0, dstimg2_1 );
	addWeighted( abs_grad_x2, 0.5, abs_grad_x2, 0.5, 0, dstimg2_2 );
	addWeighted( abs_grad_y2, 0.5, abs_grad_y2, 0.5, 0, dstimg2_3 );
	
	double minVal2_1,maxVal2_1;
	cv::minMaxLoc(dstimg2_1,&minVal2_1,&maxVal2_1);
	double minVal2_2,maxVal2_2;
	cv::minMaxLoc(dstimg2_2,&minVal2_2,&maxVal2_2);
	double minVal2_3,maxVal2_3;
	cv::minMaxLoc(dstimg2_3,&minVal2_3,&maxVal2_3);
	for(int i=0;i<dstimg2_1.rows;i++)
	{
		for(int j=0;j<dstimg2_1.cols;j++)
		{
			float temp=0;
			temp=(dstimg2_1.at<uchar>(i,j)-minVal2_1)/(maxVal2_1-minVal2_1);
			feat.push_back(temp);
			temp=(dstimg2_2.at<uchar>(i,j)-minVal2_2)/(maxVal2_2-minVal2_2);
			feat.push_back(temp);
			temp=(dstimg2_3.at<uchar>(i,j)-minVal2_3)/(maxVal2_3-minVal2_3);
			feat.push_back(temp);
		}
	}

	cv::Mat srcimg_down2;
	pyrDown(srcimg_down1,srcimg_down2,cv::Size(srcimg_down1.rows/2,srcimg_down1.cols/2));
	cv::Mat dstimg3_1,dstimg3_2,dstimg3_3;
	cv::Mat grad_x3, grad_y3;
	cv::Mat abs_grad_x3, abs_grad_y3;
	Sobel( srcimg_down2, grad_x3, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_x3, abs_grad_x3 );
	Sobel( srcimg_down2, grad_y3, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_y3, abs_grad_y3 );
	addWeighted( abs_grad_x3, 0.5, abs_grad_y3, 0.5, 0, dstimg3_1 );
	addWeighted( abs_grad_x3, 0.5, abs_grad_x3, 0.5, 0, dstimg3_2 );
	addWeighted( abs_grad_y3, 0.5, abs_grad_y3, 0.5, 0, dstimg3_3 );
	
	double minVal3_1,maxVal3_1;
	cv::minMaxLoc(dstimg3_1,&minVal3_1,&maxVal3_1);
	double minVal3_2,maxVal3_2;
	cv::minMaxLoc(dstimg3_2,&minVal3_2,&maxVal3_2);
	double minVal3_3,maxVal3_3;
	cv::minMaxLoc(dstimg3_3,&minVal3_3,&maxVal3_3);
	
	for(int i=0;i<dstimg3_1.rows;i++)
	{
		for(int j=0;j<dstimg3_1.cols;j++)
		{
			float temp=0;
			temp=(dstimg3_1.at<uchar>(i,j)-minVal3_1)/(maxVal3_1-minVal3_1);
			feat.push_back(temp);
			temp=(dstimg3_2.at<uchar>(i,j)-minVal3_2)/(maxVal3_2-minVal3_2);
			feat.push_back(temp);
			temp=(dstimg3_3.at<uchar>(i,j)-minVal3_3)/(maxVal3_3-minVal3_3);
			feat.push_back(temp);
		}
	}

}

void H_ClassifierRelation::GetLBPFeat(cv::Mat srcimg,vector<float>& feat)
{
	cv::Mat src,dst;
	cv::Mat _dst;
	srcimg.copyTo(src);
    _dst.create(src.rows-2, src.cols-2, CV_8UC1);  
	_dst.copyTo(dst);  
    dst.setTo(0);  
  
    for(int i=1;i<src.rows-1;i++) {  

        for(int j=1;j<src.cols-1;j++) {  
              
            char center = src.at<char>(i,j);  

            unsigned char code = 0;  
            code |= (src.at<char>(i-1,j-1) >= center) << 7;  
            code |= (src.at<char>(i-1,j  ) >= center) << 6;  
            code |= (src.at<char>(i-1,j+1) >= center) << 5;  
            code |= (src.at<char>(i  ,j+1) >= center) << 4;  
            code |= (src.at<char>(i+1,j+1) >= center) << 3;  
            code |= (src.at<char>(i+1,j  ) >= center) << 2;  
            code |= (src.at<char>(i+1,j-1) >= center) << 1;  
            code |= (src.at<char>(i  ,j-1) >= center) << 0;  
  
            dst.at<unsigned char>(i-1,j-1) = code;  
        }  
    }
	double minVal,maxVal;
	cv::minMaxLoc(dst,&minVal,&maxVal);
	for(int i=0;i<dst.rows;i++)
	{
		for(int j=0;j<dst.cols;j++)
		{
			float temp=(dst.at<uchar>(i,j)-minVal)/(maxVal-minVal);
			feat.push_back(temp);
		}
	}	
}

void H_ClassifierRelation::GetHOGFeat(cv::Mat srcimg,vector<float>& feat)
{
	int width=srcimg.cols;
	int height=srcimg.rows;

	cv::HOGDescriptor hog= cv::HOGDescriptor(cv::Size(width,height),cv::Size(16,16),cv::Size(8,8),cv::Size(8,8),5);
	hog.compute(srcimg,feat);
}

void H_ClassifierRelation::GetHaarFeat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetMoment1Feat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetMoment2Feat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetMoment3Feat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetOriginalFeat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetHistogramFeat(cv::Mat srcimg,vector<float>& feat)
{
	int bins = 256;    
    int hist_size[] = {bins};    
    float range[] = { 0, 256 };    
    const float* ranges[] = {range};    
  
	cv::Mat dst_hist;
	calcHist( &srcimg, 1, 0, cv::Mat(),dst_hist, 1, hist_size, ranges,true,false); 
	double max_val;  //直方图的最大值  
    cv::minMaxLoc(dst_hist, 0, &max_val, 0, 0); //计算直方图最大值  
	normalize(dst_hist, dst_hist, 0, max_val, cv::NORM_MINMAX, -1, cv::Mat() );

    for(int i=0;i<bins;i++)    
    {    
        float bin_val = dst_hist.at<float>(i)/255; // 第i灰度级上的数 
		feat.push_back(bin_val);
	}
}

void H_ClassifierRelation::GetCLCMFeat(cv::Mat srcimg,vector<float>& feat)
{
	CalcCLCM(srcimg,0,feat);//水平
	CalcCLCM(srcimg,1 ,feat);//垂直  
	CalcCLCM(srcimg,2,feat);//对角 
}

void H_ClassifierRelation::GetSuftFeat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetSiftFeat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetTamuraFeat(cv::Mat srcimg,vector<float>& feat)
{

}

void H_ClassifierRelation::GetSpectrumFeat(cv::Mat srcimg,vector<float>& feat)
{
	
}

void H_ClassifierRelation::GetShapeFeat(cv::Mat srcimg,vector<float>& feat)
{

}


void H_ClassifierRelation::Classifier_BP(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
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

void H_ClassifierRelation::Classifier_BP_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam)
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

void H_ClassifierRelation::Classifier_SVM(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	CvSVMParams Params;
	CvSVM svm;

	if(tClassifierParam.tClassifierParam_SVM.bIsOptimizeParam==true)
	{
		Params.svm_type = CvSVM::C_SVC;    
		Params.kernel_type = tClassifierParam.tClassifierParam_SVM.iKernelType;//CvSVM::RBF;    
		Params.C = tClassifierParam.tClassifierParam_SVM.fC;							//给参数赋初始值  
		Params.p = 5e-3;						//给参数赋初始值  
		Params.gamma = tClassifierParam.tClassifierParam_SVM.fGamma;					//给参数赋初始值  
		Params.term_crit = cvTermCriteria(CV_TERMCRIT_EPS, tClassifierParam.tClassifierParam_SVM.iMaxIter, tClassifierParam.tClassifierParam_SVM.fEpsilon);   
		//对不用的参数step设为0  
		CvParamGrid nuGrid = CvParamGrid(1,1,0.0);  
		CvParamGrid coeffGrid = CvParamGrid(1,1,0.0);  
		CvParamGrid degreeGrid = CvParamGrid(1,1,0.0);  

		svm.train_auto(trainData,trainLabels,cv::Mat(), cv::Mat(),Params, 10,svm.get_default_grid(CvSVM::C),svm.get_default_grid(CvSVM::GAMMA), svm.get_default_grid(CvSVM::P),nuGrid, coeffGrid,degreeGrid);
		svm.save(tClassifierParam.tClassifierParam_SVM.fileName);
	}
	else
	{
		Params.svm_type    = cv::SVM::C_SVC;
		Params.C           = tClassifierParam.tClassifierParam_SVM.fC;//1;//0.1
		Params.kernel_type = tClassifierParam.tClassifierParam_SVM.iKernelType;//cv::SVM::RBF; //SVM::LINEAR;
		Params.gamma	   = tClassifierParam.tClassifierParam_SVM.fGamma;
		Params.term_crit   = cv::TermCriteria(CV_TERMCRIT_ITER, tClassifierParam.tClassifierParam_SVM.iMaxIter, tClassifierParam.tClassifierParam_SVM.fEpsilon);

		svm.train(trainData, trainLabels, cv::Mat(), cv::Mat(), Params);
		svm.save(tClassifierParam.tClassifierParam_SVM.fileName);
	}
	///

}

void H_ClassifierRelation::Classifier_SVM_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam)
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

void H_ClassifierRelation::Classifier_RTree(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	float *priors=new float[iClassNum];
	for(int i=0;i<iClassNum;i++)
	{
		priors[i]=tClassifierParam.tClassifierParam_RTree.fPriors[i];// weights of each classification for classes
	}
    CvRTParams params = CvRTParams(tClassifierParam.tClassifierParam_RTree.iDepth,				// max depth 20
                                   10,				// min sample count
                                   0,				// regression accuracy: N/A here
                                   false,			// compute surrogate split, no missing data
                                   15,				// max number of categories (use sub-optimal algorithm for larger numbers)
                                   priors,			// the array of priors
                                   true,			// calculate variable importance
                                   0,				// number of variables randomly selected at node and used to find the best split(s).
								   tClassifierParam.tClassifierParam_RTree.iTreeNum,			// max number of trees in the forest 100
								   tClassifierParam.tClassifierParam_RTree.fEpsilon,           // forest accuracy 0.01
                                   CV_TERMCRIT_ITER | CV_TERMCRIT_EPS	// termination cirteria
                                  );
	CvRTrees rtree;
    bool train_result=rtree.train(trainData, CV_ROW_SAMPLE, trainLabels,cv::Mat(), cv::Mat(), cv::Mat(), cv::Mat(), params);
	rtree.save(tClassifierParam.tClassifierParam_RTree.fileName);
	delete[] priors;
}

void H_ClassifierRelation::Classifier_RTree_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	int iDataNumCorrect=0;
	int iDataNum=testData.rows;
	CvRTrees rtree;
	rtree.load(tClassifierParam.tClassifierParam_RTree.fileName);
	for(int i=0;i<testData.rows;i++)
	{
		float ipre = rtree.predict(testData.row(i));
		perLabels.at<float>(i,0)=ipre;
	}
}

void H_ClassifierRelation::Classifier_Boosting(const cv::Mat& trainData,const cv::Mat& trainLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	//目前只能用于2分类
	float *priors=new float[iClassNum];
	for(int i=0;i<iClassNum;i++)
	{
		priors[i]=tClassifierParam.tClassifierParam_Boosting.fPriors[i];// weights of each classification for classes
	}
	CvBoostParams params=CvBoostParams(CvBoost::REAL,
										10,//The number of weak classifiers.
										0.95, //A threshold between 0 and 1 used to save computational time. Samples with summary weight
										15,
										false,
										priors
										);
	CvBoost boost;
    boost.train(trainData, CV_ROW_SAMPLE, trainLabels,cv::Mat(), cv::Mat(), cv::Mat(), cv::Mat(),params);
	boost.save(tClassifierParam.tClassifierParam_Boosting.fileName);
}

void H_ClassifierRelation::Classifier_Boosting_Predict(const cv::Mat& testData,cv::Mat& perLabels,int iClassNum,ClassifierParam tClassifierParam)
{
	int iDataNumCorrect=0;
	int iDataNum=testData.rows;
	CvBoost boost;
	boost.load(tClassifierParam.tClassifierParam_Boosting.fileName);
	for(int i=0;i<testData.rows;i++)
	{
		float ipre = boost.predict(testData.row(i));
		perLabels.at<float>(i,0)=ipre;
	}
}