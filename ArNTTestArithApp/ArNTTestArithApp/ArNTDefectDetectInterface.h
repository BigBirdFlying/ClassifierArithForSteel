#pragma once
///<proj_info>
//==============================================================================
// 项目名 ：在线检测系统
// 文件名 ：ArNTDefectDetectInterface.h
// 作  者 ：杨朝霖
// 用  途 ：定义了算法模块的函数接口和数据类型
// 版  权 ：北京科技大学冶金工程研究院
//适用平台：WindowsNT 
//==============================================================================
///</proj_info>


///<header_info>
#include "ArNtBaseDataStruct.h"
///</header_info>

///<datastruct_info>
//==============================================================================
// 基本数据类型定义

typedef struct tagArNTInnerDefectInfo
{
	LongInt		iSize;
	TinyInt		iClass;
	TinyInt     iGrade;
	ShortInt	iLeft;
	ShortInt	iRight;
	ShortInt	iTop;
	ShortInt	iBottom;
	LongInt		iArea;
}ArNTInnerDefectInfo, *PArNTInnerDefectInfo;
///</datastruct_info>

//==============================================================================
//
///<func_info>
//描述：检测图像缺陷
//参数：PImgDataType pImgData 需要检测的图像数据
//		ShortInt iImageWidth  需要检测的图像宽度
//		ShortInt iImageHeight 需要检测的图像高度
//		ShortInt& iDefectNum  通过引用返回的检测到缺陷的数量
//		ShortInt  iCameralNo  需要检测的相机号
//返回值：PArNTInnerDefectInfo 返回的检测缺陷结果列表
PArNTInnerDefectInfo ARNT_CALL  DetectArith(PImgDataType pImgData, 
											ShortInt     iImageWidth, 
											ShortInt     iImageHeight, 
											ShortInt    &iDefectNum,
											ShortInt     iCameralNo);

///</func_info>

