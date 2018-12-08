/*
This file contains the declaration of structures and functions
used to solve the dependencies
*/


#pragma once
#include <thread>
#include <queue>
#include <vector>
#include <iostream>
#include <mutex>
#include <set>
#include <memory>
#include <map>

struct BaseNode{
	int id;
	std::vector<int> depends_on;
	std::vector<int> dependents;
	
	virtual ~BaseNode(){}
	virtual void call() const = 0;
};

template <typename F>
struct Node : BaseNode{
	F func;

	Node(int id,std::vector<int>depends_on,std::vector<int>dependents,F func){
		this->id = id;
		this->depends_on = depends_on;
		this->dependents = dependents;
		this->func = func;
	}

	// overload with call to wrapped function.
	void call() const{
		std::cout << "Func Id :" <<this->id << " has started\n"; 
		this->func();
	}
};

template <typename N1, typename N2>
N1& operator<<(N1& self, N2& node){
	self.depends_on.push_back(node.id);
	return self;
}

template <typename N1, typename N2>
N1& operator>>(N1& self, N2& node){
	self.dependents.push_back(node.id);
	return self;
}

template <typename F>
Node<F> make_node(int id,F func){
	std::vector<int>depends_on;
	std::vector<int>dependents;
	return Node<F>(id, depends_on, dependents, func);
}

struct Manager{
	// Vector of all the nodes to run.
	std::map<int, std::shared_ptr<BaseNode>> nodes;

	// Nodes whose dependencies are resolved and can be run
	std::queue<int> to_run;
	std::set<std::reference_wrapper<int>> to_run_set;

	// Contains the id of completed nodes.
	std::vector<int> completed_nodes;
	std::set<int> completed_set;

	// Mutex to make sure the update of to_run and completed are atomic
	std::mutex update_lock;

	template <typename N>
	void add_node(N node){
		this->nodes[node.id] = std::make_shared<N>(node);
	}

	// Method which atomically updates the to_run and completed
	void update(std::vector<int> dependents,int id);

	// returns bool representing if all parents
	// of current indexed node have finished
	bool if_all_parents_fin(int i);

	void execute(int src_node = 0);

};

