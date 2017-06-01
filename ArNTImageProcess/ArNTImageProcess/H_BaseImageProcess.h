#pragma once
#include "H_Common.h"
#include "H_DataTypeDef.h"

class H_BaseImageProcess
{
public:
	H_BaseImageProcess(void);
	~H_BaseImageProcess(void);

public:
	///<func_info>
	//����������ҶȽ����㷨
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������
	//int baseGray			��׼�Ҷ�ֵ
	//int offSet			��С�����Ҷ�����
	//����ֵ����
	static void Img_Gray_Change_Y(const cv::Mat& src,cv::Mat& dst,int baseGray,int offSet);
	///</func_info>

	///<func_info>
	//����������ҶȽ����㷨
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������
	//int baseGray			��׼�Ҷ�ֵ
	//int offSet			��С�����Ҷ�����
	//bool iSelect			ѡ���������Ϊ1ʱoffSet���ã�Ϊ2ʱoffSet����
	//����ֵ����
	static void Img_Gray_Change_X(const cv::Mat& src,cv::Mat& dst,int baseGray,int offSet,int iSelect); 
	///<func_info>

	///<func_info>
	//����������ͼ�ҶȽ����㷨
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������
	//int baseGray			��׼�Ҷ�ֵ
	//int block_x			X��������Ч�߽�,Ϊż��
	//int block_y			Y��������Ч�߽�,Ϊż��
	//����ֵ����
	static void Img_Gray_Change_Integral(const cv::Mat& src,cv::Mat& dst,int baseGray,int block_x,int block_y); 
	///<func_info>

	///<func_info>
	//����������ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//int iStr				ͼ�����ֱ��
	//����ֵ����
	static void Img_Save_Image_OpenCV(const cv::Mat& src,int iStr=0); 
	///<func_info>

	///<func_info>
	//��������ʾͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//int iStr				ͼ�����ֱ��
	//����ֵ����
	static void Img_Show_Image_OpenCV(const cv::Mat& src,int iStr=0); 
	///<func_info>

	///<func_info>
	//������Otsu�ָ�ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//int iStr				ͼ�����ֱ��
	//����ֵ����
	static void Img_Otsu(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>
	 
	///<func_info>
	//������Ssr�任ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//int scale				��ֵ�ڷ�������ߵı�ֵ
	//����ֵ����
	static void Img_SSR(const cv::Mat& src,cv::Mat& dst,int scale=1); 
	///<func_info>

	///<func_info>
	//������Sobelͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//int scale				ͼ����
	//int delta				ƫ��
	//int ddepth			���
	//����ֵ����
	static void Img_Sobel(const cv::Mat& src,cv::Mat& dstx,cv::Mat& dsty,cv::Mat& dstxy,int scale=1,int delta=0,int ddepth=CV_16S); 
	///<func_info>

	///<func_info>
	//������LBPͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_LBP(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//����������ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Relief(const cv::Mat& src,cv::Mat& dst1,cv::Mat& dst2); 
	///<func_info>

	///<func_info>
	//������ֱ��ͼͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Hist(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//�������ֲ��Ŵ���Сͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Expansion_And_Extrusion(const cv::Mat& src,cv::Mat& dst1,cv::Mat& dst2); 
	///<func_info>

	///<func_info>
	//����������任ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Transform(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>
	
	///<func_info>
	//������С���ֽ�ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_DWT(const cv::Mat& src,cv::Mat& dst,int nLayer); 
	///<func_info>

	///<func_info>
	//������С���ϳ�ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_IDWT(const cv::Mat& src,cv::Mat& dst,int nLayer); 
	///<func_info>

	///<func_info>
	//������ͳ��ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_GrayStatistics(const cv::Mat& src,cv::Mat& dst_all,cv::Mat& dst_row,cv::Mat& dst_col); 
	///<func_info>

	///<func_info>
	//��������ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Feather(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//����������ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_BrightLight(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//�������߶Աȶ�ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_HighContrast(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//��������ɢͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Spread(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//����������ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Sketch(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//����������Чͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Wind(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//��������ͨ��Чͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Cartoons(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������������ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_BrightStretching(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������Surfͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Surf(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������Meanshiftͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Meanshift(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������Img_Clusteringͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Clustering(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������Foldͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_Fold(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������Gaborͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_GaborFilter(const cv::Mat& src,cv::Mat& dst); 
	///<func_info>

	///<func_info>
	//������ȥ��ͼ��
	//������
	//const cv::Mat& src	Դͼ������
	//cv::Mat& dst			Ŀ��ͼ������			
	//����ֵ����
	static void Img_QuWu(const cv::Mat& src,cv::Mat& dst,int block,double w=8); 
	///<func_info>
};
