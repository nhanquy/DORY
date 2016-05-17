/*
 * Parking.h
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#ifndef PARKING_H_
#define PARKING_H_
#include "EV.h"
#include "TimeRef.h"
#include "Input.h"
#include "libs/matrix.h"
#include <vector>

#define addition_est 0.1

class Parking {
	friend class Event_Handler;
public:
	Parking();
	Parking(const TimeRef& config_time, const double& set_bw, bool set_fifo=false);
	Parking(const Parking& that);
	Parking& operator=(const Parking& that);
	virtual ~Parking();
	// Inputting tools
	void set_QTime(const TimeRef& config_time);
	void set_bandwidth(const double set_bw);
	// EV management
	void reset_fleet();
	void add_EV(int m_user_id, int m_borne_id,
			double auth_time, double dep_time, double est_demand, double est_power); 			// AUT
	void remove_EV(int m_user_id);
	// Searching tools
	int search_by_ID(int m_user_id) const; // Return -1 if EV doesn't exist
protected:
	// Can only access by event handler
	void end_of_charge(int user_id);															// FDC
	void begin_of_charge(int user_id, double charging_power);									// no Ref
	void charge_cancelling(int user_id);														// ANU
	void forced_charge(int user_id, double charging_power);										// DCF
	void changing_power(int user_id, double new_charging_power);								// PID
	void no_charge(int user_id);																// CNU
	// With new information given and set up, Parking exports
	// new input file for algorithm ACPF 2.0
	void export_ACPF_input(Input &ACPF_input);
	void set_fifo(bool m_fifo);
private:
	std::vector<EV> fleet;
	double bandwidth;
	// QTime		: Time reference for the operation of the parking
	TimeRef QTime;
	// Status flag
	bool config_done,operating,fifo;
	int config_count;
	// config_done	: Parking already setup with QTime and bandwidth derived
	// operating  	: Scheduling already begin (QTime.now() in k0 and K_H)
	// fifo			: Parking operating in FIFO mode

};
#endif /* PARKING_H_ */
