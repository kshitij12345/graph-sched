#pragma once
#include <thread>
#include <queue>
#include <vector>
#include <iostream>
#include <mutex>
#include <set>
#include <memory>
#include <map>
#include <tuple>
#include <condition_variable>
#include "Node.hpp"

namespace gsched{

struct Manager {
	// Map to hold of all the nodes.
	std::map<int, std::unique_ptr<BaseNode>> nodes;

	// Nodes whose parents have completed and can be run.
	std::queue<int> to_run;

	// Contains the id of completed nodes.
	std::set<int> completed;

	// To keep track of completion order.
	std::vector<int> completed_vec;

	// Reachable nodes from current src of
	// execution
	std::set<int> reachable_nodes;

	std::set<int> unmet_deps;
	
	// Limit maximum inflight threads
	int inflight_threads;
	int max_threads;

	// Mutex to make sure the update of to_run 
	// and completed_nodes are `atomic`
	std::mutex update_lock;

	std::condition_variable has_completed;
	bool exec_complete = false;

	std::vector<std::thread> threads;

	template <typename F, typename... Args> // F must have operator()(...)
	BaseNode& append_node(int id, F&& func){
		this->nodes[id] = std::unique_ptr<BaseNode>(new Node<F, Args...>(id, func));
		return *(this->nodes[id]);
	}

	template<typename R, typename... Args>
	BaseNode& append_node(int id, R(*func)(Args...)) {
		this->nodes[id] = std::unique_ptr<BaseNode>(new Node<R(Args...)>(id, func));
		return *(this->nodes[id]);
	}


	// Method which `atomically` updates the to_run and completed
	void enqueue_children(std::set<int> children,int id);

	// Returns bool representing if all parents
	// of current indexed node have finished
	bool if_all_parents_fin(int i);

	// Method to explore all reachable nodes from
	// the mentioned `src` node
	void explore_reachable_nodes(int src);

	// Check to see if current reachable nodes
	// have all dependencies satisfied
	void check_dependencies();

	// Start executing runnable threads
	void schedule();

	// Updates to_run with all nodes
	// with zero parents.
	void enqueue_root();

	// Execute all graphs
	void execute(int max_thread = std::thread::hardware_concurrency());
	
	// Reset all state variables for a
	// new execution.
	void clear_state();

	// Returns the order in which nodes
	// executed.
	// Note : Returns empty vector if
	// 		  none of the nodes have
	// 		  executed
	std::vector<int> execution_order(){
		return completed_vec;
	}

};

}// namespace ends