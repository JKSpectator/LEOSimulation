#pragma once
#include "OrbitalElements.h"
#include <string>
#include <vector>
//轨道数据类
namespace KeplerOrbits
{
	class EulerCoordinates;
	class CartesianCoordinates;


	typedef std::vector<CartesianCoordinates> Trajectory;//天体的轨迹

	class OrbitBody
	{
	public:
		//初始化天体的ID、轨道元素和轨道周期
		OrbitBody(
			int			id,
			OrbitalElements&	orbitalElements,
			double				siderealOrbitPeriod);

		/// <summary>
		/// 根据给定的儒略日(jd)，计算并返回天体的欧拉角
		/// </summary>
		/// <param name="jd">儒略日</param>
		/// <returns>天体的欧拉角</returns>
		EulerCoordinates GetEulaerAnglesFromJulianDay(double jd) const;

		/// <summary>
		/// 返回天体的恒星轨道周期，即天体绕其轨道中心（如恒星）旋转一周所需的时间
		/// </summary>
		/// <returns>恒星轨道周期</returns>
		double GetSiderealOrbitPeriod() const;

		int GetID() const;

		double GetSma() const;

		/// <summary>
		/// 计算并返回天体的轨迹。轨迹是一系列CartesianCoordinates，表示天体在不同时间点的位置
		/// </summary>
		/// <param name="stepCount">步数</param>
		/// <param name="time_offset">时间偏移</param>
		/// <returns>天体的轨迹</returns>
		Trajectory GetTrajectory(int stepCount, int time_offset = 0) const;
		
	private:
		/// <summary>
		/// 根据给定的偏近点角(eccentricAnomaly)，计算并返回天体的欧拉角
		/// </summary>
		/// <param name="eccentricAnomaly">偏近点角</param>
		/// <returns>天体的欧拉角</returns>
		EulerCoordinates GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const;

		OrbitalElements m_orbitalElements;//轨道属性
		int m_id;
		double m_siderealOrbitPeriod;//轨道周期
	};
}