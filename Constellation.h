#pragma once
#include "Allinclude.h"
//设定的模拟卫星的最多数量，根据内存更改
#define MAX_SATALLITE_NUM 225
//最短路径算法通用函数
int floyd(int from, int to);

/// <summary>
/// 陆地节点类
/// </summary>
class GroundStation
{
private:
	//陆地节点的id取负数，从-1开始
	int id;
	//陆地节点的名字，如果有的话会设置
	string name;
	double lat;//纬度
	double lon;//经度
public:
	//笛卡尔坐标系的xyz坐标
	double x;
	double y;
	double z;
	//通信半径，这里一般不设置，只有攻击节点配置时设置
	double comR;
	//获取纬度
	double Lat();
	//获取经度
	double Lon();
	//设置地面站的经纬度
	void SetLatAndLon(double Lat, double Lon);
	GroundStation(int ID = -1, string Name = nullptr, double Lat = 0, double Lon = 0, double X = 0, double Y = 0, double Z = 0, double R = -1);
};

/// <summary>
/// 卫星节点类
/// </summary>
class SatelliteStation
{
private:
	//卫星所属轨道平面
	int plane_num;
	//在轨道上的偏移
	int offset_num;
	//在kepler ellipse solver的偏移量
	double time_offset;
public:
	//卫星节点的id取正数，从0开始
	int id;
	//笛卡尔坐标系下坐标
	double x;
	double y;
	double z;
	//0：正常；1：被攻击，带宽减小；2：瘫痪
	int enabled;
	SatelliteStation(int ID = 0, int pnum = 0, int onum = 0, double toffset = 0, double X = 0, double Y = 0, double Z = 0, int enabledK = 0);
};

/// <summary>
/// 卫星可通信链接类
/// </summary>
class Link
{
public:
	//通信链接发送站
	SatelliteStation source;
	//通信链接接收站
	SatelliteStation target;
	//通信连接距离
	double distance;
};

/// <summary>
/// LEO星座数据存储类
/// </summary>
class Constellation
{
public:
	//地球半径*100=m
	double earth_radius = 6.378137e4;
	//卫星平面数量
	int num_planes;
	//每个平面的卫星数量
	int num_nodes_per_plane;
	//长半轴距离
	double sma;
	int m_time = 0;
	//模拟的模式，1为自动模式Auto，2为手动模式Manual，对应设置卫星轨道的方式
	int mode = 1;
	//地面站数组
	vector<GroundStation> grounds;
	//卫星数组
	vector<SatelliteStation> satellites;
	//轨道平面数组
	vector<KeplerOrbits::OrbitBody> planes;
	//链接数组
	vector<Link> links;
	//攻击地面站点
	vector<GroundStation> attack;
	//与数据来源地面站连接的卫星数组
	vector<SatelliteStation> start;
	//可以与通信来源地面站通信的卫星距离
	vector<double> sdistance;
	//与数据接收地面站连接的卫星数组
	vector<SatelliteStation> end;
	//可以与通信目标地面站通信的卫星距离
	vector<double> edistance;
	//发送到接收经过的节点id
	vector<int> pathId;
	//发送到接收经过的节点状态
	vector<int> pathState;
	//发送到接收经过的路径距离，其中第一位元素为总距离
	vector<int> pathDistance;
	//初始化函数
	Constellation(const string& datafile = "ConfigTxt\\city_data.json", const string& configfile = "ConfigTxt\\config.json");
	//地理坐标系到笛卡尔坐标系转换
	vector<double> LatLonToXYZ(double lat, double lon);
	//更新卫星的函数
	void updateSatellites();
	/// <summary>
	/// 计算卫星与地面站通信的最大距离
	/// </summary>
	/// <param name="max_deg">可通信角度0-90</param>
	/// <param name="m_sma">通信长半轴</param>
	/// <returns>通信最长距离</returns>
	double calculateMaxSpaceToGndDistance(double max_deg = 90, double m_sma = -1);
	/// <summary>
	/// 计算平面轨道间通信的最大距离
	/// </summary>
	/// <param name="min_communication_altitude">最小通信高度</param>
	/// <returns>平面轨道间通信的最大距离</returns>
	double calculateMaxISLDistance(int min_communication_altitude = 1000);
	/// <summary>
	/// 计算卫星间距离
	/// </summary>
	/// <param name="s1">卫星1</param>
	/// <param name="s2">卫星2</param>
	/// <returns>距离</returns>
	double calculateDistanceBetweenSatellites(SatelliteStation s1, SatelliteStation s2);
	/// <summary>
	/// 最短路径辅助函数
	/// </summary>
	void shortestPathByFloyd();
	/// <summary>
	/// 添加通信的数据来源地面站和目标地面站
	/// </summary>
	/// <param name="source">来源地面站</param>
	/// <param name="target">目标地面站</param>
	void AddGroundSandT(KeplerOrbits::GeoCoordinates source, KeplerOrbits::GeoCoordinates target);
	/// <summary>
	/// 添加攻击站点
	/// </summary>
	/// <param name="attackStation">攻击站点的地理坐标系的值</param>
	/// <param name="comR">攻击距离</param>
	void AddAttackStation(KeplerOrbits::GeoCoordinates attackStation, double comR = -1);
};