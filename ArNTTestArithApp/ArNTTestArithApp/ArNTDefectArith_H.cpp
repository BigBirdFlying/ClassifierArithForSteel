/////////////////////////////////////////////////
#include "ArNTDefectArith_H.h"
#include <ipps.h>
#include <ippi.h>
#include <ippcv.h>
#include <ippm.h>
#include <math.h>
#include "CommonFunc.h"
#include "H_DefectDetectInterface.h"
//构造函数
int ArNTDefectArith_H::m_gArithSelection=0;
ArNTDefectArith_H::ArNTDefectArith_H()
{
	
}

ArNTDefectArith_H::~ArNTDefectArith_H()
{

}
//初始化函数
bool ArNTDefectArith_H::_Initial(int camaraIndex)
{
	for(int i=0;i<MAX_NUM_CLASS;i++)
	{
		m_ConfigInfo_Segmentation.bClassDetect[i]=true;
	}
	//读检出配置文件
	Read_ConfigInfo_Segmentation();
	//读类别转换文件
	Read_ConfigInfo_ClassConvertSet();
	//读分类设计文件
	Read_ConfigInfo_ClassifyRelation();
	//
	m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);
	m_DefectListIndex=0;
	memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
	memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
	//
	return true;
}

//主算法函数
int  ArNTDefectArith_H::_DetectImgDefect(unsigned char* srcImg,int srcImgWidth,int srcImgHeight,PH_DefectSet pSet,int SteelNo,int ImgIndex,int CameralNo,unsigned char* dstImg)
{
	//是否是调试模式
	m_bIsDebug=false;
	//double T_start = (double)cv::getTickCount();
	//opencv格式图像
	cv::Mat cvSrcImg = cv::Mat(srcImgHeight,srcImgWidth,CV_8UC1,srcImg);
	//缺陷集结构体对象
	pSet->iItemNum=0;
	PH_DefectSet pH_DefectSet=new H_DefectSet;
	pH_DefectSet->iItemNum=0;
	PH_DefectSet pH_DefectSet_Cut=new H_DefectSet;
	pH_DefectSet_Cut->iItemNum=0;
	//图像尺寸
	m_imgSrcWidth=srcImgWidth;
	m_imgSrcHeight=srcImgHeight;
	//
	//CameralNo=5;
	m_ConfigInfo_Segmentation.tSteelInfo.iSquence=SteelNo;
	m_ConfigInfo_Segmentation.tSteelInfo.iCamera=CameralNo;
	m_ConfigInfo_Segmentation.tSteelInfo.iImgIndex=ImgIndex;
	//缺陷检出函数（位于Dll文件中）
	if(m_gArithSelection==0)
	{
		m_ConfigInfo_Segmentation.iSegmentationThreshold=m_iLastSegmentationThreshold;
	}
	else if(m_gArithSelection==1)
	{
		m_ConfigInfo_Segmentation.iSegmentationThreshold=m_iLastSegmentationThreshold+5;
	}
	H_DefectDetectInterfaceFunc(m_ConfigInfo_Segmentation.iProductionLineNo,srcImg,srcImgWidth,srcImgHeight,m_ConfigInfo_Segmentation,pH_DefectSet,m_bIsDebug);
	//缺陷分类
	for(int n=0;n<pH_DefectSet->iItemNum;n++)
	{
		if(pH_DefectSet->Items[n].iDefectClass<0)
		{
			continue;
		}
		//内部类别
		int iInterClass=pH_DefectSet->Items[n].iDefectClass;
		//获取图像
		cv::Mat srcImg_ROI;
		cv::Rect rect(pH_DefectSet->Items[n].iLeftInImage,pH_DefectSet->Items[n].iTopInImage,abs(pH_DefectSet->Items[n].iRightInImage-pH_DefectSet->Items[n].iLeftInImage),abs(pH_DefectSet->Items[n].iBottomInImage-pH_DefectSet->Items[n].iTopInImage));
		cvSrcImg(rect).copyTo(srcImg_ROI);
		//进行缺陷分类		
		if(true==m_ConfigInfo_Segmentation.bEnableClassifier && pH_DefectSet->Items[n].iDefectClass==0)
		{
			int iPreClass=0;
			DefectImgClassify(srcImg_ROI,iPreClass);
			iInterClass=m_ConfigInfo_ClassifyRelation.iClassExterToInter[iPreClass];		
		}
		//内外类别转换
		int iExterClass=0;
		for(int k=0;k<m_ConfigInfo_ClassConvertSet.iItemNum;k++)
		{
			if(iInterClass==m_ConfigInfo_ClassConvertSet.Items[k].iInternalClassNo)
			{
				iExterClass=m_ConfigInfo_ClassConvertSet.Items[k].iExternalClassNo;
			}
		}
		pH_DefectSet->Items[n].iDefectClass=iExterClass;
		//保存图像
		if(m_ConfigInfo_Segmentation.bEnableSaveDefectRoiImg)
		{
			Save_Image_OpenCV(srcImg_ROI,iExterClass,SteelNo,CameralNo,ImgIndex,pSet->iItemNum);
		}
	}
	
	//滤除不检缺陷类别
	for(int n=0;n<pH_DefectSet->iItemNum;n++)
	{
		if(pH_DefectSet->Items[n].iDefectClass>=0 && true==m_ConfigInfo_Segmentation.bClassDetect[pH_DefectSet->Items[n].iDefectClass])
		{
			pH_DefectSet_Cut->Items[pH_DefectSet_Cut->iItemNum].iDefectClass=pH_DefectSet->Items[n].iDefectClass;
			pH_DefectSet_Cut->Items[pH_DefectSet_Cut->iItemNum].fConfidence=pH_DefectSet->Items[n].fConfidence;
			pH_DefectSet_Cut->Items[pH_DefectSet_Cut->iItemNum].iLeftInImage=pH_DefectSet->Items[n].iLeftInImage;
			pH_DefectSet_Cut->Items[pH_DefectSet_Cut->iItemNum].iRightInImage=pH_DefectSet->Items[n].iRightInImage;
			pH_DefectSet_Cut->Items[pH_DefectSet_Cut->iItemNum].iTopInImage=pH_DefectSet->Items[n].iTopInImage;
			pH_DefectSet_Cut->Items[pH_DefectSet_Cut->iItemNum].iBottomInImage=pH_DefectSet->Items[n].iBottomInImage;
			pH_DefectSet_Cut->iItemNum++;
		}
	}
	//周期缺陷标识
	Defects_Period_OpenCV(pH_DefectSet_Cut,0.65,100,22,ImgIndex);
	//最终赋值
	for(int n=0;n<pH_DefectSet_Cut->iItemNum;n++)
	{
		if(pH_DefectSet_Cut->Items[n].iDefectClass<0)
		{
			continue;
		}
		if(pH_DefectSet_Cut->Items[n].iDefectClass==22)
		{
			int iExterClass=0;
			int iInterClass=pH_DefectSet_Cut->Items[n].iDefectClass;
			for(int k=0;k<m_ConfigInfo_ClassConvertSet.iItemNum;k++)
			{
				if(iInterClass==m_ConfigInfo_ClassConvertSet.Items[k].iInternalClassNo)
				{
					iExterClass=m_ConfigInfo_ClassConvertSet.Items[k].iExternalClassNo;
				}
			}
			pH_DefectSet_Cut->Items[n].iDefectClass=iExterClass;
		}
		pSet->Items[pSet->iItemNum].iDefectClass=pH_DefectSet_Cut->Items[n].iDefectClass;
		pSet->Items[pSet->iItemNum].fConfidence=pH_DefectSet_Cut->Items[n].fConfidence;
		pSet->Items[pSet->iItemNum].iLeftInImage=pH_DefectSet_Cut->Items[n].iLeftInImage;
		pSet->Items[pSet->iItemNum].iRightInImage=pH_DefectSet_Cut->Items[n].iRightInImage;
		pSet->Items[pSet->iItemNum].iTopInImage=pH_DefectSet_Cut->Items[n].iTopInImage;
		pSet->Items[pSet->iItemNum].iBottomInImage=pH_DefectSet_Cut->Items[n].iBottomInImage;
		pSet->iItemNum++;
	}
	//释放
	if(pH_DefectSet!= NULL)
	{
		delete pH_DefectSet;
	}
	if(pH_DefectSet_Cut!= NULL)
	{
		delete pH_DefectSet_Cut;
	}
	return pSet->iItemNum;
}

