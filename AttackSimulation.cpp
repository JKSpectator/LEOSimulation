#include "AttackSimulation.h"

AttackSimulation::AttackSimulation(const std::string& configfile)
{
	//初始化干扰半径
	comR = -1;
	//打开配置文件
	std::ifstream config(configfile);
	//判断配置文件是否已打开
	if (!config.is_open()) {
		std::cerr << "无法打开文件communication_config.json" << std::endl;
		return ;
	}
	// 读取文件内容到字符串
	std::string content((std::istreambuf_iterator<char>(config)), std::istreambuf_iterator<char>());
	//关闭配置文件
	config.close();
	// 解析JSON字符串
	try {
		json j = json::parse(content);
		//读取attack的物理坐标系内容
		attack = KeplerOrbits::GeoCoordinates(j["Attack"]["lat"].get<double>(), j["Attack"]["lon"].get<double>(), 0);
		//读取攻击半径
		comR = j["AR"]["value"].get<int>();
	}
	catch (json::parse_error& e) {
		std::cerr << "communication_config.json解析错误: " << e.what() << std::endl;
	}
}