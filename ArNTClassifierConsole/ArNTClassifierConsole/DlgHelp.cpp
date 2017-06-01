// DlgHelp.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgHelp.h"


// DlgHelp 对话框

IMPLEMENT_DYNAMIC(DlgHelp, CDialog)

DlgHelp::DlgHelp(CWnd* pParent /*=NULL*/)
	: CDialog(DlgHelp::IDD, pParent)
{
	m_nTextX = 1000;   
    m_nTextY = 10;   
    m_pOldFont = NULL; 
}

DlgHelp::~DlgHelp()
{
}

void DlgHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgHelp, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgHelp 消息处理程序

void DlgHelp::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	m_pOldFont = (CFont*)dc.SelectObject(&m_newFont);   
    dc.SetBkMode(TRANSPARENT); //设置背景为透明！     
    dc.SetTextColor(RGB(255,0,0));// 设置文本颜色为红色        
    dc.TextOut(m_nTextX,10,_T("欢迎使用在线检测系统分类器控制台使用帮助（北京科技大学设计研究院有限公司）")); // 在指定位置输出文本     
    dc.SelectObject(m_pOldFont);

	//
	LOGFONT logFont;      
    m_newFont.GetLogFont(&logFont);  // 获取m_newFont字体的LOGFONT结构
	dc.SetTextColor(RGB(0,255,0)); // 设置文本颜色为绿色 
	m_nTextY=20;
	
	dc.SetTextColor(RGB(0,255,255)); // 设置文本颜色为绿色
	m_nTextY += abs(logFont.lfHeight);
    dc.TextOut(10,m_nTextY,_T("一、样本提取模块：")); // 在指定位置输出文本 
	dc.SetTextColor(RGB(0,255,0)); // 设置文本颜色为绿色 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("1、打开要提取缺陷的图像路径，打开样本集的路径，点击“配置缺陷类别”按钮进行类别配置，选择分割算法。")); // 在指定位置输出文本  
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("2、点击树状结构的目录找到要打开的图片，单击后在右侧显示图像，点击“自动提取区域”将缺陷框选完成。")); // 在指定位置输出文本 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("3、双击缺陷图块，弹出待选类别，选择相应的类别即可将图片保存到相应的类别文件夹。"));
	
	dc.SetTextColor(RGB(0,255,255)); // 设置文本颜色为绿色
	m_nTextY += abs(logFont.lfHeight); 
    dc.TextOut(10,m_nTextY,_T("二、分类器设计模块：")); // 在指定位置输出文本 
	dc.SetTextColor(RGB(0,255,0)); // 设置文本颜色为绿色 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("1、打开样本集的路径，选择是否规范化样本大小，选择测试集所占的百分比，点击加载图像。")); // 在指定位置输出文本 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("2、图像加载完成后会显示在类别框内，进行特征的添加或删除，点击“提取特征向量”按钮获得特征。")); // 在指定位置输出文本
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("3、选择分类器并设置相应的参数，点击“训练并测试分类器”按钮进行分类器的训练。")); // 在指定位置输出文本
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("4、训练完成后会显示混淆矩阵及总准确率，点击“显示分类结果”按钮可显示图片的分类结果。")); // 在指定位置输出文本

	dc.SetTextColor(RGB(0,255,255)); // 设置文本颜色为绿色
	m_nTextY += abs(logFont.lfHeight);
	dc.TextOut(10,m_nTextY,_T("三、在线分类模块：")); // 在指定位置输出文本 
	dc.SetTextColor(RGB(0,255,0)); // 设置文本颜色为绿色 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("1、点击“连接数据库并分类”按钮，可读取数据进行分类。")); // 在指定位置输出文本 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("2、在创建快捷方式时，加“-Mod classify”命令参数可直接进入在线分类画面，在配置文件配置可直接实现打开即分类。")); // 在指定位置输出文本 
}

BOOL DlgHelp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_newFont.CreatePointFont(180, _T("隶书"));
	SetTimer(1,500,NULL); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgHelp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LOGFONT logFont;      
    m_newFont.GetLogFont(&logFont);  // 获取m_newFont字体的LOGFONT结构         
    m_nTextX -= 5;// 将m_nTextX的值减5        
    if (m_nTextX < 10)   // 如果m_nTextX小于10，则文本“欢迎来到鸡啄米”回到起始位置
	{
		m_nTextX = 1000;   
	}      
    Invalidate();   // 使窗口客户区无效，之后就会重绘   

	CDialog::OnTimer(nIDEvent);
}
