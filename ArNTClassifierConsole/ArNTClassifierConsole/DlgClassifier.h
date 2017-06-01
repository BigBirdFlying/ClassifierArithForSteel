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

//�������ṹ��
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


// DlgClassifier �Ի���

class DlgClassifier : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifier)

public:
	DlgClassifier(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgClassifier();

// �Ի�������
	enum { IDD = IDD_DIALOG_CLASSIFIER };

public:
	//�ṹ��
	static SampleImgSet		m_TrainImgSet;									//ѵ����
	static SampleImgSet		m_TestImgSet;									//���Լ�
	ClassifierParam			m_ClassifierParam;								//�������Ĳ���
	//ͼ���б�
	CImageList				m_TrainSetImageList;							//ѵ����ͼ���б�
	CImageList				m_TestSetImageList;								//���Լ�ͼ���б�

	CString					m_SampleSetPathName;							//������·��
	CString					m_strAppPath;									//��������·��
	CString					m_strIniFileName;								//ini�����ļ�·��
	HBITMAP					m_hbitmap;

	//��������ر���
	char					m_cstrClassifierName[128];

	int						m_iSampleSetAllImgNum;							//����������ͼ������
	int						m_ClassIndexNum;									//�������
	int						m_iFeatsDim;									//����ά��
	int						m_iNormWidth;									//ͼ��淶���
	int						m_iNormHeight;									//ͼ��淶�߶�
	int						m_iTrainClassNum[MAX_NUM_CLASS];				//ѵ����ÿ������ͼ������
	int						m_iTestClassNum[MAX_NUM_CLASS];					//���Լ�ÿ������ͼ������
	int						m_iExterToInter[MAX_NUM_CLASS];					//���Լ�ÿ������ͼ������

	int						m_iTestSetBeginIndex;							//���Լ�����ʼ����
	float					m_fTestSetPercent;								//���Լ���ռ�İٷֱ�
	//��������ضԻ���
	DlgClassifierBP			m_dlgClassifierBP;
	DlgClassifierSVM		m_dlgClassifierSVM;
	DlgClassifierTREE		m_dlgClassifierTREE;
	DlgClassifierBoosting	m_dlgClassifierBoosting;
	DlgClassifierCNN		m_dlgClassifierCNN;
	//ͼ����ʾ�Ի���
	DlgImageShow*			m_dlgImageShow;
	CPoint					m_ptMouse;										//���λ��

	//
	HFile_ini				m_FileIniOper;									//�ļ���������

	//
	HThread					m_hDefectFeatAndClassify;
	
	//����
	bool CheckSampleFilePath();												//��������Ƿ����δ����·��
	bool DeleteTree(CString strDirectory);									//�㼶ɾ��Ŀ¼
	void WriteSampleImageToTag(CString strPath,int iInClass);					//����ͼ�񵽽ṹ��
	void ShowSampleImageToList();											//��ʾ����ͼ��ͼ���б�
	void ShowConfusionMatrix(const cv::Mat& confusion_matrix);					//��ʾ��������
	void WriteClassifierParamToIniFile();									//д��������ز�����ini�����ļ�
	void WriteResultToFile(SampleImgSet ImgSet,cv::Mat iLabelClass,cv::Mat iPerClass);

	LRESULT OnMsgFuncDefectClassify(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgFuncDefectClassifyClose(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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


