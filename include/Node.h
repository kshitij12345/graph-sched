#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include "Manager.h"

class Node{

public:
	int id;
	std::set<int> depends_on;
	std::set<int> dependents;
	void (*func)();

	Node(int id,std::set<int>depends_on,std::set<int>dependents,void (*func)());
	void ThreadFunc();
	std::thread ReturnFunc();

};