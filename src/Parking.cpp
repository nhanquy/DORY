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

void Parking::reset_fleet(){
	fleet.clear();
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
	remove_EV(user_id);
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

void Parking::set_fifo(bool m_fifo){
	fifo = m_fifo;
}

//
// EXPORTING TO INPUT FOR ACPF
// Changing real-time information into technical input for algorithm ACPF
//

void Parking::export_ACPF_input(Input &ACPF_input){
	TimeRef ACPF_Time(QTime);
	ACPF_Time.set_start_horizon(QTime.time_now());
	double ref_now = QTime.hours_now();
	int n = fleet.size();
	int H = ACPF_Time.block_end();
	intVec user_id = intVec(0,n);
	intVec departure = intVec(0,n);
	intVec arrival = intVec(0,n);
	numVec u = numVec(0,n);
	numVec workload = numVec(0,n);
	for(int i=0; i<n;i++){
		user_id[i] = fleet[i].get_user_id();
		u[i] = fleet[i].get_charging_power();
		// Verifying at first if the EV is in charging or not
		if (fleet[i].is_charging())
		{
			// Getting energy left to charge to fix the time-windows
			double energy_left = fleet[i].est_demand-fleet[i].estimate_consumptions(ref_now);
			// If energy left is negative (estimated value < actual value)
			if (energy_left<=0)
			{
				// Increase the estimation value by a portion of addition_est by actual value
				energy_left = addition_est*fleet[i].estimate_consumptions(ref_now);
				fleet[i].set_est_demand((1/addition_est+1)*energy_left);
			}
			// Fixing time-windows so the scheduled charge will be untouched by ACPF
			double m_departure = ref_now+energy_left/u[i];
			departure[i] = QTime.double_to_block(m_departure);
			workload[i] = energy_left;
		}
		else
		{
			departure[i] = QTime.double_to_block(fleet[i].get_exp_departure());
			workload[i] = fleet[i].get_est_demand();
		}
		// Fixing arrival to start of horizon
		// No-wait model applied, only schedule EV already parked
		// Arrival constraints relaxed
		arrival[i] = 0;
	}
	numVec arr_bandwidth = numVec(bandwidth,H);
	// Setting new input
	ACPF_input.set_nTasks(n);
	ACPF_input.set_timeHorizon(H);
	ACPF_input.set_user_id(user_id);
	ACPF_input.set_u_max(u);
	ACPF_input.set_u_min(u);
	ACPF_input.set_dueDate(departure);
	ACPF_input.set_releaseDate(arrival);
	ACPF_input.set_workload(workload);
}
