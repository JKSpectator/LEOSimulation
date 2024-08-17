#pragma once
#include "Allinclude.h"

class Communication
{
private:
	double delay_normal;
	double delay_no_noisy;
	double delay_ideal;
	int bandwidth;
	bool is_attacked;
	double attack_probability;
	double velocity;
	int packet_size;
	int noisy;
	double distribution_left;
	double distribution_right;

	//伪随机数生成器，基于 19937 个元素的周期，适用于生成高质量的随机数
	mt19937 generator;
	//normal_distribution 是用来生成正态分布（高斯分布）随机数的模板类
	normal_distribution<double> gauss_distribution;
	//uniform_real_distribution 是用来生成在指定范围内均匀分布的随机浮点数的模板类
	uniform_real_distribution<double> uniform_distribution;

	double generate_gauss_in_range(double a, double b);

	double random_in_range(double a, double b);

public:
	Communication(const string& configfile = "ConfigTxt\\communication_config.txt");

	double communication_stt(double distance = 500000.0, int packet_size = 0, int bandwidth = -1, bool is_attacked = false, bool add_delay = true);

	double communication_stt_no_noisy(double distance = 500000.0, int packet_size = 0, int bandwidth = -1, bool add_delay = true);

	double communication_stt_ideal(double distance = 500000.0, bool add_delay = true);
};
