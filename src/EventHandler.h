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
#include "libs/QTimer.h"
#include "TimeRef.h"
#include "heuristics/Input.h"
#include "heuristics/ConstructiveHeuristic.h"
#include "Parking.h"
#include "libs/easylogging++.h"
//#include <iostream>

class Event_Handler
{
public:

#define config_control if(!config_got) return;
#define histdb_control if(!hist_db_setup) return;

	Event_Handler();
	/*
	 * Default configuration:
	 * All setup flags set to false, exept no_solution flag.
	 * algorithm_timeout = 20 sec
	 */
	// Object configuration
	std::string read_config(const char* config_id);
	/*
	 * Reading configuration file
	 * Format: JSON
	 * Example:
	 * {
	 * 	"Morning_Config":
	 * 		{
	 * 		"Start_time"	:	"07:00:00",
	 * 		"End_time"		:	"12:00:00",
	 * 		"Block_duration":	"0.25",
	 * 		"FIFO"			:	"1",
	 * 		"BA_Timeout"	:	"20"
	 * 		},
	 * "Noon_Config":
	 * 		{
	 * 		"Start_time"	:	"12:00:00",
	 * 		"End_time"		:	"16:00:00",
	 * 		"Block_duration":	"0.25",
	 * 		"FIFO"			:	"0",
	 * 		"BA_Timeout"	:	"20"
	 * 		}
	 * 	}
	 *
	 * 	Where:
	 * 		+ Start_time, End_time	: time string format
	 * 		+ BLock_duration		: duration of a scheduling interval, in term of hours.
	 * 								  Ex: 0.25 hour = 15 mins
	 * 		+ FIFO					: Boolean, true whether the FIFO schedule is applied during this period
	 * 		+ BA_Timeout			: Time out of executing AlgoBox, in seconds
	 */
	void read_config_dir(const char* config_dir)
	{
		config_file = config_dir;
	}
	bool isConfigured(){
		if (!config_got)
			response_message = "not_configured";
		return config_got;
	}
	void setup_estimation_JSON(const char* JSON_file);
	int find_user_index(int user_id) const;
	double load_demand_estimation(int index) const;
	timestr load_exp_depature(int index) const;
	// Set of EVENTs
	void CONFIG_LOAD( std::string command_line);
	void AUT(	int user_id,
				int borne_id,
				double charging_power);
	void FDC(int user_id);
	void ANU(int user_id);
	void DCF(	int user_id,
				double charging_power);
	void PID(	int user_id,
				double charging_power);
	void CNU(int user_id);
	void MCR();
	/*
	 *  Reading Event JSON
	 */
	bool getting_event(const Json::Value& root);
	/*
	 * JSON Example:
	 * {
	 * 	"AUT":[
	 * 	{
	 * 		"user_id" : 123,
	 * 		"borne_id" : 1,
	 * 		"charging_power" : 1.7
	 * 	},
	 * 	{
	 * 		"user_id" : 456,
	 * 		"borne_id" : 2,
	 * 		"charging_power" : 9.5
	 * 	}],
	 * 	"FDC":[
	 * 	{
	 * 		"user_id" :145
	 * 	},
	 * 	{
	 * 		"user_id" : 111
	 * 	}]
	 * }
	 */
	/*
	 * What to do...
	 */
	bool find_solution();
	/*
	 *  Reporting & output
	 */
	bool write_response();
	void write_no_solution();
	std::string get_message() const;
	void testing_JSON_inout();
private:
	TimeRef Event_Time;
	Json::Value m_estimation;
	ConstructiveHeuristic algorithm;
	Input algo_input;
	Parking parking;
	// Status flags
	bool config_got, estimation_json_setup, message_got, error, no_solution, FIFO,
			event_detected;
	// Run-time control
	double algorithm_timeout; // Seconds of algorithm time-out
	// Saving EV priority to matrix
	intVec prioEV;
	numVec prioPower;
	QTimer timer;
	std::string response_message;
	std::string config_file;
};

#endif /* EVENTHANDLER_H_ */
