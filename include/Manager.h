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

//Nodes whose dependencies are resolved and can be run
extern std::queue<int> To_Run;

//Contains the id of completed nodes.
extern std::vector<int> Completed;

//Mutex to make sure the update of To_Run and Completed are atomic
extern std::mutex UpdateLock;

//Method which updates the To_Run and Completed
void Update(std::vector<int> dependents,int id);

// class Node{

// public:
// 	int id;
// 	std::set<int> depends_on;
// 	std::set<int> dependents;
// 	void (*func)();

// 	Node(int id,std::set<int>depends_on,std::set<int>dependents,void (*func)());
// 	void ThreadFunc();
// 	std::thread ReturnFunc();

// };