#define _USE_MATH_DEFINES  // 定义_USE_MATH_DEFINES以启用M_PI等数学常量

#include "CoordinateSystems.h"  // 包含坐标系统相关的头文件
#include "OrbitalElements.h"  // 包含轨道元素相关的头文件
#include <cmath>  // 包含数学库

using namespace KeplerOrbits;  // 使用KeplerOrbits命名空间

// KeplerOrbits命名空间内定义的函数，计算格林尼治平均恒星时
namespace KeplerOrbits
{
	double GetGMST(double jd)
	{
		double x;
		x = 18.697374558 + 24.06570982441908 * (jd - 2451545);
		return ((x / 24) - floor(x / 24)) * 24;
	}
}

// 静态函数，计算地方恒星时
static double getlha(double alpha, double lambda, double jd)
{
	double gmst = GetGMST(jd);
	return (double)(gmst) / 12 * M_PI + lambda - alpha;
}

// 构造函数，初始化笛卡尔坐标为(0,0,0)
CartesianCoordinates::CartesianCoordinates() :
	m_x(0),
	m_y(0),
	m_z(0)
{
}

// 构造函数，初始化笛卡尔坐标为(x,y,z)
CartesianCoordinates::CartesianCoordinates(double x, double y, double z) :
	m_x(x),
	m_y(y),
	m_z(z)
{};

// 构造函数，初始化欧拉坐标
EulerCoordinates::EulerCoordinates(double phi, double theta, double psi, double r) :
	phi(phi),
	theta(theta),
	psi(psi),
	r(r)
{};

// 将欧拉坐标转换为笛卡尔坐标
EulerCoordinates::operator CartesianCoordinates() const
{
	double x = r * (cos(phi) * cos(theta) - sin(phi) * cos(psi) * sin(theta));
	double y = r * (sin(phi) * cos(theta) * cos(psi) + cos(phi) * sin(theta));
	double z = r * (sin(phi) * sin(psi));
	return CartesianCoordinates(x, y, z);
};

// 构造函数，初始化黄道坐标为(0,0)
EcliptikCoordinates::EcliptikCoordinates() : m_lambda(0), m_betta(0)
{
}

// 构造函数，初始化黄道坐标为(lambda,betta)
EcliptikCoordinates::EcliptikCoordinates(double lambda, double betta) : m_lambda(lambda), m_betta(betta)
{
}

// 从笛卡尔坐标和观测者位置转换为黄道坐标
EcliptikCoordinates::EcliptikCoordinates(const CartesianCoordinates& cartesian, const CartesianCoordinates& observerPosition)
{
	double x = cartesian.x() - observerPosition.x();
	double y = cartesian.y() - observerPosition.y();
	double z = cartesian.z() - observerPosition.z();

	m_lambda = atan2(y, x);
	if (m_lambda < 0)
	{
		m_lambda = m_lambda + 2 * M_PI;
	}
	m_betta = atan2(z, sqrt(x * x + y * y));
}

// 将黄道坐标转换为赤道坐标
EcliptikCoordinates::operator EquatorialCoordinates() const
{
	double k_obliquityOfTheEcliptic = 23.439281 / 180.0 * M_PI;

	double alpha = atan2(
		sin(m_lambda) * cos(k_obliquityOfTheEcliptic) - tan(m_betta) * sin(k_obliquityOfTheEcliptic),
		cos(m_lambda)
	);
	if (alpha < 0)
	{
		alpha = alpha + 2 * M_PI;
	}

	double delta = asin(
		sin(m_betta) * cos(k_obliquityOfTheEcliptic) +
		cos(m_betta) * sin(k_obliquityOfTheEcliptic) * sin(m_lambda)
	);

	return EquatorialCoordinates(alpha, delta);
}

// 构造函数，初始化赤道坐标为(0,0)
EquatorialCoordinates::EquatorialCoordinates() :m_alpha(0), m_delta(0)
{
}

// 构造函数，初始化赤道坐标为(alpha,delta)
EquatorialCoordinates::EquatorialCoordinates(double alpha, double delta) :m_alpha(alpha), m_delta(delta)
{
}

// 构造函数，初始化地平坐标为(0,0)
HorizontalCoordinates::HorizontalCoordinates() : m_azimuth(0), m_altitude(0)
{
}

// 构造函数，初始化地平坐标为(azimuth,altitude)
HorizontalCoordinates::HorizontalCoordinates(double azimuth, double altitude) : m_azimuth(azimuth), m_altitude(altitude)
{
}

