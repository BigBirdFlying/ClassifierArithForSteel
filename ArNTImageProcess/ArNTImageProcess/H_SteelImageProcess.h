#pragma once
#include "h_baseimageprocess.h"

class H_SteelImageProcess :
	public H_BaseImageProcess
{
public:
	H_SteelImageProcess(void);
	~H_SteelImageProcess(void);

	public:
	///<func_info>
	//�������õ������λ�úʹ�С���浽ȱ�ݼ�
	//������
	//const cv::Mat& src	Դͼ������
	//PH_DefectSet pH_DefectSet		ȱ�ݼ�
	//int dilation_size				���͵ĳߴ� Ĭ��Ϊ5
	//int iScale_x					�������� Ĭ��Ϊ5
	//int iScale_y					�������� Ĭ��Ϊ10
	//int minAreaLimit				��С�����������
	//����ֵ����
	static void Steel_RegionToDefectSet(const cv::Mat& src_region,PH_DefectSet pH_DefectSet,int dilation_size=5,int iScale_x=5,int iScale_y=10,int minAreaLimit=500);
	///</func_info>

	public:
	///<func_info>
	//�������õ��ְ�ͼ��߽�
	//������
	//const cv::Mat& src	Դͼ������
	//int& iLeft			��߽�
	//int& iRight			�ұ߽�
	//int offSet			�߽�����
	//����ֵ����
	static void Steel_FindEdge(const cv::Mat& src,int& iLeft,int& iRight,int offset=0);
	///</func_info>

	public:
	///<func_info>
	//�������г��ְ�ͼ��߽�
	//������
	//const cv::Mat& src	Դͼ������
	//const int& iLeft			��߽�
	//const int& iRight			�ұ߽�
	//cv::Mat& dst			�г��߽���ͼ��
	//����ֵ����
	static void Steel_CutEdge(const cv::Mat& src,const int& iLeft,const int& iRight,cv::Mat& dst);
	///</func_info>
};
