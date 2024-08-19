#pragma once
#include "Allinclude.h"
#include "Constellation.h"
#define aa  6.378137e4 //6371000
#define f  (1 / 298.257223563)	//����
#define e2  (f*(2-f))			//��һƫ����ƽ��

//��γ��
struct BLH
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
};

//XYZ����
struct Vec3
{
	Vec3(double x1, double y1, double z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
	double x, y, z;
};

/// <summary>
/// ���ݲ���---���½�ؽڵ�
/// </summary>
/// <param name="x">����X����</param>
/// <param name="y">����Y����</param>
/// <param name="z">����Z����</param>
void AddGround(double x, double y, double z);

/// <summary>
/// ���ݲ���---���ʹ�����ǽڵ�
/// </summary>
/// <param name="x">����X����</param>
/// <param name="y">����Y����</param>
/// <param name="z">����Z����</param>
void AddSatelliteE(double x, double y, double z);

/// <summary>
/// ���ݲ���---��Ӳ�ʹ�����ǽڵ�
/// </summary>
/// <param name="x">����X����</param>
/// <param name="y">����Y����</param>
/// <param name="z">����Z����</param>
void AddSatelliteD(double x, double y, double z);

/// <summary>
/// ���ݲ���---���ͨ������
/// </summary>
/// <param name="xs">Դ�ڵ�X����</param>
/// <param name="ys">Դ�ڵ�Y����</param>
/// <param name="zs">Դ�ڵ�Z����</param>
/// <param name="xt">Ŀ��ڵ�X����</param>
/// <param name="yt">Ŀ��ڵ�Y����</param>
/// <param name="zt">Ŀ��ڵ�Z����</param>
void AddLink(double xs, double ys, double zs, double xt, double yt, double zt);

/// <summary>
/// ���ݲ���---���ʹ�õ�ͨ������
/// </summary>
/// <param name="xs">Դ�ڵ�X����</param>
/// <param name="ys">Դ�ڵ�Y����</param>
/// <param name="zs">Դ�ڵ�Z����</param>
/// <param name="xt">Ŀ��ڵ�X����</param>
/// <param name="yt">Ŀ��ڵ�Y����</param>
/// <param name="zt">Ŀ��ڵ�Z����</param>
void AddLinkU(double xs, double ys, double zs, double xt, double yt, double zt);

//��γ��ת��(����)
BLH XYZtoLB(double X, double Y, double Z);
BLH XYZToLatLon(double X, double Y, double Z);

/// <summary>
/// ���ӻ�����---������Ӱ��ע��
/// </summary>
void lookat(int type);

/// <summary>
/// ���ӻ�����---���ļ����ز���
/// </summary>
int LoadTexture(const char* filename, int width, int height, int depth, GLenum color_type, GLenum filter_type);

/// <summary>
/// ���ӻ�����---���ز��ʵ�ʹ�ó���
/// </summary>
void LoadTextures(GLuint* texture_id, int MaxNrofTextures);

/// <summary>
/// ���ӻ�����---�������ǽڵ�
/// </summary>
void drawSatellite(Vec3 _p, bool enabled = true);

/// <summary>
/// ���ӻ�����---λ��½�ؽڵ�
/// </summary>
void drawGround(Vec3 _p);

/// <summary>
/// ���ӻ�����---���ƹ�����Χ
/// </summary>
void drawAttackSphere();

/// <summary>
/// ���ӻ�����---����3D����
/// </summary>
void drawEarth3D(float r);

/// <summary>
/// ���ӻ�����---��������
/// </summary>
void drawLine3D(float lineWidth = 2.0f);

/// <summary>
/// ���ӻ�����---����ʹ�õ�������
/// </summary>
void drawLineUSing(float lineWidth = 5.0f);

/// <summary>
/// ���ӻ�����---���»���ĺ����ص�
/// </summary>
void display();

/// <summary>
/// ���ݲ���---������������
/// </summary>
void updateConstellation();

/// <summary>
/// ���ӻ�����---��ʼ�����ӻ�����
/// </summary>
void InitWindow(int argc, char* argv[], int width, int height, const char* title);

/// <summary>
/// ���ӻ�����---�޸Ĵ�����״�ĺ����ص�
/// </summary>
void reshape(int w, int h);

/// <summary>
/// ���ӻ�����---������¼������ص�
/// </summary>
void MouseEvent(int button, int state, int x, int y);

/// <summary>
/// ���ӻ�����---����ƶ��¼������ص�
/// </summary>
void Motion(int button, int state, int x, int y);

/// <summary>
/// ���ӻ�����---һ��״̬�¼������ص�
/// </summary>
void IdleEvent();

/// <summary>
/// ���ӻ�����---���Կ��ӻ�����
/// </summary>
int test(int argc, char* argv[]);

/// <summary>
/// ���ӻ�����---��ʼ���ƿ��ӻ�����
/// </summary>
void beginWindow();