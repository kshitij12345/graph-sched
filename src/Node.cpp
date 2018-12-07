#include "Manager.h"

Node::Node(int id,std::vector<int>depends_on,std::vector<int>dependents,void (*func)()){
	this->id = id;
	this->depends_on = depends_on;
	this->dependents = dependents;
	this->func = func;
}

void Node::ThreadFunc(){
	std::cout << this->id << "Ssays hey\n"; 
	this->func();
	Update(this->dependents,this->id);

}

std::thread Node::ReturnFunc(){
	return std::thread([=]{ThreadFunc();});
}