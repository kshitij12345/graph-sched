#include "Manager.h"

bool Manager::if_all_parents_fin(int i){
	// Check if parents of i'th node
	// have executed
	for(auto parent : this->nodes[i]->parents){
		if ((completed.find(parent)) == completed.end()){
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

		completed_vec.push_back(id);
		completed.insert(id);
		
		// Check to see if any children
		// is ready to run. i.e. if all
		// parents of the child have executed.
		for(auto child : children){
			if(if_all_parents_fin(child)){
				to_run.push(child);
			}
					
		}

		// return before scope of lock
		// to ensure thread ends before
		// lock is up for grabs
		return;
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
}

void Manager::clear_state(){
	// clear all the state
	// variables for a 
	// new execution.
	this->reachable_nodes = {};
	this->completed = {};
	this->unmet_deps = {};
	this->completed_vec = {};
	this->inflight_threads = 0;
}

void Manager::execute(int src_node_idx, int max_thread){

	clear_state();
	
	// Get the src node ready to run.
	to_run.push(src_node_idx);

	explore_reachable_nodes(src_node_idx);
	check_dependencies();

	if(unmet_deps.size()){
		std::string error_msg = "";
		for(auto& node: unmet_deps){
			error_msg += "Error: Node " + std::to_string(node) + " has unmet dependencies.\n"; 
		}

		error_msg += "Fix these unmet deps\n";
		throw(error_msg);
	}

	std::vector<std::thread> threads;
	
	// Run till all nodes are completed.
	while(completed.size() < reachable_nodes.size()){
		{
			std::lock_guard<std::mutex> Lock(update_lock);

			// Start all runnable nodes.
			// given that we have thread to execute.
			if (!to_run.empty() && (this->inflight_threads < max_thread)){
				int id = to_run.front();
				to_run.pop();

				this->inflight_threads++;

				// Make sure update is called
				// at the end of execution
				auto update_func = [=] {
					(*nodes[id])();
					this->update(nodes[id]->children, id);
				};

				threads.push_back(std::thread(update_func));
			}

		} //Scope of lock ends.
		
	}

	for(auto& thread : threads){
		thread.join();
	}

}
