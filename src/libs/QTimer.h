/*
 * QTimer.h
 *
 *  Created on: May 18, 2016
 *      Author: nhanquy
 */

#ifndef QTIMER_H_
#define QTIMER_H_
#include "../TimeRef.h"
#include "easylogging++.h"

class QTimer {
public:
	QTimer();
	void set_timeout(double m_time){
		if(m_time>=300)
			LOG(WARNING)<<"Time out is set to "<<m_time<<" seconds. This is too much!";
		LOG(INFO)<<"Time out is set to "<<m_time<<" seconds.";
		timeout = m_time;
	}

	void start(){
		timestr s_H = reference.time_now();
		LOG(INFO)<<"Timer started to count down at "<<s_H<<", "<<timeout<<" seconds to go.";
		reference.set_start_horizon(s_H);
	}

	bool isTimeout()const{
		bool time_out = (reference.hours_now()*3600>=timeout);
		if(time_out) LOG(WARNING)<<"BA 1.0  Timeout!";
		return(time_out);
	}
private:
	static TimeRef reference;
	static double timeout;
};
#endif /* QTIMER_H_ */
