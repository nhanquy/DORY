/*
 * Parking.cpp
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#include "Parking.h"

Parking::Parking() :
		bandwidth(0), config_done(false), operating(false), fifo(true), config_count(0){
	// Default constructor applied for
	// Time RefQTime and vector<EV> fleet.
}

Parking::Parking(const TimeRef& config_time, const double& set_bw,
		bool set_fifo) :
		bandwidth(set_bw), QTime(config_time), config_done(true), operating(
				false), fifo(true),config_count(2){
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
	config_count = that.config_count;
}

Parking& Parking::operator =(const Parking& that) {
	bandwidth = that.bandwidth;
	QTime = that.QTime;
	config_done = that.config_done;
	operating = that.operating;
	fifo = that.fifo;
	config_count = that.config_count;
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

// EV management

void Parking::add_EV(int m_user_id, int m_borne_id, const numVec& params) {
	// Params = |auth_time|dep_time|est_demand|est_power|
	if (params.size()!=4)
	{
		std::cout<<"Error: Parameters of EV not correct!\n";
		return;
	}
	if (search_by_ID(m_user_id) == -1) {
		EV new_EV(m_user_id, m_borne_id,params[0],params[1],params[2],params[3]);
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
	for (unsigned int ind = 0; ind < fleet.size(); ind++)
		if (fleet[ind].get_user_id() == user_id)
			return ind;
	return -1;
}

// EVENTS DEFINITIONS

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

// EXPORTING TO INPUT FOR ACPF

void Parking::export_ACPF_input(Input &ACPF_input){
	TimeRef ACPF_Time(QTime);
	bl_H -= bl_now;_
	double hr_now = QTime.hours_now();
	double hr_H = QTime.hours_end();
	hr_H -= bl_now;
}
