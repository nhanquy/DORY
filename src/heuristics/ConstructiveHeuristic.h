/*
 * ConstructiveHeuristic.h
 *
 *  Created on: Feb 12, 2016
 *      Author: Nhan-Quy NGUYEN
 */

#ifndef CONSTRUCTIVEHEURISTIC_H_
#define CONSTRUCTIVEHEURISTIC_H_
#include "../libs/matrix.h"
#include "../libs/QTimer.h"
#include "Input.h"
#include "Layering.h"
/*
 *
 */
class ConstructiveHeuristic
{
public:
	ConstructiveHeuristic();
	ConstructiveHeuristic(const ConstructiveHeuristic& that);
	ConstructiveHeuristic& operator=(const ConstructiveHeuristic& that);
	bool isFeasible() const;
	void solve(Input params);
	void set_max_iter_layering(int max_iter);
	/*
	 * void priority: Sorting task by start time time and power to export a solution
	 * for ACPF 2.0 --> BA 1.0.
	 */
	void priority(intVec& list_EV, numVec& list_Power,const Input& params);
private:
	numMat resource_allocation;
	intVec start_time;
	intVec end_time;
	int layering_max_iter;
	bool feasible;
	QTimer timer;
	void init_queue(int nTasks, vector<int>& queue);
	void shuffle_queue(vector<int>& queue);
};

#endif /* CONSTRUCTIVEHEURISTIC_H_ */
