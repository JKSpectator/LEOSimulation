#pragma once
//�����״����
namespace KeplerOrbits
{
	class OrbitalElements
	{
	public:
		/// <summary>
		/// ������Բ�������״������ƫ���ʶ����˹���ı�ƽ�̶ȣ��볤���ǹ���ֱ����һ��
		/// </summary>
		/// <param name="eccentricity">ƫ����</param>
		/// <param name="semimajorAxis">������</param>
		/// <returns>��״����</returns>
		OrbitalElements& SetEllipseShape(double eccentricity, double semimajorAxis);
		/// <summary>
		/// ���ù��ƽ������ڲο�ƽ�棨ͨ���ǵ������棩����б���������λ��
		/// </summary>
		/// <param name="inclination">���</param>
		/// <param name="longitudeOfAscendingNode">������ƾ�</param>
		/// <returns>��״����</returns>
		OrbitalElements& SetOrbitalPlane(double inclination, double longitudeOfAscendingNode);
		/// <summary>
		/// ������Բ����Ͻ��㣨�������������������ĵ㣩��λ��
		/// </summary>
		/// <param name="longitudeOfPerihelion">����ƾ�</param>
		/// <returns>��״����</returns>
		OrbitalElements& SetEllipseOrientation(double longitudeOfPerihelion);
		/// <summary>
		/// ������ʱ����صĹ��Ԫ�أ�ƽ���ƾ��������ڲο�ʱ����ƽ��λ�ã��ο�ʱ�������Щ���Ԫ����Ч��ʱ��
		/// </summary>
		/// <param name="meanLongitude">ƽ���ƾ�</param>
		/// <param name="epoch">�ο�ʱ��㣬ͨ��Ϊ������</param>
		/// <returns>��״����</returns>
		OrbitalElements& SetTimeSpecific(double meanLongitude, double epoch);

		/// <summary>
		/// ���㲢���������ƽ�����
		/// </summary>
		/// <returns>ƽ�����</returns>
		double GetMeanAnomaly() const;

		/// <summary>
		/// ��������������գ�Julian Date�����㲢���������ڸ����ڵ���ʵ����ǣ�True Anomaly, �ͣ�
		/// </summary>
		/// <param name="jd">������</param>
		/// <returns>��ʵ�����</returns>
		double GetTSE(double jd) const;

		/// <summary>
		/// ���������ƽ����ǣ�Mean Anomaly�������㲢���������ƫ����ǣ�Eccentric Anomaly, E��
		/// </summary>
		/// <param name="m">ƽ�����</param>
		/// <returns>ƫ�����</returns>
		double GetEccentricAnomaly(double m) const;

		/// <summary>
		/// ���������ƫ����ǣ�Eccentric Anomaly�������㲢���������ڹ���ϵĵ�ǰ�뾶�������������ľ���
		/// </summary>
		/// <param name="eccentricAnomaly">ƫ�����</param>
		/// <returns>������������ľ���</returns>
		double GetRadius(double eccentricAnomaly) const;

		/// <summary>
		/// ���㲢��������Ľ�����ǣ�Argument of Periapsis, �أ������Ǵ������㵽����ĽǶȣ������������״����һ���ǶȲ���
		/// </summary>
		/// <returns>�������</returns>
		double GetArgumentOfPeriapsis() const;

		/// <summary>
		/// ���������ƫ����ǣ�Eccentric Anomaly�������㲢�����������ʵ����ǣ�True Anomaly, �ͣ�����ʵ������������ڹ���ϵ�ʵ��λ�ýǶȣ�����ڽ���
		/// </summary>
		/// <param name="eccentricAnomaly">ƫ�����</param>
		/// <returns>��ʵ�����</returns>
		double GetTrueAnomaly(double eccentricAnomaly) const;

		/// <summary>
		/// ���㲢������������������ྭ��Longitude of the Ascending Node, ���������ǹ��ƽ����ο�ƽ�棨������ཻ�ĵ�ľ���
		/// </summary>
		/// <returns>������ྭ</returns>
		double GetLongitudeOfAscendingNode() const;

		/// <summary>
		/// ��������������ǣ�Inclination, i�������ǹ��ƽ������ڲο�ƽ�棨������Ƶ�������б�Ƕ�
		/// </summary>
		/// <returns>�����������</returns>
		double GetInclination() const;

		/// <summary>
		/// ���㲢��������Ĺ�����ڣ�Orbit Period, T����������������������һ�������ʱ��
		/// </summary>
		/// <returns>����Ĺ������</returns>
		double GetOrbitPeriod();

	private:
		//ƫ����
		double m_eccentricity;
		//������
		double m_semimajorAxis;
		//���
		double m_inclination;
		//������ƾ�
		double m_longitudeOfAscendingNode;
		//����ƾ�
		double m_longitudeOfPerihelion;
		//ƽ���ƾ�
		double m_meanLongitude;
		//�ο�ʱ��㣬ͨ��Ϊ������
		double m_epoch;
	};
}