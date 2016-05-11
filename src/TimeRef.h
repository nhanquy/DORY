/*
 * TimeRef.h
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#ifndef TIMEREF_H_
#define TIMEREF_H_
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <sys/time.h>
#include <ctime>
#include <math.h>
typedef std::string timestr;
class TimeRef {
public:
	TimeRef();
	TimeRef(timestr set_k0, timestr set_kH, double delta_t);
	TimeRef(const TimeRef& that);
	TimeRef& operator=(const TimeRef& that);
	~TimeRef();

	// Validation tools (3)
	bool isValidTime(int hr, int min, int sec);
	bool isValideTimeStr(timestr to_valide);
	bool isInHorizon(timestr ref_time);

	// Operator (2)
	double difference(timestr t1, timestr t2); // Getting duration from t1 to t2 in term of hours
	bool timestr_parser(timestr tstring, int& hh, int& mm, int& ss);

	// Converting tools (5)

	const timestr double_to_timestr(double set_time);
	const double timestr_to_double(timestr set_time);
	const double block_to_double(int set_block);
	const int double_to_block(double set_time);
	const int timestr_to_block(timestr set_time);

	// Getting time (3)

	const double hours_now(); // Number of hours passed since the starting of scheduling horizon k=0
	const int block_now(); // Number of block passed since the starting time k=0
	const timestr time_now();		  // Getting actual system time
	// Display
	void print_time_now();
private:
	timestr start_horizon;
	timestr end_horizon;
	double block_duration; 		// Duration of a block in term of hours (double)
};

#endif /* TIMEREF_H_ */
