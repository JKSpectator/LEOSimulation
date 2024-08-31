#include "DelaySimulation.h"

double Communication::generate_gauss_in_range(double a, double b) 
{
	double mean = (a + b) / 2.0;
	double std_dev = (b - a) / 6.0;
	gauss_distribution = normal_distribution<double>(mean, std_dev);
	double num;
	do 
	{
		num = gauss_distribution(generator);
	} while (num < a || num > b);
	return num;
}

double Communication::random_in_range(double a, double b)
{
	uniform_distribution = uniform_real_distribution<double>(0, 1);
	return a + (b - a) * uniform_distribution(generator);
}

Communication::Communication(const std::string& configfile)
	: delay_normal(0), delay_no_noisy(0), delay_ideal(0),
	bandwidth(0),velocity(3e8), packet_size(12000), noisy(0),
	distribution_left(0), distribution_right(1), generator(std::random_device{}())
{
	std::ifstream config(configfile);
	//判断配置文件是否已打开
	if (!config.is_open()) {
		std::cerr << "无法打开文件" << std::endl;
		return;
	}
	// 读取文件内容到字符串
	std::string content((std::istreambuf_iterator<char>(config)), std::istreambuf_iterator<char>());
	config.close();
	// 解析JSON字符串
	try {
		json j = json::parse(content);
		bandwidth = j["bandwidth"]["value"].get<int>();
		velocity = j["velocity"]["value"].get<int>();
		packet_size = j["packet_size"].get<int>();
		noisy = j["noisy"]["type"].get<int>();
		distribution_left = j["distribution_left"].get<double>();
		distribution_right = j["distribution_right"].get<double>();
		source = KeplerOrbits::GeoCoordinates(j["Source"]["lat"].get<double>(), j["Source"]["lon"].get<double>(), 0);
		target = KeplerOrbits::GeoCoordinates(j["Target"]["lat"].get<double>(), j["Target"]["lon"].get<double>(), 0);
	}
	catch (json::parse_error& e) {
		std::cerr << "communication_config.json解析错误: " << e.what() << std::endl;
	}
}

double Communication::communication_stt(vector<int> distance, vector<int> stationState, int packet_size, int bandwidth, bool add_delay)
{
	if (packet_size == 0) {
		packet_size = this->packet_size;
	}

	if (bandwidth == -1) {
		bandwidth = this->bandwidth;
	}

	if (this->noisy != 0) {
		if (this->noisy == 1) {
			bandwidth = generate_gauss_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
		}
		else if (this->noisy == 2) {
			bandwidth = random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
		}
	}

	double delay = 0.0;
	double send_delay = static_cast<double>(packet_size) / bandwidth;//static_cast<double>(packet_size) * stationState.size() / bandwidth;
	for (int i = 0; i < stationState.size(); i++)
	{
		send_delay += static_cast<double>(packet_size) / random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
	}

	double propagation_delay = distance[0] / this->velocity;
	delay = send_delay + propagation_delay;

	if (add_delay) {
		this->delay_normal += delay;
	}

	return delay;
}

double Communication::communication_stt_no_noisy(vector<int> distance, vector<int> stationState,  int packet_size, int bandwidth, bool add_delay)
{
	if (packet_size == 0) {
		packet_size = this->packet_size;
	}

	if (bandwidth == -1) {
		bandwidth = this->bandwidth;
	}

	double delay = 0.0;
	double send_delay = static_cast<double>(packet_size) / bandwidth;//static_cast<double>(packet_size) * stationState.size() / bandwidth;
	for (int i = 0; i < stationState.size(); i++)
	{
		send_delay += static_cast<double>(packet_size) / random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
	}
	double propagation_delay = distance[0] / this->velocity;
	delay = send_delay + propagation_delay;

	if (add_delay) {
		this->delay_normal += delay;
	}

	return delay;
}

double Communication::communication_stt_ideal(vector<int> distance, bool add_delay)
{

	double delay = 0.0;
	double propagation_delay = distance[0] / this->velocity;
	delay = propagation_delay;

	if (add_delay) {
		this->delay_normal += delay;
	}

	return delay;
}
