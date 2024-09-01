#include "Constellation.h"

//邻接矩阵存储 
long long m_map[MAX_SATALLITE_NUM][MAX_SATALLITE_NUM];  
//路径矩阵
int path[MAX_SATALLITE_NUM][MAX_SATALLITE_NUM];
//卫星数量
int n;

//随机数配置
std::random_device rd;
std::mt19937 gen(rd());

// 定义分布
std::uniform_int_distribution<> dis(0, 10);
//间接设置失能卫星的概率
#define DISABLED_NUM 3

/// <summary>
/// 地面站初始化
/// </summary>
/// <param name="ID">地面站ID</param>
/// <param name="Name">地面站名字</param>
/// <param name="Lat">地面站纬度</param>
/// <param name="Lon">地面站经度</param>
/// <param name="X">地面站笛卡尔坐标系X</param>
/// <param name="Y">地面站笛卡尔坐标系Y</param>
/// <param name="Z">地面站笛卡尔坐标系Z</param>
/// <param name="R">地面站通信距离</param>
GroundStation::GroundStation(int ID, string Name, double Lat, double Lon, double X, double Y, double Z, double R)
{
	id = ID;
	name = Name;
	lat = Lat;
	lon = Lon;
	x = X;
	y = Y;
	z = Z;
	comR = R;
}

/// <summary>
/// 设置经纬度
/// </summary>
/// <param name="Lat">纬度</param>
/// <param name="Lon">经度</param>
void GroundStation::SetLatAndLon(double Lat, double Lon)
{
	lat = Lat;
	lon = Lon;
}

/// <summary>
/// 获取地面站物理坐标系的纬度
/// </summary>
/// <returns>纬度</returns>
double GroundStation::Lat()
{
	return lat;
}

/// <summary>
/// 获取地面站物理坐标系的经度
/// </summary>
/// <returns>经度</returns>
double GroundStation::Lon()
{
	return lon;
}

/// <summary>
/// 初始化卫星站
/// </summary>
/// <param name="ID">卫星ID</param>
/// <param name="pnum">轨道平面序号</param>
/// <param name="onum">卫星在轨道上等分的位置序号</param>
/// <param name="toffset">卫星时间偏移</param>
/// <param name="X">卫星笛卡尔坐标系X</param>
/// <param name="Y">卫星笛卡尔坐标系Y</param>
/// <param name="Z">卫星笛卡尔坐标系Z</param>
/// <param name="enabledK">使能情况</param>
SatelliteStation::SatelliteStation(int ID, int pnum, int onum, double toffset, double X, double Y, double Z, int enabledK)
{
	id = ID;
	plane_num = pnum;
	offset_num = onum;
	time_offset = toffset;
	x = X;
	y = Y;
	z = Z;
	enabled = enabledK;
}

