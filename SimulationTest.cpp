#include "Allinclude.h"
#include "DelaySimulation.h"
#include "Constellation.h"
#include "LEOSimulation.h"
#include "AttackSimulation.h"

int TestInit(Constellation cs, Communication comm, AttackSimulation as, const string& datafile, const string& configfile, const string& com_configfile)
{
	cs = Constellation(datafile, configfile);
	comm = Communication(com_configfile);
	as = AttackSimulation(com_configfile);
	cs.AddAttackStation(as.attack);
	cs.AddGroundSandT(comm.source, comm.target);
	return 0;
}

void testT(Constellation cs, Communication comm, int testNum)
{
	// ��ʼʱ��
	clock_t start = clock();
	for (int i = 0; i < testNum; i++)
	{
		// ���ò��Եĺ���
		cs.updateSatellites();
	}

	// ����ʱ��
	clock_t end1 = clock();

	for (int i = 0; i < testNum; i++)
	{
		// ���ò��Եĺ���
		cs.updateSatellites();
		if (cs.pathId.size() != 0)
		{
			comm.communication_stt(cs.pathDistance, cs.pathState);
			comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState);
			comm.communication_stt_ideal(cs.pathDistance);
		}
	}

	// ����ʱ��
	clock_t end2 = clock();

	// ��������ʱ�䣬ת��Ϊ��
	double duration1 = (double)(end1 - start) / CLOCKS_PER_SEC / testNum;
	double duration2 = (double)(end2 - end1) / CLOCKS_PER_SEC / testNum;

	// �������ʱ��
	std::cout << "Test Num: " << testNum << ". Function took " << duration1 << " seconds to complete." << std::endl;
	std::cout << "Test Num: " << testNum << ". FPS:  " << 1 / duration2 << "." << std::endl;
}