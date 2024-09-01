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
	TestInit(cs, comm, as);
	testT(cs, comm, 10);
	//vector<double> delay;
	//while (true)
	//{
	//	delay = com_sim();
	//	//判断是否找到可行通信路径
	//	if (delay.size() != 0)
	//	{
	//		cout << "通信延迟：" << delay[0] << endl;
	//		cout << "无噪声通信延迟：" << delay[1] << endl;
	//		cout << "最理想通信延迟：" << delay[2] << endl;
	//	}
	//}
	return 0;
}