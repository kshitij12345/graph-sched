#include "Manager.h"

bool Manager::if_all_parents_fin(int i){
	std::vector<int> parents = nodes[i]->parents;

	for(int j=0; j < parents.size(); j++){
		if ((completed_set.find(parents[j])) == completed_set.end()){
			// If not present
			return false;
		}
	}

	return true;
}

void Manager::update(std::vector<int> children,int id){
	{
		std::lock_guard<std::mutex> Lock(update_lock);

		completed_nodes.push_back(id);
		completed_set.insert(id);
		
		// Check to see if any children
		// is ready to run. i.e. if all
		// parents of the child have executed.
		for(int i = 0;i<children.size();i++){
			if(if_all_parents_fin(children[i])){
				to_run.push(children[i]);
				to_run_set.insert(std::ref(to_run.back()));
			}
					
		}

	} // Scope of Lock ends (i.e. Mutex is up for grabs)
}

void Manager::execute(int src_node_idx){
	// Get the src node ready to run.
	to_run.push(src_node_idx);

	// 1 Thread per Node Mode
	std::vector<std::thread> threads;
	
	// Run till all nodes are completed.
	while(completed_nodes.size() < nodes.size()){
		{
			std::lock_guard<std::mutex> Lock(update_lock);

			// Start all runnable nodes.
			if (!to_run.empty()){
				int id = to_run.front();
				to_run.pop();

				// Make sure update is called
				// at the end of execution
				auto update_func = [=]{nodes[id]->call(); this->update(nodes[id]->children, id);};

				threads.push_back(std::thread(update_func));
			}

		} //Scope of lock ends.
		
	}

	for(long unsigned int i = 0;i<threads.size();i++){
		threads[i].join();
	}

	std::cout << "Execution Completed \n";

	// Print node id's in order of execution.
	for(long unsigned int i = 0;i<completed_nodes.size();i++){
		std::cout << completed_nodes[i] << " ";
	}
	std::cout << "\n";
}
