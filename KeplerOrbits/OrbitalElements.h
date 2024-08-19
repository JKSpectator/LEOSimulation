#pragma once
//轨道形状参数
namespace KeplerOrbits
{
	class OrbitalElements
	{
	public:
		/// <summary>
		/// 设置椭圆轨道的形状，其中偏心率定义了轨道的扁平程度，半长轴是轨道最长直径的一半
		/// </summary>
		/// <param name="eccentricity">偏心率</param>
		/// <param name="semimajorAxis">长半轴</param>
		/// <returns>形状参数</returns>
		OrbitalElements& SetEllipseShape(double eccentricity, double semimajorAxis);
		/// <summary>
		/// 设置轨道平面相对于参考平面（通常是地球赤道面）的倾斜和升交点的位置
		/// </summary>
		/// <param name="inclination">倾角</param>
		/// <param name="longitudeOfAscendingNode">升交点黄经</param>
		/// <returns>形状参数</returns>
		OrbitalElements& SetOrbitalPlane(double inclination, double longitudeOfAscendingNode);
		/// <summary>
		/// 设置椭圆轨道上近点（轨道上离中心天体最近的点）的位置
		/// </summary>
		/// <param name="longitudeOfPerihelion">近点黄经</param>
		/// <returns>形状参数</returns>
		OrbitalElements& SetEllipseOrientation(double longitudeOfPerihelion);
		/// <summary>
		/// 设置与时间相关的轨道元素，平均黄经是天体在参考时间点的平均位置，参考时间点是这些轨道元素有效的时间
		/// </summary>
		/// <param name="meanLongitude">平均黄经</param>
		/// <param name="epoch">参考时间点，通常为儒略日</param>
		/// <returns>形状参数</returns>
		OrbitalElements& SetTimeSpecific(double meanLongitude, double epoch);

		/// <summary>
		/// 计算并返回天体的平近点角
		/// </summary>
		/// <returns>平近点角</returns>
		double GetMeanAnomaly() const;

		/// <summary>
		/// 根据输入的儒略日（Julian Date）计算并返回天体在该日期的真实近点角（True Anomaly, ν）
		/// </summary>
		/// <param name="jd">儒略日</param>
		/// <returns>真实近点角</returns>
		double GetTSE(double jd) const;

		/// <summary>
		/// 根据输入的平近点角（Mean Anomaly），计算并返回天体的偏近点角（Eccentric Anomaly, E）
		/// </summary>
		/// <param name="m">平近点角</param>
		/// <returns>偏近点角</returns>
		double GetEccentricAnomaly(double m) const;

		/// <summary>
		/// 根据输入的偏近点角（Eccentric Anomaly），计算并返回天体在轨道上的当前半径或距离中心天体的距离
		/// </summary>
		/// <param name="eccentricAnomaly">偏近点角</param>
		/// <returns>距离中心天体的距离</returns>
		double GetRadius(double eccentricAnomaly) const;

		/// <summary>
		/// 计算并返回天体的近点幅角（Argument of Periapsis, ω），这是从升交点到近点的角度，是描述轨道形状的另一个角度参数
		/// </summary>
		/// <returns>近点幅角</returns>
		double GetArgumentOfPeriapsis() const;

		/// <summary>
		/// 根据输入的偏近点角（Eccentric Anomaly），计算并返回天体的真实近点角（True Anomaly, ν）。真实近点角是天体在轨道上的实际位置角度，相对于近点
		/// </summary>
		/// <param name="eccentricAnomaly">偏近点角</param>
		/// <returns>真实近点角</returns>
		double GetTrueAnomaly(double eccentricAnomaly) const;

		/// <summary>
		/// 计算并返回天体轨道的升交点赤经（Longitude of the Ascending Node, Ω），这是轨道平面与参考平面（赤道）相交的点的经度
		/// </summary>
		/// <returns>升交点赤经</returns>
		double GetLongitudeOfAscendingNode() const;

		/// <summary>
		/// 返回天体轨道的倾角（Inclination, i），这是轨道平面相对于参考平面（如赤道或黄道）的倾斜角度
		/// </summary>
		/// <returns>天体轨道的倾角</returns>
		double GetInclination() const;

		/// <summary>
		/// 计算并返回天体的轨道周期（Orbit Period, T），这是天体绕中心天体一周所需的时间
		/// </summary>
		/// <returns>天体的轨道周期</returns>
		double GetOrbitPeriod();

	private:
		//偏心率
		double m_eccentricity;
		//长半轴
		double m_semimajorAxis;
		//倾角
		double m_inclination;
		//升交点黄经
		double m_longitudeOfAscendingNode;
		//近点黄经
		double m_longitudeOfPerihelion;
		//平均黄经
		double m_meanLongitude;
		//参考时间点，通常为儒略日
		double m_epoch;
	};
}