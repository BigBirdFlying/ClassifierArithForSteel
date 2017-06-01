#pragma once
///<proj_info>
//==============================================================================
// ��Ŀ�� �����߼��ϵͳ
// �ļ��� ��ArNTDefectDetectInterface.h
// ��  �� �����
// ��  ; ���������㷨ģ��ĺ����ӿں���������
// ��  Ȩ �������Ƽ���ѧұ�𹤳��о�Ժ
//����ƽ̨��WindowsNT 
//==============================================================================
///</proj_info>


///<header_info>
#include "ArNtBaseDataStruct.h"
///</header_info>

///<datastruct_info>
//==============================================================================
// �����������Ͷ���

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
//���������ͼ��ȱ��
//������PImgDataType pImgData ��Ҫ����ͼ������
//		ShortInt iImageWidth  ��Ҫ����ͼ����
//		ShortInt iImageHeight ��Ҫ����ͼ��߶�
//		ShortInt& iDefectNum  ͨ�����÷��صļ�⵽ȱ�ݵ�����
//		ShortInt  iCameralNo  ��Ҫ���������
//����ֵ��PArNTInnerDefectInfo ���صļ��ȱ�ݽ���б�
PArNTInnerDefectInfo ARNT_CALL  DetectArith(PImgDataType pImgData, 
											ShortInt     iImageWidth, 
											ShortInt     iImageHeight, 
											ShortInt    &iDefectNum,
											ShortInt     iCameralNo);

///</func_info>

