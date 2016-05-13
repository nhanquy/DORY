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
using namespace std;

void error(const char *msg) {
	perror(msg);
	exit(1);
}
int main(/*int argc, char *argv[]*/) {
	int argc=2;
	char *argv[2]={"localhost","10000"};
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
	bzero(buffer, 256);
	n = read(newsockfd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket");
	printf("Here is the message: %s\n", buffer);
	n = write(newsockfd, "I got your message", 18);
	if (n < 0)
		error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	//
	timestr k0 = "10:00:00";
	timestr kH = "17:00:00";
	timestr testtime = "14:30:00";
	double dt = 0.25;
	TimeRef QTime(k0, kH, dt);
	timestr st = QTime.time_now();
	cout << "Time now! " << st << endl;
	cout << "This time is between " << k0 << " and " << kH << " ? "
			<< QTime.isInHorizon(st) << endl;
	printf("This time to double = %f ", QTime.timestr_to_double(st));
	return 0;
}