//读检出配置文件
bool ArNTDefectArith_H::Read_ConfigInfo_Segmentation()
{	
	LONGSTR strAppPath;
	CCommonFunc::GetAppPath(strAppPath, STR_LEN(strAppPath));
	wchar_t  strFilePath[256];
	CCommonFunc::SafeStringPrintf(strFilePath,STR_LEN(strFilePath), L"%s\\ArNTProcessParam.xml",strAppPath);

	LONGSTR strInfo={0};
	ArNTXMLOperator	m_XMLOperator;
	if(m_XMLOperator.LoadXML(strFilePath))
	{
		LONGSTR strValue = {0};	
		LONGSTR strName = {0};
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#ProductionLineNumber", strValue))
		{
			m_ConfigInfo_Segmentation.iProductionLineNo=_wtoi(strValue);
		}

		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#DefectMaxNumForEvery", strValue))
		{
			m_ConfigInfo_Segmentation.iDefectMaxNum=_wtoi(strValue);
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#EdgeNoDetectPixel", strValue))
		{
			m_ConfigInfo_Segmentation.iEdgeNoDetect=_wtoi(strValue);		
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#EdgeThreshold", strValue))
		{
			m_ConfigInfo_Segmentation.iEdgeThreshold=_wtoi(strValue);		
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#FilterThreshold", strValue))
		{
			m_ConfigInfo_Segmentation.iFilterThreshold=_wtoi(strValue);		
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#AreaThreshold", strValue))
		{
			m_ConfigInfo_Segmentation.iAreaThreshold=_wtoi(strValue);		
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#SegmentationThreshold", strValue))
		{
			m_ConfigInfo_Segmentation.iSegmentationThreshold=_wtoi(strValue);	
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#All#DefectDetectGrade", strValue))
		{
			m_ConfigInfo_Segmentation.iDefectDetectGrade=_wtoi(strValue);
		}

		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#Classify#IsSaveDefectRoiImg", strValue))
		{
			if(_wtoi(strValue)>0)
			{
				m_ConfigInfo_Segmentation.bEnableSaveDefectRoiImg=true;
			}
			else
			{
				m_ConfigInfo_Segmentation.bEnableSaveDefectRoiImg=false;
			}
		}
		if(m_XMLOperator.GetValueByString(L"ArNTProcessParam#Classify#EnableClassifier", strValue))
		{
			if(_wtoi(strValue)>0)
			{
				m_ConfigInfo_Segmentation.bEnableClassifier=true;
			}
			else
			{
				m_ConfigInfo_Segmentation.bEnableClassifier=false;
			}
		}

		m_ConfigInfo_Segmentation.iCameraIndex=0;
		while(ArNT_TRUE)
		{
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Edge#Camera%d", m_ConfigInfo_Segmentation.iCameraIndex);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.iCameraIndex++;
			}else
			{
				break;
			}
		}
		for(ShortInt i = 0; i < m_ConfigInfo_Segmentation.iCameraIndex; i++)
		{
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Edge#Camera%d#CameraNum", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.iCameraNo[i]=_wtoi(strValue);
			}
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Edge#Camera%d#LeftEdgeValue", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.iLeftEdgeLimit[m_ConfigInfo_Segmentation.iCameraNo[i]]=_wtof(strValue);
			}
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Edge#Camera%d#RightEdgeValue", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.iRightEdgeLimit[m_ConfigInfo_Segmentation.iCameraNo[i]]=_wtof(strValue);
			}
		}
		
		m_ConfigInfo_Segmentation.iClassIndex=0;
		while(ArNT_TRUE)
		{
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Class#Class%d", m_ConfigInfo_Segmentation.iClassIndex);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.iClassIndex++;
			}else
			{
				break;
			}
		}
		for(ShortInt i = 0; i < m_ConfigInfo_Segmentation.iClassIndex; i++)
		{
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Class#Class%d#ClassName", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.iClassNo[i]=_wtoi(strValue);
			}
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Class#Class%d#ClassDetect", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				int temp=_wtoi(strValue);
				if(temp>0)
				{
					m_ConfigInfo_Segmentation.bClassDetect[i]=true;
				}
				else
				{
					m_ConfigInfo_Segmentation.bClassDetect[i]=false;
				}
			}
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ArNTProcessParam#Class#Class%d#ClassThreshold", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_Segmentation.fClassThreshold[i]=_wtof(strValue);
			}
		}
		//
		m_iLastSegmentationThreshold=m_ConfigInfo_Segmentation.iSegmentationThreshold;
		//
		AddLog(L"载入ArNTProcessParam.xml成功！");
		return true;
	}
	else
	{
		return false;
	}	
}
//读类别转换配置文件
bool ArNTDefectArith_H::Read_ConfigInfo_ClassConvertSet()
{
	m_ConfigInfo_ClassConvertSet.iItemNum=0;

	LONGSTR strAppPath;
	CCommonFunc::GetAppPath(strAppPath, STR_LEN(strAppPath));
	wchar_t  strFilePath[256];
	CCommonFunc::SafeStringPrintf(strFilePath,STR_LEN(strFilePath), L"%s\\ArNTClassTable.xml",strAppPath);

	LONGSTR strInfo={0};
	ArNTXMLOperator	m_XMLOperator;
	if(m_XMLOperator.LoadXML(strFilePath))
	{
		LONGSTR strValue = {0};	
		LONGSTR strName = {0};

		int index=0;
		int InterNo=0;
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"缺陷类别表#缺陷类别#类别总数");
		if(m_XMLOperator.GetValueByString(strName, strValue))
		{
			index=_wtoi(strValue);
		}
		for(ShortInt i = 0; i < index; i++)
		{
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"缺陷类别表#缺陷类别#类别%d#内部编号", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iInternalClassNo=_wtoi(strValue);
			}
			CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"缺陷类别表#缺陷类别#类别%d#外部编号", i);
			if(m_XMLOperator.GetValueByString(strName, strValue))
			{
				m_ConfigInfo_ClassConvertSet.Items[m_ConfigInfo_ClassConvertSet.iItemNum].iExternalClassNo=_wtoi(strValue);
			}
			m_ConfigInfo_ClassConvertSet.iItemNum++;
		}
		
		AddLog(L"载入ArNTClassTable.xml成功！");
		return true;
	}
	else
	{
		return false;
	}	
}

