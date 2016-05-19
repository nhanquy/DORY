/*
 * QServer.cpp
 *
 *  Created on: May 17, 2016
 *      Author: nhanquy
 */

#include "QServer.h"
using namespace std;

void error(const char *msg) {
	perror(msg);
	exit(1);
}

QServer::QServer():	con_etab(false),
sockfd(0), portno(0),clilen(0){
}

QServer::QServer(int m_port):con_etab(false),
		sockfd(0), portno(m_port),clilen(0){
}
QServer::~QServer() {
	close_server_socket();
}

bool QServer::etablish_connection() {
	/* First call to socket() function */
	con_etab = false;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
	con_etab = true;
	return(true);
}

bool QServer::read_buffer(int newsockfd) {
	if(!con_etab)
		return false;
	bzero(buffer, 1024);
	int n = read(newsockfd, buffer, 1023);
	if (n < 0){
		error("ERROR reading from socket");
		return false;
	}
	printf("Here is the message: %s\n", buffer);
	return true;
}

bool QServer::write_response(int newsockfd,const char* message) {
	if(!con_etab)
		return false;
	int n = write(newsockfd, message, 18);
	if (n < 0){
		error("ERROR writing to socket");
		return false;
	}
	else return true;
}
bool QServer::accept_connection(int& newsockfd){
	newsockfd = accept(sockfd,
	               (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd<0)
		return false;
	else return true;
}
void QServer::close_client_socket(int newsockfd) {
	close(newsockfd);
}
//
void QServer::close_server_socket(){
	close(sockfd);
}
//
std::string QServer::get_buffer(){
	std::string result(buffer);
	return(result);
}