// 将赤道坐标转换为地平坐标
HorizontalCoordinates EquatorialCoordinates::ConvertToHorizontalCoordinates(const GeoCoordinates& observer, double jd) const
{
	double h = getlha(m_alpha, observer.longitude(), jd);
	if (h < 0)
	{
		h = h + 2 * M_PI;
	}
	double azimuth = atan2(
		sin(h),
		cos(h) * sin(observer.latitude()) - tan(m_delta) * cos(observer.latitude())
	);
	double altitude = asin(sin(observer.latitude()) * sin(m_delta) + cos(observer.latitude()) * cos(m_delta) * cos(h));
	return HorizontalCoordinates(azimuth, altitude);
}

// 将地平坐标转换为极坐标
CartesianCoordinates HorizontalCoordinates::ConvertToPolar() const
{
	// Note that Azimuth(A) is measured from the South point, turning positive to the West.
	return CartesianCoordinates(
		sin(m_azimuth) * (M_PI / 2.0 - m_altitude),
		-cos(m_azimuth) * (M_PI / 2.0 - m_altitude),
		0.0);
}

// 构造函数，初始化地理坐标为(0,0,0)
GeoCoordinates::GeoCoordinates() : m_latitude(0), m_longitude(0), m_height(0)
{
}

// 构造函数，初始化地理坐标为(latitude,longitude,height)
GeoCoordinates::GeoCoordinates(double latitude, double longitude, double height) : m_latitude(latitude), m_longitude(longitude), m_height(height)
{
}

// 将地理坐标转换为笛卡尔坐标
GeoCoordinates::operator CartesianCoordinates() const
{
	double lon = m_longitude * M_PI / 180;      //经度
	double lat = m_latitude * M_PI / 180;
	double hei = m_height;

	// variable
	double a = 6378137.0;       //地球赤道半径 ，单位是 m
	double b = 6356752.31424518;    //地球短半轴 ，单位是 m
	double N = a / (sqrt(1 - ((a * a - b * b) / (a * a)) * sin(lat) * sin(lat)));

	return CartesianCoordinates((N + hei) * cos(lat) * cos(lon), (N + hei) * cos(lat) * sin(lon), ((b * b * N) / (a * a) + hei) * sin(lat));
}

// 将弧度转换为度
double ToDeg(double rad)
{
	return rad / M_PI * 180;
}

// 输出运算符重载，用于输出笛卡尔坐标
std::ostream& KeplerOrbits::operator << (std::ostream& os, const CartesianCoordinates& cartesian)
{
	return os << "x: " << cartesian.x() << " y: " << cartesian.y() << " z: " << cartesian.z();
}

// 输出运算符重载，用于输出地平坐标
std::ostream& KeplerOrbits::operator << (std::ostream& os, const HorizontalCoordinates& horizontal)
{
	return os << "azimuth : " << ToDeg(horizontal.m_azimuth) << " altitude: " << ToDeg(horizontal.m_altitude);
}

// 输出运算符重载，用于输出赤道坐标
std::ostream& KeplerOrbits::operator << (std::ostream& os, const EquatorialCoordinates& equatorial)
{
	return os << "right ascension: " << ToDeg(equatorial.m_alpha) << " declination: " << ToDeg(equatorial.m_delta);
}

// 输出运算符重载，用于输出黄道坐标
std::ostream& KeplerOrbits::operator << (std::ostream& os, const EcliptikCoordinates& ecliptik)
{
	return os << "longitude: " << ToDeg(ecliptik.m_lambda) << " latitude: " << ToDeg(ecliptik.m_betta);
}

// 返回笛卡尔坐标的x分量的引用
double& CartesianCoordinates::x()
{
	return m_x;
};

// 返回笛卡尔坐标的x分量的常量引用（const版本）
const double& CartesianCoordinates::x() const
{
	return m_x;
};

// 返回笛卡尔坐标的y分量的引用
double& CartesianCoordinates::y()
{
	return m_y;
};

// 返回笛卡尔坐标的y分量的常量引用（const版本）
const double& CartesianCoordinates::y() const
{
	return m_y;
};

// 返回笛卡尔坐标的z分量的引用
double& CartesianCoordinates::z()
{
	return m_z;
};

// 返回笛卡尔坐标的z分量的常量引用（const版本）
const double & CartesianCoordinates::z() const
{
	return m_z;
};

// 返回地理坐标的纬度的引用
double& GeoCoordinates::latitude()
{
	return m_latitude;
}

// 返回地理坐标的纬度的常量引用（const版本）
const double& GeoCoordinates::latitude() const
{
	return m_latitude;
}

// 返回地理坐标的经度的引用
double& GeoCoordinates::longitude()
{
	return m_longitude;
}

// 返回地理坐标的经度的常量引用（const版本）
const double& GeoCoordinates::longitude() const
{
	return m_longitude;
}

// 返回地理坐标的高度的引用
double& GeoCoordinates::height()
{
	return m_height;
}

// 返回地理坐标的高度的常量引用（const版本）
const double& GeoCoordinates::height() const
{
	return m_height;
}