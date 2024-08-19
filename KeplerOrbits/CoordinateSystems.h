#pragma once
//����ϵͷ�ļ�
#include <iostream>

namespace KeplerOrbits
{
	//���ڼ������������(jd)�ĸ�������ƽ������ʱ(GMST)����������ѧ������ʱ�������һ������
	double GetGMST(double jd);

	//�洢������������Ԫ�أ���볤�ᡢƫ���ʡ���ǵ�
	class OrbitalElements;

	//��ά�ռ��еĵѿ�������ϵ
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
	
	//��������ϵ
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

	//��ƽ����ϵ���洢��ķ�λ��(azimuth)�͸߶Ƚ�(altitude)����λ��������ڱ����ĽǶȣ��߶Ƚ�������ڵ�ƽ�ߵĽǶȡ�
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

	//ŷ��������ϵ����ͨ������������ά�ռ��е���ת�������������Ƕȣ�phi��theta��psi���Լ�һ����ʾ�����r
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

	//�������ϵ������һ������ѧ�г��õ�����ϵͳ���洢��ĳྭ(alpha)�ͳ�γ(delta)
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

	//�Ƶ�����ϵ��������һ������ѧ��ʹ�õ�����ϵͳ���洢��Ļƾ�(lambda)�ͻ�γ(betta)
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