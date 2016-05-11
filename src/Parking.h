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
#include "libs/matrix.h"
#include <vector>
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
	void set_db_dir(const char* m_db_dir);
	// EV management
	void add_EV(int m_user_id, int m_borne_id, double m_auth_time,double m_charging_power);
	void remove_EV(int m_user_id);
	// Searching tools
	int search_by_ID(int m_user_id) const; // Return -1 if EV doesn't exist
protected:
	// Can only access by event handler
	void authentification(int user_id, int borne_id, double auth_time,double charging_power=0);	// AUT
	void end_of_charge(int user_id);															// FDC
	void begin_of_charge(int user_id, double charging_power);									// no Ref
	void charge_cancelling(int user_id);														// ANU
	void forced_charge(int user_id, double charging_power);										// DCF
	void changing_power(int user_id, double new_charging_power);								// PID
	void no_charge(int user_id);																// CNU
	// With new information given and set up, Parking exports
	// new input file for algorithm ACPF 2.0
	void export_ACPF_input(const char*direction);

private:
	std::vector<EV> fleet;
	double bandwidth;
	// QTime		: Time reference for the operation of the parking
	TimeRef QTime;
	// Status flag
	bool config_done,operating,fifo,db_setup;
	int config_count;
	// config_done	: Parking already setup with QTime and bandwidth derived
	// operating  	: Scheduling already begin (QTime.now() in k0 and K_H)
	// fifo			: Parking operating in FIFO mode
	// db_setup 	: DB setup sucessfully and ready to be accessed
	// Database direction
	char* db_dir;
	//


};
#endif /* PARKING_H_ */
