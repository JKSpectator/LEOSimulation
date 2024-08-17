#include "OrbitBody.h"
#include "CoordinateSystems.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace KeplerOrbits;


OrbitBody::OrbitBody(int id, OrbitalElements&	orbitalElements, double siderealOrbitPeriod) :
		m_id(id),
		m_orbitalElements(orbitalElements),
		m_siderealOrbitPeriod(siderealOrbitPeriod)
{};


EulerCoordinates OrbitBody::GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const
{
	double trueAnomaly = m_orbitalElements.GetTrueAnomaly(eccentricAnomaly);
	double argumentOfPeriapsis = m_orbitalElements.GetArgumentOfPeriapsis();

	double psi = trueAnomaly + argumentOfPeriapsis;

	double r = m_orbitalElements.GetRadius(eccentricAnomaly);
	return EulerCoordinates(
		psi, 
		m_orbitalElements.GetLongitudeOfAscendingNode(),
		m_orbitalElements.GetInclination(),
		r);
}

EulerCoordinates OrbitBody::GetEulaerAnglesFromJulianDay(double jd) const
{
	double meanMotion = 2.0 * M_PI / m_siderealOrbitPeriod;
	double currentMeanAnomaly = m_orbitalElements.GetMeanAnomaly() + meanMotion * m_orbitalElements.GetTSE(jd);

	double eccentricAnomaly = m_orbitalElements.GetEccentricAnomaly(currentMeanAnomaly);

	return GetEulaerAnglesFromEccentricAnomaly(eccentricAnomaly);
};

double OrbitBody::GetSiderealOrbitPeriod() const
{
	return m_siderealOrbitPeriod;
}

int OrbitBody::GetID() const
{
	return m_id;
}

Trajectory OrbitBody::GetTrajectory(int stepCount, int time_offset) const
{
	Trajectory trajectory;
	trajectory.resize(stepCount);
	double t = 0;
	//time_offset %= 72;
	for (int j = 0; j < stepCount; j++)
	{
		t = j * 2 * M_PI / stepCount + 0.1 * time_offset * M_PI / 180;
		trajectory[j] = GetEulaerAnglesFromEccentricAnomaly(t);
	}
	return trajectory;
}
