/*
 * EV.h
 *
 *  Created on: May 4, 2016
 *      Author: nhanquy
 */

#ifndef EV_H_
#define EV_H_
#include "TimeRef.h"

class EV {
public:
	EV(); 												// Create new EV
	EV(int m_user_id, int m_borne_id, double m_auth_time,
			double m_charging_power);	// Add new EV with carr arrival event
	EV(const EV& that);
	EV& operator=(const EV& that);
	~EV();

	// Setting parameters

	void set_user_id(idInt m_value);			// Unsigned int
	void set_auth_time(double m_value); // Double, number of hours since starting of scheduling horizon
	void set_borne_id(idInt m_value);			// Unsigned int
	void set_est_demand(const char* string_date);		// Double, in kW.h

	// Getting informations

	const int get_user_id();
	const int get_borne_id();
	const double get_auth_time();
	const double get_charging_power();
	const double get_est_demand();

	// Events																	// Reference code
	void begin_to_charge(double m_power, double m_stime);			// ref BOC
	void changing_charge_power(double new_charging_power, double change_time); // ref PID
	void end_of_charge(double e_stime);								// ref FDC
	void interupting(double set_time);								// ref ANU
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
	bool charging, fully_charged, halted;
	// Estimation
	double est_demand;
	// Energy management
	double last_changing_time;
	double total_energy;
};

#endif /* EV_H_ */
