/*
 * EventHandler.cpp
 *
 *  Created on: May 13, 2016
 *      Author: nhanquy
 */

#include "EventHandler.h"

Event_Handler::Event_Handler() :
		config_got(false),
		estimation_json_setup(false),
		message_got(false),
		error(false),
		no_solution(true),
		FIFO(true),
		event_detected(false),
		algorithm_timeout(20)
{
}

/*
 * OBJECT CONFIGURATIONS
 */
std::string Event_Handler::read_config(const char* config_id)
{
	config_got = false;
	message_got = false;
	std::ifstream jsonDoc(config_file, std::ifstream::binary);
	if (!jsonDoc.is_open())
	{
		LOG(ERROR)<< "Unable to open file" << config_file;
		return "Unable to open configuration file";
	}
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonDoc, root);
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		LOG(ERROR)<< "Failed to parse input file\n"<< reader.getFormattedErrorMessages();
		return "Configuration file parsing failed.";
	}
	// Re-map root
	root = root[config_id];
	if (root.isNull())
		return "Configuration id not existed.";
	// Get number of jobs
	LOG(DEBUG)<<"Begin to configure Parking";
	timestr start_time = root["Start_time"].asString();
	LOG(DEBUG)<<"Start time read: "<<start_time;
	timestr end_time = root["End_time"].asString();
	LOG(DEBUG)<<"End time read: "<<end_time;
	double set_bw = root["Bandwidth"].asDouble();
	LOG(DEBUG)<<"Bandwidth read: "<<set_bw;
	double delta_t = root["Block_duration"].asDouble();
	LOG(DEBUG)<<"Block duration read: "<<delta_t;
	FIFO = root["FIFO"].asBool();
	LOG(DEBUG)<<"FIFO read: "<<FIFO;
	// BA general configurations
	algorithm_timeout = root["BA_timeout"].asDouble();
	LOG(DEBUG)<<"BA Timeout read: "<<algorithm_timeout;
	// Time Configuration
	Event_Time = TimeRef(start_time, end_time, delta_t);
	// Parking configurations
	parking.set_QTime(Event_Time);
	parking.set_bandwidth(set_bw);
	parking.set_fifo(FIFO);
	config_got = true;
	message_got = true;
	return "Configuration loading done";
}

void Event_Handler::setup_estimation_JSON(const char* JSON_file)
{
	estimation_json_setup = false;
	std::ifstream jsonDoc(JSON_file, std::ifstream::binary);
	if (!jsonDoc.is_open())
	{
		LOG(ERROR)<< "Unable to open estimation file" << JSON_file;
		return;
	}
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonDoc, m_estimation);
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		LOG(ERROR)<< "Failed to parse estimation file"<< reader.getFormattedErrorMessages();
		return;
	}
	if (m_estimation.isNull())
	{
		LOG(ERROR)<< "Failed to load estimation data";
		return;
	}
	estimation_json_setup = true;
	LOG(INFO)<<"Estimation data loaded sucessfully";
}

// Database tools
int  Event_Handler::find_user_index(int user_id) const
{
	for (unsigned int index=0;index<m_estimation.size();index++)
	{
		if (m_estimation[index]["user_id"].asInt() == user_id)
			return index;
	}
	return 0;
}
//
double Event_Handler::load_demand_estimation(int index) const
{
	return m_estimation[index]["demand"].asDouble();
}
//
timestr Event_Handler::load_exp_depature(int index) const
{
	return m_estimation[index]["departure"].asString();
}
// Event definition
void Event_Handler::CONFIG_LOAD(std::string command_line)
{
	std::istringstream iss(command_line);
	std::string config_id;
	std::string config_command;
	iss >> config_command >> config_id;
	LOG(DEBUG)<<"Command line: "<<command_line;
	LOG(DEBUG)<<"Configuration id parsed: "<<config_id;
	response_message = read_config(config_id.c_str());
	event_detected = true;
}
void Event_Handler::AUT(int user_id,
						int borne_id,
						double charging_power)
{
	//bool existed;
	LOG(DEBUG)<<"AUT event being read...";
	int est_index = find_user_index(user_id);
	double est_demand = load_demand_estimation(est_index);
	timestr exp_departure = load_exp_depature(est_index);
	double hours_dep = Event_Time.timestr_to_block(exp_departure);
	// If the departure time exceeds the ending horizon,
	// so the departure time will set to ending horizon to standardize ACPF input.
	if (hours_dep > Event_Time.hours_end() || hours_dep == 0) // In case EV depart after the scheduling ended or exact 24h ahead
		hours_dep = Event_Time.hours_end();
	// TODO: Becareful here!!
	double hours_arr = Event_Time.hours_now();
	parking.add_EV(user_id, borne_id, hours_arr, hours_dep, est_demand,
			charging_power);
	event_detected = true;
}

