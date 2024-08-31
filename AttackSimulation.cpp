#include "AttackSimulation.h"

AttackSimulation::AttackSimulation(const std::string& configfile)
{
	//��ʼ�����Ű뾶
	comR = -1;
	//�������ļ�
	std::ifstream config(configfile);
	//�ж������ļ��Ƿ��Ѵ�
	if (!config.is_open()) {
		std::cerr << "�޷����ļ�" << std::endl;
		return ;
	}
	// ��ȡ�ļ����ݵ��ַ���
	std::string content((std::istreambuf_iterator<char>(config)), std::istreambuf_iterator<char>());
	config.close();
	// ����JSON�ַ���
	try {
		json j = json::parse(content);

		attack = KeplerOrbits::GeoCoordinates(j["Attack"]["lat"].get<double>(), j["Attack"]["lon"].get<double>(), 0);
		comR = j["AR"]["value"].get<int>();
	}
	catch (json::parse_error& e) {
		std::cerr << "communication_config.json��������: " << e.what() << std::endl;
	}
}