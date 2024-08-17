#include "Constellation.h"

long long map[MAX_SATALLITE_NUM][MAX_SATALLITE_NUM];  //邻接矩阵存储 
int path[MAX_SATALLITE_NUM][MAX_SATALLITE_NUM]; //路径矩阵
int dist[MAX_SATALLITE_NUM];      //d[i]表示起点到i这个点的距离 
int visit[MAX_SATALLITE_NUM];  //节点是否被访问 
int n;

GroundStation::GroundStation(int ID, string Name, double Lat, double Lon, double X, double Y, double Z)
{
	id = ID;
	name = Name;
	lat = Lat;
	lon = Lon;
	x = X;
	y = Y;
	z = Z;
}

SatelliteStation::SatelliteStation(int ID, int pnum, int onum, double toffset, double X, double Y, double Z)
{
	id = ID;
	plane_num = pnum;
	offset_num = onum;
	time_offset = toffset;
	x = X;
	y = Y;
	z = Z;
}

Constellation::Constellation(const string& datafile, const string& configfile)
{
	//地面站信息获取
	ifstream data(datafile);
	int g_id = -1;
	if (data.is_open())
	{
		string line;
		getline(data, line);
		while (getline(data, line))
		{
			istringstream iss(line);
			string key;
			if (getline(iss, key, ' '))
			{
				string lat,lon;
				getline(iss, lat, ' ');
				getline(iss, lon, ' ');
				vector<double> a = LatLonToXYZ(stod(lat), stod(lon));
				grounds.push_back(GroundStation(g_id, key, stod(lat), stod(lon), a[0], a[1], a[2]));
				g_id--;
			}
		}
	}
	data.close();
	//卫星信息
	ifstream config(configfile);
	int s_id = 0;
	if (config.is_open())
	{
		string line;
		getline(config, line);
		while (getline(config, line))
		{
			istringstream iss(line);
			string key;
			if (getline(iss, key, ' '))
			{
				string value;
				getline(iss, value, ' ');
				if (key == "planesNum")
				{
					num_planes = stoi(value);
				}
				else if (key == "nodesNum")
				{
					num_nodes_per_plane = stoi(value);
				}
				else if (key == "sma")
				{
					sma = stod(value) * 10 + earth_radius;
				}
			}
		}
	}
	config.close();
	//根据卫星信息计算卫星的位置信息
	for (int i = 0; i < num_planes; i++)
	{
		KeplerOrbits::OrbitalElements orbitalElements;
		orbitalElements.SetEllipseShape(0, sma);
		orbitalElements.SetOrbitalPlane(70, (360 / num_planes) * i);
		orbitalElements.SetEllipseOrientation(0);
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
			double tf = (planes[i].GetSiderealOrbitPeriod() / num_nodes_per_plane) * j;
			SatelliteStation s = SatelliteStation(s_id, i, j, 0, trajectory[j].x(), trajectory[j].y(), trajectory[j].z());
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
	double max_distance = calculateMaxSpaceToGndDistance();
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < satellites.size(); j++)
		{
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

void Constellation::updateSatellites()
{
	m_time++;
	links.clear();
	start.clear();
	end.clear();
	sdistance.clear();
	edistance.clear();
	pathId.clear();
	int s_id = 0;
	for (int i = 0; i < num_planes; i++)
	{
		KeplerOrbits::Trajectory trajectory = planes[i].GetTrajectory(num_nodes_per_plane, m_time);
		for (int j = 0; j < num_nodes_per_plane; j++)
		{
			double tf = (planes[i].GetSiderealOrbitPeriod() / num_nodes_per_plane) * j;
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
	double max_distance = calculateMaxSpaceToGndDistance();
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < satellites.size(); j++)
		{
			double d = sqrt(pow(satellites[j].x - grounds[i].x, 2) + pow(satellites[j].y - grounds[i].y, 2) + pow(satellites[j].z - grounds[i].z, 2));
			//cout << d << endl;
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

double Constellation::calculateMaxSpaceToGndDistance(double max_deg)
{
	max_deg *= (M_PI / 180);
	return sqrt(pow(sma, 2) - pow(earth_radius * sin(max_deg), 2)) - earth_radius * cos(max_deg);
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
			map[i][j] = INT_MAX;
			path[i][j] = j;
		}
	}
	for (int i = 0; i < links.size(); i++)
	{
		map[links[i].source.id + 1][links[i].target.id + 1] = (int)links[i].distance;
		map[links[i].target.id + 1][links[i].source.id + 1] = (int)links[i].distance;
	}
	for (int i = 0; i < start.size(); i++)
	{
		map[0][start[i].id + 1] = (int)sdistance[i];
		map[start[i].id + 1][0] = (int)sdistance[i];
	}
	for (int i = 0; i < end.size(); i++)
	{
		map[n - 1][end[i].id + 1] = (int)edistance[i];
		map[end[i].id + 1][n - 1] = (int)edistance[i];
	}
	int ans = floyd(0, n - 1);
	if (ans < INT_MAX)
	{
		int k = path[0][n - 1];
		while (k != n - 1)
		{
			pathId.push_back(k - 1);
			k = path[k][n - 1];
		}
		pathDistance = ans;
	}
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
				if (map[i][j] > (map[i][k] + map[k][j]) && i != j) {

					// 距离
					map[i][j] = map[i][k] + map[k][j];

					// 路径
					path[i][j] = path[i][k];
				}
			}
		}
	}

	return map[from][to];
}

// 求最短路径
int dijkstra(int from, int to) { //从起点到目标点 

	int i, j;

	// 起点为已访问状态
	visit[from] = 1;

	// 初始化dist
	for (i = 1; i <= n; i++) {
		visit[i] = 0;   //一开始每个点都没被访问 
		dist[i] = map[from][i];  //起点到其他点的距离 
	}

	// N 个顶点需要做 N - 1 次循环
	for (i = 1; i < n; ++i) {

		//对除起点的每一个点进行最短计算 
		int min = INT_MAX;  //记录最小dist[i] 

		int pos;

		// 循环遍历所有顶点
		for (j = 1; j <= n; ++j) {

			//记录最小dist[i]的点 
			if (!visit[j] && min > dist[j]) {
				pos = j;
				min = dist[j];
			}
		}

		// 将顶点放入S中
		visit[pos] = 1;

		// 更新dist
		for (j = 1; j <= n; ++j) {
			if (!visit[j] && (dist[j] > (dist[pos] + map[pos][j]))) { //如果j节点没有被访问过&&j节点到起节点的最短路径>pos节点到起节点的最短路径+pos节点到j节点的路径  
				dist[j] = dist[pos] + map[pos][j];    //更新j节点到起节点的最短路径    
			}
		}
	}

	return dist[to];
}