bool ArNTDefectArith_H::Read_ConfigInfo_ClassifyRelation()
{
	//加载ini文件
	if(true==m_ConfigInfo_Segmentation.bEnableClassifier)
	{
		int iLength;

		TCHAR szFileName[MAX_PATH];
		::GetModuleFileName(NULL, szFileName, MAX_PATH);
		CString strFileName_App = szFileName;
		int nIndex = strFileName_App.ReverseFind('\\');
		CString strAppPath = strFileName_App.Left(nIndex);

		CString strFileName;
		strFileName.Format(L"%s\\ClassifierInteraction.ini",strAppPath);
		CString strValue=L"";

		Read_IniFile(strFileName,L"ImgSize",L"Norm",strValue);
		m_ConfigInfo_ClassifyRelation.bIsNorm=_ttoi(strValue);

		Read_IniFile(strFileName,L"ImgSize",L"Width",strValue);
		m_ConfigInfo_ClassifyRelation.iImgNormWidth=_ttoi(strValue);

		Read_IniFile(strFileName,L"ImgSize",L"Height",strValue);
		m_ConfigInfo_ClassifyRelation.iImgNormHeight=_ttoi(strValue);

		Read_IniFile(strFileName,L"Feats",L"FeatsNum",strValue);
		m_ConfigInfo_ClassifyRelation.iFeatsNum=_ttoi(strValue);

		for(int i=0;i<m_ConfigInfo_ClassifyRelation.iFeatsNum;i++)
		{
			CString strKey=L"";
			strKey.Format(L"Feat%d",i);
			if(0==Read_IniFile(strFileName,L"Feats",strKey,strValue))
			{
				break;
			}
			else
			{
				iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
				WideCharToMultiByte(CP_ACP, 0, strValue, -1, m_ConfigInfo_ClassifyRelation.strFeatName[i], iLength, NULL, NULL);
				//m_ConfigInfo_ClassifyRelation.strFeatName[i]=strValue;
			}
		}

		Read_IniFile(strFileName,L"Classifier",L"ClassNum",strValue);
		m_ConfigInfo_ClassifyRelation.iClassNum=_ttoi(strValue);

		Read_IniFile(strFileName,L"Classifier",L"ClassifierName",strValue);
		iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strValue, -1, m_ConfigInfo_ClassifyRelation.strClassifierName, iLength, NULL, NULL);
		//m_ConfigInfo_ClassifyRelation.strClassifierName=strValue;

		Read_IniFile(strFileName,L"Classifier",L"ClassifierFileName",strValue);
		iLength = WideCharToMultiByte(CP_ACP, 0, strValue, -1, NULL, 0, NULL, NULL);  
		WideCharToMultiByte(CP_ACP, 0, strValue, -1, m_ConfigInfo_ClassifyRelation.strClassifierFileName, iLength, NULL, NULL);
		//m_ConfigInfo_ClassifyRelation.strClassifierFileName=strValue;

		for(int i=0;i<m_ConfigInfo_ClassifyRelation.iClassNum;i++)
		{
			CString strKey=L"";
			strKey.Format(L"Class%d",i);
			Read_IniFile(strFileName,L"ClassConversion",strKey,strValue);
			m_ConfigInfo_ClassifyRelation.iClassExterToInter[i]=_ttoi(strValue);
		}
		//
		//char cstrClassifierName[128];
		//int iLength1 = WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strClassifierName, -1, NULL, 0, NULL, NULL);  
		//WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strClassifierName, -1, cstrClassifierName, iLength1, NULL, NULL);
		//char cstrClassifierFileName[128];
		//int iLength2 = WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strClassifierFileName, -1, NULL, 0, NULL, NULL);  
		//WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strClassifierFileName, -1, cstrClassifierFileName, iLength2, NULL, NULL);	
		H_ClassifierPredictFromExternal_Init(m_ConfigInfo_ClassifyRelation.strClassifierName,m_ConfigInfo_ClassifyRelation.strClassifierFileName,m_ClassifierObject);

		AddLog(L"载入ClassifierInteraction.ini成功！");
		return true;
	}
	else
	{
		return false;
	}	
}

