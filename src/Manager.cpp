#include "Manager.h"

// utility function
// void print_queue(std::queue<int> q)
// {
//   while (!q.empty())
//   {
//     std::cout << q.front() << " ";
//     q.pop();
//   }
//   std::cout << std::endl;
//   return;
// }

bool Manager::if_all_parents_fin(int i){
	std::vector<int> depends_on = (nodes[i])->depends_on;// Nodes are indexed rather than mapped. Change

	for(int j=0; j < depends_on.size(); j++){
		if ((completed_set.find(depends_on[j])) == completed_set.end()){
			// If not present
			return false;
		}
	}

	return true;
}

//Update To_Run and Completed atomically.
void Manager::update(std::vector<int> dependents,int id){
	{
		std::lock_guard<std::mutex> Lock(update_lock);

		completed_nodes.push_back(id);
		completed_set.insert(id);
		
		for(int i = 0;i<dependents.size();i++){
			if(if_all_parents_fin(dependents[i])){
				to_run.push(dependents[i]);
				to_run_set.insert(std::ref(to_run.back()));
			}
					
		}

	} // Scope of Lock ends (i.e. Mutex is up for grabs)
}

void Manager::execute(int src_node_idx){
	// Get the src node ready to run.
	to_run.push(src_node_idx);

	// 1 Thread per Node Mode
	std::vector<std::thread> Threads;
	
	// Run till all nodes are completed.
	while(completed_nodes.size() < nodes.size()){
		{
			std::lock_guard<std::mutex> Lock(update_lock);
			if (!to_run.empty()){
				// start all runnable nodes.
				int id = to_run.front();
				to_run.pop();
				auto update_func = [=]{nodes[id]->call(); this->update(nodes[id]->dependents, id);};
				Threads.push_back(std::thread(update_func));
			}

		}
		
	}

	// Join all the threads
	for(long unsigned int i = 0;i<Threads.size();i++){
		Threads[i].join();
	}

	// Should be empty
	// print_queue(to_run);

	std::cout << "Execution Completed \n";

	// Print Nodes in order of execution.
	for(long unsigned int i = 0;i<completed_nodes.size();i++){
		std::cout << completed_nodes[i] << " ";
	}
	std::cout << "\n";
}
