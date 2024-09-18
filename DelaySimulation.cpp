#include "DelaySimulation.h"  // 包含延迟模拟的头文件

// 生成指定范围内的高斯分布随机数
double Communication::generate_gauss_in_range(double a, double b)
{
	double mean = (a + b) / 2.0;  // 计算均值
	double std_dev = (b - a) / 6.0;  // 计算标准差
	gauss_distribution = normal_distribution<double>(mean, std_dev);  // 创建高斯分布
	double num;
	do
	{
		num = gauss_distribution(generator);  // 生成随机数
	} while (num < a || num > b);  // 确保随机数在指定范围内
	return num;
}

// 生成指定范围内的均匀分布随机数
double Communication::random_in_range(double a, double b)
{
	uniform_distribution = uniform_real_distribution<double>(0, 1);  // 创建均匀分布
	return a + (b - a) * uniform_distribution(generator);  // 生成随机数
}

// 构造函数，从配置文件加载通信参数
Communication::Communication(const std::string& configfile)
	: delay_normal(0), delay_no_noisy(0), delay_ideal(0),
	bandwidth(0), velocity(3e8), packet_size(12000), noisy(0),
	distribution_left(0), distribution_right(1), generator(std::random_device{}())
{
	std::ifstream config(configfile);  // 打开配置文件
	// 判断配置文件是否已打开
	if (!config.is_open()) {
		std::cerr << "无法打开文件" << std::endl;
		return;
	}
	// 读取文件内容到字符串
	std::string content((std::istreambuf_iterator<char>(config)), std::istreambuf_iterator<char>());
	config.close();  // 关闭文件
	// 解析JSON字符串
	try {
		json j = json::parse(content);  // 解析JSON
		bandwidth = j["bandwidth"]["value"].get<int>();  // 获取带宽
		velocity = j["velocity"]["value"].get<int>();  // 获取速度
		packet_size = j["packet_size"].get<int>();  // 获取数据包大小
		noisy = j["noisy"]["type"].get<int>();  // 获取噪声类型
		distribution_left = j["distribution_left"].get<double>();  // 获取分布左值
		distribution_right = j["distribution_right"].get<double>();  // 获取分布右值
		source = KeplerOrbits::GeoCoordinates(j["Source"]["lat"].get<double>(), j["Source"]["lon"].get<double>(), 0);  // 获取源坐标
		target = KeplerOrbits::GeoCoordinates(j["Target"]["lat"].get<double>(), j["Target"]["lon"].get<double>(), 0);  // 获取目标坐标
	}
	catch (json::parse_error& e) {
		std::cerr << "communication_config.json解析错误: " << e.what() << std::endl;  // 捕获并处理解析错误
	}
}

// 计算通信延迟（带噪声）
double Communication::communication_stt(vector<int> distance, vector<int> stationState, int packet_size, int bandwidth, bool add_delay)
{
	if (packet_size == 0) {
		packet_size = this->packet_size;  // 如果未指定数据包大小，则使用默认值
	}

	if (bandwidth == -1) {
		bandwidth = this->bandwidth;  // 如果未指定带宽，则使用默认值
	}

	if (this->noisy != 0) {
		if (this->noisy == 1) {
			bandwidth = generate_gauss_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);  // 使用高斯分布调整带宽
		}
		else if (this->noisy == 2) {
			bandwidth = random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);  // 使用均匀分布调整带宽
		}
	}

	double delay = 0.0;
	double send_delay = static_cast<double>(packet_size) / bandwidth;  // 计算发送延迟
	for (int i = 0; i < stationState.size(); i++)
	{
		send_delay += static_cast<double>(packet_size) / random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);  // 为每个站点增加发送延迟
	}

	double propagation_delay = distance[0] / this->velocity;  // 计算传播延迟
	delay = send_delay + propagation_delay;  // 计算总延迟

	if (add_delay) {
		this->delay_normal += delay;  // 如果需要，累加延迟
	}

	return delay;  // 返回计算的延迟
}

// 计算通信延迟（无噪声）
double Communication::communication_stt_no_noisy(vector<int> distance, vector<int> stationState, int packet_size, int bandwidth, bool add_delay)
{
	if (packet_size == 0) {
		packet_size = this->packet_size;  // 如果未指定数据包大小，则使用默认值
	}

	if (bandwidth == -1) {
		bandwidth = this->bandwidth;  // 如果未指定带宽，则使用默认值
	}

	double delay = 0.0;
	double send_delay = static_cast<double>(packet_size) / bandwidth;  // 计算发送延迟
	for (int i = 0; i < stationState.size(); i++)
	{
		send_delay += static_cast<double>(packet_size) / random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);  // 为每个站点增加发送延迟
	}
	double propagation_delay = distance[0] / this->velocity;  // 计算传播延迟
	delay = send_delay + propagation_delay;  // 计算总延迟

	if (add_delay) {
		this->delay_normal += delay;  // 如果需要，累加延迟
	}

	return delay;  // 返回计算的延迟
}

// 计算理想情况下的通信延迟
double Communication::communication_stt_ideal(vector<int> distance, bool add_delay)
{
	double delay = 0.0;
	double propagation_delay = distance[0] / this->velocity;  // 计算传播延迟
	delay = propagation_delay;  // 理想情况下只有传播延迟

	if (add_delay) {
		this->delay_normal += delay;  // 如果需要，累加延迟
	}

	return delay;  // 返回计算的延迟
}