void Event_Handler::FDC(int user_id)
{
	parking.end_of_charge(user_id);
	event_detected = true;
}
void Event_Handler::ANU(int user_id)
{
	parking.charge_cancelling(user_id);
	event_detected = true;
}
void Event_Handler::DCF(int user_id,
						double charging_power)
{
	parking.forced_charge(user_id, charging_power);
	event_detected = true;
}
void Event_Handler::PID(int user_id,
						double charging_power)
{
	parking.changing_power(user_id, charging_power);
	event_detected = true;
}
void Event_Handler::CNU(int user_id)
{
	parking.no_charge(user_id);
	event_detected = true;
}
void Event_Handler::MCR()
{
	message_got = false;
	response_message = "Message corrupted!";
	event_detected = true;
}
;
/*
 * Getting json_input events
 */
bool Event_Handler::getting_event(const Json::Value& root)
{
	// AUT: Authentification
	event_detected = false;
	const Json::Value root_AUT = root["AUT"];
	for (unsigned int index = 0; index < root_AUT.size(); index++)
	{
		try
		{
			int user_id = root_AUT[index]["user_id"].asInt();
			int borne_id = root_AUT[index]["borne_id"].asInt();
			double charging_power =
					root_AUT[index]["charging_power"].asDouble();
			AUT(user_id, borne_id, charging_power);
		} catch (...)
		{
			MCR();
			return false;
		}
	}
	// FDC: Fin de Charge
	const Json::Value root_FDC = root["FDC"];
	for (unsigned int index = 0; index < root_FDC.size(); index++)
	{
		try
		{
			FDC(root_FDC[index]["user_id"].asInt());
		} catch (...)
		{
			MCR();
			return false;
		}
	}
	// ANU: Anulation de Charge
	const Json::Value root_ANU = root["ANU"];
	for (unsigned int index = 0; index < root_ANU.size(); index++)
	{
		try
		{
			ANU(root_ANU[index]["user_id"].asInt());
		} catch (...)
		{
			MCR();
			return false;
		}
	}
	// DCF: Depart de la Charge Forcee
	const Json::Value root_DCF = root["DCF"];
	for (unsigned int index = 0; index < root_DCF.size(); index++)
	{
		try
		{
			int user_id = root_DCF[index]["user_id"].asInt();
			double charging_power =
					root_DCF[index]["charging_power"].asDouble();
			DCF(user_id, charging_power);
		} catch (...)
		{
			MCR();
			return false;
		}
	}
	// PID: Puissance neccesaire inferieure a consigne
	const Json::Value root_PID = root["PID"];
	for (unsigned int index = 0; index < root_PID.size(); index++)
	{
		try
		{
			int user_id = root_PID[index]["user_id"].asInt();
			double charging_power =
					root_PID[index]["charging_power"].asDouble();
			PID(user_id, charging_power);
		} catch (...)
		{
			MCR();
			return false;
		}
	}
	// CNU: Charge null
	const Json::Value root_CNU = root["CNU"];
	for (unsigned int index = 0; index < root_CNU.size(); index++)
	{
		try
		{
			CNU(root_CNU[index]["user_id"].asInt());
		} catch (...)
		{
			MCR();
			return false;
		}
	}
	message_got = true;
	return event_detected;
}

/*
 *  Resolution procedure
 *  1. Parking export new input
 *  2. ACPF get the input to solve for solution
 *  3. Solution analyzer is used to sort EVs queue
 */
bool Event_Handler::find_solution()
{
	if (FIFO)
	{
		LOG(INFO)<<"FIFO configuration, no need to find BA Solution.";
		return false;
	}
	QTimer timer;
	timer.set_timeout(algorithm_timeout);
	timer.start();
	Input ACPF_input;
	parking.export_ACPF_input(ACPF_input);
	algorithm.solve(ACPF_input);
	if(algorithm.isFeasible())
	{
		// Exporting result
		algorithm.priority(prioEV,prioPower,ACPF_input);
		LOG(INFO)<<"Solution found.";
		no_solution = false;
		return true;
	}
	else
	LOG(INFO)<<"Algorithm cannot find a feasible solution.";
	no_solution = true;
	return false;
}

bool Event_Handler::write_response()
{
	if (prioEV.size() == 0 || prioPower.size() == 0 || no_solution)
	{
		LOG(ERROR)<<"No EV priority is set.";
		write_no_solution();
		return false;
	}
	try
	{
		Json::Value root;
		root.clear();
		for(unsigned int index = 0; index<prioEV.size(); index++)
		{
			root[index]["user_id"] = prioEV[index];
			root[index]["power"] = prioPower[index];
			root[index]["borne_id"] = parking.search_borne_ID(prioEV[index]);
		}
		Json::StyledWriter writer;
		response_message=writer.write(root);
		LOG(DEBUG)<<"Response to client: "<<response_message;
		return true;
	}
	catch(...)
	{
		LOG(ERROR)<<"Cannot write solution to JSON msg.";
		write_no_solution();
		return false;
	}
}

std::string Event_Handler::get_message() const
{
	return response_message;
}

void Event_Handler::testing_JSON_inout()
{
	prioEV.clear();
	prioPower.clear();
	int n = 3;
	for (int i = 0; i < n; i++)
	{
		prioEV.push_back(i * 111);
		prioPower.push_back(i * 11.5);
	}
	if (write_response())
	{
		std::cout << response_message;
	}
}
void Event_Handler::write_no_solution()
{
	LOG(ERROR)<<"No solution signal set";
	response_message = "no_solution";
}
