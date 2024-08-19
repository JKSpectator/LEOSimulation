#pragma once
#include "OrbitalElements.h"
#include <string>
#include <vector>
//���������
namespace KeplerOrbits
{
	class EulerCoordinates;
	class CartesianCoordinates;


	typedef std::vector<CartesianCoordinates> Trajectory;//����Ĺ켣

	class OrbitBody
	{
	public:
		//��ʼ�������ID�����Ԫ�غ͹������
		OrbitBody(
			int			id,
			OrbitalElements&	orbitalElements,
			double				siderealOrbitPeriod);

		/// <summary>
		/// ���ݸ�����������(jd)�����㲢���������ŷ����
		/// </summary>
		/// <param name="jd">������</param>
		/// <returns>�����ŷ����</returns>
		EulerCoordinates GetEulaerAnglesFromJulianDay(double jd) const;

		/// <summary>
		/// ��������ĺ��ǹ�����ڣ����������������ģ�����ǣ���תһ�������ʱ��
		/// </summary>
		/// <returns>���ǹ������</returns>
		double GetSiderealOrbitPeriod() const;

		int GetID() const;

		/// <summary>
		/// ���㲢��������Ĺ켣���켣��һϵ��CartesianCoordinates����ʾ�����ڲ�ͬʱ����λ��
		/// </summary>
		/// <param name="stepCount">����</param>
		/// <param name="time_offset">ʱ��ƫ��</param>
		/// <returns>����Ĺ켣</returns>
		Trajectory GetTrajectory(int stepCount, int time_offset = 0) const;
		
	private:
		/// <summary>
		/// ���ݸ�����ƫ�����(eccentricAnomaly)�����㲢���������ŷ����
		/// </summary>
		/// <param name="eccentricAnomaly">ƫ�����</param>
		/// <returns>�����ŷ����</returns>
		EulerCoordinates GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const;

		OrbitalElements m_orbitalElements;//�������
		int m_id;
		double m_siderealOrbitPeriod;//�������
	};
}