/// <summary>
/// 初始化星座模拟器
/// </summary>
/// <param name="datafile">数据文件路径</param>
/// <param name="configfile">配置文件路径</param>
Constellation::Constellation(const string& datafile, const string& configfile)
{
	attack.push_back(GroundStation(0, "null", 0, 0, 0, 0, 0));
	//地面站信息获取
	ifstream data(datafile);
	// 读取文件内容到字符串
	std::string content((std::istreambuf_iterator<char>(data)), std::istreambuf_iterator<char>());
	int g_id = -1;
	if (!data.is_open()) {
		cerr << "无法打开文件city_data.json" << endl;
		return ;
	}
	data.close();
	// 解析JSON字符串
	try {
		json j = json::parse(content);

		// 遍历JSON数组
		for (const auto& city : j) {
			string name = city["name"];
			double lat = city["lat"];
			double lon = city["lon"];
			vector<double> a = LatLonToXYZ((lat), (lon));
			grounds.push_back(GroundStation(g_id, name, (lat), (lon), a[0], a[1], a[2]));
			g_id--;
		}
	}
	catch (json::parse_error& e) {
		std::cerr << "JSON解析错误: " << e.what() << std::endl;
	}
	//卫星信息
	mode = 1;//Mode=1自动生成轨道，Mode=2手动设置轨道参数
	vector<double> Ecc;
	vector<double> Sma;
	vector<double> Inc;
	vector<double> Lta;
	vector<double> Ltp;
	ifstream config(configfile);
	if (!config.is_open()) {
		cerr << "无法打开文件config.json" << endl;
		return;
	}
	// 读取文件内容到字符串
	std::string content1((std::istreambuf_iterator<char>(config)), std::istreambuf_iterator<char>());
	int s_id = 0;
	config.close();

	try {
		json j = json::parse(content1);

		// 访问数据
		if (j["mode"].get<std::string>() == "Auto")
			mode = 1;
		else
			mode = 2;
		if (mode == 1)
		{
			num_planes = j["planesNum"].get<int>();
			num_nodes_per_plane = j["nodesNum"].get<int>();
			sma = j["sma"].get<double>() * 10 + earth_radius;
		}
		else
		{
			num_planes = j["manualPlanesNum"].get<int>();
			num_nodes_per_plane = j["manualNodesNum"].get<int>();
			// 遍历轨道数组
			for (const auto& orbit : j["orbits"]) {
				Ecc.push_back(orbit["ecc"].get<double>());
				Sma.push_back(orbit["sma"].get<double>());
				Inc.push_back(orbit["inc"].get<double>());
				Lta.push_back(orbit["lta"].get<double>());
				Ltp.push_back(orbit["ltp"].get<double>());
			}
		}
	}
	catch (json::parse_error& e) {
		std::cerr << "config.json解析错误: " << e.what() << std::endl;
	}
	//根据轨道信息计算卫星的位置信息
	for (int i = 0; i < num_planes; i++)
	{
		KeplerOrbits::OrbitalElements orbitalElements;
		if (mode == 1)
		{
			orbitalElements.SetEllipseShape(0, sma);
			orbitalElements.SetOrbitalPlane(70, (360 / num_planes)* i);
			orbitalElements.SetEllipseOrientation(0);
		}
		else
		{
			orbitalElements.SetEllipseShape(Ecc[i], Sma[i]);
			orbitalElements.SetOrbitalPlane(Inc[i], Lta[i]);
			orbitalElements.SetEllipseOrientation(Ltp[i]);
		}
		KeplerOrbits::OrbitBody orbitalBody = KeplerOrbits::OrbitBody(i, orbitalElements, orbitalElements.GetOrbitPeriod());
		if(planes.size()==0)
			planes.push_back(orbitalBody);
		else
		{
			if (planes.size() % 2 == 0)
			{
				planes.insert(planes.begin(), orbitalBody);
			}
			else
			{
				planes.push_back(orbitalBody);
			}
		}
	}
	for (int i = 0; i < num_planes; i++)
	{
		KeplerOrbits::Trajectory trajectory = planes[i].GetTrajectory(num_nodes_per_plane);
		for (int j = 0; j < num_nodes_per_plane; j++)
		{
			//double tf = (planes[i].GetSiderealOrbitPeriod() / num_nodes_per_plane) * j;
			SatelliteStation s = SatelliteStation(s_id, i, j, 0, trajectory[j].x(), trajectory[j].y(), trajectory[j].z(), 0);
			s_id++;
			satellites.push_back(s);
			if (j != 0)
			{
				if (j == num_nodes_per_plane - 1)
				{
					Link l = Link();
					l.source = satellites[s_id - 2];
					l.target = satellites[s_id - 1];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 2], satellites[s_id - 1]);
					links.push_back(l);
					Link ll = Link();
					ll.source = satellites[s_id - 1];
					ll.target = satellites[s_id - num_nodes_per_plane];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 1], satellites[s_id - num_nodes_per_plane]);
					links.push_back(ll);
				}
				else
				{
					Link l = Link();
					l.source = satellites[s_id - 2];
					l.target = satellites[s_id - 1];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 2], satellites[s_id - 1]);
					links.push_back(l);
				}
			}
		}
	}
	double max_distance = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < satellites.size(); j++)
		{
			if (mode == 1)
			{
				max_distance = calculateMaxSpaceToGndDistance();
			}
			else
			{
				max_distance = calculateMaxSpaceToGndDistance(90, planes[j / num_nodes_per_plane].GetSma());
			}
			double d = sqrt(pow(satellites[j].x - grounds[i].x, 2) + pow(satellites[j].y - grounds[i].y, 2) + pow(satellites[j].z - grounds[i].z, 2));
			if ((int)d < (int)max_distance)
			{
				if (i == 0)
				{
					start.push_back(satellites[j]);
					sdistance.push_back(d);
				}
				else
				{
					end.push_back(satellites[j]);
					edistance.push_back(d);
				}
			}
		}
	}
}

