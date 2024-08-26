#include "Allinclude.h"
#include "DelaySimulation.h"
#include "LEOSimulation.h"
#include "Constellation.h"

Constellation cs;
Communication comm;

void SInit()
{
	cs = Constellation();
	comm = Communication();
	cs.attack[0].SetLatAndLon(comm.attack.latitude(), comm.attack.longitude());
	cs.attack[0].x = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[0];
	cs.attack[0].y = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[1];
	cs.attack[0].z = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[2];
	cs.AddGroundSandT(comm.source, comm.target);
}

int main(int argc, char* argv[])
{
	SInit();

	while (true)
	{
		//更新卫星数据
		cs.updateSatellites();
		//判断是否找到可行通信路径
		if (cs.pathId.size() != 0)
		{
			cout << "通信延迟：" << comm.communication_stt(cs.pathDistance, cs.pathState) << endl;
			cout << "无噪声通信延迟：" << comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState) << endl;
			cout << "最理想通信延迟：" << comm.communication_stt_ideal(cs.pathDistance) << endl;
		}
	}
	return 0;
}