#include "Algorithm_initial.h"
//////////////////////////////////////////////////////////////////////////
//��Ա��������ʼֵ

//////////////////////////////////////////////////////////////////////////
//���캯��
DefectsMergeXML::DefectsMergeXML()
{	
	//debug = false;//״̬ת��,true��ʱ������һЩcore�����ڲ���ͼ����ʾ
	//////////////////////////////////////////////////////////////////////////
	iItemNumCeiling = 0;
	iItemNumFloor = 0;
	iMaxXYRatio = 0;
	iMaxDefectSize = 0;
	XNeighbors = 0;
	YNeighbors = 0;
	MCF_width=32;
	MCF_hight=32;
	Useless_threshold=10000;
	HeadandTail_Removed=1;

	LONGSTR strValue = {0};	
	LONGSTR strName = {0};
	CCommonFunc::GetAppPath(m_strAppPath,STR_LEN(m_strAppPath));//��ȡ��ǰ����·��
	CCommonFunc::SafeStringPrintf(m_strConfigFile,STR_LEN(m_strConfigFile),L"%s\\ArNT_Algorithm_Config.xml",this->m_strAppPath);//��ȡ��ǰxml�ĵ�ȫ·����
	if(m_XMLOperator.LoadXML(m_strConfigFile) == ArNT_FALSE)
	{
		CCommonFunc::ErrorBox(L"xml�ļ���ȡʧ����");
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#ȱ����������#��������", strValue))
	{
		iItemNumFloor = _wtoi(strValue);//Ĭ��ֵΪ30.
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#ȱ����������#��������", strValue))
	{
		iItemNumCeiling = _wtoi(strValue);//Ĭ��ֵΪ50
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#ȱ����������#����������", strValue))
	{
		iMaxDefectSize = _wtof(strValue);//Ĭ��ֵΪ10.0
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#ȱ����������#���鳤���", strValue))
	{
		iMaxXYRatio = _wtof(strValue);//Ĭ��ֵΪ0.3
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#ȱ�ݺϲ�����#���������", strValue))
	{
		XNeighbors = _wtoi(strValue);//Ĭ��ֵΪ30
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#ȱ�ݺϲ�����#�߶�������", strValue))
	{
		YNeighbors = _wtoi(strValue);//Ĭ��ֵΪ150
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#MCF������Ϣ#MCFѵ������", strValue))
	{
		MCF_width = _wtoi(strValue);//64
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#MCF������Ϣ#MCFѵ����߶�", strValue))
	{
		MCF_hight = _wtoi(strValue);//128
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�#��Чͼ���ж���ֵ", strValue))
	{
		Useless_threshold = _wtoi(strValue);//128
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�#�Ƿ���иְ�ͷβ�г�", strValue))
	{
		HeadandTail_Removed = _wtoi(strValue);//128
	}
	//��Чͼ��
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#�����жϹ���", strValue))
	{
		if(_wtoi(strValue) > 0) 
		{
			m_CamNoUseArea.bEnable = ArNT_TRUE;
		}
		else
		{
			m_CamNoUseArea.bEnable  = ArNT_FALSE;
		}
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#�ж�ѡ����#����", strValue))
	{
		m_CamNoUseArea.iCol_Left  = _wtoi(strValue);//50
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#�ж�ѡ����#����", strValue))
	{
		m_CamNoUseArea.iCol_Medium  = _wtoi(strValue);//2048
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#�ж�ѡ����#����", strValue))
	{
		m_CamNoUseArea.iCol_Right  = _wtoi(strValue);//4056
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#ͼ���ϲ�ѡ����������", strValue))
	{
		m_CamNoUseArea.iTop_Lenght  = _wtoi(strValue);//100
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#ͼ���²�ѡ����������", strValue))
	{
		m_CamNoUseArea.iBottom_Lenght  = _wtoi(strValue);//100
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#��Чͼ���ж�OpenCV��#��Чͼ���ж���ֵ", strValue))
	{
		m_CamNoUseArea.fNoUse_threshold  = _wtof(strValue);//5.0
	}
	//�ж�ͷβ
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�жϸְ�ͼ��ͷ����β��#�����жϹ���", strValue))
	{
		if(_wtoi(strValue) > 0) 
		{
			m_HeadTailJudge.bEnable = ArNT_TRUE;
		}
		else
		{
			m_HeadTailJudge.bEnable  = ArNT_FALSE;
		}
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�жϸְ�ͼ��ͷ����β��#ͼ���������ƶ�����", strValue))
	{
		m_HeadTailJudge.iCross = _wtoi(strValue);//50
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�жϸְ�ͼ��ͷ����β��#������������ȡ����", strValue))
	{
		m_HeadTailJudge.iLenght  = _wtoi(strValue);//100
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�жϸְ�ͼ��ͷ����β��#�����ж���ֵ", strValue))
	{
		m_HeadTailJudge.fThreshold  = _wtof(strValue);//3
	}
	///
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�ְ�߽��Զ�ʶ��ʱ�г��߽�#�����г�����", strValue))
	{
			if(_wtoi(strValue) > 0) 
			{
				m_EdgeAutoCut.bEnable = ArNT_TRUE;
			}
			else
			{
				m_EdgeAutoCut.bEnable  = ArNT_FALSE;
			}
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�ְ�߽��Զ�ʶ��ʱ�г��߽�#�г�����", strValue))
	{
		m_EdgeAutoCut.iCut_Lenght  = _wtoi(strValue);//20
	}
	///
	m_CamEffectAreaSet.iCamNum=0;
	while(ArNT_TRUE)
	{
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ȱ�ݼ���㷨������Ϣ#�����Чͼ����������#���%d", m_CamEffectAreaSet.iCamNum + 1);
		if(m_XMLOperator.GetValueByString(strName, strValue))
		{
			m_CamEffectAreaSet.iCamNum++;
		}else
		{
			break;
		}
	}
	for(ShortInt i = 0; i < m_CamEffectAreaSet.iCamNum; i++)
	{
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ȱ�ݼ���㷨������Ϣ#�����Чͼ����������#���%d#Ӳ����", i + 1);
		if(m_XMLOperator.GetValueByString(strName, strValue))
		{
			m_CamEffectAreaSet.iCamEffectArea[i].iHardEdge= _wtoi(strValue);
		}
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"ȱ�ݼ���㷨������Ϣ#�����Чͼ����������#���%d#������", i + 1);
		if(m_XMLOperator.GetValueByString(strName, strValue))
		{
			m_CamEffectAreaSet.iCamEffectArea[i].iSoftEdge= _wtoi(strValue);
		}
	}

	///
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�ְ���ȱ�ݼ��#���ý�̼�⹦��", strValue))
	{
			if(_wtoi(strValue) > 0) 
			{
				m_Defects_Scarring.bEnable = ArNT_TRUE;
			}
			else
			{
				m_Defects_Scarring.bEnable  = ArNT_FALSE;
			}
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�ְ���ȱ�ݼ��#�߲�������", strValue))
	{
		m_Defects_Scarring.iNoUse_Width  = _wtoi(strValue);//32
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�ְ���ȱ�ݼ��#��̼��ı߲���ȷ�Χ", strValue))
	{
		m_Defects_Scarring.iEffect_Width  = _wtoi(strValue);//64
	}
	if(m_XMLOperator.GetValueByString(L"ȱ�ݼ���㷨������Ϣ#�ְ���ȱ�ݼ��#���Ŷ���ֵ", strValue))
	{
		m_Defects_Scarring.iConf_Threshold  = _wtoi(strValue);//300
	}
}
DefectsMergeXML::~DefectsMergeXML(){}