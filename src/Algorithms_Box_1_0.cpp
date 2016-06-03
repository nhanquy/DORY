//============================================================================
// Name        : Algorithms_Box_1_0.cpp
// Author      : Nhan-Quy NGUYEN
// Version     : 1.0
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
#include "TCPSockets/tcpacceptor.h"
#include "libs/easylogging++.h"
#define ERROR_LIMIT 100
INITIALIZE_EASYLOGGINGPP
using namespace std;
int main(	int argc,
			char *argv[])
{
	static const std::string BA_version_check = "BA 1.0.3, last build 14:18:00 06-06-2016";
	/*
	 * usage = config_dir, config_id, db_dir, port_no, log_file
	 */
	if (argc < 5 || argc > 6)
	{
		// wrong usage
		exit(1);
	}
	bool version_check;
	const char* config_dir = argv[1];
	const char* config_id = argv[2];
	const char* db_dir = argv[3];
	int port_no = atoi(argv[4]);
	const char* log_dir;
	if (argc == 6)
		log_dir = argv[5];
	else
		log_dir = "logs/BA_LOG.log";
	/*
	 * Setting logging
	 */
	try
	{ // In case file-name corrupted
		el::Loggers::reconfigureAllLoggers(
				el::ConfigurationType::ToStandardOutput, "false");
		el::Configurations c;
		c.setGlobally(el::ConfigurationType::Filename, log_dir);
		c.parseFromText("*DEBUG:\n FILENAME = \"logs/BA_DEBUG.log\"");
		el::Loggers::setDefaultConfigurations(c, true);
	} catch (...)
	{
		// Since logger isn't configured yet, it cannot be called to show the error.
		// Exit error
		exit(1);
	}
	// Add event
	Event_Handler event;
	event.read_config(config_dir, config_id);
	event.setup_db_handler(db_dir);
	Json::Value root;
	Json::Reader reader;
	// Configuring server
	TCPStream* stream = NULL;
	TCPAcceptor* acceptor = NULL;
	acceptor = new TCPAcceptor(port_no);
	if (acceptor->start() == 0)
	{
		int error_count = 0;
		while (error_count < ERROR_LIMIT)
		{
			stream = acceptor->accept();
			if (stream != NULL)
			{
				ssize_t len;
				char line[1024];
				bool communicating = true;
				while (communicating)
				{
					version_check=false;
					len = stream->receive(line, sizeof(line));
					std::string str_line(line);
					bool parsingSuccessful = reader.parse(line, root);
					if (!parsingSuccessful)
					{
						// Shutdown case
						if (str_line.find("shutdown") != std::string::npos)
						{
							std::string shutdown_message =
									"Server shutting down...";
							stream->send(shutdown_message.c_str(),
									shutdown_message.size());
							delete stream;
							LOG(INFO)<<"Closing socket... Shutdown server...";
							exit(0);
						}
						// close_socket case
						else if (str_line.find("close_socket")
								!= std::string::npos)
						{
							std::string s_message = "Socket closing...";
							stream->send(s_message.c_str(),
									s_message.size());
							LOG(INFO)<<"Socket closed";
							communicating = false;
						}
						else if (str_line.find("version")
								!= std::string::npos)
						{
							std::string s_message = BA_version_check;
							stream->send(s_message.c_str(),
									s_message.size());
							LOG(INFO)<<"Version checked";
							version_check=true;
						}
						else
						{
							// Report to the user the failure and their locations in the document.
							LOG(ERROR)<< "Failed to parse input file: "<< reader.getFormattedErrorMessages();
							event.MCR();
						}
					}
					else
					{ // Parsing successfully
					  // Open the historical database to prepare estimation data
					  // for Event Handler to acquire it later
						event.open_hist_db();
					  // Getting the events from client's message
						bool event_catched = event.getting_event(root);
					  // Close the event when done
						event.close_hist_db();
					  // Find solution
						if (event_catched) event.find_solution();
						else event.MCR();
					  // Write message
						event.write_response();
					}
					//Send response
					line[len] = 0;
					LOG(INFO)<<"Server received: "<<line;
					if (communicating && !version_check)
						stream->send(event.get_message().c_str(),
								event.get_message().size());
				}	// End while communicating
			}
			else
				// If stream->accept encounter error(s)
				error_count++;
			// Error or not, closing steam after all
			delete stream;
		}
	}
	LOG(ERROR)<<"Could not start the server at port "<<port_no;
	exit(-1);
}
