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

void Manager::clear_state(){
	this->reachable_nodes = {};
	this->completed = {};
}

void Manager::execute(int src_node_idx){
	// Get the src node ready to run.
	to_run.push(src_node_idx);

	// reset completion order for this execution
	this->completed_vec = {};

	this->explore_reachable_nodes(src_node_idx);

	// 1 Thread per Node Mode
	std::vector<std::thread> threads;
	
	// Run till all nodes are completed.
	while(completed.size() < reachable_nodes.size()){
		{
			std::lock_guard<std::mutex> Lock(update_lock);

			// Start all runnable nodes.
			if (!to_run.empty()){
				int id = to_run.front();
				to_run.pop();

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

	// for next execution
	Manager::clear_state();
}
