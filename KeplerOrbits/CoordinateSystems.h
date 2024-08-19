#pragma once
//坐标系头文件
#include <iostream>

namespace KeplerOrbits
{
	//用于计算给定儒略日(jd)的格林尼治平均恒星时(GMST)，这是天文学中用于时间测量的一个参数
	double GetGMST(double jd);

	//存储描述天体轨道的元素，如半长轴、偏心率、倾角等
	class OrbitalElements;

	//三维空间中的笛卡尔坐标系
	class CartesianCoordinates
	{
	public:
		CartesianCoordinates();
		CartesianCoordinates(double x, double y, double z);

		double& x();
		const double& x() const;

		double& y();
		const double& y() const;

		double& z();
		const double& z() const;

	private:
		double m_x, m_y, m_z;
	};
	
	//地理坐标系
	class GeoCoordinates
	{
	public:
		GeoCoordinates();

		GeoCoordinates(double latitude, double longitude, double height);

		double& latitude();
		const double& latitude() const;

		double& longitude();
		const double& longitude() const;

		double& height();
		const double& height() const;

		operator CartesianCoordinates() const;

	private:
		double m_latitude, m_longitude, m_height;
	};

	//地平坐标系，存储点的方位角(azimuth)和高度角(altitude)。方位角是相对于北方的角度，高度角是相对于地平线的角度。
	class HorizontalCoordinates
	{
		friend std::ostream& operator << (std::ostream& os, const HorizontalCoordinates& horizontal);
	public:
		HorizontalCoordinates();

		HorizontalCoordinates(double azimuth, double altitude);

		CartesianCoordinates ConvertToPolar() const;
		
	private:
		double m_azimuth, m_altitude;
	};

	//欧拉角坐标系，这通常用于描述三维空间中的旋转。它包含三个角度：phi、theta和psi，以及一个表示距离的r
	class EulerCoordinates
	{
	public:
		EulerCoordinates();

		EulerCoordinates(double phi, double theta, double psi, double r);
		
		operator CartesianCoordinates() const;

		std::ostream& operator<<(std::ostream& os) const;

	private:
		double phi, theta, psi, r;
	};

	//赤道坐标系，这是一种天文学中常用的坐标系统，存储点的赤经(alpha)和赤纬(delta)
	class EquatorialCoordinates
	{
		friend std::ostream& operator << (std::ostream& os, const EquatorialCoordinates& cartesian);
	public:
		EquatorialCoordinates();

		EquatorialCoordinates(double alpha, double delta);

		HorizontalCoordinates ConvertToHorizontalCoordinates(const GeoCoordinates& observer, double jd) const;
		
	private:
		double m_alpha, m_delta;
	};

	//黄道坐标系，这是另一种天文学中使用的坐标系统，存储点的黄经(lambda)和黄纬(betta)
	class EcliptikCoordinates
	{
		friend std::ostream& operator << (std::ostream& os, const EcliptikCoordinates& cartesian);
	public:
		EcliptikCoordinates();

		EcliptikCoordinates(double lambda, double betta);

		EcliptikCoordinates(const CartesianCoordinates& cartesian, const CartesianCoordinates& observerPosition);

		operator EquatorialCoordinates() const;
		
	private:
		double m_lambda, m_betta;
	};

	std::ostream& operator << (std::ostream& os, const CartesianCoordinates& cartesian);
	std::ostream& operator << (std::ostream& os, const HorizontalCoordinates& horizontal);
	std::ostream& operator << (std::ostream& os, const EquatorialCoordinates& equatorial);
	std::ostream& operator << (std::ostream& os, const EcliptikCoordinates& ecliptik);
}