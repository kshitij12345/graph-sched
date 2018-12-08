#include "Manager.h"
#include <unistd.h>

/********** Functions to Run *****************/
void printer0(){
	std::cout << "Hello from 0\n";
	return;
}

void printer1(){
	usleep(500);
	std::cout << "Hello from 1\n";
	return;
}

void printer2(){
	std::cout << "Hello from 2\n";
	return;
}

void printer3(){
	std::cout << "Hello from 3\n";
	return;
}
/*********************************************/

int main()
{
	Manager m;
	
	// Info of First Node
	std::vector<int> depends_on;
	std::vector<int> dependents;
	//depends_on.push_back(0);
	dependents.push_back(1);
	dependents.push_back(2);
	m.nodes.push_back(Node(0,depends_on,dependents,printer0));
	
	// Info of Second Node
	std::vector<int>dep2;
	std::vector<int>depn2;
	dep2.push_back(0);
	depn2.push_back(3);
	m.nodes.push_back(Node(1,dep2,depn2,printer1));

	// Info of Third Node
	std::vector<int>dep3;
	std::vector<int>depn3;
	dep3.push_back(0);
	depn3.push_back(3);
	m.nodes.push_back(Node(2,dep3,depn3,printer2));

	// Info of Fourth Node
	std::vector<int>dep4;
	std::vector<int>depn4;
	dep4.push_back(1);
	dep4.push_back(2);
	m.nodes.push_back(Node(3,dep4,depn4,printer3));

	m.execute();
	
	return 0;
}