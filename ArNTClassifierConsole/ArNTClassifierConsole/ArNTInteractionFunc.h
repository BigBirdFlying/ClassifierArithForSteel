#pragma once
#include "H_Common.h"
#include "HDBOper.h"
#include "HFile.h"
#include "DataDefCommon.h"

bool ReadDefectRecordFromDB(HDBOper& hDBOper,CString strIP,CString strDBName,CString strType,CString strSQLCondition,long& iDefectMaxIndex, SteelDefectInfoSet& DefectSet);
bool GetDefectImg(CString strIP,int iCamIndex,int iSquence,int iImgIndex,int iImgWidth,int iImgHeight,cv::Mat& image);
void GetDefectImgROI(const cv::Mat& src,cv::Rect rect,cv::Mat& dst);

void DefectImgClassify_Init(ConfigInfo_ClassifyRelation tConfigInfo_ClassifyRelation,ClassifierObject &tClassifierObject);
int DefectImgClassify(cv::Mat src,ConfigInfo_ClassifyRelation tConfigInfo_ClassifyRelation,ClassifierObject	tClassifierObject);

void WriteDefectClassToDB(HDBOper& hDBOper,int iperClass,int iDefectNo);

void AddLog(CString strInfo);