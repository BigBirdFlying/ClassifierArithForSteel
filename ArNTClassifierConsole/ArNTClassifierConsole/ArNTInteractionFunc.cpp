#include "stdafx.h"
#include "ArNTInteractionFunc.h"
#include "H_DefectDetectInterface.h"

bool ReadDefectRecordFromDB(HDBOper& hDBOper,CString strIP,CString strDBName,CString strType,CString strSQLCondition,long& iDefectMaxIndex, SteelDefectInfoSet& DefectSet)
{
	if(hDBOper.m_bConnected==false)
	{
		hDBOper.ConnectDB(strIP,strDBName,L"ARNTUSER",L"ARNTUSER",strType);
		hDBOper.GetDefectMaxNum(iDefectMaxIndex,L"select top 1 * from Defect order by ID desc");
	}
	CString strSQL=L"";
	strSQL.Format(L"select * from Defect where DefectNo>%d and Class=%d",iDefectMaxIndex,0);
	//strSQL.Format(L"select * from Defect where DefectNo>%d",iDefectMaxIndex);
	hDBOper.SearchDefect(DefectSet,iDefectMaxIndex,strSQL);
	return hDBOper.m_bConnected;
}

void DefectImgClassify_Init(ConfigInfo_ClassifyRelation tConfigInfo_ClassifyRelation,ClassifierObject &tClassifierObject)
{
	H_ClassifierPredictFromExternal_Init(tConfigInfo_ClassifyRelation.strClassifierName,tConfigInfo_ClassifyRelation.strClassifierFileName,tClassifierObject);
}

int DefectImgClassify(cv::Mat src,ConfigInfo_ClassifyRelation tConfigInfo_ClassifyRelation,ClassifierObject	tClassifierObject)
{
	//CString strInfo=L"";
	//strInfo.Format(L"相机号：%d  缺陷号：%d  当前类别：%d",iCamIndex,DefectSet.Items[i].iDefectNo,DefectSet.Items[i].iClass);
	//AddLog(strInfo);
	
	//图像归一化
	if(tConfigInfo_ClassifyRelation.bIsNorm)
	{
		cv::resize(src,src,cv::Size(tConfigInfo_ClassifyRelation.iImgNormWidth,tConfigInfo_ClassifyRelation.iImgNormHeight));
	}
	//提取特征
	vector<float> vFeat;
	for(int i=0;i<64;i++)
	{
		if(strcmp(tConfigInfo_ClassifyRelation.strFeatName[i],"")!=0)
		{
			//char featName[128];
			//int iLength = WideCharToMultiByte(CP_ACP, 0, tConfigInfo_ClassifyRelation.strFeatName[i], -1, NULL, 0, NULL, NULL);  
			//WideCharToMultiByte(CP_ACP, 0, tConfigInfo_ClassifyRelation.strFeatName[i], -1, featName, iLength, NULL, NULL);	
			H_GetFeat(src,vFeat,tConfigInfo_ClassifyRelation.strFeatName[i]);
		}
		else
		{
			continue;
		}
	}
	//测试集数据准备
	cv::Mat testData=cvCreateMat(1,vFeat.size(),CV_32FC1);
	cv::Mat OneImgFeat(vFeat); 
	cv::Mat OneImgFeatT=OneImgFeat.reshape(0,1).clone();
	OneImgFeatT.copyTo(testData.row(0));

	cv::Mat testLabels= cvCreateMat(1, 1, CV_32FC1);
	cv::Mat preLabels= cvCreateMat(1, 1, CV_32FC1); 

	//分类器参数
	ClassifierParam			tClassifierParam;
	if(strcmp(tConfigInfo_ClassifyRelation.strClassifierName,"BP_Classifier")==0)
	{
		tClassifierParam.tClassifierParam_BP.fEpsilon=0.0001;
		tClassifierParam.tClassifierParam_BP.iHideNode=30;
		tClassifierParam.tClassifierParam_BP.iMaxIter=10000;
		strcpy(tClassifierParam.tClassifierParam_BP.fileName,tConfigInfo_ClassifyRelation.strClassifierFileName);
	}
	else if(strcmp(tConfigInfo_ClassifyRelation.strClassifierName,"SVM_Classifier")==0)
	{
		tClassifierParam.tClassifierParam_SVM.fEpsilon=0.0001;
		tClassifierParam.tClassifierParam_SVM.iKernelType=2;
		tClassifierParam.tClassifierParam_SVM.iMaxIter=10000;
		strcpy(tClassifierParam.tClassifierParam_SVM.fileName,tConfigInfo_ClassifyRelation.strClassifierFileName);
	}
	else if(strcmp(tConfigInfo_ClassifyRelation.strClassifierName,"RTree_Classifier")==0)
	{
		tClassifierParam.tClassifierParam_RTree.fEpsilon=0.0001;
		tClassifierParam.tClassifierParam_RTree.iTreeNum=100;
		tClassifierParam.tClassifierParam_RTree.iMaxIter=10000;
		for(int i=0;i<tConfigInfo_ClassifyRelation.iClassNum;i++)
		{
			tClassifierParam.tClassifierParam_RTree.fPriors[i]=1;
		}
		strcpy(tClassifierParam.tClassifierParam_RTree.fileName,tConfigInfo_ClassifyRelation.strClassifierFileName);
	}
	else if(strcmp(tConfigInfo_ClassifyRelation.strClassifierName,"Boosting_Classifier")==0)
	{
		tClassifierParam.tClassifierParam_Boosting.fEpsilon=0.0001;
		for(int i=0;i<tConfigInfo_ClassifyRelation.iClassNum;i++)
		{
			tClassifierParam.tClassifierParam_Boosting.fPriors[i]=1;
		}
		tClassifierParam.tClassifierParam_BP.iMaxIter=10000;
		strcpy(tClassifierParam.tClassifierParam_BP.fileName,tConfigInfo_ClassifyRelation.strClassifierFileName);
	}
	////最小误差
	//tClassifierParam.fEpsilon=0.0001;
	////隐藏层
	//tClassifierParam.iHideNode=30;
	////最大迭代次数
	//tClassifierParam.iMaxIter=10000;
	////分类器文件名
	////int iLength = WideCharToMultiByte(CP_ACP, 0, tConfigInfo_ClassifyRelation.strClassifierFileName, -1, NULL, 0, NULL, NULL);  
	////WideCharToMultiByte(CP_ACP, 0, tConfigInfo_ClassifyRelation.strClassifierFileName, -1, tClassifierParam.fileName, iLength, NULL, NULL);
	//strcpy(tClassifierParam.fileName,tConfigInfo_ClassifyRelation.strClassifierFileName);
	//分类器名
/*	char cstrClassifierName[128];
	iLength = WideCharToMultiByte(CP_ACP, 0, tConfigInfo_ClassifyRelation.strClassifierName, -1, NULL, 0, NULL, NULL);  
	WideCharToMultiByte(CP_ACP, 0, tConfigInfo_ClassifyRelation.strClassifierName, -1, cstrClassifierName, iLength, NULL, NULL);*/	
	//预测
	//H_ClassifierPredict(tConfigInfo_ClassifyRelation.strClassifierName,testData,testLabels,preLabels,tConfigInfo_ClassifyRelation.iClassNum,tClassifierParam);
	H_ClassifierPredictFromExternal(tConfigInfo_ClassifyRelation.strClassifierName,testData,testLabels,preLabels,tConfigInfo_ClassifyRelation.iClassNum,tClassifierObject);
	int preClass=(int)preLabels.at<float>(0,0);
	return preClass;
	
}

