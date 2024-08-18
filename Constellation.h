#pragma once
#include "Allinclude.h"
#define MAX_SATALLITE_NUM 225

int floyd(int from, int to);
int dijkstra(int from, int to);

class GroundStation
{
private:
	int id;//½�ؽڵ��idȡ��������-1��ʼ
	string name;
	double lat;//γ��
	double lon;//����
public:
	double x;
	double y;
	double z;
	double Lat();
	double Lon();
	void SetLatAndLon(double Lat, double Lon);
	GroundStation(int ID = -1, string Name = nullptr, double Lat = 0, double Lon = 0, double X = 0, double Y = 0, double Z = 0);
};

class SatelliteStation
{
private:
	int plane_num;//�����������ƽ��
	int offset_num;//�ڹ���ϵ�ƫ��
	double time_offset;//��kepler ellipse solver��ƫ����
public:
	int id;//���ǽڵ��idȡ��������0��ʼ
	double x;
	double y;
	double z;
	int enabled;//0��������1���������������С��2��̱��
	SatelliteStation(int ID = 0, int pnum = 0, int onum = 0, double toffset = 0, double X = 0, double Y = 0, double Z = 0, int enabledK = 0);
};

class Link
{
public:
	SatelliteStation source;
	SatelliteStation target;
	double distance;
};

class Constellation
{
public:
	double earth_radius = 6.378137e4;
	int num_planes;//����ƽ������
	int num_nodes_per_plane;//ÿ��ƽ�����������
	double sma;//���������
	int m_time = 0;
	vector<GroundStation> grounds;//����վ����
	vector<SatelliteStation> satellites;//��������
	vector<KeplerOrbits::OrbitBody> planes;
	vector<Link> links;//��������
	vector<GroundStation> attack;//��������վ��
	vector<SatelliteStation> start;//��������Դ����վ���ӵ���������
	vector<double> sdistance;
	vector<SatelliteStation> end;//�����ݽ��յ���վ���ӵ���������
	vector<double> edistance;
	vector<int> pathId;//���͵����վ����Ľڵ�/
	vector<int> pathState;
	vector<int> pathDistance;
	vector<KeplerOrbits::OrbitBody> orbits;
	Constellation(const string& datafile = "ConfigTxt\\city_data.txt", const string& configfile = "ConfigTxt\\config.txt");
	vector<double> LatLonToXYZ(double lat, double lon);
	void updateSatellites();
	double calculateMaxSpaceToGndDistance(double max_deg = 90);
	double calculateMaxISLDistance(int min_communication_altitude = 1000);
	double calculateDistanceBetweenSatellites(SatelliteStation s1, SatelliteStation s2);
	void shortestPathByFloyd();
	void AddGroundSandT(KeplerOrbits::GeoCoordinates source, KeplerOrbits::GeoCoordinates target);
};

