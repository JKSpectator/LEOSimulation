#include "Allinclude.h"
#include "DelaySimulation.h"
#include "LEOSimulation.h"
#include "Constellation.h"
#include "ShareMemory.h"

Constellation cs;
Communication comm;
#if defined(_WIN64)

int main(int argc, char* argv[])
{
	SYSTEMTIME sys_time;
	LPVOID pBuffer;
	string strMapName("global_share_memory");
	HANDLE hMap = NULL;

	hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"global_share_memory");
	if (hMap == NULL) {
		cout << "no memorymap,i will creat a new one" << endl;
		hMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 0X1000, L"global_share_memory");
	}

	pBuffer = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	vector<double> start;

	cs = Constellation();
	comm = Communication();
	cs.attack[0].SetLatAndLon(comm.attack.latitude(), comm.attack.longitude());
	cs.attack[0].x = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[0];
	cs.attack[0].y = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[1];
	cs.attack[0].z = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[2];
	cs.AddGroundSandT(comm.source, comm.target);

	while (true)
	{
		cs.updateSatellites();
		if (cs.pathId.size() != 0)
		{
			start.clear();
			cout << "测试延迟并发送" << endl;
			//cout << "通信延迟：" << comm.communication_stt(cs.pathDistance, cs.pathState) << endl;
			start.push_back(comm.communication_stt(cs.pathDistance, cs.pathState));
			//cout << "无噪声通信延迟：" << comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState) << endl;
			start.push_back(comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState));
			//cout << "最理想通信延迟：" << comm.communication_stt_ideal(cs.pathDistance) << endl;
			start.push_back(comm.communication_stt_ideal(cs.pathDistance));
			memcpy((double*)pBuffer, &start[0], sizeof(start));
		}
	}
	UnmapViewOfFile(pBuffer);
	CloseHandle(hMap);
	return 0;
}
#elif defined(_WIN32)

GLuint texture_id;
GLfloat aspect = 1.0;//可视化窗口分辨率
vector<Vec3> linkS;//可通信链接起始点
vector<Vec3> linkT;//可通信链接终点
vector<Vec3> linkSU;//正在使用的通信链接起始点
vector<Vec3> linkTU;//正在使用的通信链接终点
Vec3 attack = Vec3(0, 0, 0);//地面攻击节点
vector<Vec3> grounds;//地面通信节点
vector<Vec3> satellitesE;//使能的卫星节点
vector<Vec3> satellitesD;//不使能的卫星节点
string bmpfile = "OpenGL/2k_earth_daymap.bmp";//地图文件

//辅助视角变化参数
bool mouseLeftDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;

void AddGround(double x, double y, double z)
{
	Vec3 p(x, y, z);
	grounds.push_back(p);
}

void AddSatelliteE(double x, double y, double z)
{
	Vec3 p(x, y, z);
	satellitesE.push_back(p);
}

void AddSatelliteD(double x, double y, double z)
{
	Vec3 p(x, y, z);
	satellitesD.push_back(p);
}

void AddLink(double xs, double ys, double zs, double xt, double yt, double zt)
{
	Vec3 s(xs, ys, zs);
	linkS.push_back(s);
	Vec3 t(xt, yt, zt);
	linkT.push_back(t);
}

void AddLinkU(double xs, double ys, double zs, double xt, double yt, double zt)
{
	Vec3 s(xs, ys, zs);
	linkSU.push_back(s);
	Vec3 t(xt, yt, zt);
	linkTU.push_back(t);
}

