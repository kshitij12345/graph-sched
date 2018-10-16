#include "Manager.h"

std::queue<int> To_Run;
std::vector<int> Completed;
std::mutex UpdateLock;
std::set<std::reference_wrapper<int>> To_Run_Set;
std::set<std::reference_wrapper<int>> Completed_Set;

//Update To_Run and Completed atomically.
void Update(std::vector<int> dependents,int id){
	{
		std::lock_guard<std::mutex> Lock(UpdateLock);

		for(int i = 0;i<dependents.size();i++){
			if (To_Run_Set.find(dependents[i]) == To_Run_Set.end()){
				/* TO-DO: Check if all dependencies have run */
				// check_if_all_run()
				To_Run.push(dependents[i]);
				To_Run_Set.insert(std::ref(To_Run.back()));	
			}else{
				std::cout << dependents[i] << " Exists\n When Updating " << id << "\n";
			}
		}

		Completed.push_back(id);
		Completed_Set.insert(std::ref(Completed.back()));
		
	}

}