﻿#include "OrbitalElements.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
double STD_GRAVITATIONAL_PARAMATER_EARTH = 3.986004418e14;//引力常数
using namespace KeplerOrbits;

OrbitalElements& OrbitalElements::SetEllipseShape(double eccentricity, double semimajorAxis)
{
	m_eccentricity = eccentricity;
	m_semimajorAxis = semimajorAxis;
	return *this;
};

OrbitalElements& OrbitalElements::SetOrbitalPlane(double inclination, double longitudeOfAscendingNode)
{
	m_inclination = inclination;
	m_longitudeOfAscendingNode = longitudeOfAscendingNode;
	return *this;
};

OrbitalElements& OrbitalElements::SetEllipseOrientation(double longitudeOfPerihelion)
{
	m_longitudeOfPerihelion = longitudeOfPerihelion;
	return *this;
};

OrbitalElements& OrbitalElements::SetTimeSpecific(double meanLongitude, double epoch)
{
	m_meanLongitude = meanLongitude;
	m_epoch = epoch;
	return *this;
};

double OrbitalElements::GetMeanAnomaly() const
{
	return m_meanLongitude - m_longitudeOfPerihelion;
};

double OrbitalElements::GetTSE(double jd) const
{
	return jd - m_epoch;
};

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

double OrbitalElements::GetRadius(double eccentricAnomaly) const
{
	return m_semimajorAxis * (1 - m_eccentricity * cos(eccentricAnomaly));
};

double OrbitalElements::GetArgumentOfPeriapsis() const
{
	return m_longitudeOfPerihelion - m_longitudeOfAscendingNode;
};

double OrbitalElements::GetTrueAnomaly(double eccentricAnomaly) const
{
	return 2.0 * atan2(
			sqrt(1.0 + m_eccentricity) * sin(eccentricAnomaly / 2.0),
			sqrt(1.0 - m_eccentricity) * cos(eccentricAnomaly / 2.0)
		);
};

double OrbitalElements::GetLongitudeOfAscendingNode() const
{
	return m_longitudeOfAscendingNode;
}

double OrbitalElements::GetInclination() const
{
	return m_inclination;
}

double OrbitalElements::GetSemimajorAxis() const
{
	return m_semimajorAxis;
}

double OrbitalElements::GetOrbitPeriod()
{
	double a = pow(m_semimajorAxis, 3) / STD_GRAVITATIONAL_PARAMATER_EARTH;
	return 2 * M_PI * sqrt(a);
}