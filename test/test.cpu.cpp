#include <thread>
#include <cassert>
#include <iterator>

#include <gsched/Manager.hpp>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

using namespace gsched;

TEST_CASE( "Node DSL constructs graph correctly.", "[node-dsl]" ) {

	auto dummy_func = [](){};

	Manager m;
	auto& node0 = m.append_node(0, dummy_func);
	auto& node1 = m.append_node(1, dummy_func);
	auto& node2 = m.append_node(2, dummy_func);
	auto& node3 = m.append_node(3, dummy_func);
	auto& node4 = m.append_node(4, dummy_func);
	auto& node5 = m.append_node(5, dummy_func);

	// Use the Node DSL to construct a graph.
	node0 >> (node1, node2) >> (node3, node4, node5);

	// Expected parent/child relationship of the nodes.
	std::set<int> expected_node0_parents {};
	std::set<int> expected_node0_children {1, 2};
	std::set<int> expected_node1_parents {0};
	std::set<int> expected_node1_children {3, 4, 5};
	std::set<int> expected_node2_parents {0};
	std::set<int> expected_node2_children {3, 4, 5};
	std::set<int> expected_node3_parents {1, 2};
	std::set<int> expected_node3_children {};
	std::set<int> expected_node4_parents {1, 2};
	std::set<int> expected_node4_children {};
	std::set<int> expected_node5_parents {1, 2};
	std::set<int> expected_node5_children {};

	REQUIRE(node0.parents == expected_node0_parents);
	REQUIRE(node1.parents == expected_node1_parents);
	REQUIRE(node2.parents == expected_node2_parents);
	REQUIRE(node3.parents == expected_node3_parents);
	REQUIRE(node4.parents == expected_node4_parents);
	REQUIRE(node5.parents == expected_node5_parents);

	REQUIRE(node0.children == expected_node0_children);
	REQUIRE(node1.children == expected_node1_children);
	REQUIRE(node2.children == expected_node2_children);
	REQUIRE(node3.children == expected_node3_children);
	REQUIRE(node4.children == expected_node4_children);
	REQUIRE(node5.children == expected_node5_children);
	
}

TEST_CASE( "Graph execution order is correct.", "[manager]" ) {
	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	
	node0 >> (node1, node2);
	m.execute();

	std::vector<int> expected_order_0 = {0, 2, 1};
	if (std::thread::hardware_concurrency() == 1){
		expected_order_0 = {0, 1, 2};
	}

	REQUIRE(m.execution_order() == expected_order_0);
	
	auto& node3 = m.append_node(3, fun0);
	(node1, node2) >> node3;
	m.execute();
	
	std::vector<int> expected_order_1 = {0, 2, 1, 3};
	if (std::thread::hardware_concurrency() == 1){
		expected_order_1 = {0, 1, 2, 3};
	}
	
	REQUIRE(m.execution_order() == expected_order_1);
}

TEST_CASE( "Reachable nodes.", "[manager]" ) {
	auto func = []() {};

	Manager m;

	auto& node0 = m.append_node(0, func);
	auto& node1 = m.append_node(1, func);
	auto& node2 = m.append_node(2, func);
	auto& node3 = m.append_node(3, func);

	node0 >> (node1, node2);
	m.explore_reachable_nodes(0);
	std::set<int> expected_nodes_from_0 = {0, 1, 2};
	REQUIRE(m.reachable_nodes == expected_nodes_from_0);
	m.clear_state();

	m.explore_reachable_nodes(1);
	std::set<int> expected_nodes_from_1 = {1};
	REQUIRE(m.reachable_nodes == expected_nodes_from_1);
	m.clear_state();

	node2 >> node3;
	m.explore_reachable_nodes(2);
	std::set<int> expected_nodes_from_2 = {2, 3};
	REQUIRE(m.reachable_nodes == expected_nodes_from_2);
	m.clear_state();	
}

TEST_CASE( "Unmet Dependencies.", "[manager]" ) {
	auto func = []() {};

	Manager m;

	auto& node0 = m.append_node(0, func);
	auto& node1 = m.append_node(1, func);
	auto& node2 = m.append_node(2, func);
	auto& node3 = m.append_node(3, func);

	node0 >> (node1, node2) >> node3;

	auto check_deps = [&](int src_node) {
		m.clear_state();
		m.explore_reachable_nodes(src_node);
		m.check_dependencies();
	};

	REQUIRE_NOTHROW(check_deps(0));
	REQUIRE_THROWS(check_deps(1));
	REQUIRE_THROWS(check_deps(2));
	REQUIRE_THROWS(check_deps(3));
}

TEST_CASE( "Max Thread.", "[manager]" ) {
	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	auto& node3 = m.append_node(3, fun0);

	node0 >> (node1, node2) >> node3;
	m.execute(1);

	std::vector<int> expected_order = {0, 1, 2, 3};
	REQUIRE(m.execution_order() == expected_order);

	m.execute();
	std::vector<int> expected_order_mul_thread = {0, 2, 1, 3};
	if (std::thread::hardware_concurrency() == 1){
		expected_order_mul_thread = {0, 1, 2, 3};
	}
	
	REQUIRE(m.execution_order() == expected_order_mul_thread);
}

TEST_CASE( "Multiple Graphs", "[manager]") {
	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	auto& node3 = m.append_node(3, fun0);
	auto& node4 = m.append_node(4, fun1);
	auto& node5 = m.append_node(5, fun0);
	auto& node6 = m.append_node(6, fun1);
	auto& node7 = m.append_node(7, fun1);
	auto& node8 = m.append_node(8, fun0);
	auto& node9 = m.append_node(9, fun1);
	auto& node10 = m.append_node(10, fun0);

	node0 >> (node1, node2);

	node3 >> node4;

	// lone ranger
	node5;

	(node6, node7) >> node8 >> (node9, node10);

	std::set<int> cmpl_nodes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	SECTION("Default Threads") {
		m.execute();
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	}

	SECTION("1 Thread"){
		m.execute(1);
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	}

	SECTION("2 Thread"){
		m.execute(2);
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	}
	
	SECTION("3 Thread"){
		m.execute(3);
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	}

	SECTION("4 Thread"){
		m.execute(4);
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	}
	
	SECTION("5 Thread"){
		m.execute(5);
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	}
}
