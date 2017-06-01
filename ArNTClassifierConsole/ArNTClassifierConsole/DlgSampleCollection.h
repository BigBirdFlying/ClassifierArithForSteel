#pragma once
#include "afxcmn.h"
#include "afxwin.h"
//#include "H_DataTypeDef.h"

//����������ö�����Ӧ��
#define NAME_TO_STR(name)  (#name)
typedef struct  
{                 
    unsigned id;  
    char  *desc;  
}MY_DESCRIBE_COMM_TYPE;                               
char * get_desc_by_id(MY_DESCRIBE_COMM_TYPE *desc,unsigned id);//ͨ��id�õ�������Ϣ
int get_id_by_desc(MY_DESCRIBE_COMM_TYPE *desc,char *des);//ͨ��������Ϣ�õ�id��
#define MY_DECRIBE_DECLARE_BEGIN(type) static MY_DESCRIBE_COMM_TYPE  MY_describe_desc_##type[]={   
#define MY_DECRIBE_DECLARE_END  {0xffff,"end"} };  
#define MY_DECRIBE_ADD_DESC(id,str) {id,#str},    
#define MY_DESCRIBE_BYID(type,id) get_desc_by_id(MY_describe_desc_##type,id) 
#define MY_DESCRIBE_BYDESC(type,desc) get_id_by_desc(MY_describe_desc_##type,desc)   
#define LOAD_PRODUCTLINE(ProductLine) MY_DECRIBE_DECLARE_BEGIN(ProductLine)\
	MY_DECRIBE_ADD_DESC(TY_Common_0X,TY_Common_0X)\
	MY_DECRIBE_ADD_DESC(BP_ChongGang_1X,BP_ChongGang_1X)\
	MY_DECRIBE_ADD_DESC(BP_ChongGang_2X,BP_ChongGang_2X)\
	MY_DECRIBE_ADD_DESC(BP_ChongGang_3X,BP_ChongGang_3X)\
	MY_DECRIBE_ADD_DESC(BP_BeiHaiChengDe_1X,BP_BeiHaiChengDe_1X)\
	MY_DECRIBE_ADD_DESC(BP_BeiHaiChengDe_2X,BP_BeiHaiChengDe_2X)\
	MY_DECRIBE_ADD_DESC(BP_LiuGang_1X,BP_LiuGang_1X)\
	MY_DECRIBE_ADD_DESC(KHB_NanGang_5000X,KHB_NanGang_5000X)\
	MY_DECRIBE_ADD_DESC(KHB_XiangTan_3800X,KHB_XiangTan_3800X)\
	MY_DECRIBE_ADD_DESC(KHB_XiangTan_5000X,KHB_XiangTan_5000X)\
	MY_DECRIBE_ADD_DESC(KHB_ChongGang_4100X,KHB_ChongGang_4100X)\
	MY_DECRIBE_ADD_DESC(KHB_XingChengTeGang_1X,KHB_XingChengTeGang_1X)\
	MY_DECRIBE_ADD_DESC(RZ_ChongGang_1780X,RZ_ChongGang_1780X)\
	MY_DECRIBE_ADD_DESC(RZ_DingXin_1780X,RZ_DingXin_1780X)\
	MY_DECRIBE_ADD_DESC(RZ_DingXin_850X,RZ_DingXin_850X)\
	MY_DECRIBE_ADD_DESC(RZ_LiuGang_1450X,RZ_LiuGang_1450X)\
	MY_DECRIBE_ADD_DESC(GR_BeiHaiChengDe_1X,GR_BeiHaiChengDe_1X)\
	MY_DECRIBE_ADD_DESC(GR_BeiHaiChengDe_2X,GR_BeiHaiChengDe_2X)\
	MY_DECRIBE_ADD_DESC(GR_BeiHaiChengDe_3X,GR_BeiHaiChengDe_3X)\
	MY_DECRIBE_ADD_DESC(LZ_TianChengBuXiu_1X,LZ_TianChengBuXiu_1X)\
	MY_DECRIBE_ADD_DESC(LB_XiNanLv_1X,LB_XiNanLv_1X)\
	MY_DECRIBE_ADD_DESC(LB_XinJiangZhongHe_1X,LB_XinJiangZhongHe_1X)\
	MY_DECRIBE_ADD_DESC(BC_HuaiGangBangCai_1X,BC_HuaiGangBangCai_1X)\
	MY_DECRIBE_ADD_DESC(TY_Common_999X,TY_Common_999X)\
	MY_DECRIBE_DECLARE_END 