//经纬度转换(弧度)
BLH XYZtoLB(double X, double Y, double Z)
{
	BLH res(0, 0, 0);
	double B = 0.0, N = 0.0, H = 0.0, R0, R1, deltaH, deltaB;
	R0 = sqrt(pow(X, 2) + pow(Y, 2));
	R1 = sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2));
	//经度直接求解
	res.L = atan2(Y, X);
	//迭代求大地维度和大地高
	N = aa;
	H = R1 - aa;
	B = atan2(Z * (N + H), R0 * (N * (1 - e2) + H));
	do
	{
		deltaH = N;//判断收敛所用
		deltaB = B;
		N = aa / sqrt(1 - e2 * pow(sin(B), 2));
		H = R0 / cos(B) - N;
		B = atan2(Z * (N + H), R0 * (N * (1 - e2) + H));
	} while (fabs(deltaH - H) > 1.0e-3 && fabs(deltaB - B) > 1.0e-9);
	res.B = B;
	res.H = H;
	return res;
}

BLH XYZToLatLon(double X, double Y, double Z)
{
	BLH blh(0, 0, 0);
	blh.H = aa;
	blh.B = asin(Z / blh.H);
	blh.L = asin((Z / blh.H) / cos(blh.B));
	return blh;
}

int LoadTexture(const char* filename, int width, int height, int depth, GLenum color_type, GLenum filter_type)
{
	GLubyte* raw_bitmap;
	FILE* file;
	errno_t err = fopen_s(&file, filename, "rb");
	if (err != 0)
	{
		return 1;
	}

	raw_bitmap = (GLubyte*)malloc(width * height * depth * sizeof(GLubyte));
	if (raw_bitmap == NULL)
	{
		fclose(file);
		return 2;
	}

	fread(raw_bitmap, width * height * depth, 1, file);
	fclose(file);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, color_type, GL_UNSIGNED_BYTE, raw_bitmap);
	//    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, color_type, GL_UNSIGNED_BYTE, raw_bitmap);
	free(raw_bitmap);

	return 0;
}

void LoadTextures(GLuint texture_id, int MaxNrofTextures)
{
	string name_earth = bmpfile;
	glGenTextures(MaxNrofTextures, &texture_id);
	glBindTexture(GL_TEXTURE_2D, 1);
	if (LoadTexture(name_earth.c_str(), 2048, 1024, 3, GL_BGR_EXT, GL_LINEAR))
	{
		cout << "load texture error";
		return;
	}
}

