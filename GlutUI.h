#pragma once
#include "Allinclude.h"
#include "Constellation.h"
#define aa  6.378137e4 //6371000
#define f  (1 / 298.257223563)	//����
#define e2  (f*(2-f))			//��һƫ����ƽ��
#define PI 3.1415926

//��γ�Ⱥ�������
typedef struct BLH
{
	BLH(double b, double l, double h)
	{
		B = b;
		L = l;
		H = h;
	}
	double B;//ά��
	double L;//����
	double H;//��
}BLH;

typedef struct Vec3
{
	Vec3(double x1, double y1, double z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
	double x, y, z;
}Vec3;

void AddGround(double x, double y, double z);

void AddSatellite(double x, double y, double z);

void AddLink(double xs, double ys, double zs, double xt, double yt, double zt);

void AddLinkU(double xs, double ys, double zs, double xt, double yt, double zt);
//��γ��ת��(����)
BLH XYZtoLB(double X, double Y, double Z);

BLH XYZToLatLon(double X, double Y, double Z);


void lookat(int type);

int LoadTexture(const char* filename, int width, int height, int depth, GLenum color_type, GLenum filter_type);

void LoadTextures(GLuint* texture_id, int MaxNrofTextures);

Vec3 calSatellitePosition(float r);

void drawSatellite(Vec3 _p, bool enabled = true);

void drawGround(Vec3 _p);

void drawAttackSphere();

void drawEarth3D(float r);

void drawLine3D(float lineWidth = 1.0f);
void drawLineUSing(float lineWidth = 5.0f);

void display();

void keyboard(unsigned char key, int x, int y);

void InitWindow(int argc, char* argv[], int width, int height, const char* title);

void changeView();

void reshape(int w, int h);

void specialKeys(int key, int x, int y);


//����¼�
void MouseEvent(int button, int state, int x, int y);
//����ƶ��¼�
void Motion(int button, int state, int x, int y);

void IdleEvent();

int test(int argc, char* argv[]);

void beginWindow();