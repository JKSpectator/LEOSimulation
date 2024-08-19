#pragma once
#include "Allinclude.h"
/// <summary>
/// ͨ����ʱģ����
/// </summary>
class Communication
{
private:
	double delay_normal;//�洢����ģ����ͨ����ʱ
	double delay_no_noisy;//�洢������ģ���µ�ͨ����ʱ
	double delay_ideal;//�洢����ģ���µ�ͨ����ʱ
	int bandwidth;//����
	double velocity;//��Ų������ٶ�
	int packet_size;//����С
	int noisy;//�������ã�1����˹������2����������
	double distribution_left;//����������˵�
	double distribution_right;//���������Ҷ˵�

	//α����������������� 19937 ��Ԫ�ص����ڣ����������ɸ������������
	mt19937 generator;
	//normal_distribution ������������̬�ֲ�����˹�ֲ����������ģ����
	normal_distribution<double> gauss_distribution;
	//uniform_real_distribution ������������ָ����Χ�ھ��ȷֲ��������������ģ����
	uniform_real_distribution<double> uniform_distribution;

	/// <summary>
	/// �����������ɸ�˹����
	/// </summary>
	/// <param name="a">��˵�</param>
	/// <param name="b">�Ҷ˵�</param>
	/// <returns>���ɵ�����</returns>
	double generate_gauss_in_range(double a, double b);

	/// <summary>
	/// �����������ɾ�������
	/// </summary>
	/// <param name="a">��˵�</param>
	/// <param name="b">�Ҷ˵�</param>
	/// <returns>���ɵ�����</returns>
	double random_in_range(double a, double b);

public:
	KeplerOrbits::GeoCoordinates source;//��Ϣ���ͽڵ�
	KeplerOrbits::GeoCoordinates target;//��Ϣ���սڵ�
	KeplerOrbits::GeoCoordinates attack;//�����ڵ�

	Communication(const string& configfile = "ConfigTxt\\communication_config.txt");

	/// <summary>
	/// ��ȡģ���ͨ��ʱ��
	/// </summary>
	/// <param name="distance">��������</param>
	/// <param name="stationState">�����ǽڵ��ܹ������</param>
	/// <param name="packet_size">���ݰ��Ĵ�С</param>
	/// <param name="bandwidth">����</param>
	/// <param name="add_delay">�Ƿ��¼ʱ��</param>
	/// <returns>ʱ��</returns>
	double communication_stt(vector<int> distance, vector<int> stationState, int packet_size = 0, int bandwidth = -1, bool add_delay = true);

	/// <summary>
	/// ��ȡ������ģ���ͨ��ʱ��
	/// </summary>
	/// <param name="distance">��������</param>
	/// <param name="stationState">�����ǽڵ��ܹ������</param>
	/// <param name="packet_size">���ݰ��Ĵ�С</param>
	/// <param name="bandwidth">����</param>
	/// <param name="add_delay">�Ƿ��¼ʱ��</param>
	/// <returns>������ʱ��</returns>
	double communication_stt_no_noisy(vector<int> distance, vector<int> stationState, int packet_size = 0, int bandwidth = -1, bool add_delay = true);

	/// <summary>
	/// ��ȡ�������ģ���ͨ��ʱ��
	/// </summary>
	/// <param name="distance">��������</param>
	/// <param name="add_delay">�Ƿ��¼ʱ��</param>
	/// <returns>ʱ��</returns>
	double communication_stt_ideal(vector<int> distance, bool add_delay = true);
};
