#pragma once

//����Ԥ��������
#define MAX_NUM_CLASS  64
#define MAX_NUM_FEAT   64

//ȱ�����ݽṹ�嶨��

//ȱ�ݼ���
typedef struct tagH_Defect
{	
	int		iLeftInImage;		
	int		iRightInImage;		
	int		iTopInImage;		
	int		iBottomInImage;		
	float	fConfidence;
	int		iDefectClass;
}*PH_Defect, H_Defect;
typedef struct tagH_DefectSet
{
	enum {MAX_ITEMNUM = 1024};
	H_Defect Items[MAX_ITEMNUM];
	int iItemNum;
}H_DefectSet, *PH_DefectSet;
//����������
typedef struct tagClassifierParam_BP
{
	int iMaxIter;
	float fEpsilon;
	int iHideNode;
	char fileName[128];
}ClassifierParam_BP,*PClassifierParam_BP;
typedef struct tagClassifierParam_SVM
{
	int iMaxIter;
	float fEpsilon;
	int iKernelType;
	float fC;
	float fGamma;
	char fileName[128];
	bool bIsOptimizeParam;
}ClassifierParam_SVM,*PClassifierParam_SVM;
typedef struct tagClassifierParam_RTree
{
	int iMaxIter;
	float fEpsilon;
	int iDepth;
	int	iTreeNum;
	char fileName[128];
	float fPriors[MAX_NUM_CLASS];
}ClassifierParam_RTree,*PClassifierParam_RTree;
typedef struct tagClassifierParam_Boosting
{
	int iMaxIter;
	float fEpsilon;
	char fileName[128];
	float fPriors[MAX_NUM_CLASS];
}ClassifierParam_Boosting,*PClassifierParam_Boosting;
typedef struct tagClassifierParam
{
	ClassifierParam_BP tClassifierParam_BP;
	ClassifierParam_SVM tClassifierParam_SVM;
	ClassifierParam_RTree tClassifierParam_RTree;
	ClassifierParam_Boosting tClassifierParam_Boosting;
}ClassifierParam,*PClassifierParam;
//ͼ����Ӧ�ְ���Ϣ
typedef struct tagSteelInfo
{
	int iSquence;
	int iCamera;
	int iImgIndex;
}SteelInfo, *PSteelInfo;
//ȱ�ݼ����Ӧ����
typedef struct tagConfigInfo_Segmentation
{
	int iProductionLineNo;

	int iDefectMaxNum;
	int iEdgeNoDetect;
	int iEdgeThreshold;
	int iFilterThreshold;
	int iAreaThreshold;
	int iSegmentationThreshold;
	int iDefectDetectGrade;

	bool bEnableSaveDefectRoiImg;
	bool bEnableClassifier;

	enum {MAX_CAMERA_NUM = 16};
	int iCameraIndex;
	int iCameraNo[MAX_CAMERA_NUM];
	int iLeftEdgeLimit[MAX_CAMERA_NUM];
	int iRightEdgeLimit[MAX_CAMERA_NUM];
	
	enum {MAX_CLASS_NUM = 64};
	int iClassIndex;
	int iClassNo[MAX_CLASS_NUM];
	bool bClassDetect[MAX_CLASS_NUM];
	float fClassThreshold[MAX_CLASS_NUM];

	SteelInfo tSteelInfo;
}ConfigInfo_Segmentation, *PConfigInfo_Segmentation;
//ȱ���������ת��
typedef struct tagConfigInfo_ClassConvert
{	
	int iInternalClassNo;
	int iExternalClassNo;
	char strClassName[128];
}*PConfigInfo_ClassConvert, ConfigInfo_ClassConvert;
typedef struct tagConfigInfo_ClassConvertSet
{
	enum {MAX_ITEMNUM = 1024};
	ConfigInfo_ClassConvert Items[MAX_ITEMNUM];
	int iItemNum;
}ConfigInfo_ClassConvertSet, *PConfigInfo_ClassConvertSet;
//����������
typedef struct tagClassifierObject{
	CvANN_MLP mlp;
	CvSVM svm;
	CvRTrees rtree;
	CvBoost boost;
}ClassifierObject,*PClassifierObject;
//ȱ�ݷ����������
typedef struct tagConfigInfo_ClassifyRelation
{
	bool bIsNorm;
	int iImgNormWidth;
	int iImgNormHeight;
	int iClassNum;
	int iFeatsNum;
	char strFeatName[MAX_NUM_FEAT][128];
	int iClassExterToInter[MAX_NUM_CLASS];
	char strClassifierName[128];
	char strClassifierFileName[128];
}ConfigInfo_ClassifyRelation,*PConfigInfo_ClassifyRelation;