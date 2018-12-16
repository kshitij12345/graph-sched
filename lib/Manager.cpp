#include "Manager.h"

bool Manager::if_all_parents_fin(int i){
	// Check if parents of i'th node
	// have executed
	for(auto parent : this->nodes[i]->parents){
		if ((this->completed.find(parent)) == this->completed.end()){
			// If not present
			return false;
		}
	}

	return true;
}

void Manager::update(std::set<int> children,int id){
	{
		std::lock_guard<std::mutex> Lock(update_lock);
		
		this->inflight_threads--;

		this->completed_vec.push_back(id);
		this->completed.insert(id);
		
		// Check to see if any children
		// is ready to run. i.e. if all
		// parents of the child have executed.
		for(auto child : children){
			if(if_all_parents_fin(child)){
				this->to_run.push(child);
			}
					
		}

	} // Scope of Lock ends (i.e. Mutex is up for grabs)
}

void Manager::explore_reachable_nodes(int src){
	this->reachable_nodes.insert(src);
	auto& node = *(this->nodes[src]);
	// explore children
	for(auto& child: node.children){
		Manager::explore_reachable_nodes(child);
	}
}

void Manager::check_dependencies(){
	for(auto& node_id : this->reachable_nodes){
		auto& node = *(this->nodes[node_id]);
		// check if all parents exist in reachable
		for (auto& parent: node.parents){
			if(reachable_nodes.find(parent) == reachable_nodes.end()){
				this->unmet_deps.insert(node_id);
			}
		}
	}

	// Check if any reachable node
	// has unmet dependencies.
	if(this->unmet_deps.size()){
		std::string error_msg = "";
		for(auto& node: unmet_deps){
			error_msg += "Error: Node " + std::to_string(node) + " has unmet dependencies.\n"; 
		}

		error_msg += "Fix these unmet deps\n";
		throw(error_msg);
	}
}

void Manager::clear_state(){
	// clear all the state
	// variables for a 
	// new execution.
	reachable_nodes = {};
	completed = {};
	unmet_deps = {};
	completed_vec = {};
	inflight_threads = 0;
	threads.resize(0);
	exec_complete = false;
}

void Manager::schedule(){
	{
		std::lock_guard<std::mutex> Lock(update_lock);

		// Start all runnable nodes.
		while(!to_run.empty() && inflight_threads < max_threads){
			int id = to_run.front();
			to_run.pop();

			inflight_threads++;

			// Make sure update is called
			// at the end of execution
			auto update_func = [this, id] {
				(*nodes[id])();
				update(nodes[id]->children, id);
				schedule();
			};

			threads.push_back(std::thread(update_func));
		}

	} //Scope of lock ends.

	// Notify if all reachable nodes have completed.
	if (completed.size() == reachable_nodes.size()){
		this->exec_complete = true;
		has_completed.notify_one();
	}
}

void Manager::execute(int src_node_idx, int max_threads){
	this->max_threads = max_threads;
	clear_state();
	
	// Get the src node ready to run.
	to_run.push(src_node_idx);

	explore_reachable_nodes(src_node_idx);
	check_dependencies();

	// Schedule first node
	Manager::schedule();

	// wait until all reachable threads have executed.
	std::unique_lock<std::mutex> lock(update_lock);
	has_completed.wait(lock, [this]{return this->exec_complete;});
	
	for(auto& thread : threads){
		thread.join();
	}
	
}