void drawSatellite(Vec3 _p, bool enabled)
{
	glPushMatrix();

	glTranslated(_p.x, _p.y, _p.z);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glScaled(1000.0f, 1000.0f, 1000.0f);
	if(enabled ==true)
		glColor3f(0.0f, 0.0f, 1.0f);
	else
		glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	//Front Face
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	//Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	//Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	//Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	//Right Face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	//Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();



	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);*/
	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawGround(Vec3 _p)
{
	glPushMatrix();

	glTranslated(_p.x, _p.y, _p.z);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glScaled(500.0f, 500.0f, 500.0f);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	//Front Face
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	//Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	//Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	//Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	//Right Face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	//Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();



	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);*/
	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawAttack(Vec3 _p)
{
	glPushMatrix();

	glTranslated(_p.x, _p.y, _p.z);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glScaled(500.0f, 500.0f, 500.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	//Front Face
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	//Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	//Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	//Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	//Right Face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
	//Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();



	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);*/
	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawAttackSphere()
{
	glPushMatrix();
	glPointSize(5);
	glColor4f(1, 0, 0, 0.5);
	glTranslatef(attack.x, attack.y, attack.z);
	GLUquadricObj* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluSphere(quad, 20000, 50, 50);
	gluDeleteQuadric(quad);
	glPopMatrix();
	glColor4f(1, 1, 1, 1);
}

void drawEarth3D(float r)
{
	glBindTexture(GL_TEXTURE_2D, 1);
	glPushMatrix();
	glRotatef(86, 0, 0, 1);
	GLUquadricObj* sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricTexture(sphere, GL_TRUE);
	gluSphere(sphere, r, 100, 100);
	gluDeleteQuadric(sphere);
	glPopMatrix();
}

void drawLine3D(float lineWidth)
{
	glLineWidth(lineWidth);
	glPushMatrix();
	for (int i = 0; i < linkS.size(); i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(linkS[i].x, linkS[i].y, linkS[i].z);
		glColor3f(1, 1, 1);
		glVertex3f(linkT[i].x, linkT[i].y, linkT[i].z);
		glColor3f(1, 1, 1);
		glEnd();
	}
	glPopMatrix();
}

void drawLineUSing(float lineWidth)
{
	glLineWidth(lineWidth);
	glPushMatrix();
	for (int i = 0; i < linkSU.size(); i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(linkSU[i].x, linkSU[i].y, linkSU[i].z);
		glColor3f(1, 1, 0);
		glVertex3f(linkTU[i].x, linkTU[i].y, linkTU[i].z);
		glColor3f(1, 1, 0);
		glEnd();
	}
	glColor3f(1, 1, 1);
	glPopMatrix();
	glColor3f(1, 1, 1);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓存区和深度缓冲区

	float earthR = aa;
	glRotatef(cameraAngleX, 1, 0, 0);
	glRotatef(cameraAngleY, 0, 1, 0);
	cameraAngleX = 0;
	cameraAngleY = 0;

	updateConstellation();

	glPushMatrix();

	drawEarth3D(earthR);//draw earth
	//drawAttackSphere();
	for (int i = 0; i < satellitesE.size(); i++)
		drawSatellite(satellitesE[i], true);// load satellite
	for (int i = 0; i < satellitesD.size(); i++)
		drawSatellite(satellitesD[i], false);// load satellite
	for (int i = 0; i < grounds.size(); i++)
	{
		drawGround(grounds[i]);//load ground
	}
	drawAttack(attack);
	drawLine3D();// draw track lines
	drawLineUSing();
	glPopMatrix();

	glutSwapBuffers();
}

void updateConstellation()
{
	cs.updateSatellites();
	satellitesD.clear();
	satellitesE.clear();
	for (int i = 0; i < cs.satellites.size(); i++)
	{
		if (cs.satellites[i].enabled != 2)
			AddSatelliteE(cs.satellites[i].x, cs.satellites[i].y, cs.satellites[i].z);
		else
			AddSatelliteD(cs.satellites[i].x, cs.satellites[i].y, cs.satellites[i].z);
	}
	linkS.clear();
	linkT.clear();
	linkSU.clear();
	linkTU.clear();
	for (int i = 0; i < cs.links.size(); i++)
	{
		AddLink(cs.links[i].source.x, cs.links[i].source.y, cs.links[i].source.z, cs.links[i].target.x, cs.links[i].target.y, cs.links[i].target.z);
	}
	for (int i = 0; i < cs.start.size(); i++)
	{
		AddLink(cs.start[i].x, cs.start[i].y, cs.start[i].z, cs.grounds[0].x, cs.grounds[0].y, cs.grounds[0].z);
	}
	for (int i = 0; i < cs.end.size(); i++)
	{
		AddLink(cs.end[i].x, cs.end[i].y, cs.end[i].z, cs.grounds[1].x, cs.grounds[1].y, cs.grounds[1].z);
	}
	for (int i = 0; i < cs.pathId.size(); i++)
	{
		if (i == 0)
		{
			AddLinkU(cs.grounds[0].x, cs.grounds[0].y, cs.grounds[0].z, cs.satellites[cs.pathId[i]].x, cs.satellites[cs.pathId[i]].y, cs.satellites[cs.pathId[i]].z);
		}
		else
		{
			AddLinkU(cs.satellites[cs.pathId[i - 1]].x, cs.satellites[cs.pathId[i - 1]].y, cs.satellites[cs.pathId[i - 1]].z, cs.satellites[cs.pathId[i]].x, cs.satellites[cs.pathId[i]].y, cs.satellites[cs.pathId[i]].z);
		}
		if (i == cs.pathId.size() - 1)
		{
			AddLinkU(cs.grounds[1].x, cs.grounds[1].y, cs.grounds[1].z, cs.satellites[cs.pathId[i]].x, cs.satellites[cs.pathId[i]].y, cs.satellites[cs.pathId[i]].z);
		}
	}
	if (cs.pathId.size() != 0)
	{
		cout << "通信延迟：" << comm.communication_stt(cs.pathDistance, cs.pathState) << endl;
		cout << "无噪声通信延迟：" << comm.communication_stt_no_noisy(cs.pathDistance, cs.pathState) << endl;
		cout << "最理想通信延迟：" << comm.communication_stt_ideal(cs.pathDistance) << endl;
	}
}

void InitWindow(int argc, char* argv[], int width, int height, const char* title)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);//指定窗口大小
	glutInitWindowPosition(100, 100);//指定窗口左上角位置
	glutCreateWindow(title);//创建openGL渲染窗口

	glClearColor(0, 0, 0, 1);//设置清除颜色
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	LoadTextures(texture_id, 3);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void lookat(int type)
{
	switch (type) {
	case 1:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(3 * grounds[0].x, 3 * grounds[0].y, 3 * grounds[0].z, grounds[0].x, grounds[0].y, grounds[0].z, 0, 1, 0);
		break;
	default:
		break;
	}
}

