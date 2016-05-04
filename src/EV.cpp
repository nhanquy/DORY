/*
 * EV.cpp
 *
 *  Created on: May 4, 2016
 *      Author: nhanquy
 */

#include "EV.h"

EV::EV() :
		auth_time(0), charging_power(0), borne_id(0), user_id(0), starting_time(
				0), charging(false), fully_charged(false), halted(false), est_demand(
				0), last_changing_time(0), total_energy(0) {
}

EV::EV(int m_user_id, int m_borne_id, double m_auth_time,
		double m_charging_power) :
		user_id(m_user_id), borne_id(m_borne_id), auth_time(m_auth_time), charging_power(
				m_charging_power), charging(false), fully_charged(false), halted(
				false), starting_time(0), est_demand(0), last_changing_time(0), total_energy(
				0) {

}

EV::EV(const EV& that) {
	auth_time = that.auth_time;
	charging_power = that.charging_power;
	borne_id = that.borne_id;
	user_id = that.user_id;
	starting_time = that.starting_time;
	charging = that.charging;
	fully_charged = that.fully_charged;
	est_demand = that.est_demand;
	last_changing_time = that.last_changing_time;
	total_energy = that.total_energy;
	halted = that.halted;
}

EV& EV::operator =(const EV& that) {
	auth_time = that.auth_time;
	charging_power = that.charging_power;
	borne_id = that.borne_id;
	user_id = that.user_id;
	starting_time = that.starting_time;
	charging = that.charging;
	fully_charged = that.fully_charged;
	est_demand = that.est_demand;
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

void EV::set_est_demand(const char* string_date) {
	// TODO: Referencing predicted value in database by date-time string
}

/*
 *  Getting informations
 */

const int EV::get_user_id() {
	return user_id;
}
const int EV::get_borne_id() {
	return borne_id;
}
const double EV::get_auth_time() {
	return auth_time;
}
const double EV::get_charging_power() {
	return charging_power;
}
const double EV::get_est_demand() {
	return est_demand;
}

/*
 *  Events definitions
 */

void EV::begin_to_charge(double m_power, double m_stime) {
	starting_time = m_stime;
	charging_power = m_power;
	last_changing_time = m_stime;
	total_energy = 0;
	charging = true;
}

void EV::changing_charge_power(double new_charging_power, double change_time) {
	if (!charging) {
		std::cout << "Cannot modify a charge hasn't been started yet! Error!\n";
		return;
	}
	total_energy += charging_power * (change_time - last_changing_time);
	charging_power = new_charging_power;
	last_changing_time = change_time;
}

void EV::end_of_charge(double e_stime) {
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
	if (!charging) {
		std::cout << "Cannot end a charge hasn't been started yet! Error!\n";
		return;
	}
	if (fully_charged) {
		std::cout << "Cannot interrupt a charge has been completely charged! Error!\n";
		return;
	}
	total_energy += charging_power * (set_time - last_changing_time);
	last_changing_time = set_time;
	charging = false;
	halted = true;
}