int ArNTDefectArith_H::Read_IniFile(CString strFileName,CString strSectionName,CString strKeyName,CString& strKeyValue)
{
	DWORD dd=::GetPrivateProfileString(strSectionName, strKeyName, _T(""), strKeyValue.GetBuffer(1024), 1024, strFileName);
	strKeyValue.ReleaseBuffer();
	if(0==dd)
	{
		return 0;
	}
	return 1;
}

//加日志文件
void ArNTDefectArith_H::AddLog(StrPointer strLogInfo)
{
	LONGSTR strAppName = {0};
	LONGSTR m_strLogPath = {0};
	LONGSTR m_strAppPath = {0};
	CCommonFunc::GetAppPath(m_strAppPath, STR_LEN(m_strAppPath));
	CCommonFunc::GetAppName(strAppName, STR_LEN(strAppName));
	CCommonFunc::SafeStringPrintf(m_strLogPath, STR_LEN(m_strLogPath), L"%s\\Log\\%s", m_strAppPath, strAppName);
	
	SYSTEMTIME NowTime;
	GetLocalTime(&NowTime);
	CCommonFunc::CreateDir(m_strLogPath);

	LONGSTR strLogFile = {0};
	CCommonFunc::SafeStringPrintf(strLogFile, STR_LEN(strLogFile), L"%s\\%d%02d%02d.txt", m_strLogPath, NowTime.wYear, NowTime.wMonth, NowTime.wDay);
	SHORTSTR strTime = {0};
	CCommonFunc::SafeStringPrintf(strTime, STR_LEN(strTime), L"%02d时%02d分%02d秒", NowTime.wHour, NowTime.wMinute, NowTime.wSecond);
	LONGSTR strTempLogInfo = {0};
	CCommonFunc::SafeStringPrintf(strTempLogInfo, STR_LEN(strTempLogInfo), L"[%s]:%s", strTime, strLogInfo);
	CCommonFunc::WriteStringToFile( strLogFile, strTempLogInfo);
}

