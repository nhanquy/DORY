/*
 * Layering.h
 *
 *  Created on: Feb 11, 2016
 *      Author: Nhan-Quy NGUYEN
 */

#ifndef LAYERING_H_
#define LAYERING_H_
#include "../libs/matrix.h"
#include "../libs/QTimer.h"
#include "Input.h"
#include "../libs/easylogging++.h"
/*
 *
 */
class Layering
{
public:
	explicit Layering();
	numMat resource_allocation;
	intVec start_time;
	intVec end_time;
	void solve(Input params,std::vector<int> tasks_queue);
	bool isFeasible() const;
private:
	int nTasks;
	int timeHorizon;
	numVec used_resource;
	QNum max_resource;
	QNum get_peak(int start, int end, numVec res_row, QNum& sum);
	QNum round_res(QNum workload, QInt dur, QNum res_lb);
	void initialize(int set_nTasks, int timeHorizon);
	void get_used_resource(const numVec& bandwidth);
	void minmax_planning(int jobID, QNum workload, QNum res_lb,
			QNum res_ub, QInt releaseDate, QInt dueDate);
	void allocate_resource(int start,int end,QNum res,numVec& res_row);
	bool feasible;
	QTimer timer;
};

#endif /* LAYERING_H_ */
