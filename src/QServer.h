/*
 * QServer.h
 *
 *  Created on: May 17, 2016
 *      Author: nhanquy
 */

#ifndef QSERVER_H_
#define QSERVER_H_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class QServer {
public:
	QServer();
	QServer(int m_port);
	virtual ~QServer();
	void set_port_no(int m_port);
	bool etablish_connection();
	//
	bool read_buffer(int newsockfd);
	bool write_response(int newsockfd,const char* message);
	bool accept_connection(int& newsockfd);
	void close_client_socket(int newsockfd);
	//
	void close_server_socket();
	std::string get_buffer();
private:
	bool con_etab;
	int sockfd, portno;
	socklen_t clilen;
	char buffer[1024];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
};
/*
 * Les choses
 * et
 * LA MOUUUUUCHE
 */
#endif /* QSERVER_H_ */
