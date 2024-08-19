#pragma once
#include "Allinclude.h"
/// <summary>
/// 通信延时模拟类
/// </summary>
class Communication
{
private:
	double delay_normal;//存储正常模拟下通信延时
	double delay_no_noisy;//存储无噪声模拟下的通信延时
	double delay_ideal;//存储理想模拟下的通信延时
	int bandwidth;//带宽
	double velocity;//电磁波传播速度
	int packet_size;//包大小
	int noisy;//噪声配置，1：高斯噪声；2：均匀噪声
	double distribution_left;//噪声区间左端点
	double distribution_right;//噪声区间右端点

	//伪随机数生成器，基于 19937 个元素的周期，适用于生成高质量的随机数
	mt19937 generator;
	//normal_distribution 是用来生成正态分布（高斯分布）随机数的模板类
	normal_distribution<double> gauss_distribution;
	//uniform_real_distribution 是用来生成在指定范围内均匀分布的随机浮点数的模板类
	uniform_real_distribution<double> uniform_distribution;

	/// <summary>
	/// 按照区间生成高斯噪声
	/// </summary>
	/// <param name="a">左端点</param>
	/// <param name="b">右端点</param>
	/// <returns>生成的噪声</returns>
	double generate_gauss_in_range(double a, double b);

	/// <summary>
	/// 按照区间生成均匀噪声
	/// </summary>
	/// <param name="a">左端点</param>
	/// <param name="b">右端点</param>
	/// <returns>生成的噪声</returns>
	double random_in_range(double a, double b);

public:
	KeplerOrbits::GeoCoordinates source;//信息发送节点
	KeplerOrbits::GeoCoordinates target;//信息接收节点
	KeplerOrbits::GeoCoordinates attack;//攻击节点

	Communication(const string& configfile = "ConfigTxt\\communication_config.txt");

	/// <summary>
	/// 获取模拟的通信时延
	/// </summary>
	/// <param name="distance">距离数组</param>
	/// <param name="stationState">各卫星节点受攻击情况</param>
	/// <param name="packet_size">数据包的大小</param>
	/// <param name="bandwidth">带宽</param>
	/// <param name="add_delay">是否记录时延</param>
	/// <returns>时延</returns>
	double communication_stt(vector<int> distance, vector<int> stationState, int packet_size = 0, int bandwidth = -1, bool add_delay = true);

	/// <summary>
	/// 获取无噪声模拟的通信时延
	/// </summary>
	/// <param name="distance">距离数组</param>
	/// <param name="stationState">各卫星节点受攻击情况</param>
	/// <param name="packet_size">数据包的大小</param>
	/// <param name="bandwidth">带宽</param>
	/// <param name="add_delay">是否记录时延</param>
	/// <returns>无噪声时延</returns>
	double communication_stt_no_noisy(vector<int> distance, vector<int> stationState, int packet_size = 0, int bandwidth = -1, bool add_delay = true);

	/// <summary>
	/// 获取理想情况模拟的通信时延
	/// </summary>
	/// <param name="distance">距离数组</param>
	/// <param name="add_delay">是否记录时延</param>
	/// <returns>时延</returns>
	double communication_stt_ideal(vector<int> distance, bool add_delay = true);
};
