//============================================================================
// Name        : Algorithms_Box_1_0.cpp
// Author      : Nhan-Quy NGUYEN
// Version     :
// Copyright   : Copyright Park'n'Plug 2016
// Description : Algorithms box 1.0 which helps to getting events from GO
//				 and analyzing the request to create suitable input file for
//				 the chosen algorithm
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "TimeRef.h"
#include "EventHandler.h"
#include "QServer.h"
#include "libs/easylogging++.h"

INITIALIZE_EASYLOGGINGPP
using namespace std;
int main(int argc, char *argv[]) {
	/*
	 * argv = config_dir, config_id, db_dir, port_no
	 */
	const char* config_dir = argv[1];
	const char* config_id = argv[2];
	const char* db_dir = argv[3];
	int port_no = atoi(argv[4]);
	// Add event
	Event_Handler event;
	event.read_config(config_dir, config_id);
	event.setup_db_handler(db_dir);
	Json::Value root;
	Json::Reader reader;
	// Configuring server
	QServer jsonserver(port_no);
	if (!jsonserver.etablish_connection())
		exit(1);
	int pid,client_socket;
	while (1) {
		jsonserver.accept_connection(client_socket);
		pid = fork();
		if (pid < 0)
			LOG(ERROR) << "ERROR on fork";
		if (pid == 0) {
			jsonserver.close_server_socket();
			// Read buffer
			jsonserver.read_buffer(client_socket);
			// Parsing into JSON value
			bool parsingSuccessful = reader.parse(jsonserver.get_buffer(),
					root);
			if (!parsingSuccessful) {
				// report to the user the failure and their locations in the document.
				LOG(ERROR) << "Failed to parse input file\n"<< reader.getFormattedErrorMessages();
				event.MCR();
				exit(1);
			}
			else{
				// Message got!
				// Event handling to Parking
				event.open_hist_db();
				event.getting_event(root);
				event.close_hist_db();
				// Find solution
				event.find_solution();
				// Write message
				event.write_response();
			}
			//Send response
			jsonserver.write_response(client_socket,event.get_message().c_str());
			exit(0);
		} else
			jsonserver.close_client_socket(client_socket);
	}
	return 0;
}
