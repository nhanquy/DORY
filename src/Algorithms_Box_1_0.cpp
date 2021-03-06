//============================================================================
// Name        : Algorithms_Box_1_0.cpp
// Author      : Nhan-Quy NGUYEN
// Version     : 1.0
// Copyright   : Copyright Park'n'Plug 2016
// Description : Algorithms box 1.0 which helps to getting events from GO
//				 and analyzing the request to create suitable input file for
//				 the chosen algorithm
//============================================================================

//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
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
#define ELPP_DISABLE_WARNING_LOGS
#define ELPP_DISABLE_FATAL_LOGS
#define ELPP_DISABLE_VERBOSE_LOGS
#define ELPP_DISABLE_TRACE_LOGS

using namespace std;\

void read_params(	const char* config_file,
					std::string& est_json_dir,
					std::string& log_dir,
					int& port_no)
{
	std::ifstream jsonDoc(config_file, std::ifstream::binary);
	if (!jsonDoc.is_open())
	{
		//LOG(ERROR)<< "Unable to open configuration file" << config_file;
		exit(-1);
	}
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonDoc, root);
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		//LOG(ERROR)<< "Failed to parse configuration file\n"<< reader.getFormattedErrorMessages();
		exit(-1);
	}
	// BA Parameters read
	Json::Value ba_params = root["BA_PARAMS"];
	log_dir = ba_params["LOGSDIR"].asString();
	est_json_dir = ba_params["DB_FILE"].asString();
	port_no = ba_params["PORT"].asInt();
	if (log_dir.size() == 0)
		log_dir = "logs";
	// DEBUG flag
	bool debug_active = ba_params["DEBUG"].asBool();
	/*
	 * Setting logging
	 */
	string log_file = log_dir + "/ba_log";
	string deb_file = log_dir + "/ba_deb";
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Filename,
			log_file);
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput,
			"false");
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, "true");
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize,
			"262144"); // 250 kb
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::LogFlushThreshold,
			"100"); // 100 lines
	if (debug_active)
	{
		el::Loggers::reconfigureAllLoggers(el::Level::Debug,
				el::ConfigurationType::Filename, deb_file);
		el::Loggers::reconfigureAllLoggers(el::Level::Debug,
				el::ConfigurationType::LogFlushThreshold, "100");
		el::Loggers::reconfigureAllLoggers(el::Level::Debug,
				el::ConfigurationType::MaxLogFileSize, "262144");
	}
	else
	{
		el::Loggers::reconfigureAllLoggers(el::Level::Debug,
				el::ConfigurationType::Enabled, "false");
		//el::Loggers::reconfigureAllLoggers(el::Level::Debug,el::ConfigurationType::ToFile,"false");
	}
	LOG(INFO)<<"Log file is configured sucessfully!";
	LOG(DEBUG)<<"STARTING TO DEBUG...";
	LOG(INFO)<<"LOGSDIRS read: "<<log_dir;
	LOG(INFO)<<"Est_Json read: "<<est_json_dir;
	LOG(INFO)<<"PORT read: "<<port_no;
	LOG(INFO)<<"Debug active: "<<debug_active;
}
int main(int argc, char *argv[])
{
	static const std::string BA_version_check =
			"BA 1.0.4, last build on 16-06-2016 at 17h00";
	/*
	 * usage = config_dir
	 */
	if (argc != 2)
	{
		// wrong usage
		exit(1);
	}
	bool version_check;
	const char* config_dir = argv[1];
	string est_json_dir, log_dir;
	int port_no;
	read_params(config_dir, est_json_dir, log_dir, port_no);
	// -------------------
	// SETUP EVENT HANDLER
	// -------------------
	Event_Handler event;
	event.setup_estimation_JSON(est_json_dir.c_str());
	event.read_config_dir(config_dir);
	// -----------------
	// PREPARING SERVER
	// -----------------
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
					version_check = false;
					len = stream->receive(line, sizeof(line));
					line[len] = 0;// Remove extra len gabarge from previous message.
					std::string str_line(line);
					//std::cout<<"Client sent: "<<str_line<<endl;
					bool parsingSuccessful = reader.parse(line, root);
					if (!parsingSuccessful)
					{
						// -------------
						// Shutdown case
						// -------------
						if (str_line.find("shutdown") != std::string::npos)
						{
							std::string shutdown_message =
									"Server shutting down...";
							stream->send(shutdown_message.c_str(),
									shutdown_message.size());
							delete stream;
							LOG(INFO)<<"Closing socket... Shutdown server...";
							exit(0);
						} // End shutdown case
						  // -------------------
						  // Closing_socket case
						  // -------------------
						else if (str_line.find("close_socket")
								!= std::string::npos)
						{
							std::string s_message = "Socket closing...";
							stream->send(s_message.c_str(), s_message.size());
							LOG(INFO)<<"Socket closed";
							communicating = false;
						} // End closing socket case
						  // ----------------------
						  // Checking version case
						  // ----------------------
						else if (str_line.find("version") != std::string::npos)
						{
							std::string s_message = BA_version_check;
							stream->send(s_message.c_str(), s_message.size());
							LOG(INFO)<<"Version checked";
							version_check = true;
						}						// End checking version case
												// ------------------------
												// Read configuration case
												// ------------------------
						else if (str_line.find("config") != std::string::npos)
						{
							event.CONFIG_LOAD(str_line);
						} // End read configuration case
						else
						{
							// Report to the user the failure and their locations in the document.
							LOG(ERROR)<< "Failed to parse input file: "<< reader.getFormattedErrorMessages();
							event.MCR();
						}
					}
					else if(event.isConfigured())
					{ // Parsing successfully
					  // Getting the events from client's message
						bool event_catched = event.getting_event(root);
					  // Find solution
						if (event_catched) event.find_solution();
						else event.MCR();
					  // Write message
						event.write_response();
					}
					//Send response
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
