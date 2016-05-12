/*
 * Input.h
 *
 *  Created on: 8 f�vr. 2016
 *      Author: Nhan Quy NGUYEN
 *      Reading standard DAT file to numerical data
 */

#ifndef INPUT_H_
#define INPUT_H_
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
//
#include "libs/json/json.h"
#include "libs/matrix.h"
//
using namespace std;
class Input
{
	friend class Parking;
public:
	// Mars 2016
	Input();
	Input(char const* JsonFile);
	Input(const Input& that);
	~Input();
	template<typename T> void readArray(Json::Value jsonArray, vector<T>& arr);
	Input& operator=(const Input& setInput);
	void display();
	// Getting data methods
	// System
	const QInt get_nTasks() const;
	const QInt get_timeHorizon() const;
	const numVec get_bandwidth() const;
	// Each job
	const QInt get_releaseDate(QInt jobID) const;
	const QInt get_dueDate(QInt jobID) const;
	const QNum get_workload(QInt jobID) const;
	const QNum get_res_ub(QInt jobID) const;
	const QNum get_res_lb(QInt jobID) const;
	void get_job(const QInt& jobID, QInt& rD, QInt& dD, QNum& wl, QNum& uM, QNum& um) const;
	// Adding setting tools for ACPF 2.0, BA 1.0 (May 2016)
protected: // Can only accessed by class Parking
	void set_nTasks(int m_n);
	void set_timeHorizon(int m_H);
	void set_user_id(const intVec& m_user_id);
	void set_releaseDate(const intVec& m_releaseDate);
	void set_dueDate(const intVec& m_dueDate);
	void set_workload(const numVec& m_workload);
	void set_u_min(const numVec& m_u_min);
	void set_u_max(const numVec& m_u_max);
	void set_bandwidth(const numVec& bandwidth);
private:
	QInt nTasks,timeHorizon;
	intVec user_id,releaseDate, dueDate;
	numVec bandwidth,u_min, u_max, workload;
};

#endif /* INPUT_H_ */
