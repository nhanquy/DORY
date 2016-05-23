/*
 * DBHandler.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: nhanquy
 */

#include "DBHandler.h"

DB_Handler::DB_Handler() : db(nullptr) ,db_is_open(false){
}
DB_Handler::~DB_Handler(){
	close_db();
}
void DB_Handler::set_db_dir(const char* set_dir) {
	db_dir = std::string(set_dir);
	LOG(DEBUG)<<"db_dir ="<< db_dir.c_str();
}

bool DB_Handler::open_db() {
	try {
		int rc = sqlite3_open(db_dir.c_str(), &db);
		if (rc) {
			LOG(ERROR)<<"Can't open database:"<< sqlite3_errmsg(db);
			sqlite3_close(db);
			return (false);
		}
		db_is_open = true;
		return true;
	} catch (...) {
		db_is_open = false;
		return false;
	}
}
void DB_Handler::close_db() {
	db_is_open = false;
	sqlite3_close(db);
}

/*
 *	set_user_id: finding all user_id exist on the user database to stock into vector user_id
 * 	input are user_table_name and user_id_field
 */

double DB_Handler::get_est_demand(const int user_id, bool& user_existed){
	if (!db_is_open)
		return 0;
	std::string query;
	sqlite3_stmt * stmt;
	query = "SELECT demand FROM estimation WHERE user_id="+ num2string<int>(user_id)+ ";";
	LOG(DEBUG)<<"query="<<query.c_str();
	sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
	if (sqlite3_column_int(stmt, 0)) {
		double result = sqlite3_column_double(stmt, 0);
		LOG(DEBUG)<<"user_id="<<user_id<<" est_demand ="<<result;
		user_existed = true;
		return(result);
	} else
	{
		user_existed = false;
		query ="SELECT demand FROM estimation WHERE user_id=0;";//Loading default parameters
		LOG(DEBUG)<<"query="<<query.c_str();
		sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
		sqlite3_step(stmt);
		double result = sqlite3_column_double(stmt, 0);
		LOG(DEBUG)<<"user_id="<<user_id<<" load  default est_demand "<<result;
		return(result);
	}
}


std::string DB_Handler::get_exp_depature(const int user_id, bool& user_existed){
	if (!db_is_open)
		return 0;
	std::string query;
	sqlite3_stmt * stmt;
	query = "SELECT departure FROM estimation WHERE user_id="+ num2string<int>(user_id)+ ";";
	LOG(DEBUG)<<"query="<<query.c_str();
	sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
	if (sqlite3_column_int(stmt, 0)) {
		std::string result(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		LOG(DEBUG)<<"user_id="<<user_id<<" exp_depature = "<<result.c_str();
		user_existed = true;
		return(result);
	} else
	{
		user_existed = false;
		query ="SELECT departure FROM estimation WHERE user_id=0;";//Loading default parameters
		LOG(DEBUG)<<"query="<<query.c_str();
		sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
		sqlite3_step(stmt);
		std::string result(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		LOG(DEBUG)<<"user_id="<<user_id<<" load default exp_depature = "<<result.c_str();
		return(result);
	}
}
