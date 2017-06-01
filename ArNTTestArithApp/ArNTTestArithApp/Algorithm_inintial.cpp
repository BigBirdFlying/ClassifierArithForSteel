#include "Algorithm_initial.h"
//////////////////////////////////////////////////////////////////////////
//成员变量赋初始值

//////////////////////////////////////////////////////////////////////////
//构造函数
DefectsMergeXML::DefectsMergeXML()
{	
	//debug = false;//状态转换,true的时候会包含一些core函数内部的图像显示
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
	CCommonFunc::GetAppPath(m_strAppPath,STR_LEN(m_strAppPath));//获取当前程序路径
	CCommonFunc::SafeStringPrintf(m_strConfigFile,STR_LEN(m_strConfigFile),L"%s\\ArNT_Algorithm_Config.xml",this->m_strAppPath);//获取当前xml文档全路径名
	if(m_XMLOperator.LoadXML(m_strConfigFile) == ArNT_FALSE)
	{
		CCommonFunc::ErrorBox(L"xml文件读取失败了");
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#缺陷数量控制#数量下限", strValue))
	{
		iItemNumFloor = _wtoi(strValue);//默认值为30.
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#缺陷数量控制#数量上限", strValue))
	{
		iItemNumCeiling = _wtoi(strValue);//默认值为50
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#缺陷数量控制#最大区块面积", strValue))
	{
		iMaxDefectSize = _wtof(strValue);//默认值为10.0
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#缺陷数量控制#区块长宽比", strValue))
	{
		iMaxXYRatio = _wtof(strValue);//默认值为0.3
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#缺陷合并条件#宽度最大距离", strValue))
	{
		XNeighbors = _wtoi(strValue);//默认值为30
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#缺陷合并条件#高度最大距离", strValue))
	{
		YNeighbors = _wtoi(strValue);//默认值为150
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#MCF控制信息#MCF训练块宽度", strValue))
	{
		MCF_width = _wtoi(strValue);//64
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#MCF控制信息#MCF训练块高度", strValue))
	{
		MCF_hight = _wtoi(strValue);//128
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定#无效图像判定阈值", strValue))
	{
		Useless_threshold = _wtoi(strValue);//128
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定#是否进行钢板头尾切除", strValue))
	{
		HeadandTail_Removed = _wtoi(strValue);//128
	}
	//无效图像
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#启用判断功能", strValue))
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
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#判定选择列#左列", strValue))
	{
		m_CamNoUseArea.iCol_Left  = _wtoi(strValue);//50
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#判定选择列#中列", strValue))
	{
		m_CamNoUseArea.iCol_Medium  = _wtoi(strValue);//2048
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#判定选择列#右列", strValue))
	{
		m_CamNoUseArea.iCol_Right  = _wtoi(strValue);//4056
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#图像上部选择像素数量", strValue))
	{
		m_CamNoUseArea.iTop_Lenght  = _wtoi(strValue);//100
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#图像下部选择像素数量", strValue))
	{
		m_CamNoUseArea.iBottom_Lenght  = _wtoi(strValue);//100
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#无效图像判定OpenCV版#无效图像判定阈值", strValue))
	{
		m_CamNoUseArea.fNoUse_threshold  = _wtof(strValue);//5.0
	}
	//判断头尾
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#判断钢板图像头部和尾部#启用判断功能", strValue))
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
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#判断钢板图像头部和尾部#图像左右列移动距离", strValue))
	{
		m_HeadTailJudge.iCross = _wtoi(strValue);//50
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#判断钢板图像头部和尾部#列上下左右所取长度", strValue))
	{
		m_HeadTailJudge.iLenght  = _wtoi(strValue);//100
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#判断钢板图像头部和尾部#方差判断阈值", strValue))
	{
		m_HeadTailJudge.fThreshold  = _wtof(strValue);//3
	}
	///
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#钢板边界自动识别时切除边界#启用切除功能", strValue))
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
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#钢板边界自动识别时切除边界#切除长度", strValue))
	{
		m_EdgeAutoCut.iCut_Lenght  = _wtoi(strValue);//20
	}
	///
	m_CamEffectAreaSet.iCamNum=0;
	while(ArNT_TRUE)
	{
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"缺陷检测算法配置信息#相机有效图像区域设置#相机%d", m_CamEffectAreaSet.iCamNum + 1);
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
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"缺陷检测算法配置信息#相机有效图像区域设置#相机%d#硬设置", i + 1);
		if(m_XMLOperator.GetValueByString(strName, strValue))
		{
			m_CamEffectAreaSet.iCamEffectArea[i].iHardEdge= _wtoi(strValue);
		}
		CCommonFunc::SafeStringPrintf(strName, STR_LEN(strName), L"缺陷检测算法配置信息#相机有效图像区域设置#相机%d#软设置", i + 1);
		if(m_XMLOperator.GetValueByString(strName, strValue))
		{
			m_CamEffectAreaSet.iCamEffectArea[i].iSoftEdge= _wtoi(strValue);
		}
	}

	///
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#钢板结疤缺陷检测#启用结疤检测功能", strValue))
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
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#钢板结疤缺陷检测#边部不检宽度", strValue))
	{
		m_Defects_Scarring.iNoUse_Width  = _wtoi(strValue);//32
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#钢板结疤缺陷检测#结疤检测的边部宽度范围", strValue))
	{
		m_Defects_Scarring.iEffect_Width  = _wtoi(strValue);//64
	}
	if(m_XMLOperator.GetValueByString(L"缺陷检测算法配置信息#钢板结疤缺陷检测#置信度阈值", strValue))
	{
		m_Defects_Scarring.iConf_Threshold  = _wtoi(strValue);//300
	}
}
DefectsMergeXML::~DefectsMergeXML(){}