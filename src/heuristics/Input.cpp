/*
 * Input.cpp
 *
 *  Created on: 8 f�vr. 2016
 *      Author: Nhan Quy NGUYEN
 */

#include "Input.h"
#define str_var(s) #s
#define disp_vector(var_vec) disp_vector(var_vec,str_var(var_vec))
#define disp_matrix(var_vec) disp_matrix(var_vec,str_var(var_vec))
Input::Input(char const* fileName) {
	std::ifstream jsonDoc(fileName, std::ifstream::binary);
	if (!jsonDoc.is_open()) {
		DEBUG_LOG << "Unable to open file" << fileName
				<< "\nProgram terminating...\n";
		return;
	}
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonDoc, root);
	if (!parsingSuccessful) {
		// report to the user the failure and their locations in the document.
		DEBUG_LOG << "Failed to parse input file\n"
				<< reader.getFormattedErrorMessages();
		return;
	}
	// Get number of jobs
	nTasks = root["nTasks"].asInt();
	// Get time horizon
	timeHorizon = root["timeHorizon"].asInt();
	// Initializing
	vector<int> intTaskVec;
	releaseDate = intTaskVec;
	dueDate = intTaskVec;
	user_id = intTaskVec;
	//
	vector<float> floatTaskVec;
	u_min = floatTaskVec;
	u_max = floatTaskVec;
	workload = floatTaskVec;
	//
	vector<float> floatHVec;
	bandwidth = floatHVec;
	// Getting array
	Json::Value jsonArray = root["releaseDate"];
	readArray(jsonArray, releaseDate);
	//
	jsonArray = root["dueDate"];
	readArray(jsonArray, dueDate);
	// Controlling dueDate
	for (int k = 0; k < nTasks; ++k)
		if (dueDate[k] >= timeHorizon) // Because starting at 0; k \in {0,...,H-1} : H elements
				{
			dueDate[k] = timeHorizon - 1;
		}
	//
	jsonArray = root["user_id"];
	readArray(jsonArray, user_id);
	//
	jsonArray = root["u_min"];
	readArray(jsonArray, u_min);
	//
	jsonArray = root["u_max"];
	readArray(jsonArray, u_max);
	//
	jsonArray = root["workload"];
	readArray(jsonArray, workload);
	//
	jsonArray = root["bandwidth"];
	readArray(jsonArray, bandwidth);
}
//
Input::Input(const Input& setInput) {
	nTasks = setInput.nTasks;
	timeHorizon = setInput.timeHorizon;
	releaseDate = setInput.releaseDate;
	dueDate = setInput.dueDate;
	bandwidth = setInput.bandwidth;
	u_min = setInput.u_min;
	u_max = setInput.u_max;
	workload = setInput.workload;
	user_id = setInput.user_id;
}
//
void Input::display() {
	DEBUG_LOG << "\nDisplay INPUT \n";
	DEBUG_LOG << "nTask: " << nTasks << " timeHorizon: " << timeHorizon << "\n";
	disp_vector(releaseDate);
	disp_vector(dueDate);
	disp_vector(workload);
	disp_vector(u_min);
	disp_vector(u_max);
	disp_vector(bandwidth);
	DEBUG_LOG << "\n------------------\n";
}
Input::Input() :
		nTasks(0), timeHorizon(0), user_id(), releaseDate(), dueDate(), bandwidth(), u_min(), u_max(), workload() {
}
//
Input& Input::operator=(const Input& setInput) {
	nTasks = setInput.nTasks;
	timeHorizon = setInput.timeHorizon;
	releaseDate = setInput.releaseDate;
	dueDate = setInput.dueDate;
	bandwidth = setInput.bandwidth;
	u_min = setInput.u_min;
	u_max = setInput.u_max;
	workload = setInput.workload;
	user_id = setInput.user_id;
	return *this;
}
//
Input::~Input() {
	releaseDate.clear();
	dueDate.clear();
	bandwidth.clear();
	u_min.clear();
	u_max.clear();
	workload.clear();
}
//
template<typename T> void Input::readArray(Json::Value jsonArray,
		vector<T>& arr) {
	for (unsigned int index = 0; index < jsonArray.size(); ++index) {
		arr.push_back((T) jsonArray[index].asFloat());
	}
}
// Getting data methods
// System
const QInt Input::get_user_ID(QInt index) const{
	return(user_id[index]);
}
const QInt Input::get_nTasks() const {
	return nTasks;
}
const QInt Input::get_timeHorizon() const {
	return timeHorizon;
}
const numVec Input::get_bandwidth() const {
	return bandwidth;
}
// Each job
const QInt Input::get_releaseDate(QInt index) const {
	return releaseDate[index];
}
const QInt Input::get_dueDate(QInt index) const {
	return dueDate[index];
}
const QNum Input::get_workload(QInt index) const {
	return workload[index];
}
const QNum Input::get_res_ub(QInt index) const {
	return u_max[index];
}
const QNum Input::get_res_lb(QInt index) const {
	return u_min[index];
}
void Input::get_job(const QInt& index, QInt& rD, QInt& dD, QNum& wl, QNum& uM,
		QNum& um) const {
	rD = releaseDate[index];
	dD = dueDate[index];
	wl = workload[index];
	uM = u_max[index];
	um = u_min[index];
}

// Adding setting tools for ACPF 2.0, BA 1.0 (May 2016)
void Input::set_nTasks(int m_n) {
	nTasks = m_n;
}
void Input::set_timeHorizon(int m_H) {
	timeHorizon = m_H;
}
void Input::set_user_id(const intVec& m_user_id) {
	user_id = m_user_id;
}
void Input::set_releaseDate(const intVec& m_releaseDate) {
	releaseDate = m_releaseDate;
}
void Input::set_dueDate(const intVec& m_dueDate){
	dueDate = m_dueDate;
}
void Input::set_workload(const numVec& m_workload){
	workload = m_workload;
}
void Input::set_u_min(const numVec& m_u_min){
	u_min = m_u_min;
}
void Input::set_u_max(const numVec& m_u_max){
	u_max = m_u_max;
}
void Input::set_bandwidth(const numVec& m_bandwidth){
	bandwidth = m_bandwidth;
}
