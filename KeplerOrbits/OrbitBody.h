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
		//初始化天体的ID、轨道元素和轨道周期
		OrbitBody(
			int			id,
			OrbitalElements&	orbitalElements,
			double				siderealOrbitPeriod);
		//根据给定的儒略日(jd)，计算并返回天体的欧拉角
		EulerCoordinates GetEulaerAnglesFromJulianDay(double jd) const;
		//返回天体的恒星轨道周期，即天体绕其轨道中心（如恒星）旋转一周所需的时间
		double GetSiderealOrbitPeriod() const;

		int GetID() const;
		//根据给定的步数(stepCount)，计算并返回天体的轨迹。轨迹是一系列CartesianCoordinates，表示天体在不同时间点的位置
		Trajectory GetTrajectory(int stepCount, int time_offset = 0) const;
		
	private:
		//根据给定的偏近点角(eccentricAnomaly)，计算并返回天体的欧拉角。这是GetEulaerAnglesFromJulianDay函数的一个辅助函数
		EulerCoordinates GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const;

		OrbitalElements m_orbitalElements;//轨道属性
		int m_id;
		double m_siderealOrbitPeriod;//轨道周期
	};
}