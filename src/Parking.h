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
#include <vector>
class Parking {
	friend class Event_Handler;
public:
	Parking();
	Parking(const Parking& that);
	Parking& operator=(const Parking& that);
	virtual ~Parking();
	// EV management
	void add_EV(int m_user_id, int m_borne_id, double m_auth_time,double m_charging_power);
	void remove_EV(int m_user_id);

	// Searching tools
	int search_by_ID(int m_user_id) const; // Return -1 if EV doesn't exist
	int find_duplicate(int m_user_id) const;
protected:
	// Can only access by event handler
	void authentification(int user_id, int borne_id, double auth_time,double charging_power=0);
	void end_of_charge(int user_id);
	void begin_of_charge(int user_id, double charging_power);
	void charge_cancelling(int user_id);
	void forced_charge(int user_id, double charging_power);
	void changing_power(int user_id, double new_charging_power);
	void no_charge(int user_id);
	// With new information given and set up, Parking exports
	// new input file for algorithm ACPF 2.0
	void export_algo_input(const char*direction);

private:
	std::vector<EV> fleet;
	double bandwidth;
	TimeRef QTime;
	// QTime		: Time reference for the operation of the parking
	// Status flag
	bool config_read,operating,fifo,msg_read;
	// config_read	: Parking already setup with QTime and bandwidth derived from config file
	// operating  	: Scheduling already begin (QTime.now() in k0 and K_H)
	// fifo			: Parking operating in FIFO mode
	// msg_read		: new message from GO read successfully

	// Database direction
	const char* db_dir;
	//

};
#endif /* PARKING_H_ */
