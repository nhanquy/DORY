/*
 * ConstructiveHeuristic.cpp
 *
 *  Created on: Feb 12, 2016
 *      Author: Nhan-Quy NGUYEN
 */

#include "ConstructiveHeuristic.h"
#include <algorithm>    // std::random_shuffle
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
ConstructiveHeuristic::ConstructiveHeuristic():
	layering_max_iter(1),feasible(false)
{
}
ConstructiveHeuristic::ConstructiveHeuristic(const ConstructiveHeuristic& that)
{
	resource_allocation = that.resource_allocation;
	start_time = that.start_time;
	end_time = that.end_time;
	feasible = that.feasible;
	layering_max_iter = that.layering_max_iter;
}
ConstructiveHeuristic& ConstructiveHeuristic::operator=(const ConstructiveHeuristic& that)
{
	resource_allocation = that.resource_allocation;
	start_time = that.start_time;
	end_time = that.end_time;
	feasible = that.feasible;
	layering_max_iter = that.layering_max_iter;
	return *this;
}
void ConstructiveHeuristic::init_queue(int nTasks, vector<int>& queue)
{
	vector<int> fresh_vector(nTasks);
	queue=fresh_vector;
	for (int i=0;i<nTasks;++i)
		queue[i]=i;
}

void ConstructiveHeuristic::shuffle_queue(vector<int>& queue)
{
	std::random_shuffle(queue.begin(),queue.end());
}

void ConstructiveHeuristic::set_max_iter_layering(int max_iter)
{
	layering_max_iter=max_iter;
}

void ConstructiveHeuristic::solve(Input params)
{
	std::vector<int> layering_queue;
	init_queue(params.get_nTasks(),layering_queue);
	Layering layering;
	for(int iter=0; iter<layering_max_iter;++iter)
	{
		disp_vector(layering_queue, "Tasks queue");
		layering.solve(params,layering_queue);
		if (layering.isFeasible())
		{
			LOG(DEBUG) << "Feasible solution found!! \n";
			feasible=true;
			break;
		} else shuffle_queue(layering_queue);
		// Timer control
		if(timer.isTimeout())
		{
			feasible = false;
			return;
		}
	}
	if (feasible) // Getting solution
	{
		resource_allocation = layering.resource_allocation;
		start_time = layering.start_time;
		end_time = layering.end_time;
	}
}

bool ConstructiveHeuristic::isFeasible()const{
	return(feasible);
}

bool dominance(int arr1, int arr2, float pow1, float pow2){
	if (arr1<arr2)
		return true;
	else if(arr1 == arr2 && pow1>pow2)
		return true;
	return false;
}

void ConstructiveHeuristic::priority(intVec& list_EV, numVec& list_Power, const Input& params){
	list_EV.clear();
	list_Power.clear();
	intVec list_Arrival;
	unsigned int nTasks = start_time.size();
	LOG(DEBUG)<<"n="<<nTasks;
	for (unsigned int i=0;i<nTasks;i++){
		list_EV.push_back(params.get_user_ID(i));
		list_Power.push_back(resource_allocation[i][start_time[i]]);
		list_Arrival.push_back(start_time[i]);
	}
	// Bubble simple sort for the priority of EV
	for (int i=0;i<nTasks;++i)
		for(int j=i;j<nTasks;j++)
			if (dominance(list_Arrival[j], list_Arrival[i],list_Power[j], list_Power[i]))
			{
				swapping<QInt,intVec>(list_Arrival,i,j);
				swapping<QNum,numVec>(list_Power,i,j);
				swapping<QInt,intVec>(list_EV,i,j);
			}
}
