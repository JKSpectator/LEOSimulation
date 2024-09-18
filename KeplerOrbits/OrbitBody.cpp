#include "OrbitBody.h"  // 包含OrbitBody类的头文件

#include "CoordinateSystems.h"  // 包含坐标系统相关的头文件

#define _USE_MATH_DEFINES  // 定义_USE_MATH_DEFINES以启用M_PI等数学常量

#include <math.h>  // 包含数学库

using namespace KeplerOrbits;  // 使用KeplerOrbits命名空间

// 构造函数，初始化OrbitBody对象
OrbitBody::OrbitBody(int id, OrbitalElements& orbitalElements, double siderealOrbitPeriod) :
	m_id(id),
	m_orbitalElements(orbitalElements),
	m_siderealOrbitPeriod(siderealOrbitPeriod)
{};

// 根据偏心异常计算欧拉角
EulerCoordinates OrbitBody::GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const
{
	double trueAnomaly = m_orbitalElements.GetTrueAnomaly(eccentricAnomaly);  // 计算真近点角
	double argumentOfPeriapsis = m_orbitalElements.GetArgumentOfPeriapsis();  // 计算近点幅角

	double psi = trueAnomaly + argumentOfPeriapsis;  // 计算ψ角

	double r = m_orbitalElements.GetRadius(eccentricAnomaly);  // 计算轨道半径
	return EulerCoordinates(  // 返回欧拉坐标
		psi,
		m_orbitalElements.GetLongitudeOfAscendingNode(),  // 升交点经度
		m_orbitalElements.GetInclination(),  // 轨道倾角
		r);
}

// 根据儒略日计算欧拉角
EulerCoordinates OrbitBody::GetEulaerAnglesFromJulianDay(double jd) const
{
	double meanMotion = 2.0 * M_PI / m_siderealOrbitPeriod;  // 计算平均运动
	double currentMeanAnomaly = m_orbitalElements.GetMeanAnomaly() + meanMotion * m_orbitalElements.GetTSE(jd);  // 计算当前平均近点角

	double eccentricAnomaly = m_orbitalElements.GetEccentricAnomaly(currentMeanAnomaly);  // 计算偏心异常

	return GetEulaerAnglesFromEccentricAnomaly(eccentricAnomaly);  // 根据偏心异常计算欧拉角
};

// 获取恒星轨道周期
double OrbitBody::GetSiderealOrbitPeriod() const
{
	return m_siderealOrbitPeriod;
}

// 获取轨道体的ID
int OrbitBody::GetID() const
{
	return m_id;
}

// 获取半长轴
double OrbitBody::GetSma() const
{
	return 		m_orbitalElements.GetSemimajorAxis();
}

// 获取轨迹
Trajectory OrbitBody::GetTrajectory(int stepCount, int time_offset) const
{
	Trajectory trajectory;  // 定义轨迹对象
	trajectory.resize(stepCount);  // 调整轨迹大小
	double t = 0;
	// time_offset %= 72;  // 这行代码被注释掉了，可能是用于调整时间偏移
	for (int j = 0; j < stepCount; j++)
	{
		t = j * 2 * M_PI / stepCount + 0.1 * time_offset * M_PI / 180;  // 计算每个步骤的时间
		trajectory[j] = GetEulaerAnglesFromEccentricAnomaly(t);  // 根据时间计算欧拉角并存储在轨迹中
	}
	return trajectory;  // 返回轨迹
}