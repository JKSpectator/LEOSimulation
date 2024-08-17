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

	//α����������������� 19937 ��Ԫ�ص����ڣ����������ɸ������������
	mt19937 generator;
	//normal_distribution ������������̬�ֲ�����˹�ֲ����������ģ����
	normal_distribution<double> gauss_distribution;
	//uniform_real_distribution ������������ָ����Χ�ھ��ȷֲ��������������ģ����
	uniform_real_distribution<double> uniform_distribution;

	double generate_gauss_in_range(double a, double b);

	double random_in_range(double a, double b);

public:
	Communication(const string& configfile = "ConfigTxt\\communication_config.txt");

	double communication_stt(double distance = 500000.0, int packet_size = 0, int bandwidth = -1, bool is_attacked = false, bool add_delay = true);

	double communication_stt_no_noisy(double distance = 500000.0, int packet_size = 0, int bandwidth = -1, bool add_delay = true);

	double communication_stt_ideal(double distance = 500000.0, bool add_delay = true);
};
