#pragma once
#include "Allinclude.h"
#define MAX_SATALLITE_NUM 225

int floyd(int from, int to);
int dijkstra(int from, int to);

class GroundStation
{
private:
	int id;//陆地节点的id取负数，从-1开始
	string name;
	double lat;//纬度
	double lon;//经度
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
	int plane_num;//卫星所属轨道平面
	int offset_num;//在轨道上的偏移
	double time_offset;//在kepler ellipse solver的偏移量
public:
	int id;//卫星节点的id取正数，从0开始
	double x;
	double y;
	double z;
	int enabled;//0：正常；1：被攻击，带宽减小；2：瘫痪
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
	int num_planes;//卫星平面数量
	int num_nodes_per_plane;//每个平面的卫星数量
	double sma;//长半轴距离
	int m_time = 0;
	vector<GroundStation> grounds;//地面站数组
	vector<SatelliteStation> satellites;//卫星数组
	vector<KeplerOrbits::OrbitBody> planes;
	vector<Link> links;//链接数组
	vector<GroundStation> attack;//攻击地面站点
	vector<SatelliteStation> start;//与数据来源地面站连接的卫星数组
	vector<double> sdistance;
	vector<SatelliteStation> end;//与数据接收地面站连接的卫星数组
	vector<double> edistance;
	vector<int> pathId;//发送到接收经过的节点/
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

