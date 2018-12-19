#include <gsched/Manager.hpp>

namespace gsched{

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

void Manager::enqueue_root(){
	for (auto const& node : nodes){
		if (node.second->parents.size() == 0){
			to_run.push(node.first);
		}
	}
}

void Manager::enqueue_children(std::set<int> children,int id){
	{
		std::lock_guard<std::mutex> Lock(update_lock);
		
		// Notify that this thread has completed.
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

		// Start all runnable nodes given that
		// we have task to run and are below
		// max_threads limit.
		while(!to_run.empty() && inflight_threads < max_threads){
			int id = to_run.front();
			to_run.pop();

			inflight_threads++;

			// Make sure update is called
			// at the end of execution
			auto update_func = [this, id] {
				(*nodes[id])();
				enqueue_children(nodes[id]->children, id);
				schedule();
			};

			threads.push_back(std::thread(update_func));
		}

	} //Scope of lock ends.

	// Notify main thread if all reachable nodes have completed.
	if ( to_run.empty() and inflight_threads == 0){
		this->exec_complete = true;
	 	has_completed.notify_one();
	}
}

void Manager::execute(int max_threads){
	this->max_threads = std::max(max_threads, 1);
	clear_state();

	// Get the src node ready to run.
	enqueue_root();

	// Schedule first node
	Manager::schedule();

	// Wait until all reachable threads have executed.
	std::unique_lock<std::mutex> lock(update_lock);
	has_completed.wait(lock, [this]{return this->exec_complete;});
	
	for(auto& thread : threads){
		thread.join();
	}
	
}

}//namespace ends
