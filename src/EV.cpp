/*
 * EV.cpp
 *
 *  Created on: May 4, 2016
 *      Author: nhanquy
 */

#include "EV.h"

EV::EV() :
		// Identification
		user_id(0), borne_id(0),
		// Times management
		auth_time(0), charging_power(0), starting_time(0),
		// Status flags
		charging(false), fully_charged(false), halted(false),
		// Forecasting
		est_demand(0),
		exp_departure(0),
		// Energy management
		last_changing_time(0), total_energy(0) {
}

EV::EV(int m_user_id, int m_borne_id, double m_auth_time, double set_departure,
		double m_est_demand,double m_charging_power) :
		/*-----------------
		 *  Event ref AUT
		  -----------------*/
		// Identification
		user_id(m_user_id), borne_id(m_borne_id),
		// Times management
		auth_time(m_auth_time), charging_power(m_charging_power), starting_time(0),
		// Status flags
		charging(false), fully_charged(false), halted(false),
		// Forecasting
		est_demand(m_est_demand), exp_departure(set_departure),
		// Energy management
		last_changing_time(0), total_energy(0) {
}

EV::EV(const EV& that) {
	// Identifications
	borne_id = that.borne_id;
	user_id = that.user_id;
	// Times
	auth_time = that.auth_time;
	charging_power = that.charging_power;
	starting_time = that.starting_time;
	// Status flags
	charging = that.charging;
	fully_charged = that.fully_charged;
	// Estimation
	est_demand = that.est_demand;
	exp_departure = that.exp_departure;
	// Energy management
	last_changing_time = that.last_changing_time;
	total_energy = that.total_energy;
	halted = that.halted;
}

EV& EV::operator =(const EV& that) {
	// Identifications
	auth_time = that.auth_time;
	charging_power = that.charging_power;
	borne_id = that.borne_id;
	user_id = that.user_id;
	starting_time = that.starting_time;
	// Status flags
	charging = that.charging;
	fully_charged = that.fully_charged;
	// Estimation
	est_demand = that.est_demand;
	exp_departure = that.exp_departure;
	// Energy management
	last_changing_time = that.last_changing_time;
	total_energy = that.total_energy;
	halted = that.halted;
	return *this;
}

EV::~EV() {
}

/*
 *  Setting parameters
 */

void EV::set_user_id(idInt m_value) {
	user_id = m_value;
}

void EV::set_auth_time(double m_value) {
	auth_time = m_value;
}
void EV::set_borne_id(idInt m_value) {
	borne_id = m_value;
}

void EV::set_est_demand(double m_value) {
	est_demand = m_value;
}

/*
 *  Getting informations
 */

int EV::get_user_id() const{
	return user_id;
}
int EV::get_borne_id() const{
	return borne_id;
}
double EV::get_auth_time() const{
	return auth_time;
}
double EV::get_charging_power() const{
	return charging_power;
}
double EV::get_est_demand() const{
	return est_demand;
}

double EV::get_exp_departure() const{
	return exp_departure;
}

bool EV::is_charging()const{
	return charging;
}
/*
 *  Events definitions
 */

void EV::begin_to_charge(double m_power, double m_stime) {
	// Event ref: BOC
	if (charging)
	{
		std::cout<<"Error: EV already in charge. Cannot start again! \n";
		return;
	}
	starting_time = m_stime;
	charging_power = m_power;
	last_changing_time = m_stime;
	total_energy = 0;
	charging = true;
}

void EV::changing_charge_power(double new_charging_power, double change_time) {
	// Event ref: PID
	if (!charging) {
		std::cout << "Error: Cannot modify a charge hasn't been started yet! \n";
		return;
	}
	total_energy += charging_power * (change_time - last_changing_time);
	charging_power = new_charging_power;
	last_changing_time = change_time;
}

void EV::end_of_charge(double e_stime) {
	// Event ref: FDC
	if (!charging) {
		std::cout << "Cannot end a charge hasn't been started yet! Error!\n";
		return;
	}
	total_energy += charging_power * (e_stime - last_changing_time);
	last_changing_time = e_stime;
	charging_power = 0;
	fully_charged = true;
	charging = false;
}

void EV::interupting(double set_time) {
	// Event ref: ANU
	if (!charging) {
		std::cout << "Cannot end a charge hasn't been started yet! Error!\n";
		return;
	}
	if (fully_charged) {
		std::cout
				<< "Cannot interrupt a charge has been completely charged! Error!\n";
		return;
	}
	total_energy += charging_power * (set_time - last_changing_time);
	last_changing_time = set_time;
	charging_power = 0;
	charging = false;
	halted = true;
}

/*
 *  Getting charging informations (2)
 */

double EV::estimate_SOC(double actual_time)const{
	if(est_demand>0)
	{
		double SOC = estimate_consumptions(actual_time)/est_demand *100;
		return((SOC>100)?100:SOC);
	}
	return(100);
}

double EV::estimate_consumptions(double actual_time)const{
	return(total_energy+(actual_time - last_changing_time)*charging_power);
}



