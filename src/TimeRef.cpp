/*
 * TimeRef.cpp
 *
 *  Created on: May 3, 2016
 *      Author: nhanquy
 */

#include "TimeRef.h"

TimeRef::TimeRef(timestr set_k0, timestr set_kH, double delta_t) :
		start_horizon(set_k0), end_horizon(set_kH), block_duration(delta_t) {
}

TimeRef::TimeRef(const TimeRef& that) {
	start_horizon = that.start_horizon;
	end_horizon = that.end_horizon;
	block_duration = that.block_duration;
}

TimeRef& TimeRef::operator =(const TimeRef& that) {
	start_horizon = that.start_horizon;
	end_horizon = that.end_horizon;
	block_duration = that.block_duration;
	return *this;
}
TimeRef::~TimeRef() {
}

/*
 *  Validating tools (3)
 */

bool TimeRef::isValidTime(int hr, int min, int sec) {
	return (((hr >= 0) && (hr < 24)) && ((min >= 0) && (min < 60))
			&& ((sec >= 0) && (sec < 60)));
}

bool TimeRef::isValideTimeStr(timestr to_valide) {
	std::stringstream ss;
	const char delim = ':';
	char c1, c2;
	int hr, min, sec;
	ss.clear();
	ss.str(to_valide);
	if ((ss >> hr >> c1 >> min >> c2 >> sec) && (c1 == delim) && (c1 == c2)
			&& isValidTime(hr, min, sec)) {
		return true;
	} else {
		std::cout<<"Time invalid";
		return false;
	}
}


bool TimeRef::isInHorizon(timestr to_valide)
{
	return(difference(start_horizon,to_valide)>=0 &&
			difference(to_valide,end_horizon)>=0);
}

/*
 *  Operator ()
 */

double TimeRef::difference(timestr t1, timestr t2) {
	int h1, h2, m1, m2, s1, s2;
	if (!timestr_parser(t1, h1, m1, s1) || !timestr_parser(t2, h2, m2, s2)) {
		std::cout << "Invalid time-string hh:mm:ss \n";
		return -1;
	}
	double result = (double)(h2-h1)+(double)(m2-m1)/60+(double)(s2-s1)/3600;
	return(result);
}

bool TimeRef::timestr_parser(timestr tstring, int& hh, int& mm, int& ss) {
	std::stringstream strs;
	char c1, c2;
	strs.clear();
	strs.str(tstring);
	if ((strs >> hh >> c1 >> mm >> c2 >> ss)&& isValidTime(hh, mm, ss)) {
		return true;
	} else {
		return false;
	}
}

/*
 *  Converting tools (5)
 */

const timestr TimeRef::double_to_timestr(double set_time)
{
	int hr = floor(set_time);
	int mn = floor((set_time-hr)*60);
	int sc = floor((set_time-hr)*3600-mn*60);
	char buff[128];
	sprintf(buff,"%d:%d:%d",hr,mn,sc);
	std::string result(buff);
	return(result);
}

const double TimeRef::timestr_to_double(timestr set_time)
 {
	 return(difference(start_horizon,set_time));
 }

const double TimeRef::block_to_double(int set_block)
{
	return (set_block*block_duration);
}

const int TimeRef::double_to_block(double set_time)
{
	return(floor(set_time/block_duration));
}

const int TimeRef::timestr_to_block(timestr set_time)
{
	double dt = timestr_to_double(set_time);
	return(double_to_block(dt));
}

// Getting time

const timestr TimeRef::time_now()
{
	static int seconds_last = 99;
	char TimeString[128];

	timeval curTime;
	gettimeofday(&curTime, NULL);
	if (seconds_last == curTime.tv_sec)
		return "NULL";

	seconds_last = curTime.tv_sec;

	strftime(TimeString, 80, "%H:%M:%S", localtime(&curTime.tv_sec));
	return(std::string(TimeString));
}

const double TimeRef::hours_now()
{
	timestr tt = time_now();
	return(timestr_to_double(tt));
}

const int TimeRef::block_now()
{
	timestr tt = time_now();
	return(timestr_to_block(tt));
}