//保存图像
void ArNTDefectArith_H::Save_Image_OpenCV(cv::Mat src ,int iClass,int iSteel,int iCamera,int iImgIndex,int num) 
{
	TCHAR szAppName[MAX_PATH];
	::GetModuleFileName(NULL, szAppName, MAX_PATH);
	CString strAppName = szAppName;
	int nIndex = strAppName.ReverseFind('\\');
	CString strAppPath = strAppName.Left(nIndex);
	CString strLogFilePath;
	strLogFilePath.Format(L"%s\\MyImg",strAppPath);
	//int IsExist=PathFileExists(strLogFilePath);
	//if(!IsExist)
	{
		CreateDirectory(strLogFilePath, NULL);
	}
	//判断数量
	int iTotalNum=0;
	CString strWildcard;
	strWildcard.Format(L"%s\\*.*",strLogFilePath);
	CFileFind finder;
	BOOL bFind = finder.FindFile(strWildcard);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			continue;
		}
		iTotalNum++;
	} 
	finder.Close();
	if(iTotalNum>10000)
	{
		return; 
	}
	//stringstream ss;
	//string str;
	//ss<<iStr;
	//ss>>str;
	//const char* c_str = str.c_str();
	//char* s=".bmp";
	//str+=s;
	string filename=CStringA(strLogFilePath);
	char strfilename[128];
	sprintf(strfilename,"\\C%d__S%d__C%d__I%d__N%d.bmp",iClass,iSteel,iCamera,iImgIndex,num);
	filename+=strfilename;
	//cv::imwrite(filename,src);
	IplImage* image=&(IplImage)src;
	const char* p=filename.data();
	cvSaveImage(p,image);
}

