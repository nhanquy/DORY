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
#include "TimeRef.h"
using namespace std;

int main() {
	timestr k0 = "10:00:00";
	timestr kH = "17:00:00";
	timestr testtime = "14:30:00";
	double dt = 0.25;
	TimeRef QTime(k0,kH,dt);
	timestr st = QTime.time_now();
	cout << "Time now! "<<st<<endl;
	cout << "This time is between "<<k0<<" and "<<kH<<" ? "<<QTime.isInHorizon(st)<<endl;
	printf("This time to double = %f ", QTime.timestr_to_double(st));
	return 0;
}
