#pragma once
#include "OrbitalElements.h"
#include <string>
#include <vector>

namespace KeplerOrbits
{
	class EulerCoordinates;
	class CartesianCoordinates;


	typedef std::vector<CartesianCoordinates> Trajectory;

	class OrbitBody
	{
	public:
		//��ʼ�������ID�����Ԫ�غ͹������
		OrbitBody(
			int			id,
			OrbitalElements&	orbitalElements,
			double				siderealOrbitPeriod);
		//���ݸ�����������(jd)�����㲢���������ŷ����
		EulerCoordinates GetEulaerAnglesFromJulianDay(double jd) const;
		//��������ĺ��ǹ�����ڣ����������������ģ�����ǣ���תһ�������ʱ��
		double GetSiderealOrbitPeriod() const;

		int GetID() const;
		//���ݸ����Ĳ���(stepCount)�����㲢��������Ĺ켣���켣��һϵ��CartesianCoordinates����ʾ�����ڲ�ͬʱ����λ��
		Trajectory GetTrajectory(int stepCount, int time_offset = 0) const;
		
	private:
		//���ݸ�����ƫ�����(eccentricAnomaly)�����㲢���������ŷ���ǡ�����GetEulaerAnglesFromJulianDay������һ����������
		EulerCoordinates GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const;

		OrbitalElements m_orbitalElements;//�������
		int m_id;
		double m_siderealOrbitPeriod;//�������
	};
}