//缺陷分类
void ArNTDefectArith_H::DefectImgClassify(cv::Mat src,int &preClass)
{	
	//图像归一化
	if(m_ConfigInfo_ClassifyRelation.bIsNorm)
	{
		cv::resize(src,src,cv::Size(m_ConfigInfo_ClassifyRelation.iImgNormWidth,m_ConfigInfo_ClassifyRelation.iImgNormHeight));
	}
	//提取特征
	vector<float> vFeat;
	for(int i=0;i<MAX_NUM_FEAT;i++)
	{
		//if(m_ConfigInfo_ClassifyRelation.strFeatName[i].Compare(L"")!=0)
		if(strcmp(m_ConfigInfo_ClassifyRelation.strFeatName[i],"")!=0)
		{
			//char featName[128];
			//int iLength = WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strFeatName[i], -1, NULL, 0, NULL, NULL);  
			//WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strFeatName[i], -1, featName, iLength, NULL, NULL);	
			H_GetFeat(src,vFeat,m_ConfigInfo_ClassifyRelation.strFeatName[i]);
		}
		else
		{
			continue;
		}
	}
	//测试集数据准备
	cv::Mat testData=cv::Mat::zeros(1,vFeat.size(),CV_32FC1);
	cv::Mat testLabels=cv::Mat::zeros(1,1,CV_32FC1);
	cv::Mat preLabels=cv::Mat::zeros(1,1,CV_32FC1);
	//cv::Mat testData=cvCreateMat(1,vFeat.size(),CV_32FC1);
	//cv::Mat testLabels= cvCreateMat(1, 1, CV_32FC1);
	//cv::Mat preLabels= cvCreateMat(1, 1, CV_32FC1); 
	cv::Mat OneImgFeat(vFeat); 
	cv::Mat OneImgFeatT=OneImgFeat.reshape(0,1).clone();
	OneImgFeatT.copyTo(testData.row(0));
	//释放向量
	vector<float>().swap(vFeat);

	//分类器名
	//char cstrClassifierName[128];
	//int iLength = WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strClassifierName, -1, NULL, 0, NULL, NULL);  
	//WideCharToMultiByte(CP_ACP, 0, m_ClassifyConfig.strClassifierName, -1, cstrClassifierName, iLength, NULL, NULL);	
	//预测
	H_ClassifierPredictFromExternal(m_ConfigInfo_ClassifyRelation.strClassifierName,testData,testLabels,preLabels,m_ConfigInfo_ClassifyRelation.iClassNum,m_ClassifierObject);
	//H_ClassifierPredict(cstrClassifierName,testData,testLabels,perLabels,tClassifyConfig.iClassNum,tClassifierParam);
	preClass=(int)preLabels.at<float>(0,0);	
}

