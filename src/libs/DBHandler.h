/*
 * DBHandler.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Nhan-Quy NGUYEN
 *  Handler of the SQLITE database of NEMO box
 *	Using to numericalize the database and processing to update the prediction or histo_base
 *	For CBF_d, ACPF 2.0
 */

#ifndef DBHANDLER_H_
#define DBHANDLER_H_
#include "sqlite3.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>

#define DEBUG
class DB_Handler {
public:
	DB_Handler();
	virtual ~DB_Handler();

	// Setting database direction
	void set_db_dir(const char* set_dir);
	// Actions taken upon this database
	const bool open_db();
	void close_db();
	// Setting user id by user table
	void set_user_id(std::string const & user_table_name,std::string const & id_field_name);
	/*
	 * Methods used to update the historical data table at a given date
	 * which transforms segments data into standardized one.
	 */
	void update_hist(std::string const & date);
	void getting_conso_id(const int set_user_id, std::string const& set_date,std::vector<int>& save_vector, double& total_hour);
	void update_user_date(const int set_user_id, std::string const& set_date, const int set_units, const double set_duration);
	void update_user_week_normalise(const int set_week, const int set_year, const int set_user_id);
	void update_week_normalise(const int set_week, const int set_year);

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
	std::vector<int> user_id;
	char *zErrMsg;
	bool db_is_open;
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);
};

#endif /* DBHANDLER_H_ */
