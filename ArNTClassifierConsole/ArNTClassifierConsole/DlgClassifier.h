#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "DlgClassifierBP.h"
#include "DlgClassifierSVM.h"
#include "DlgClassifierTREE.h"
#include "DlgClassifierBoosting.h"
#include "DlgClassifierCNN.h"
#include "ImgProcessPrimary.h"
#include "ImgProcessClassifier.h"
#include "DlgImageShow.h"

#include "HFile.h"
#include "HThread.h"

#define WM_THREAD_DEFECT_CLASSIFY WM_USER+16
#define WM_THREAD_DEFECT_CLASSIFY_CLOSE WM_USER+18

//样本集结构体
typedef struct tagSampleImg
{
	int iInternalClassNo;
	int iExternalClassNo;
	cv::Mat img;
	vector<float> feat;
}SampleImg,*PSampleImg;
typedef struct tagSampleImgSet
{
	enum {MAX_ITEMNUM = 10000};
	SampleImg Items[MAX_ITEMNUM];
	int iItemNum;
}SampleImgSet,*PSampleImgSet;


// DlgClassifier 对话框

class DlgClassifier : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifier)

public:
	DlgClassifier(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgClassifier();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSIFIER };

public:
	//结构体
	static SampleImgSet		m_TrainImgSet;									//训练集
	static SampleImgSet		m_TestImgSet;									//测试集
	ClassifierParam			m_ClassifierParam;								//分类器的参数
	//图像列表
	CImageList				m_TrainSetImageList;							//训练集图像列表
	CImageList				m_TestSetImageList;								//测试集图像列表

	CString					m_SampleSetPathName;							//样本集路径
	CString					m_strAppPath;									//程序所在路径
	CString					m_strIniFileName;								//ini配置文件路径
	HBITMAP					m_hbitmap;

	//分类器相关变量
	char					m_cstrClassifierName[128];

	int						m_iSampleSetAllImgNum;							//样本集的总图像数量
	int						m_ClassIndexNum;									//类别数量
	int						m_iFeatsDim;									//特征维度
	int						m_iNormWidth;									//图像规范宽度
	int						m_iNormHeight;									//图像规范高度
	int						m_iTrainClassNum[MAX_NUM_CLASS];				//训练集每个类别的图像数量
	int						m_iTestClassNum[MAX_NUM_CLASS];					//测试集每个类别的图像数量
	int						m_iExterToInter[MAX_NUM_CLASS];					//测试集每个类别的图像数量

	int						m_iTestSetBeginIndex;							//测试集的起始索引
	float					m_fTestSetPercent;								//测试集所占的百分比
	//分类器相关对话框
	DlgClassifierBP			m_dlgClassifierBP;
	DlgClassifierSVM		m_dlgClassifierSVM;
	DlgClassifierTREE		m_dlgClassifierTREE;
	DlgClassifierBoosting	m_dlgClassifierBoosting;
	DlgClassifierCNN		m_dlgClassifierCNN;
	//图像显示对话框
	DlgImageShow*			m_dlgImageShow;
	CPoint					m_ptMouse;										//鼠标位置

	//
	HFile_ini				m_FileIniOper;									//文件操作对象

	//
	HThread					m_hDefectFeatAndClassify;
	
	//函数
	bool CheckSampleFilePath();												//检查样本是否存在未配置路径
	bool DeleteTree(CString strDirectory);									//层级删除目录
	void WriteSampleImageToTag(CString strPath,int iInClass);					//样本图像到结构体
	void ShowSampleImageToList();											//显示样本图像到图像列表
	void ShowConfusionMatrix(const cv::Mat& confusion_matrix);					//显示混淆矩阵
	void WriteClassifierParamToIniFile();									//写分类器相关参数到ini配置文件
	void WriteResultToFile(SampleImgSet ImgSet,cv::Mat iLabelClass,cv::Mat iPerClass);

	LRESULT OnMsgFuncDefectClassify(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgFuncDefectClassifyClose(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	CButton m_checkNorm;
	CButton m_checkBoxAutoGetFeat;
	CListBox m_listBoxFeatFrom;
	CListBox m_listBoxFeatTo;
	CTabCtrl m_tabCtrlClassifier;	
	CComboBox m_ComboImgNormSize;
	CComboBox m_camboTestSetPercent;
	CComboBox m_comboTrainClassNum;
	CComboBox m_comboTestClassNum;
	CListCtrl m_listTrainImgShow;
	CListCtrl m_listTestImgShow;
	CListBox m_ListShowInfo;
	CStatic m_staticResultShow;
	CStatic m_picResuleShow;
	CEdit m_editResultAccuracy;
	CProgressCtrl m_ProgressLoadingSampleImg;

	afx_msg void OnBnClickedButtonGetImgsamplepath();	
	afx_msg void OnBnClickedButtonLoadingSampleimg();	
	afx_msg void OnBnClickedButtonFeatAdd();
	afx_msg void OnBnClickedButtonFeatSub();
	afx_msg void OnBnClickedCheckAutoGetFeat();
	afx_msg void OnBnClickedButtonGetFeat();	
	afx_msg void OnTcnSelchangeTabClassifier(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnCbnSelchangeComboImgsize();
	afx_msg void OnCbnSelchangeComboTestsetPercent();
	afx_msg void OnCbnSelchangeComboTrainClassnum();
	afx_msg void OnCbnSelchangeComboTestClassnum();
	afx_msg void OnBnClickedButtonTrainClassifier();
	afx_msg void OnBnClickedButtonTestClassifier();	
	afx_msg void OnBnClickedButtonTrainAndTestClassifier();
	afx_msg void OnLvnItemActivateListTrainimgshow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonShowclassifyResult();
};