/// <summary>
/// 物理坐标系到笛卡尔坐标系的转换
/// </summary>
/// <param name="lat"></param>
/// <param name="lon"></param>
/// <returns></returns>
vector<double> Constellation::LatLonToXYZ(double lat, double lon) {
	vector<double> ans;
	double an;
	lat *= (M_PI / 180);
	lon *= (M_PI / 180);
	an= earth_radius * cos(lat) * cos(lon);
	ans.push_back(an);
	an= earth_radius * cos(lat) * sin(lon);
	ans.push_back(an);
	an = earth_radius * sin(lat);
	ans.push_back(an);
	return ans;
}

#ifdef ATTACK_K
void Constellation::updateSatellites()
{
	m_time++;
	links.clear();
	start.clear();
	end.clear();
	sdistance.clear();
	edistance.clear();
	pathId.clear();
	pathDistance.clear();
	pathState.clear();
	int s_id = 0;
	for (int i = 0; i < num_planes; i++)
	{
		KeplerOrbits::Trajectory trajectory = planes[i].GetTrajectory(num_nodes_per_plane, m_time);
		for (int j = 0; j < num_nodes_per_plane; j++)
		{
			//double tf = (planes[i].GetSiderealOrbitPeriod() / num_nodes_per_plane) * j;
			satellites[s_id].x = trajectory[j].x();
			satellites[s_id].y = trajectory[j].y();
			satellites[s_id].z = trajectory[j].z();
			s_id++;
			if (j != 0)
			{
				if (j == num_nodes_per_plane - 1)
				{
					Link l = Link();
					l.source = satellites[s_id - 2];
					l.target = satellites[s_id - 1];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 2], satellites[s_id - 1]);
					links.push_back(l);
					Link ll = Link();
					ll.source = satellites[s_id - 1];
					ll.target = satellites[s_id - num_nodes_per_plane];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 1], satellites[s_id - num_nodes_per_plane]);
					links.push_back(ll);
				}
				else
				{
					Link l = Link();
					l.source = satellites[s_id - 2];
					l.target = satellites[s_id - 1];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 2], satellites[s_id - 1]);
					links.push_back(l);
				}
			}
		}
	}
	//计算平面轨道间卫星的通信
	int max_isl_range = calculateMaxISLDistance();
	for (int i = 0; i < num_planes; i++)
	{
		int j;
		if (i == num_planes - 1)
			j = 0;
		else
			j = i + 1;
		for (int k = 0; k < num_nodes_per_plane; k++)
		{
			int d = calculateDistanceBetweenSatellites(satellites[k + i * num_nodes_per_plane], satellites[k + j * num_nodes_per_plane]);
			if (d < max_isl_range)
			{
				Link l = Link();
				l.source = satellites[k + i * num_nodes_per_plane];
				l.target = satellites[k + j * num_nodes_per_plane];
				l.distance = d;
				links.push_back(l);
			}
		}
	}
	//计算地面站与卫星可能的连接
	double max_distance = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < satellites.size(); j++)
		{
			satellites[j].enabled = 0;
			double d = sqrt(pow(satellites[j].x - grounds[i].x, 2) + pow(satellites[j].y - grounds[i].y, 2) + pow(satellites[j].z - grounds[i].z, 2));
			if (mode == 1)
			{
				max_distance = calculateMaxSpaceToGndDistance();
			}
			else
			{
				max_distance = calculateMaxSpaceToGndDistance(90, planes[j / num_nodes_per_plane].GetSma());
			}
			if ((int)d < (int)max_distance)
			{
				if (i == 0)
				{
					start.push_back(satellites[j]);
					sdistance.push_back(d);
				}
				else
				{
					end.push_back(satellites[j]);
					edistance.push_back(d);
				}
			}
		}
	}
	for (int j = 0; j < satellites.size() && attack.size()>0; j++)
	{
		double d = sqrt(pow(satellites[j].x - attack[0].x, 2) + pow(satellites[j].y - attack[0].y, 2) + pow(satellites[j].z - attack[0].z, 2));
		if((int)attack[0].comR > 0)
			max_distance = (int)attack[0].comR;
		else
		{
			if (mode == 1)
			{
				max_distance = calculateMaxSpaceToGndDistance();
			}
			else
			{
				max_distance = calculateMaxSpaceToGndDistance(90, planes[j / num_nodes_per_plane].GetSma());
			}
		}
		if ((int)d < (int)max_distance)
		{
			satellites[j].enabled = dis(gen) < DISABLED_NUM ? 2 : 1;
		}
	}
	if (start.size() != 0 && end.size() != 0)
		shortestPathByFloyd();
}
#else
void Constellation::updateSatellites()
{
	m_time++;
	links.clear();
	start.clear();
	end.clear();
	sdistance.clear();
	edistance.clear();
	pathId.clear();
	pathDistance.clear();
	pathState.clear();
	int s_id = 0;
	for (int i = 0; i < num_planes; i++)
	{
		KeplerOrbits::Trajectory trajectory = planes[i].GetTrajectory(num_nodes_per_plane, m_time);
		for (int j = 0; j < num_nodes_per_plane; j++)
		{
			//double tf = (planes[i].GetSiderealOrbitPeriod() / num_nodes_per_plane) * j;
			satellites[s_id].x = trajectory[j].x();
			satellites[s_id].y = trajectory[j].y();
			satellites[s_id].z = trajectory[j].z();
			s_id++;
			if (j != 0)
			{
				if (j == num_nodes_per_plane - 1)
				{
					Link l = Link();
					l.source = satellites[s_id - 2];
					l.target = satellites[s_id - 1];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 2], satellites[s_id - 1]);
					links.push_back(l);
					Link ll = Link();
					ll.source = satellites[s_id - 1];
					ll.target = satellites[s_id - num_nodes_per_plane];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 1], satellites[s_id - num_nodes_per_plane]);
					links.push_back(ll);
				}
				else
				{
					Link l = Link();
					l.source = satellites[s_id - 2];
					l.target = satellites[s_id - 1];
					l.distance = calculateDistanceBetweenSatellites(satellites[s_id - 2], satellites[s_id - 1]);
					links.push_back(l);
				}
			}
		}
	}
	//计算平面轨道间卫星的通信
	int max_isl_range = calculateMaxISLDistance();
	for (int i = 0; i < num_planes; i++)
	{
		int j;
		if (i == num_planes - 1)
			j = 0;
		else
			j = i + 1;
		for (int k = 0; k < num_nodes_per_plane; k++)
		{
			int d = calculateDistanceBetweenSatellites(satellites[k + i * num_nodes_per_plane], satellites[k + j * num_nodes_per_plane]);
			if (d < max_isl_range)
			{
				Link l = Link();
				l.source = satellites[k + i * num_nodes_per_plane];
				l.target = satellites[k + j * num_nodes_per_plane];
				l.distance = d;
				links.push_back(l);
			}
		}
	}
	//计算地面站与卫星可能的连接
	double max_distance = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < satellites.size(); j++)
		{
			satellites[j].enabled = 0;
			double d = sqrt(pow(satellites[j].x - grounds[i].x, 2) + pow(satellites[j].y - grounds[i].y, 2) + pow(satellites[j].z - grounds[i].z, 2));
			if (mode == 1)
			{
				max_distance = calculateMaxSpaceToGndDistance();
			}
			else
			{
				max_distance = calculateMaxSpaceToGndDistance(90, planes[j / num_nodes_per_plane].GetSma());
			}
			if ((int)d < (int)max_distance)
			{
				if (i == 0)
				{
					start.push_back(satellites[j]);
					sdistance.push_back(d);
				}
				else
				{
					end.push_back(satellites[j]);
					edistance.push_back(d);
				}
			}
		}
	}
	if (start.size() != 0 && end.size() != 0)
		shortestPathByFloyd();
}
#endif // ATTACK_K

