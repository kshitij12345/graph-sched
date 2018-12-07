#include "Manager.h"

std::queue<int> To_Run;
std::vector<int> Completed;
std::mutex UpdateLock;
std::set<std::reference_wrapper<int>> To_Run_Set;
std::set<int> Completed_Set;
std::vector<Node> nodes;

bool if_all_parents_fin(int i){
	Node node = nodes[i-1];// Nodes are indexed rather than mapped. Change

	for(int j=0; j < node.depends_on.size(); j++){
		if ((Completed_Set.find(node.depends_on[j])) == Completed_Set.end()){
			// If not present
			return false;
		}
	}

	return true;
}

//Update To_Run and Completed atomically.
void Update(std::vector<int> dependents,int id){
	{
		std::lock_guard<std::mutex> Lock(UpdateLock);

		Completed.push_back(id);
		Completed_Set.insert(id);
		
		for(int i = 0;i<dependents.size();i++){
			// If does not exist in To_Run then Add
			//if (To_Run_Set.find(dependents[i]) == To_Run_Set.end()){
				/* TO-DO: Check if all dependencies have run */
				if(if_all_parents_fin(dependents[i])){
					To_Run.push(dependents[i]);
					To_Run_Set.insert(std::ref(To_Run.back()));
				}
					
			//}
		}

	} // Scope of Lock ends (i.e. Mutex is up for grabs)
}
