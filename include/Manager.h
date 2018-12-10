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
	// This is the base struct for Nodes.
	// It allows us to have derived nodes
	// for different function signatures
	// to exist in a single container.
	int id;
	std::set<int> parents;
	std::set<int> children;
	
	virtual ~BaseNode(){}
	virtual void call() const = 0;
};

template <typename F>
struct Node : BaseNode{
	// Derived from Base, it holds the actual
	// function. It can hold function of any
	// signature.
	F func;

	Node(int id,F func){
		this->id = id;
		this->func = func;
	}

	void call() const{
		std::cout << "Func Id :" <<this->id << " has started\n"; 
		this->func();
	}
};


/********** UTIL FUNCTIONS *******************/
// Below are helper functions
// to make it simpler to create
// the dependency graph.
template <typename Node1, typename Node2>
Node1& operator<<(Node1& self, Node2& node){
	self->parents.insert(node->id);
	node->children.insert(self->id);
	return self;
}

template <typename Node1, typename Node2>
Node1& operator>>(Node1& self, Node2& node){
	self->children.insert(node->id);
	node->parents.insert(self->id);
	return self;
}

/***********************************************/

struct Manager{
	// Map to hold of all the nodes.
	std::map<int, std::shared_ptr<BaseNode>> nodes;

	// Nodes whose parents have completed and can be run.
	std::queue<int> to_run;

	// Contains the id of completed nodes.
	std::set<int> completed;

	// To keep track of completion order.
	std::vector<int> completed_vec;

	// Mutex to make sure the update of to_run 
	// and completed_nodes are `atomic`
	std::mutex update_lock;

	template <typename F>
	std::shared_ptr<BaseNode> append_node(int id, F func){
		this->nodes[id] = std::make_shared<Node<F>>(Node<F>(id, func));
		return this->nodes[id];
	}

	// Method which `atomically` updates the to_run and completed
	void update(std::set<int> children,int id);

	// Returns bool representing if all parents
	// of current indexed node have finished
	bool if_all_parents_fin(int i);

	void execute(int src_node = 0);

	// Returns the order in which nodes
	// executed.
	// Note : Returns empty vector if
	// 		  none of the nodes have
	// 		  executed
	std::vector<int> execution_order(){
		return completed_vec;
	}

};

