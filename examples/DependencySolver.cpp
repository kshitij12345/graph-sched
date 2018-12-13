#include <thread>
#include <cassert>
#include <iterator>

#include "Manager.h"
#include "node_dsl.hpp"
/********** Functions to Run *****************/
void printer0(){
	std::cout << "Hello from 0\n";
	return;
}

int printer1(){
	std::this_thread::sleep_for(std::chrono::microseconds(10000));
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

#define printv(v,t) std::copy(v.begin(), v.end(), std::ostream_iterator<t>(std::cout, " "));
 
int main()
{
	Manager m;

	auto& node0 = m.append_node(0, printer0);
	auto& node1 = m.append_node(1, printer1);
	auto& node2 = m.append_node(2, printer2);
	auto& node3 = m.append_node(3, printer3);

	node0 >> (node1, node2) >> node3;
	

#define inspect(n) \
	std::cout << "Parents"#n << '\n';\
	printv(node##n.parents)\
	std::cout << "\nchildren"#n << '\n';\
	printv(node##n.children)

	inspect(0)
	inspect(1)
	inspect(2)
	inspect(3)

	//(node0, node1) >> (node2, node3);

	// node3 << node1 << node2;

	m.execute();

	// Expected Order of Execution
	std::vector<int> expected_order = {0, 2, 1, 3};
	std::cout << "Execution Order" << '\n';
	for(auto& i : m.execution_order()) std::cout << i << ' ';
	std::cout << '\n';
	assert(expected_order == m.execution_order() && "Graph didn't execute in expected order!!");
	
	return 0;
}
