/*
 * EV.h
 *
 *  Created on: May 4, 2016
 *      Author: nhanquy
 */

#ifndef EV_H_
#define EV_H_

class EV {
public:
	EV(); 												// Create new EV
	EV(int m_user_id, int m_borne_id, double m_auth_time,
			double m_charging_power);					// Add new EV with carr arrival event
	EV(const EV& that);
	EV& operator=(const EV& that);
	~EV();

	void set_user_id(idInt m_value);			// Unsigned int
	void set_auth_time(double m_value); // Double, number of hours since starting of scheduling horizon
	void set_borne_id(idInt m_value);			// Unsigned int
	void set_charging_power(double m_value);	// Double, in kW
	void set_est_demand(double m_value);		// Double, in kW.h

	const int get_user_id();
	const int get_borne_id();
	const double get_parking_time();
	const double get_charging_power();
	const double get_est_demand();

	// Events												// Reference code
	void begin_to_charge(double set_charging_power);		// ref BOC
	void changing_charge_power(double new_charging_power); 	// ref PID
	void end_of_charge();									// ref FDC
	void interupting();										// ref ANU
	void pause();

	// Getting charging information
	const double estimate_SOC(double actual_time);
	const double estimate_consumptions(double actual_time);

private:
	// Identification
	double auth_time;
	double charging_power;
	idInt borne_id, user_id;
	double starting_time;
	// Status flag
	bool charging, fully_charged;
	// Estimation
	double est_demand;
};

#endif /* EV_H_ */
