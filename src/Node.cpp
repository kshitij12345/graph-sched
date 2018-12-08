#include "Manager.h"

Node::Node(int id,std::vector<int>depends_on,std::vector<int>dependents,void (*func)()){
	this->id = id;
	this->depends_on = depends_on;
	this->dependents = dependents;
	this->func = func;
}

// Not required
void Node::operator()(){
	std::cout << this->id << "Ssays hey\n"; 
	this->func();
}