double Constellation::calculateMaxSpaceToGndDistance(double max_deg, double m_sma)
{
	if (m_sma == -1)
		m_sma = sma;
	max_deg *= (M_PI / 180);
	return sqrt(pow(m_sma, 2) - pow(earth_radius * sin(max_deg), 2)) - earth_radius * cos(max_deg);
}

double Constellation::calculateMaxISLDistance(int min_communication_altitude)
{
	int c = (int)earth_radius + min_communication_altitude;
	int b = (int)sma;
	double B = M_PI / 2;
	double C = asin((c * sin(B)) / b);
	double	A = M_PI - B - C;
	double a = (b * sin(A)) / sin(B);
	return int(a * 2);
}

double Constellation::calculateDistanceBetweenSatellites(SatelliteStation s1, SatelliteStation s2)
{
	return sqrt(pow(s1.x - s2.x, 2) + pow(s1.y - s2.y, 2) + pow(s1.z - s2.z, 2));
}

void Constellation::shortestPathByFloyd()
{
	n = satellites.size() + 2;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			m_map[i][j] = INT_MAX;
			path[i][j] = j;
		}
	}
	for (int i = 0; i < links.size(); i++)
	{
		if (links[i].source.enabled != 2 && links[i].target.enabled != 2)
		{
			m_map[links[i].source.id + 1][links[i].target.id + 1] = (int)links[i].distance;
			m_map[links[i].target.id + 1][links[i].source.id + 1] = (int)links[i].distance;
		}
	}
	for (int i = 0; i < start.size(); i++)
	{
		if (start[i].enabled != 2)
		{
			m_map[0][start[i].id + 1] = (int)sdistance[i];
			m_map[start[i].id + 1][0] = (int)sdistance[i];
		}
	}
	for (int i = 0; i < end.size(); i++)
	{
		if (end[i].enabled != 2)
		{
			m_map[n - 1][end[i].id + 1] = (int)edistance[i];
			m_map[end[i].id + 1][n - 1] = (int)edistance[i];
		}
	}
	int ans = floyd(0, n - 1);
	if (ans < INT_MAX)
	{
		int k = path[0][n - 1];
		pathDistance.push_back(ans);
		while (k != n - 1)
		{
			pathId.push_back(k - 1);
			pathState.push_back(satellites[k - 1].enabled);
			k = path[k][n - 1];
		}

		for (int i = 0; i < pathId.size(); i++)
		{
			if (i == 0)
			{
				pathDistance.push_back(m_map[0][pathId[0] + 1]);
			}
			else
			{
				pathDistance.push_back(m_map[pathId[i-1]+1][pathId[i]]);
			}
			if (i == pathId.size() - 1)
			{
				pathDistance.push_back(m_map[pathId[i]+1][n - 1]);
			}
		}
	}
}

