/////////////////////////////////////////////////
#pragma once
//#include "ArNTDefectDetectInterface.h"
#include "stdafx.h"
#include "ArNTGrayBmp.h"
#include "H_Common.h"
#include "H_DataTypeDef.h"

#define M_DefectListRows 128
#define M_DefectListCols 256
//��
class ArNTDefectArith_H	
{
public:
	ArNTDefectArith_H();
	~ArNTDefectArith_H(void);

public:
	ConfigInfo_Segmentation				m_ConfigInfo_Segmentation;//�������
	ClassifierObject					m_ClassifierObject;//����������
	ConfigInfo_ClassifyRelation			m_ConfigInfo_ClassifyRelation;//����������������
	ConfigInfo_ClassConvertSet			m_ConfigInfo_ClassConvertSet;//�������ת��

	int									m_imgSrcWidth;
	int									m_imgSrcHeight;
	static int							m_gArithSelection;
	int									m_iLastSegmentationThreshold;

	bool								m_bIsDebug;
	//����ȱ��
	cv::Mat m_DefectList;
	int m_DefectListIndex;
	bool m_IsPeriodDefect;
	int m_PeriodDefectSit[M_DefectListCols];
	int m_SideCountSit[M_DefectListCols];
	
	//int 				m_InterToExter[64];
public:
	//����⺯��
	int _DetectImgDefect( unsigned char* srcImg,int srcImgWidth,int srcImgHeight,PH_DefectSet pSet,int SteelNo=0,int ImgIndex=0,int CameralNo=0,unsigned char* dstImg=NULL);
	//��ʼ������
	bool _Initial(int camaraIndex=0);
	//����־����
	void AddLog(StrPointer strLogInfo);
	//����������ļ�
	bool Read_ConfigInfo_Segmentation();
	//�����ת�������ļ�
	bool Read_ConfigInfo_ClassConvertSet();
	//����������������ļ�
	bool Read_ConfigInfo_ClassifyRelation();
	//ͼ�񱣴�
	void Save_Image_OpenCV(cv::Mat src ,int iClass,int iSteel,int iCamera,int iImgIndex,int num); 
	//��Ini�����ļ�
	int Read_IniFile(CString strFileName,CString strSectionName,CString strKeyName,CString& strKeyValue);
	//ȱ��ͼ�����
	void DefectImgClassify(cv::Mat src,int &preClass);
	//����ȱ�ݱ�ʶ
	void Defects_Period_OpenCV(PH_DefectSet pSet,float factor,int interval,int PeriodClass,int ImgIndex);
};
