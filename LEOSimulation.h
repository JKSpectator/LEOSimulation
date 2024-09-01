#pragma once
#include "Allinclude.h"
#include "Constellation.h"
#ifdef ATTACK_K
#include "AttackSimulation.h"
#endif // ATTACK_K

#define aa  6.378137e4 //6371000
#define f  (1 / 298.257223563)	//扁率
#define e2  (f*(2-f))			//第一偏心率平方

//经纬度
struct BLH
{
	BLH(double b, double l, double h)
	{
		B = b;
		L = l;
		H = h;
	}
	//维度
	double B;
	//经度
	double L;
	//高
	double H;
};

//XYZ坐标
struct Vec3
{
	Vec3(double x1, double y1, double z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
	double x, y, z;
};

/// <summary>
/// 初始化星座和通信模拟器
/// </summary>
int SInit(bool attackIn = true, const string& datafile = "ConfigJSON\\city_data.json", const string& configfile = "ConfigJSON\\config.json", const string& com_configfile = "ConfigJSON\\communication_config.json");

/// <summary>
/// 更新卫星，计算通信连通性，输出包含三种通信仿真的时间组成的数组（正常通信、无噪声通信、理想通信）
/// </summary>
vector<double> com_sim();

/// <summary>
/// 测试初始化星座和通信模拟器
/// </summary>
int TestInit(Constellation cs, Communication comm, AttackSimulation as, const string& datafile = "ConfigTest\\city_data.json", const string& configfile = "ConfigTest\\config.json", const string& com_configfile = "ConfigTest\\communication_config.json");

/// <summary>
/// 测试多次，计算平均FPS和计算通信链路时间
/// </summary>
/// <param name="cs">星座模拟器</param>
/// <param name="comm">通信模拟器</param>
/// <param name="testNum">测试次数</param>
void testT(Constellation cs, Communication comm, int testNum = 1);