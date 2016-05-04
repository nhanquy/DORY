/*
 * EV.cpp
 *
 *  Created on: May 4, 2016
 *      Author: nhanquy
 */

#include "EV.h"

EV::EV() :
		auth_time(0), charging_power(0), borne_id(0), user_id(0), starting_time(
				0), charging(false), fully_charged(false), est_demand(0) {
}

EV::EV(int m_user_id, int m_borne_id, double m_auth_time,
		double m_charging_power) :
		user_id(m_user_id), borne_id(m_borne_id), auth_time(m_auth_time), charging_power(
				m_charging_power), charging(false), fully_charged(false), est_demand(0){

}


EV::~EV() {
	// TODO Auto-generated destructor stub
}

