/*
 * EventHandler.cpp
 *
 *  Created on: May 13, 2016
 *      Author: nhanquy
 */

#include "EventHandler.h"

Event_Handler::Event_Handler() :
		config_got(false),hist_db_setup(false), message_got(false), error(false), no_solution(
				true), algorithm_timeout(20) {
}

Event_Handler::~Event_Handler() {
	// TODO Auto-generated destructor stub
}

void Event_Handler::read_config(char* config_file, char* config_id) {
	std::ifstream jsonDoc(config_file, std::ifstream::binary);
	if (!jsonDoc.is_open()) {
		DEBUG_LOG << "Unable to open file" << config_file
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
	// Re-map root
	root = root[config_id];
	// Get number of jobs
	timestr start_time  = root["Start_time"].asString();
	timestr end_time = root["End_time"].asString();
	double set_bw = root["Bandwidth"].asDouble();
	double delta_t = root["Block_duration"].asDouble();
	bool FIFO = root["FIFO"].asBool();
	// BA general configurations
	algorithm_timeout = root["BA_timeout"].asDouble();
	// Time Configuration
	Event_Time = TimeRef(start_time,end_time,delta_t);
	// Parking configurations
	parking.set_QTime(Event_Time);
	parking.set_bandwidth(set_bw);
	parking.set_fifo(FIFO);
	config_got=true;
}