void ArNTDefectArith_H::Defects_Period_OpenCV(PH_DefectSet pSet,float factor,int interval,int PeriodClass,int ImgIndex)
{
	//针对横向较宽的缺陷的限制，主要应用于西铝头部大缺陷的情况
	int LimitWidth=m_imgSrcWidth*0.2;
	
	cv::Mat RowZero=cv::Mat::zeros(1,M_DefectListCols,CV_8UC1);
	if(ImgIndex<10)//索引小于10时进行数值的初始化
	{
		m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);//缺陷列表图像
		m_DefectListIndex=0;//缺陷行索引
		memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
		memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
	}
	else
	{
		int ReductionFactor=m_imgSrcWidth/M_DefectListCols;//画板与实际图像尺寸的比值
		//1、拼接画板
		if(pSet->iItemNum>0)
		{
			if(0==m_DefectListIndex)
			{
				if(m_bIsDebug)
				{
					cv::namedWindow("DefectList",0);
				}			
			}
			cv::Rect rect(0,m_DefectListIndex%M_DefectListRows,M_DefectListCols,1);
			RowZero.copyTo(m_DefectList(rect));//清空当前行
			for(int i=0;i<pSet->iItemNum;i++)
			{
				if(abs(pSet->Items[i].iRightInImage-pSet->Items[i].iLeftInImage)<LimitWidth)
				{
					int iLeft=pSet->Items[i].iLeftInImage/ReductionFactor;
					int iRight=pSet->Items[i].iRightInImage/ReductionFactor;
					for ( ;iLeft<iRight;iLeft++)
					{
						m_DefectList.at<uchar>(m_DefectListIndex%M_DefectListRows,iLeft)=255;
					}
				}
			}
			//
			//m_DefectList.at<uchar>(m_DefectListIndex,20)=255;
			//m_DefectList.at<uchar>(m_DefectListIndex,21)=255;
			//m_DefectList.at<uchar>(m_DefectListIndex,22)=255;
			//m_DefectList.at<uchar>(m_DefectListIndex,23)=255;
			//
			m_DefectListIndex++;			
		}
		if(m_bIsDebug)
		{
			cv::imshow("DefectList",m_DefectList);
		}
		//2、画板拼接完成，验证是否存在周期性缺陷
		for(int j=0;j<M_DefectListCols;j++)
		{
			int Col_Count=0;
			for(int i=0;i<M_DefectListRows;i++)
			{
				if(m_DefectList.at<uchar>(i,j)>128)
				{
					Col_Count++;
				}
			}
			if(Col_Count>=M_DefectListRows*factor && -1!=m_PeriodDefectSit[j])//存在周期缺陷
			{
				m_PeriodDefectSit[j]=1;
			}					
		}
		//3、存在周期缺陷
		for(int i=0;i<pSet->iItemNum;i++)
		{			
			for(int j=0;j<M_DefectListCols;j++)
			{
				if(m_PeriodDefectSit[j]==1)
				{
					if(j>=pSet->Items[i].iLeftInImage/ReductionFactor && j<=pSet->Items[i].iRightInImage/ReductionFactor)
					{
						pSet->Items[i].iDefectClass=PeriodClass;
						m_PeriodDefectSit[j]=-1;
					}
				}
				if(m_PeriodDefectSit[j]==-1)
				{
					if(j>=pSet->Items[i].iLeftInImage/ReductionFactor && j<=pSet->Items[i].iRightInImage/ReductionFactor)
					{
						m_SideCountSit[j]++;
						if(m_SideCountSit[j]%interval!=0)
						{
							if(abs(pSet->Items[i].iRightInImage-pSet->Items[i].iLeftInImage)<LimitWidth)//if(6!=pSet->Items[i].iDefectClass)//针对西铝添加的
							{
								pSet->Items[i].iDefectClass=-1;
							}
						}
						else
						{
							pSet->Items[i].iDefectClass=PeriodClass;
						}

						//隐藏缺陷点个数超过某值时，打破周期重新计算
						if(m_SideCountSit[j]>interval*50)
						{
							m_DefectList=cv::Mat::zeros( M_DefectListRows,M_DefectListCols, CV_8UC1);//缺陷列表图像
							m_DefectListIndex=0;//缺陷行索引
							memset(&m_PeriodDefectSit,0,M_DefectListCols*sizeof(m_PeriodDefectSit[0]));
							memset(&m_SideCountSit,0,M_DefectListCols*sizeof(m_SideCountSit[0]));
						}
					}
				}
			}
		}			
	}
}