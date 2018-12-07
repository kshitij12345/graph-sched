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

class Node{

public:
	int id;
	std::vector<int> depends_on;
	std::vector<int> dependents;
	void (*func)();

	Node(int id,std::vector<int>depends_on,std::vector<int>dependents,void (*func)());
	void ThreadFunc();
	std::thread ReturnFunc();

};

//Nodes whose dependencies are resolved and can be run
extern std::queue<int> To_Run;

//Contains the id of completed nodes.
extern std::vector<int> Completed;

//Mutex to make sure the update of To_Run and Completed are atomic
extern std::mutex UpdateLock;

// Vector of all the nodes to run.
extern std::vector<Node> nodes;

//Method which updates the To_Run and Completed
void Update(std::vector<int> dependents,int id);

