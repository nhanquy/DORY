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
public:
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
private:
	QInt nTasks;
	QInt timeHorizon;
	intVec releaseDate;
	intVec dueDate;
	numVec bandwidth;
	numVec u_min;
	numVec u_max;
	numVec workload;
};

#endif /* INPUT_H_ */
