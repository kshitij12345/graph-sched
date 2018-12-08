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
	std::vector<int> parents;
	std::vector<int> children;
	
	virtual ~BaseNode(){}
	virtual void call() const = 0;
};

template <typename F>
struct Node : BaseNode{
	// Derived from Base, it holds the actual
	// function. It can hold function of any
	// signature.
	F func;

	Node(int id,std::vector<int>parents,std::vector<int>children,F func){
		this->id = id;
		this->parents = parents;
		this->children = children;
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
	self.parents.push_back(node.id);
	return self;
}

template <typename Node1, typename Node2>
Node1& operator>>(Node1& self, Node2& node){
	self.children.push_back(node.id);
	return self;
}

// Wrapper like std::make_* to avoid
// the need to write template arguments
// i.e. weirds function signatures.
template <typename F>
Node<F> make_node(int id,F func){
	std::vector<int>parents;
	std::vector<int>children;
	return Node<F>(id, parents, children, func);
}

/***********************************************/

struct Manager{
	// Map to hold of all the nodes.
	std::map<int, std::shared_ptr<BaseNode>> nodes;

	// Nodes whose parents have completed and can be run
	std::queue<int> to_run;
	std::set<std::reference_wrapper<int>> to_run_set;

	// Contains the id of completed nodes.
	std::vector<int> completed_nodes;
	std::set<int> completed_set;

	// Mutex to make sure the update of to_run 
	// and completed_nodes are `atomic`
	std::mutex update_lock;

	template <typename N>
	void add_node(N node){
		this->nodes[node.id] = std::make_shared<N>(node);
	}

	// Method which `atomically` updates the to_run and completed
	void update(std::vector<int> children,int id);

	// Returns bool representing if all parents
	// of current indexed node have finished
	bool if_all_parents_fin(int i);

	void execute(int src_node = 0);

};

