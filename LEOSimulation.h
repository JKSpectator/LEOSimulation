#pragma once
#include "Allinclude.h"
#include "Constellation.h"
#define aa  6.378137e4 //6371000
#define f  (1 / 298.257223563)	//扁率
#define e2  (f*(2-f))			//第一偏心率平方

//经纬度
struct BLH
{
	BLH(double b, double l, double h)
	{
		B = b;
		L = l;
		H = h;
	}
	double B;//维度
	double L;//经度
	double H;//高
};

//XYZ坐标
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
/// 数据部分---添加陆地节点
/// </summary>
/// <param name="x">世界X坐标</param>
/// <param name="y">世界Y坐标</param>
/// <param name="z">世界Z坐标</param>
void AddGround(double x, double y, double z);

/// <summary>
/// 数据部分---添加使能卫星节点
/// </summary>
/// <param name="x">世界X坐标</param>
/// <param name="y">世界Y坐标</param>
/// <param name="z">世界Z坐标</param>
void AddSatelliteE(double x, double y, double z);

/// <summary>
/// 数据部分---添加不使能卫星节点
/// </summary>
/// <param name="x">世界X坐标</param>
/// <param name="y">世界Y坐标</param>
/// <param name="z">世界Z坐标</param>
void AddSatelliteD(double x, double y, double z);

/// <summary>
/// 数据部分---添加通信链接
/// </summary>
/// <param name="xs">源节点X坐标</param>
/// <param name="ys">源节点Y坐标</param>
/// <param name="zs">源节点Z坐标</param>
/// <param name="xt">目标节点X坐标</param>
/// <param name="yt">目标节点Y坐标</param>
/// <param name="zt">目标节点Z坐标</param>
void AddLink(double xs, double ys, double zs, double xt, double yt, double zt);

/// <summary>
/// 数据部分---添加使用的通信链接
/// </summary>
/// <param name="xs">源节点X坐标</param>
/// <param name="ys">源节点Y坐标</param>
/// <param name="zs">源节点Z坐标</param>
/// <param name="xt">目标节点X坐标</param>
/// <param name="yt">目标节点Y坐标</param>
/// <param name="zt">目标节点Z坐标</param>
void AddLinkU(double xs, double ys, double zs, double xt, double yt, double zt);

//经纬度转换(弧度)
BLH XYZtoLB(double X, double Y, double Z);
BLH XYZToLatLon(double X, double Y, double Z);

/// <summary>
/// 可视化部分---设置摄影机注视
/// </summary>
void lookat(int type);

/// <summary>
/// 可视化部分---从文件加载材质
/// </summary>
int LoadTexture(const char* filename, int width, int height, int depth, GLenum color_type, GLenum filter_type);

/// <summary>
/// 可视化部分---加载材质到使用场景
/// </summary>
void LoadTextures(GLuint* texture_id, int MaxNrofTextures);

/// <summary>
/// 可视化部分---绘制卫星节点
/// </summary>
void drawSatellite(Vec3 _p, bool enabled = true);

/// <summary>
/// 可视化部分---位置陆地节点
/// </summary>
void drawGround(Vec3 _p);

/// <summary>
/// 可视化部分---绘制攻击范围
/// </summary>
void drawAttackSphere();

/// <summary>
/// 可视化部分---绘制3D地球
/// </summary>
void drawEarth3D(float r);

/// <summary>
/// 可视化部分---绘制链接
/// </summary>
void drawLine3D(float lineWidth = 2.0f);

/// <summary>
/// 可视化部分---绘制使用的链接线
/// </summary>
void drawLineUSing(float lineWidth = 5.0f);

/// <summary>
/// 可视化部分---更新画面的函数回调
/// </summary>
void display();

/// <summary>
/// 数据部分---更新卫星数据
/// </summary>
void updateConstellation();

/// <summary>
/// 可视化部分---初始化可视化窗口
/// </summary>
void InitWindow(int argc, char* argv[], int width, int height, const char* title);

/// <summary>
/// 可视化部分---修改窗口形状的函数回调
/// </summary>
void reshape(int w, int h);

/// <summary>
/// 可视化部分---鼠标点击事件函数回调
/// </summary>
void MouseEvent(int button, int state, int x, int y);

/// <summary>
/// 可视化部分---鼠标移动事件函数回调
/// </summary>
void Motion(int button, int state, int x, int y);

/// <summary>
/// 可视化部分---一般状态事件函数回调
/// </summary>
void IdleEvent();

/// <summary>
/// 可视化部分---测试可视化函数
/// </summary>
int test(int argc, char* argv[]);

/// <summary>
/// 可视化部分---开始绘制可视化窗口
/// </summary>
void beginWindow();