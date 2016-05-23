/*
 * Layering.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: Nhan-Quy NGUYEN
 */

#include "Layering.h"
using namespace std;
//
Layering::Layering() :
		nTasks(0), timeHorizon(0), max_resource(0), feasible(true) {
}

QNum Layering::get_peak(int start, int end, numVec res_row, QNum& sum) {
	if ((unsigned int) end >= res_row.size()) {
		LOG(DEBUG) << "res_row out of rank: ending at " << end << " instead of "
				<< res_row.size();
		feasible = false;
		return -1;
	}
	numVec sub_row = sub_vector(res_row, start, end);
	numVec sub_res = sub_vector(used_resource, start, end);
	numVec sum_vec = vec_sum(sub_row, sub_res);
	sum = sum_vector<QNum>(sum_vec);
	int pos;
	QNum maxpeak = max_vector<QNum>(sum_vec, pos);

	 LOG(DEBUG)<<"Get_peak: start "<<start<<" end "<<end<<endl;
	 disp_vector(sub_row,"sub_row");
	 disp_vector(sub_res,"sub_res");
	 disp_vector(sum_vec,"sum_res");
	 LOG(DEBUG) << "Max peak: " << maxpeak << endl << endl;
	return (maxpeak);
}
QNum Layering::round_res(QNum workload, QInt dur, QNum res_lb) {
	if (dur == 0) {
		LOG(DEBUG) << "Division by zeros!";
		return (0);
	}
	QNum rr = (QNum) workload / dur;
	return (rr < res_lb ? res_lb : rr);
}

void Layering::initialize(int set_nTasks, int set_timeHorizon) {
	feasible = true;
	nTasks = set_nTasks;
	timeHorizon = set_timeHorizon;
	numMat freshRes(nTasks, numVec(timeHorizon));
	numVec freshRV(timeHorizon);
	intVec freshTV(nTasks);
	resource_allocation = freshRes;
	start_time = freshTV;
	end_time = freshTV;
	used_resource = freshRV;
}

void Layering::get_used_resource(const numVec& bandwidth) {
	/* DEBUG
	 disp_vector(bandwidth, "BANDWIDTH");
	 END DEBUG */
	int pos;
	unsigned int size = bandwidth.size();
	if (used_resource.size() != size) // haven't been initialized yet
			{
		numVec freshVec(size);
		used_resource = freshVec;
	}
	max_resource = max_vector<QNum>(bandwidth, pos);
	for (unsigned int i = 0; i < size; ++i) {
		used_resource[i] = max_resource - bandwidth[i];
	}
}

void Layering::minmax_planning(int jobID, QNum workload, QNum res_lb,
		QNum res_ub, QInt releaseDate, QInt dueDate) {
	LOG(DEBUG) << "Minmax planning for task " << jobID << endl;
	disp_vector(used_resource, "before planning: used_resource");
	QInt dur_max = (QInt) ceil(workload / res_lb);
	if (dur_max > dueDate - releaseDate)
		dur_max = dueDate - releaseDate;
	QInt dur_min = (QInt) ceil(workload / res_ub);
	QNum res;
	QNum minmaxPeak = max_resource + 1; // +1 to check infeasibility
	QNum minmaxSum;
	QNum peak;
	QNum sum;
	numVec res_row(timeHorizon);
	for (QInt dur = dur_max; dur >= dur_min; --dur) {
		for (QInt start = releaseDate; start <= dueDate - dur; ++start) {
			res = round_res(workload, dur, res_lb);
			allocate_resource(start, start + dur - 1, res, res_row);
			peak = get_peak(start, start + dur - 1, res_row, sum);
			if ((peak < minmaxPeak)
					|| (peak == minmaxPeak && sum < minmaxSum)) {
				minmaxPeak = peak;
				minmaxSum = sum;
				start_time[jobID] = start;
				end_time[jobID] = start + dur;
				resource_allocation[jobID] = res_row;
			}
			// Timer control
			if(timer.isTimeout())
			{
				feasible = false;
				return;
			}
		}
	}
	if (minmaxPeak > max_resource)
		feasible = false;
	else
		used_resource = vec_sum(used_resource, resource_allocation[jobID]);
	disp_vector(used_resource,"after planning: used_resource");
}

void Layering::allocate_resource(int start, int end, QNum res,
		numVec& res_row) {
	int size = res_row.size();
	for (int i = 0; i < size; ++i)
		if (i >= start && i <= end)
			res_row[i] = res;
		else
			res_row[i] = 0;
}

void Layering::solve(Input params, std::vector<int> tasks_queue) {
	LOG(DEBUG) << "Begin to layer tasks...\n";
	initialize(params.get_nTasks(), params.get_timeHorizon());
	get_used_resource(params.get_bandwidth());
	int tasks_count = 0;
	int jobID;
	QInt releaseDate;
	QInt dueDate;
	QNum workload;
	QNum res_UB;
	QNum res_LB;
	while ((feasible) && (tasks_count < nTasks)) {
		jobID = tasks_queue[tasks_count];
		params.get_job(jobID, releaseDate, dueDate, workload, res_UB, res_LB);
		minmax_planning(jobID, workload, res_LB, res_UB, releaseDate, dueDate);
		++tasks_count;
	}
	if (feasible) {
		LOG(INFO) << "Layering done!";
		disp_matrix(resource_allocation, "Resource allocation");
	} else {
		LOG(INFO) << "Unable to find solution!";
		disp_matrix(resource_allocation, "Resource allocation");
	}
}

bool Layering::isFeasible() const {
	return (feasible);
}