void reshape(int w, int h)
{
	aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspect, 0.1, 3e5);
	lookat(1);
}

//鼠标事件
void MouseEvent(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}
}
//鼠标移动事件
void Motion(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX) * 0.1;
		cameraAngleX += (y - mouseY) * 0.1;
		mouseX = x;
		mouseY = y;
	}

	// 请求重新绘制场景
	glutPostRedisplay();
}

void IdleEvent()
{
	glutPostRedisplay();
}

void beginWindow()
{
	glutDisplayFunc(display);//添加显示回调函数

	glutIdleFunc(IdleEvent);
	glutReshapeFunc(reshape);

	glutMouseFunc(MouseEvent);
	glutMotionFunc(Motion);
	glutMainLoop();//启动程序
}

int test(int argc, char* argv[])
{
	InitWindow(argc, argv, 1920, 1080, "earth");
	return 1;
}

void testGround()
{
	for (int i = 0; i < cs.grounds.size(); i++)
	{
		AddGround(cs.grounds[i].x, cs.grounds[i].y, cs.grounds[i].z);
	}
	attack.x = cs.attack[0].x;
	attack.y = cs.attack[0].y;
	attack.z = cs.attack[0].z;
	for (int i = 0; i < cs.satellites.size(); i++)
	{
		if (cs.satellites[i].enabled != 2)
			AddSatelliteE(cs.satellites[i].x, cs.satellites[i].y, cs.satellites[i].z);
		else
			AddSatelliteD(cs.satellites[i].x, cs.satellites[i].y, cs.satellites[i].z);
	}
	for (int i = 0; i < cs.links.size(); i++)
	{
		AddLink(cs.links[i].source.x, cs.links[i].source.y, cs.links[i].source.z, cs.links[i].target.x, cs.links[i].target.y, cs.links[i].target.z);
	}
	for (int i = 0; i < cs.start.size(); i++)
	{
		AddLink(cs.start[i].x, cs.start[i].y, cs.start[i].z, cs.grounds[0].x, cs.grounds[0].y, cs.grounds[0].z);
	}
	for (int i = 0; i < cs.end.size(); i++)
	{
		AddLink(cs.end[i].x, cs.end[i].y, cs.end[i].z, cs.grounds[1].x, cs.grounds[1].y, cs.grounds[1].z);
	}
}

int main(int argc, char* argv[])
{
	cs = Constellation();
	comm = Communication();
	cs.attack[0].SetLatAndLon(comm.attack.latitude(), comm.attack.longitude());
	cs.attack[0].x = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[0];
	cs.attack[0].y = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[1];
	cs.attack[0].z = cs.LatLonToXYZ(cs.attack[0].Lat(), cs.attack[0].Lon())[2];
	cs.AddGroundSandT(comm.source, comm.target);

	test(argc, argv);
	testGround();
	beginWindow();
	return 0;
}
#else
int main(int argc, char* argv[])
{
	cout << Hello World << endl;
	return 0;
}
#endif