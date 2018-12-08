/*
This file contains the declaration of structures and functions
used to solve the dependencies
*/


#pragma once
#include <stdio.h>
#include <thread>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include <mutex>
#include <set>

struct Node{
	int id;
	std::vector<int> depends_on;
	std::vector<int> dependents;
	void (*func)();

	Node(int id,std::vector<int>depends_on,std::vector<int>dependents,void (*func)()){
		this->id = id;
		this->depends_on = depends_on;
		this->dependents = dependents;
		this->func = func;
	}

	void operator()(){
		std::cout << "Func Id :" <<this->id << " has started\n\n"; 
		this->func();
	}
};

struct Manager{
	// Vector of all the nodes to run.
	std::vector<Node> nodes;

	// Nodes whose dependencies are resolved and can be run
	std::queue<int> to_run;
	std::set<std::reference_wrapper<int>> to_run_set;

	// Contains the id of completed nodes.
	std::vector<int> completed_nodes;
	std::set<int> completed_set;

	// Mutex to make sure the update of to_run and completed are atomic
	std::mutex update_lock;

	// Method which atomically updates the to_run and completed
	void update(std::vector<int> dependents,int id);

	// returns bool representing if all parents
	// of current indexed node have finished
	bool if_all_parents_fin(int i);

	void execute(int src_node = 0);

};

