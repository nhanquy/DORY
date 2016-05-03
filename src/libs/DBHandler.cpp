/*
 * DBHandler.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: nhanquy
 */

#include "DBHandler.h"

DB_Handler::DB_Handler() :
		zErrMsg(0), db_is_open(false) {
}

DB_Handler::~DB_Handler() {
}

void DB_Handler::set_db_dir(const char* set_dir) {
	db_dir = std::string(set_dir);
#ifdef DEBUG
	printf("db_dir = %s\n", db_dir.c_str());
#endif
}

const bool DB_Handler::open_db() {
	try {
		int rc = sqlite3_open(db_dir.c_str(), &db);
		if (rc) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
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

int DB_Handler::callback(void *NotUsed, int argc, char **argv,
		char **azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

/*
 *	set_user_id: finding all user_id exist on the user database to stock into vector user_id
 * 	input are user_table_name and user_id_field
 */

void DB_Handler::set_user_id(std::string const & user_table_name,
		std::string const & id_field_name) {
	// Reset vector
	if (user_id.size() > 0)
		user_id.clear();
	// Open database
	if (open_db()) {
		sqlite3_stmt * stmt;
		std::string query;
		query = "SELECT " + id_field_name + " FROM " + user_table_name + ";";
		sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
		sqlite3_step(stmt);

		while (sqlite3_column_int(stmt, 0)) {
#ifdef DEBUG
			fprintf(stderr, "user_id=%d\n", (int) sqlite3_column_int(stmt, 0));
#endif
			user_id.push_back((int) sqlite3_column_int(stmt, 0));
			sqlite3_step(stmt);
		}
	}
	close_db();
}

/*
 *	Take segmented daily data from 2 table conso and seg_conso, then transfer to
 *	historical data table to normalized data.
 *	tables: consos, consos_segment
 *	fields: id in consos
 *			conso_id and idt in consos_segment
 */
void DB_Handler::update_hist(std::string const & date) {
	if (open_db()) {
		std::vector<int> conso_id;
		std::string query;
		sqlite3_stmt * stmt;
		for (unsigned int i = 0; i < user_id.size(); ++i) {
			double sum_time = 0;
			getting_conso_id(user_id[i], date, conso_id, sum_time);
			int sum_units = 0;
			// Getting all data from this conso_id
			for (unsigned int j = 0; j < conso_id.size(); ++j) {
				query = "SELECT units from consos_segment WHERE conso_id="
						+ num2string<int>(conso_id[j]) + ";";
				sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
				sqlite3_step(stmt);
				while (sqlite3_column_int(stmt, 0)) {
#ifdef DEBUG
					fprintf(stderr, "conso_id=%d have unit=%d \n", conso_id[j],
							(int) sqlite3_column_int(stmt, 0));
#endif
					// Summing all the consumption
					sum_units += (int) sqlite3_column_int(stmt, 0);
					sqlite3_step(stmt);
				}
			}
			//Pushing summing unite
#ifdef DEBUG
			fprintf(stderr, "user_id=%d have unit=%d \n", user_id[i],
					sum_units);
#endif
			update_user_date(user_id[i], date, sum_units, sum_time);
		}
		// Closing the database
		close_db();
	}
}
/*
 * Getting all the conso_id during a date by set_user_id and set_date
 */
void DB_Handler::getting_conso_id(const int set_user_id,
		std::string const& set_date, std::vector<int>& save_vector,
		double& total_hours) {
	if (!db_is_open)
		return;
	total_hours = 0;
	std::string query;
	sqlite3_stmt * stmt;
	if (save_vector.size() > 0)
		save_vector.clear();
	query =
			"SELECT id,JulianDay(eoc_time ),JulianDay(boc_time) FROM consos WHERE user_id="
					+ num2string<int>(set_user_id) + " AND date(auth_time)=\""
					+ set_date + "\";";
#ifdef DEBUG
	fprintf(stderr, "query=%s \n", query.c_str());
#endif
	sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
	while (sqlite3_column_int(stmt, 0)) {
		double eoc = sqlite3_column_double(stmt, 1);
		double boc = sqlite3_column_double(stmt, 2);
		double duration = (eoc - boc) * 24;

#ifdef DEBUG
		fprintf(stderr, "conso_id=%d of user_id=%d in %f hours\n",
				(int) sqlite3_column_int(stmt, 0), set_user_id, duration);
#endif

		total_hours += duration;
		save_vector.push_back((int) sqlite3_column_int(stmt, 0));
		sqlite3_step(stmt);
	}
}

/*
 * Update new data for user_id at set_date in hist table.
 * If entry doesn't exsit, create one new
 */

void DB_Handler::update_user_date(const int set_user_id,
		std::string const& set_date, const int set_units,
		const double set_duration) {
	if (!db_is_open)
		return;
	// Getting the id of the conso to be updated
	std::string query;
	sqlite3_stmt * stmt;
	query = "SELECT EXISTS(SELECT 1 FROM hist WHERE user_id="
			+ num2string<int>(set_user_id) + " AND date=\"" + set_date + "\");";

#ifdef DEBUG
	fprintf(stderr, "query=%s \n", query.c_str());
#endif

	sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
#ifdef DEBUG
	fprintf(stderr, "result=%d \n", sqlite3_column_int(stmt, 0));
#endif
	if (sqlite3_column_int(stmt, 0) == 1) {
		// If this row has been existed
		// UPDATE
		query = "UPDATE hist SET conso_units = " + num2string<int>(set_units)
				+ ",duration=" + num2string<double>(set_duration)
				+ " WHERE user_id=" + num2string<int>(set_user_id)
				+ " AND date=\"" + set_date + "\";";
	} else {
		query = "INSERT INTO hist (date,user_id,conso_units,duration) VALUES(\""
				+ set_date + "\"," + num2string<int>(set_user_id) + ","
				+ num2string<int>(set_units) + ","
				+ num2string<double>(set_duration) + ");";
	}

#ifdef DEBUG
	fprintf(stderr, "query=%s \n", query.c_str());
#endif

	sqlite3_exec(db, query.c_str(), 0, 0, 0);
}

/*
 *  Getting all the daily conso of user_id in table hist
 *  to calculate the average week value.
 *  Getting data from table: 	hist
 *  Update data on table: 		week_norm
 */

void DB_Handler::update_user_week_normalise(const int set_week,
		const int set_year, const int set_user_id) {
	if (!db_is_open)
		return;
	int eventsNo = 0;
	double total_conso = 0;
	double total_dur = 0;
	/*
	 *  Getting data
	 */
	std::string query;
	sqlite3_stmt * stmt;
	query = "SELECT conso_units, duration FROM hist WHERE user_id="
			+ num2string<int>(set_user_id) + " AND strftime('%Y',date)=\""
			+ num2string<int>(set_year) + "\" AND strtime('%W',date)=\""
			+ num2string<int>(set_week) + "\";";
#ifdef DEBUG
	fprintf(stderr, "query=%s \n", query.c_str());
#endif
	sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
	while (sqlite3_column_int(stmt, 0)) {
		total_conso += sqlite3_column_double(stmt, 0);
		total_dur += sqlite3_column_double(stmt, 1);
		eventsNo++;
	}
	/*
	 * Updating
	 */
	if (eventsNo > 0)
		return;
	/*
	 * Note: Only update existing data, week with zero consumption will be
	 * eliminated to avoid outsider.
	 */
	//
	double norm_conso = total_conso / eventsNo;
	double norm_dur = total_dur / eventsNo;
	//
	query = "SELECT EXISTS(SELECT 1 FROM week_norm WHERE user_id="
			+ num2string<int>(set_user_id) + " AND week ="
			+ num2string<int>(set_week) + " AND year ="
			+ num2string<int>(set_year) + ");";
#ifdef DEBUG
	fprintf(stderr, "query=%s \n", query.c_str());
#endif
	sqlite3_prepare(db, query.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
#ifdef DEBUG
	fprintf(stderr, "result=%d \n", sqlite3_column_int(stmt, 0));
#endif
	if (sqlite3_column_int(stmt, 0) == 1) {
		// If this row has been existed
		// UPDATE
		query = "UPDATE week_norm SET conso_norm = "
				+ num2string<double>(norm_conso) + ",duration_norm="
				+ num2string<double>(norm_dur) + " WHERE user_id="
				+ num2string<int>(set_user_id) + " AND week ="
				+ num2string<int>(set_week) + " AND year ="
				+ num2string<int>(set_year) + ");";
	} else {
		query =
				"INSERT INTO week_norm (user_id,year,week,conso_norm, duration_norm) VALUES("
						+ num2string<int>(set_user_id) + ","
						+ num2string<int>(set_year) + ","
						+ num2string<int>(set_week) + ","
						+ num2string<double>(norm_conso) + ","
						+ num2string<double>(norm_dur) + ");";
	}
#ifdef DEBUG
	fprintf(stderr, "query=%s \n", query.c_str());
#endif
}

void DB_Handler::update_week_normalise(const int set_week, const int set_year) {
	if (open_db()) {
		for (unsigned int i = 0; i < user_id.size(); ++i) {
			update_user_week_normalise(set_week,set_year, user_id[i]);
		}
		// Closing the database
		close_db();
	}
}
