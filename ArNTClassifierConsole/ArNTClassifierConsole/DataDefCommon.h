
#pragma once
typedef struct tagConfigInfo_Local
{
	bool	bAutoClassify;
	int		iDefectCameraList;
	int		iDefectInteral;
	int		iDefectOffset;	
	int		iDefectImgIP;	
	int		iCameraNum;
	int		iImgWidth;
	int		iImgHeight;
	CString strDefectDBIP;
	CString strDefectImgIP[8];

	bool	bIsEnd;
}ConfigInfo_Local,*PConfigInfo_Local;

//typedef struct tagConfigInfo_ClassifyRelation
//{
//	bool bIsNorm;
//	int iImgNormWidth;
//	int iImgNormHeight;
//	int iClassNum;
//	int iFeatsNum;
//	CString strFeatName[64];
//	int iClassExterToInter[64];
//	CString strClassifierName;
//	CString strClassifierFileName;
//}ConfigInfo_ClassifyRelation,*PConfigInfo_ClassifyRelation;