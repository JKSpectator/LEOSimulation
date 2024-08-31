#include "Allinclude.h"
#include "DelaySimulation.h"
#include "LEOSimulation.h"
#include "Constellation.h"

#ifdef ATTACK_K
#include "AttackSimulation.h"
Constellation cs;
Communication comm;
AttackSimulation as;
int SInit(bool attackIn, const string& datafile, const string& configfile, const string& com_configfile)
{
	cs = Constellation(datafile, configfile);
	comm = Communication(com_configfile);
	if (attackIn == true)
	{
		as = AttackSimulation(com_configfile);
		cs.AddAttackStation(as.attack);
	}
	cs.AddGroundSandT(comm.source, comm.target);
	return 0;
}
#else
Constellation cs;
Communication comm;

int SInit(bool attackIn,  const string& datafile, const string& configfile, const string& com_configfile)
{
	cs = Constellation(datafile, configfile);
	comm = Communication(com_configfile);
	cs.AddGroundSandT(comm.source, comm.target);
	if (attackIn == true)
		return 1;
	else
		return 0;
}
#endif // ATTACK_K

void testT() 
{
	// 开始时间
	clock_t start = clock();

	// 调用测试的函数
	cs.updateSatellites();

	// 结束时间
	clock_t end1 = clock();

	if (cs.pathId.size() != 0)
	{
		cout << "通信延迟：" << comm.communication_stt(cs.pathDistance, cs.pathState) << endl;
		cout << "无噪声通信延迟：" << comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState) << endl;
		cout << "最理想通信延迟：" << comm.communication_stt_ideal(cs.pathDistance) << endl;
	}

	// 结束时间
	clock_t end2 = clock();

	// 计算运行时间，转换为秒
	double duration1 = (double)(end1 - start) / CLOCKS_PER_SEC;
	double duration2 = (double)(end2 - end1) / CLOCKS_PER_SEC;

	// 输出运行时间
	std::cout << "Function took " << duration1 << " seconds to complete." << std::endl;
	std::cout << "FPS:  " << 1/duration2 << std::endl;
}

vector<double> com_sim()
{
	vector<double> ans;
	//更新卫星数据
	cs.updateSatellites();
	//判断是否找到可行通信路径
	if (cs.pathId.size() != 0)
	{
		ans.push_back(comm.communication_stt(cs.pathDistance, cs.pathState));
		ans.push_back(comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState));
		ans.push_back(comm.communication_stt_ideal(cs.pathDistance));
	}
	return ans;
}
int main(int argc, char* argv[])
{
	SInit();
	testT();
	vector<double> delay;
	while (true)
	{
		delay = com_sim();
		//判断是否找到可行通信路径
		if (delay.size() != 0)
		{
			cout << "通信延迟：" << delay[0] << endl;
			cout << "无噪声通信延迟：" << delay[1] << endl;
			cout << "最理想通信延迟：" << delay[2] << endl;
		}
	}
	return 0;
}