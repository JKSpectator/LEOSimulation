#pragma once
#include "Allinclude.h"
class AttackSimulation
{
public:
	//�����ڵ�ĵ�������
	KeplerOrbits::GeoCoordinates attack;
	//�����ڵ�Ĺ�����Χ��һ�㲻���ã��������ڵ��ͬ��һ������վ��ͨ�������Ƕ����ƹ�����Χ
	double comR;
	/// <summary>
	/// ��ʼ������
	/// </summary>
	/// <param name="configfile">�����ļ��ĵ�ַ</param>
	AttackSimulation(const string& configfile = "ConfigJSON\\communication_config.json");
};

