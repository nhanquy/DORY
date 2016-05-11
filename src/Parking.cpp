/*
 * Parking.cpp
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#include "Parking.h"

Parking::Parking() :
		bandwidth(0), config_done(false), operating(false), fifo(true), db_setup(
				false), config_count(0), db_dir("") {
	// Default constructor applied for
	// Time RefQTime and vector<EV> fleet.
}

Parking::Parking(const TimeRef& config_time, const double& set_bw,
		bool set_fifo = false) :
		bandwidth(set_bw), QTime(config_time), config_done(true), operating(
				false), fifo(true), db_setup(false), config_count(2), db_dir("") {
	// Default constructor applied for
	// vector<EV> fleet
}

Parking::~Parking() {
	fleet.clear();
}

Parking::Parking(const Parking& that) {
	bandwidth = that.bandwidth;
	QTime = that.QTime;
	config_done = that.config_done;
	operating = that.operating;
	fifo = that.fifo;
	db_setup = that.db_setup;
	db_dir = that.db_dir;
	config_count = that.config_count
}

Parking& Parking::operator =(const Parking& that) {
	bandwidth = that.bandwidth;
	QTime = that.QTime;
	config_done = that.config_done;
	operating = that.operating;
	fifo = that.fifo;
	db_setup = that.db_setup;
	db_dir = that.db_dir;
	config_count = that.config_count
	return *this;
}

// Inputting tools

void Parking::set_QTime(const TimeRef& config_time) {
	QTime = config_time;
	config_count++;
	if (config_count == 2)
		config_done = true;
}

void Parking::set_bandwidth(const double set_bw) {
	bandwidth = set_bw;
	config_count++;
	if (config_count == 2)
		config_done = true;
}

void Parking::set_db_dir(const char* m_db_dir) {
	db_dir = m_db_dir;
	db_setup = true;
}

// EV management

void Parking::add_EV(int m_user_id, int m_borne_id, double m_auth_time,
		double m_charging_power) {

	if (search_by_ID(m_user_id) == -1) {
		EV new_EV(m_user_id, m_borne_id, m_auth_time, m_charging_power);
		fleet.push_back(new_EV);
	} else
		std::cout << "Warning: EV already in parking!\n";
}

void Parking::remove_EV(int m_user_id) {
	int index = search_by_ID(m_user_id);
	if (index >= 0) {
		fleet.erase(fleet.begin() + index);
	} else
		std::cout << "Warning: Cannot delete a non-existent EV!\n";
}

// Searching tools
int Parking::search_by_ID(int user_id) const {
	for (int ind = 0; ind < fleet.size(); ind++)
		if (fleet[ind].get_user_id() == user_id)
			return ind;
	return -1;
}

// EVENTS DEFINITIONS

void Parking::authentification(int user_id, int borne_id, double auth_time,
		double charging_power = 0) {
	add_EV(user_id, borne_id, auth_time, charging_power);
}

void Parking::end_of_charge(int user_id) {
	int ind = search_by_ID(user_id);
	if(ind==-1){
		std::cout<<"Error: EV non-existent!\n";
		return;
	}
	fleet[ind].end_of_charge(QTime.hours_now());
	QTime.print_time_now();
	std::cout<<"EV "<<user_id<<"'s charging has been ended. \n";
}

void Parking::begin_of_charge(int user_id, double charging_power){
	int ind = search_by_ID(user_id);
	if(ind==-1){
		std::cout<<"Error: EV non-existent!\n";
		return;
	}
	fleet[ind].begin_to_charge(charging_power,QTime.hours_now());
	QTime.print_time_now();
	std::cout<<"EV "<<user_id<<" begin to charge at rate "<<charging_power<<"kW. \n";
}

void Parking::charge_cancelling(int user_id){
	int ind = search_by_ID(user_id);
	if(ind==-1){
		std::cout<<"Error: EV non-existent!\n";
		return;
	}
	fleet[ind].interupting(QTime.hours_now());
	QTime.print_time_now();
	std::cout<<"EV "<<user_id<<"'s charging has been cancelled. \n";
}

void Parking::forced_charge(int user_id, double charging_power){
	int ind = search_by_ID(user_id);
	if(ind==-1){
		std::cout<<"Error: EV non-existent!\n";
		return;
	}
	fleet[ind].begin_to_charge(charging_power,QTime.hours_now());
	QTime.print_time_now();
	std::cout<<"EV "<<user_id<<" forced to charge at rate "<<charging_power<<"kW. \n";
}

void Parking::changing_power(int user_id, double new_charging_power){
	int ind = search_by_ID(user_id);
	if(ind==-1){
		std::cout<<"Error: EV non-existent!\n";
		return;
	}
	fleet[ind].changing_charge_power(new_charging_power,QTime.hours_now());
	QTime.print_time_now();
	std::cout<<"EV "<<user_id<<" changing charge rate to "<<new_charging_power<<"kW. \n";
}

void Parking::no_charge(int user_id){
	int ind = search_by_ID(user_id);
	if(ind==-1){
		std::cout<<"Error: EV non-existent!\n";
		return;
	}
	fleet[ind].end_of_charge(QTime.hours_now());
	QTime.print_time_now();
	std::cout<<"EV "<<user_id<<" consumes no power. \n";
}

