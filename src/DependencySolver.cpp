#include "Manager.h"
#include <unistd.h>

/********** Functions to Run *****************/
void printer0(){
	std::cout << "Hello from 0\n";
	return;
}

int printer1(){
	usleep(500);
	std::cout << "Hello from 1\n";
	return 1;
}

int printer2(){
	std::cout << "Hello from 2\n";
	return 2;
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
	dependents.push_back(1);
	dependents.push_back(2);
	m.add_node(0,depends_on,dependents,printer0);

	// Info of Second Node
	std::vector<int>dep2;
	std::vector<int>depn2;
	dep2.push_back(0);
	depn2.push_back(3);
	m.add_node(1,dep2,depn2,printer1);

	// Info of Third Node
	std::vector<int>dep3;
	std::vector<int>depn3;
	dep3.push_back(0);
	depn3.push_back(3);
	m.add_node(2,dep3,depn3,printer2);

	// Info of Fourth Node
	std::vector<int>dep4;
	std::vector<int>depn4;
	dep4.push_back(1);
	dep4.push_back(2);
	m.add_node(3,dep4,depn4,printer3);

	m.execute();

	// TO-DO : assert Completed order is 0, 2, 1, 3
	
	return 0;
}
