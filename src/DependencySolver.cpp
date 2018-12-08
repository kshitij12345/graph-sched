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

	auto node0 = make_node(0, printer0);
	auto node1 = make_node(1, printer1);
	auto node2 = make_node(2, printer2);
	auto node3 = make_node(3, printer3);

	// dependencies	
	node0>>node1>>node2;

	node1<<node0;
	node1>>node3;

	node2<<node0;
	node2>>node3;

	node3<<node2<<node1;

	m.add_node(node0);
	m.add_node(node1);
	m.add_node(node2);
	m.add_node(node3);

	m.execute();

	// TO-DO : assert Completed order is 0, 2, 1, 3
	
	return 0;
}
