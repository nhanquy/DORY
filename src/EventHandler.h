/*
 * EventHandler.h
 *
 *  Created on: May 13, 2016
 *  Author: nhanquy
 *  The Event handler class helps to capture JSON message from GO and analyze resource allocation from ACPF2.0
 *
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_
#include <string>
#include "libs/matrix.h"
#include "libs/DBHandler.h"
#include "TimeRef.h"
#include "Input.h"
#include "Layering.h"
#include "Parking.h"
class Event_Handler {
public:

#define config_control if(!config_got) return;
#define hist_db control if(!hist_db_setup) return;

	Event_Handler();
	// Object configuration
	void read_config(char* config_file, char* config_id);
	void setup_db_handler(char* db_dir);
	virtual ~Event_Handler();
	double load_demande_estimation(int user_id);
private:
	TimeRef Event_Time;
	DB_Handler hist_db;
	Layering algorithm;
	Input algo_input;
	Parking parking;
	// Status flags
	bool config_got,hist_db_setup, message_got, error, no_solution;
	// Run-time control
	double algorithm_timeout; // Seconds of algorithm time-out
	//

};

#endif /* EVENTHANDLER_H_ */