//���Ŷ�ӦID
typedef struct tagNumToClass
{
	int iId;
	CString strClass;

}NumToClass, *PNumToClass;
typedef struct tagNumToClassSet
{
	enum {MAX_ITEMNUM = 1024};
	NumToClass Items[MAX_ITEMNUM];
	int iItemNum;
}NumToClassSet, *PNumToClassSet;
//����ͼƬ��Ӧͼ��λ��
typedef struct tagIndexToSitImg
{
	int iIndex;
	cv::Rect rect;

}IndexToSitImg, *PIndexToSitImg;
typedef struct tagIndexToSitImgSet
{
	enum {MAX_ITEMNUM = 1024};
	IndexToSitImg Items[MAX_ITEMNUM];
	int iItemNum;
}IndexToSitImgSet, *PIndexToSitImgSet;
// DlgSampleCollection �Ի���

class DlgSampleCollection : public CDialog
{
	DECLARE_DYNAMIC(DlgSampleCollection)

public:
	DlgSampleCollection(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgSampleCollection();

// �Ի�������
	enum { IDD = IDD_DIALOG_SAMPLE_COLLECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

//
public:
	//����
	CImageList				m_treeImageList;							//��ͼ���б�
	CImageList				m_sampleImageList;							//����ͼƬ�б�
	HBITMAP					m_hbitmap;									//��˾ͼ���λͼ
	CMenu					m_menuAddDefect;							//��������˫����ĵ����˵�
	//�����
	HICON					m_hIconTree[8];								//ͼ�������� 
	HTREEITEM				m_hRoot[5];									//���ڵ�	
	int						m_treeNum;									//
	bool					m_IsUnfold;									//�Ƿ�չ��
	//�ļ�·��
	CString					m_ImgTreePathName;							//����ͼ��·����
	CString					m_SampleSetPathName;						//������·����
	//ͼ��
	cv::Mat					m_grayImage;								//���ŻҶ�ͼ��
	cv::Mat					m_colorImage;								//���Ų�ɫͼ��
	//�ṹ��	
	NumToClassSet			m_NumToClassSet;							//�����ID�Ķ�Ӧ��ϵ
	IndexToSitImgSet		m_IndexToSitImgSet;							//ȱ��������λ�õĶ�Ӧ��ϵ
	//
	ConfigInfo_Segmentation		m_ConfigInfo_Segmentation;				//DLL�ļ����ص������ļ���Ϣ
	//���
	CRect						m_ImgRegion;
	int							m_iMouseMode;
	bool						m_bIsEffectMove;
	CPoint						m_ptBegin,m_ptEnd;
	//����
	void FindFileToTree(LPCTSTR lpszFileName,int iIndex);				//����bmp�ļ������б���
	void ShowImgToMFCFromPath(int ID,LPCTSTR lpszImgPathName);			//��·����ȡͼ�񵽿ؼ�
	void ShowImgToMFCFormMat(int ID,cv::Mat src);						//��Matͼ�񵽿ؼ�
public:
	CTreeCtrl m_TreeImagePath;
	CListCtrl m_ListImgSample;
	CComboBox m_comboBoxSelectSegArith;
	CListBox m_listBoxShowLogInfo;
	CStatic   m_Logo;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonOpendir();
	afx_msg void OnTvnSelchangedTreeImagepath(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAutoseg();	
	afx_msg void OnLvnItemActivateListImgsample(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSamplesetPath();	
	afx_msg void OnBnClickedButtonConfigdefectclass();	
	CComboBox m_comboTestImageProcessFunc;
	afx_msg void OnCbnSelchangeComboTestFunc();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