bool GetDefectImg(CString strIP,int iCamIndex,int iSquence,int iImgIndex,int iImgWidth,int iImgHeight,cv::Mat& image)
{
	CString strImgPath=L"";
	strImgPath.Format(L"\\\\%s\\CamDefectImage%d\\%d\\%04d.img",strIP,iCamIndex,iSquence,iImgIndex);

	FILE *fp=NULL;
	USES_CONVERSION;
	const char * imgpath=T2A(strImgPath.GetBuffer(0));
	fp=fopen(imgpath,"rb");

	if(fp==NULL)
	{
		//AfxMessageBox(L"文件打开失败");
		return false;
	}
	fseek(fp,0,SEEK_END); //定位到文件末 
	long nFileLen = ftell(fp); //文件长度
	char* Data = (char*)malloc(sizeof(char) * nFileLen);
	memset(Data, 0, sizeof(char) * nFileLen);
	fseek(fp,0,SEEK_SET); //定位到文件头
	while(!feof(fp))
	{
		fread(Data,nFileLen,1,fp);
	}
	fclose(fp);

	int offset=nFileLen-iImgWidth*iImgHeight;
	char *img=NULL;
	//img = (char*)malloc(sizeof(char) * iImgWidth*iImgHeight);
	//memset(img, 0, sizeof(char) * iImgWidth*iImgHeight);
	img=Data+offset;
	cv::Mat src = cv::Mat(iImgHeight,iImgWidth,CV_8UC1,img);
	
	//图像是倒着的,所以要映射翻转一下，但由于缺陷区域本身是按着img的图像格式存储的，所以现场使用时不用进行翻转
	//cv::Mat map_x,map_y;
	//map_x.create( src.size(), CV_32FC1 );
	//map_y.create( src.size(), CV_32FC1 );
	//for( int i = 0; i < src.rows; i++ )
	//{ 
	//	for( int j = 0; j < src.cols; j++ )
	//	{
	//		map_x.at<float>(i,j) = j ;
	//		map_y.at<float>(i,j) = src.rows - i ;
	//	}
	//}
	//remap( src, image, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
	
	//
	image=src.clone();
	//
	free(Data);
	return true;
	
}

void GetDefectImgROI(const cv::Mat& src,cv::Rect rect,cv::Mat& dst)
{
	src(rect).copyTo(dst);
}

//加日志文件
void AddLog(CString strInfo)
{
	TCHAR szFileName[MAX_PATH];
	if (::GetModuleFileName(NULL, szFileName, MAX_PATH))
	{
		//去掉文件名
		CString strFileName = szFileName;
		int nIndex = strFileName.ReverseFind('\\');
		CString strDirectory = strFileName.Left(nIndex);
		CString strText = _T("");
	}

	USES_CONVERSION;
	const char * info=T2A(strInfo.GetBuffer(0));
	TXTFILE_OPEN_START("log.txt")
		TXTFILE_ADD("提示",info)//TXTFILE_ADD("图像索引",(float)ImgIndex)
	TXTFILE_CLOSE_END
}

void WriteDefectClassToDB(HDBOper& hDBOper,int iperClass,int iDefectNo)
{
	if(hDBOper.m_bConnected==true)
	{
		CString strSQL=L"";
		strSQL.Format(L"update Defect set Class=%d where DefectNo=%d",iperClass,iDefectNo);
		hDBOper.UpdateDefectClass(strSQL);
	}
}