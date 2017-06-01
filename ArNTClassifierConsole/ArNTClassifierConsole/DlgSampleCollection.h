#pragma once
#include "afxcmn.h"
#include "afxwin.h"
//#include "H_DataTypeDef.h"

//产线名称与枚举类对应表
#define NAME_TO_STR(name)  (#name)
typedef struct  
{                 
    unsigned id;  
    char  *desc;  
}MY_DESCRIBE_COMM_TYPE;                               
char * get_desc_by_id(MY_DESCRIBE_COMM_TYPE *desc,unsigned id);//通过id得到描述信息
int get_id_by_desc(MY_DESCRIBE_COMM_TYPE *desc,char *des);//通过描述信息得到id号
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

//类别号对应ID
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
//索引图片对应图像位置
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
// DlgSampleCollection 对话框

class DlgSampleCollection : public CDialog
{
	DECLARE_DYNAMIC(DlgSampleCollection)

public:
	DlgSampleCollection(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgSampleCollection();

// 对话框数据
	enum { IDD = IDD_DIALOG_SAMPLE_COLLECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

//
public:
	//变量
	CImageList				m_treeImageList;							//树图标列表
	CImageList				m_sampleImageList;							//样本图片列表
	HBITMAP					m_hbitmap;									//公司图标的位图
	CMenu					m_menuAddDefect;							//样本区域双击后的弹出菜单
	//树相关
	HICON					m_hIconTree[8];								//图标句柄数组 
	HTREEITEM				m_hRoot[5];									//树节点	
	int						m_treeNum;									//
	bool					m_IsUnfold;									//是否展开
	//文件路径
	CString					m_ImgTreePathName;							//加载图像路径名
	CString					m_SampleSetPathName;						//样本集路径名
	//图像
	cv::Mat					m_grayImage;								//单张灰度图像
	cv::Mat					m_colorImage;								//单张彩色图像
	//结构体	
	NumToClassSet			m_NumToClassSet;							//类别与ID的对应关系
	IndexToSitImgSet		m_IndexToSitImgSet;							//缺陷索引与位置的对应关系
	//
	ConfigInfo_Segmentation		m_ConfigInfo_Segmentation;				//DLL文件加载的配置文件信息
	//鼠标
	CRect						m_ImgRegion;
	int							m_iMouseMode;
	bool						m_bIsEffectMove;
	CPoint						m_ptBegin,m_ptEnd;
	//函数
	void FindFileToTree(LPCTSTR lpszFileName,int iIndex);				//加载bmp文件到树列表里
	void ShowImgToMFCFromPath(int ID,LPCTSTR lpszImgPathName);			//从路径获取图像到控件
	void ShowImgToMFCFormMat(int ID,cv::Mat src);						//从Mat图像到控件
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
