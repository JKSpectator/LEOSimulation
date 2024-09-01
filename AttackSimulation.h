#pragma once
#include "Allinclude.h"
class AttackSimulation
{
public:
	//攻击节点的地理坐标
	KeplerOrbits::GeoCoordinates attack;
	//攻击节点的攻击范围，一般不设置，将攻击节点等同于一个地面站，通过攻击角度限制攻击范围
	double comR;
	/// <summary>
	/// 初始化函数
	/// </summary>
	/// <param name="configfile">配置文件的地址</param>
	AttackSimulation(const string& configfile = "ConfigJSON\\communication_config.json");
};

