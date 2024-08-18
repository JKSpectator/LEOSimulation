#include "DelaySimulation.h"


double Communication::generate_gauss_in_range(double a, double b) 
{
	double mean = (a + b) / 2.0;
	double std_dev = (b - a) / 6.0;
	gauss_distribution = normal_distribution<double>(mean, std_dev);
	double num;
	do 
	{
		num = gauss_distribution(generator);
	} while (num < a || num > b);
	return num;
}

double Communication::random_in_range(double a, double b)
{
	uniform_distribution = uniform_real_distribution<double>(0, 1);
	return a + (b - a) * uniform_distribution(generator);
}

Communication::Communication(const std::string& configfile)
	: delay_normal(0), delay_no_noisy(0), delay_ideal(0),
	bandwidth(0), is_attacked(false), attack_probability(0),
	velocity(3e8), packet_size(12000), noisy(0),
	distribution_left(0), distribution_right(1), generator(std::random_device{}())
{
	std::ifstream config(configfile);
	if (config.is_open()) {
		string line;
		while (getline(config, line)) {
			istringstream iss(line);
			string key;
			if (getline(iss, key, ' ')) {
				string value;
				if (key == "bandwidth") {
					if (getline(iss, value, ' ')) {
						bandwidth = stoi(value);
					}
				}
				else if (key == "velocity") {
					if (getline(iss, value, ' ')) {
						velocity = stod(value);
					}
				}
				else if (key == "packet_size") {
					if (getline(iss, value, ' ')) {
						packet_size = stoi(value);
					}
				}
				else if (key == "noisy") {
					if (getline(iss, value, ' ')) {
						noisy = stoi(value);
					}
				}
				else if (key == "distribution_left") {
					if (getline(iss, value, ' ')) {
						distribution_left = stod(value);
					}
				}
				else if (key == "distribution_right") {
					if (getline(iss, value, ' ')) {
						distribution_right = stod(value);
					}
				}
				else if (key == "Source")
				{
					double lat = 0, lon = 0;
					if (getline(iss, value, ' ')) {
						lat = stod(value);
						getline(iss, value, ' ');
						lon = stod(value);
					}
					source = KeplerOrbits::GeoCoordinates(lat, lon, 0);
				}
				else if (key == "Target")
				{
					double lat = 0, lon = 0;
					if (getline(iss, value, ' ')) {
						lat = stod(value);
						getline(iss, value, ' ');
						lon = stod(value);
					}
					target = KeplerOrbits::GeoCoordinates(lat, lon, 0);
				}
				else if (key == "Attack")
				{
					double lat = 0, lon = 0;
					if (getline(iss, value, ' ')) {
						lat = stod(value);
						getline(iss, value, ' ');
						lon = stod(value);
					}
					attack = KeplerOrbits::GeoCoordinates(lat, lon, 0);
				}
			}
		}
	}
	config.close();
}

double Communication::communication_stt(vector<int> distance, vector<int> stationState, int packet_size, int bandwidth, bool is_attacked, bool add_delay)
{
	if (packet_size == 0) {
		packet_size = this->packet_size;
	}

	if (bandwidth == -1) {
		bandwidth = this->bandwidth;
	}

	if (this->noisy != 0) {
		if (this->noisy == 1) {
			bandwidth = generate_gauss_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
		}
		else if (this->noisy == 2) {
			bandwidth = random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
		}
	}

	double delay = 0.0;
	double send_delay = static_cast<double>(packet_size) / bandwidth;//static_cast<double>(packet_size) * stationState.size() / bandwidth;
	for (int i = 0; i < stationState.size(); i++)
	{
		send_delay += static_cast<double>(packet_size) / random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
	}

	double propagation_delay = distance[0] / this->velocity;
	delay = send_delay + propagation_delay;

	if (add_delay) {
		this->delay_normal += delay;
	}

	return delay;
}

double Communication::communication_stt_no_noisy(vector<int> distance, vector<int> stationState,  int packet_size, int bandwidth, bool add_delay)
{
	if (packet_size == 0) {
		packet_size = this->packet_size;
	}

	if (bandwidth == -1) {
		bandwidth = this->bandwidth;
	}

	double delay = 0.0;
	double send_delay = static_cast<double>(packet_size) / bandwidth;//static_cast<double>(packet_size) * stationState.size() / bandwidth;
	for (int i = 0; i < stationState.size(); i++)
	{
		send_delay += static_cast<double>(packet_size) / random_in_range(this->distribution_left * bandwidth, this->distribution_right * bandwidth);
	}
	double propagation_delay = distance[0] / this->velocity;
	delay = send_delay + propagation_delay;

	if (add_delay) {
		this->delay_normal += delay;
	}

	return delay;
}

double Communication::communication_stt_ideal(vector<int> distance, bool add_delay)
{

	double delay = 0.0;
	double propagation_delay = distance[0] / this->velocity;
	delay = propagation_delay;

	if (add_delay) {
		this->delay_normal += delay;
	}

	return delay;
}