void Constellation::AddGroundSandT(KeplerOrbits::GeoCoordinates source, KeplerOrbits::GeoCoordinates target)
{
	GroundStation gsT = GroundStation(-1 * grounds.size() - 1, "Target", target.latitude(), target.longitude(), LatLonToXYZ(target.latitude(), target.longitude())[0], LatLonToXYZ(target.latitude(), target.longitude())[1], LatLonToXYZ(target.latitude(), target.longitude())[2]);
	grounds.insert(grounds.begin(), gsT);
	GroundStation gsS = GroundStation(-1 * grounds.size() - 1, "Source", source.latitude(), source.longitude(), LatLonToXYZ(source.latitude(), source.longitude())[0], LatLonToXYZ(source.latitude(), source.longitude())[1], LatLonToXYZ(source.latitude(), source.longitude())[2]);
	grounds.insert(grounds.begin(), gsS);
}

void Constellation::AddAttackStation(KeplerOrbits::GeoCoordinates attackStation, double comR)
{
	attack[0].SetLatAndLon(attackStation.latitude(), attackStation.longitude());
	attack[0].comR = comR;
	attack[0].x = LatLonToXYZ(attack[0].Lat(), attack[0].Lon())[0];
	attack[0].y = LatLonToXYZ(attack[0].Lat(), attack[0].Lon())[1];
	attack[0].z = LatLonToXYZ(attack[0].Lat(), attack[0].Lon())[2];
}

// 求最短路径
int floyd(int from, int to) { //从起点到目标点 

	int i, j, k;

	// 中间节点k
	for (k = 0; k < n; k++) {

		// 起点i
		for (i = 0; i < n; i++) {

			// 终点j
			for (j = 0; j < n; j++) {


				// 更新距离和路径
				if (m_map[i][j] > (m_map[i][k] + m_map[k][j]) && i != j) {

					// 距离
					m_map[i][j] = m_map[i][k] + m_map[k][j];

					// 路径
					path[i][j] = path[i][k];
				}
			}
		}
	}

	return m_map[from][to];
}