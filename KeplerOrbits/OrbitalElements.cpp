#include "OrbitalElements.h"  // 包含轨道元素类的头文件

#define _USE_MATH_DEFINES  // 定义_USE_MATH_DEFINES以启用M_PI等数学常量

#include <cmath>  // 包含数学库
#include <limits>  // 包含标准限制库

// 定义地球的标准引力参数
double STD_GRAVITATIONAL_PARAMATER_EARTH = 3.986004418e14;

using namespace KeplerOrbits;  // 使用KeplerOrbits命名空间

// 设置椭圆的形状（偏心率和半长轴）
OrbitalElements& OrbitalElements::SetEllipseShape(double eccentricity, double semimajorAxis)
{
	m_eccentricity = eccentricity;
	m_semimajorAxis = semimajorAxis;
	return *this;
};

// 设置轨道平面（倾角和升交点经度）
OrbitalElements& OrbitalElements::SetOrbitalPlane(double inclination, double longitudeOfAscendingNode)
{
	m_inclination = inclination;
	m_longitudeOfAscendingNode = longitudeOfAscendingNode;
	return *this;
};

// 设置椭圆的定向（近点经度）
OrbitalElements& OrbitalElements::SetEllipseOrientation(double longitudeOfPerihelion)
{
	m_longitudeOfPerihelion = longitudeOfPerihelion;
	return *this;
};

// 设置与时间相关的元素（平均经度和历元）
OrbitalElements& OrbitalElements::SetTimeSpecific(double meanLongitude, double epoch)
{
	m_meanLongitude = meanLongitude;
	m_epoch = epoch;
	return *this;
};

// 获取平均近点角
double OrbitalElements::GetMeanAnomaly() const
{
	return m_meanLongitude - m_longitudeOfPerihelion;
};

// 获取时间从历元起的秒数
double OrbitalElements::GetTSE(double jd) const
{
	return jd - m_epoch;
};

// 计算偏心异常
double OrbitalElements::GetEccentricAnomaly(double m) const
{
	double e = 0.0;
	double lastDelta = 2.0;
	for (double delta = 1, e_ = m; delta < lastDelta;)
	{
		e = m_eccentricity * sin(e_) + m;
		lastDelta = delta;
#ifdef __EMSCRIPTEN__
		delta = fabs(e_ - e);
#else
		delta = std::abs(e_ - e);
#endif
		e_ = e;
	}
	return e;
};

// 计算轨道半径
double OrbitalElements::GetRadius(double eccentricAnomaly) const
{
	return m_semimajorAxis * (1 - m_eccentricity * cos(eccentricAnomaly));
};

// 获取近点幅角
double OrbitalElements::GetArgumentOfPeriapsis() const
{
	return m_longitudeOfPerihelion - m_longitudeOfAscendingNode;
};

// 计算真近点角
double OrbitalElements::GetTrueAnomaly(double eccentricAnomaly) const
{
	return 2.0 * atan2(
		sqrt(1.0 + m_eccentricity) * sin(eccentricAnomaly / 2.0),
		sqrt(1.0 - m_eccentricity) * cos(eccentricAnomaly / 2.0)
	);
};

// 获取升交点经度
double OrbitalElements::GetLongitudeOfAscendingNode() const
{
	return m_longitudeOfAscendingNode;
}

// 获取轨道倾角
double OrbitalElements::GetInclination() const
{
	return m_inclination;
}

// 获取半长轴
double OrbitalElements::GetSemimajorAxis() const
{
	return m_semimajorAxis;
}

// 计算轨道周期
double OrbitalElements::GetOrbitPeriod()
{
	double a = pow(m_semimajorAxis, 3) / STD_GRAVITATIONAL_PARAMATER_EARTH;
	return 2 * M_PI * sqrt(a);
}