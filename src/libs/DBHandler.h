/*
 * DBHandler.h
 *
 *  Created on: Mar 8, 2016
 *  Modified on: May 2016 to adapt to the management of hist_db
 *  for the Event_Handler class.
 *      Author: Nhan-Quy NGUYEN
 *  Handler of the SQLITE database of NEMO box
 */

#ifndef DBHANDLER_H_
#define DBHANDLER_H_
#include "sqlite3.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include "easylogging++.h"
class DB_Handler {
public:
	DB_Handler();
	virtual ~DB_Handler();
	// Setting database direction
	void set_db_dir(const char* set_dir);
	// Actions taken upon this database
	bool open_db();
	void close_db();
	// Setting user id by user table
	/*
	 * Methods used to update the historical data table at a given date
	 * which transforms segments data into standardized one.
	 */
	double get_est_demand(const int user_id, bool& user_existed);
	/*
	 * Table est_demand in database hist. Update daily
	 * If user_id not found, loading default demand (ID 000).
	 */
	std::string get_exp_depature(const int user_id, bool& user_existed);
	//Some tools
	template <typename T> std::string num2string(T set_num)
	{
		std::stringstream ss;
		ss << set_num;
		return ss.str();
	}
private:
	sqlite3 *db;
	std::string db_dir;
	bool db_is_open;
};

#endif /* DBHANDLER_H_ */
