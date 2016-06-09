/*
 * TimeRef.h
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#ifndef TIMEREF_H_
#define TIMEREF_H_
#include <string>
#include <sstream>
#include <sys/time.h>
#include <ctime>
#include <math.h>
#include "libs/easylogging++.h"
typedef std::string timestr;
class TimeRef {
public:
	TimeRef();
	TimeRef(timestr set_k0, timestr set_kH, double delta_t);
	TimeRef(const TimeRef& that);
	TimeRef& operator=(const TimeRef& that);
	~TimeRef();

	// Validation tools (3)
	bool isValidTime(int hr, int min, int sec) const;
	bool isValideTimeStr(const timestr& to_valide) const;
	bool isInHorizon(const timestr& ref_time) const;

	// Operator (2)
	double difference(timestr t1, timestr t2) const; // Getting duration from t1 to t2 in term of hours
	bool timestr_parser(const timestr& tstring, int& hh, int& mm, int& ss) const;

	// Converting tools (5)

	timestr double_to_timestr(double set_time) const;
	double timestr_to_double(timestr set_time) const;
	double block_to_double(int set_block) const;
	int double_to_block(double set_time) const;
	int timestr_to_block(timestr set_time) const;

	// Getting time (3)
	int block_end() const;
	double hours_end() const;
	double hours_now() const; 			// Number of hours passed since the starting of scheduling horizon k=0
	int block_now() const; 				// Number of block passed since the starting time k=0
	timestr time_now()  const;		  	// Getting actual system time
	double get_block_duration() const;	// Get block duration
	// Display
	void print_time_now();
	// setting time
	void set_start_horizon(timestr m_s);
	void set_end_horizon(timestr m_e);
	void set_block_horizon(double m_d);
private:
	timestr start_horizon;
	timestr end_horizon;
	double block_duration; 		// Duration of a block in term of hours (double)
};

#endif /* TIMEREF_H_ */
