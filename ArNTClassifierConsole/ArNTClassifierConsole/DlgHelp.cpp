// DlgHelp.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgHelp.h"


// DlgHelp �Ի���

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


// DlgHelp ��Ϣ�������

void DlgHelp::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	m_pOldFont = (CFont*)dc.SelectObject(&m_newFont);   
    dc.SetBkMode(TRANSPARENT); //���ñ���Ϊ͸����     
    dc.SetTextColor(RGB(255,0,0));// �����ı���ɫΪ��ɫ        
    dc.TextOut(m_nTextX,10,_T("��ӭʹ�����߼��ϵͳ����������̨ʹ�ð����������Ƽ���ѧ����о�Ժ���޹�˾��")); // ��ָ��λ������ı�     
    dc.SelectObject(m_pOldFont);

	//
	LOGFONT logFont;      
    m_newFont.GetLogFont(&logFont);  // ��ȡm_newFont�����LOGFONT�ṹ
	dc.SetTextColor(RGB(0,255,0)); // �����ı���ɫΪ��ɫ 
	m_nTextY=20;
	
	dc.SetTextColor(RGB(0,255,255)); // �����ı���ɫΪ��ɫ
	m_nTextY += abs(logFont.lfHeight);
    dc.TextOut(10,m_nTextY,_T("һ��������ȡģ�飺")); // ��ָ��λ������ı� 
	dc.SetTextColor(RGB(0,255,0)); // �����ı���ɫΪ��ɫ 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("1����Ҫ��ȡȱ�ݵ�ͼ��·��������������·�������������ȱ����𡱰�ť����������ã�ѡ��ָ��㷨��")); // ��ָ��λ������ı�  
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("2�������״�ṹ��Ŀ¼�ҵ�Ҫ�򿪵�ͼƬ�����������Ҳ���ʾͼ�񣬵�����Զ���ȡ���򡱽�ȱ�ݿ�ѡ��ɡ�")); // ��ָ��λ������ı� 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("3��˫��ȱ��ͼ�飬������ѡ���ѡ����Ӧ����𼴿ɽ�ͼƬ���浽��Ӧ������ļ��С�"));
	
	dc.SetTextColor(RGB(0,255,255)); // �����ı���ɫΪ��ɫ
	m_nTextY += abs(logFont.lfHeight); 
    dc.TextOut(10,m_nTextY,_T("�������������ģ�飺")); // ��ָ��λ������ı� 
	dc.SetTextColor(RGB(0,255,0)); // �����ı���ɫΪ��ɫ 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("1������������·����ѡ���Ƿ�淶��������С��ѡ����Լ���ռ�İٷֱȣ��������ͼ��")); // ��ָ��λ������ı� 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("2��ͼ�������ɺ����ʾ�������ڣ�������������ӻ�ɾ�����������ȡ������������ť���������")); // ��ָ��λ������ı�
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("3��ѡ���������������Ӧ�Ĳ����������ѵ�������Է���������ť���з�������ѵ����")); // ��ָ��λ������ı�
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("4��ѵ����ɺ����ʾ����������׼ȷ�ʣ��������ʾ����������ť����ʾͼƬ�ķ�������")); // ��ָ��λ������ı�

	dc.SetTextColor(RGB(0,255,255)); // �����ı���ɫΪ��ɫ
	m_nTextY += abs(logFont.lfHeight);
	dc.TextOut(10,m_nTextY,_T("�������߷���ģ�飺")); // ��ָ��λ������ı� 
	dc.SetTextColor(RGB(0,255,0)); // �����ı���ɫΪ��ɫ 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("1��������������ݿⲢ���ࡱ��ť���ɶ�ȡ���ݽ��з��ࡣ")); // ��ָ��λ������ı� 
	m_nTextY += abs(logFont.lfHeight); 	           
    dc.TextOut(20,m_nTextY,_T("2���ڴ�����ݷ�ʽʱ���ӡ�-Mod classify�����������ֱ�ӽ������߷��໭�棬�������ļ����ÿ�ֱ��ʵ�ִ򿪼����ࡣ")); // ��ָ��λ������ı� 
}

BOOL DlgHelp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_newFont.CreatePointFont(180, _T("����"));
	SetTimer(1,500,NULL); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void DlgHelp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	LOGFONT logFont;      
    m_newFont.GetLogFont(&logFont);  // ��ȡm_newFont�����LOGFONT�ṹ         
    m_nTextX -= 5;// ��m_nTextX��ֵ��5        
    if (m_nTextX < 10)   // ���m_nTextXС��10�����ı�����ӭ���������ס��ص���ʼλ��
	{
		m_nTextX = 1000;   
	}      
    Invalidate();   // ʹ���ڿͻ�����Ч��֮��ͻ��ػ�   

	CDialog::OnTimer(nIDEvent);
}
