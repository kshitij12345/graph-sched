#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include "Manager.h"

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