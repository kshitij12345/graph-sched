#include <algorithm>
#include <gsched/Manager.hpp>

namespace gsched{

bool Manager::if_all_parents_fin(int i){
	// Check if parents of i'th node
	// have executed
	for(const auto& parent : this->nodes[i]->parents_ref){
		auto result = std::find(this->completed.begin(), this->completed.end(), parent.get().id);
		if (result == this->completed.end()){
			return false;
		}
	}

	return true;
}

void Manager::enqueue_root(){
	for (auto const& node : nodes){
		if (node.second->parents_ref.size() == 0){
			to_run.push(node.first);
		}
	}
}

void Manager::enqueue_children(vec_BaseNode_ref children,int id){
	{
		std::lock_guard<std::mutex> Lock(update_lock);
		
		// Notify that this thread has completed.
		this->inflight_threads--;

		//this->completed_vec.push_back(id);
		this->completed.push_back(id);
		
		// Check to see if any children
		// is ready to run. i.e. if all
		// parents of the child have executed.
		for(auto child : children){
			auto child_id = child.get().id;
			if(if_all_parents_fin(child_id)){
				this->to_run.push(child_id);
			}
					
		}

	} // Scope of Lock ends (i.e. Mutex is up for grabs)
}

void Manager::clear_state(){
	// clear all the state
	// variables for a 
	// new execution.
	completed = {};
	inflight_threads = 0;
	threads.resize(0);
	exec_complete = false;
}

void Manager::schedule(){
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
			enqueue_children(nodes[id]->children_ref, id);
			schedule();
		};

		threads.push_back(std::thread(update_func));
	}
	
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
