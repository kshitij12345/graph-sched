#include "Node.h"

/********** Functions to Run *****************/
void printer1(){
	//_sleep(10000);
	std::cout << "Hello1\n";
	return;
}

void printer2(){
	std::cout << "Hello2\n";
	return;
}

void printer3(){
	std::cout << "Hello3\n";
	return;
}

void printer4(){
	std::cout << "Hello4\n";
	return;
}
/*********************************************/

// utility function
void print_queue(std::queue<int> q)
{
  while (!q.empty())
  {
    std::cout << q.front() << " ";
    q.pop();
  }
  std::cout << std::endl;
  return;
}

int main()
{
	// Vector of all the nodes to run.
	std::vector<Node> nodes;

	// Info of First Node
	std::set<int> depends_on;
	std::set<int> dependents;
	depends_on.insert(0);
	dependents.insert(2);
	dependents.insert(3);
	nodes.push_back(Node(1,depends_on,dependents,printer1));
	
	// Info of Second Node
	std::set<int>dep2;
	std::set<int>depn2;
	dep2.insert(1);
	depn2.insert(4);
	nodes.push_back(Node(2,dep2,depn2,printer2));

	// Info of Third Node
	std::set<int>dep3;
	std::set<int>depn3;
	dep3.insert(1);
	depn3.insert(4);
	nodes.push_back(Node(3,dep3,depn3,printer3));

	// Info of Fourth Node
	std::set<int>dep4;
	std::set<int>depn4;
	dep4.insert(3);
	dep4.insert(2);
	nodes.push_back(Node(4,dep4,depn4,printer4));

	// Get the src node ready to run.
	To_Run.push(1);
	print_queue(To_Run);

	// 1 Thread per Node Mode
	std::vector<std::thread> Threads;
	
	// Run till all nodes are completed.
	while(Completed.size() < nodes.size()){
		{
			std::lock_guard<std::mutex> Lock(UpdateLock);
			if (!To_Run.empty()){
				int id = To_Run.front();
				To_Run.pop();
				Threads.push_back(nodes[id-1].ReturnFunc());
			}

		}
		
	}

	// Join all the threads
	for(long unsigned int i = 0;i<Threads.size();i++){
		Threads[i].join();
	}

	// Should be empty
	print_queue(To_Run);

	std::cout << "Completed \n";
	// Print Nodes in order they completed.
	for(long unsigned int i = 0;i<Completed.size();i++){
		std::cout << Completed[i] << " ";
	}
	
	return 0;
}
