/*
 * Parking.h
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#ifndef PARKING_H_
#define PARKING_H_

class Parking {
public:
	Parking();
	virtual ~Parking();
};

class EV{
public:
	EV();
	EV(int m_user_id, int m_borne_id, double m_auth_time, double m_charging_power);
	EV(const EV& that);
	EV& operator=(const EV& that);
	~EV;

	void set_user_id();
	void set_auth_time();
	void set_borne_id();
	void set_charging_power();
	void set_est_demand();

	const int get_user_id();
	const int get_borne_id();
	const double get_parking_time();
	const double get_charging_power();
	const double get_est_demand();
	// Event
	void begin_to_charge(double set_charging_power);
	void end_of_charge();
	void interupting();
	void pause();

	// Getting charging information
	const double estimate_SOC(double actual_time);
	const double estimate_consumptions(double actual_time);

private:
	double auth_time;
	double charging_power;
	int borne_id,user_id;
	double starting_time;
	bool charging, fully_charged;
	double est_demand;
};

#endif /* PARKING_H_ */
