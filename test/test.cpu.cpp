#include <thread>
#include <cassert>
#include <iterator>

#include <gsched/Manager.hpp>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

using namespace gsched;

TEST_CASE( "Node DSL constructs graph correctly.", "[node]" ) {

	std::cout << "Node DSL constructs graph correctly.\n";

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

	auto compare = [](vec_BaseNode_ref BaseNode::*ptr ,const BaseNode& node, std::vector<int> expected){
		std::vector<int> vec;
		for (const auto& parent : node.*ptr){
			vec.push_back(parent.get().id);
		}

		REQUIRE(vec == expected);
	};

	SECTION("Parents"){
		compare(&BaseNode::parents_ref, node0, {});
		compare(&BaseNode::parents_ref, node1, {0});
		compare(&BaseNode::parents_ref, node2, {0});
		compare(&BaseNode::parents_ref, node3, {1, 2});
		compare(&BaseNode::parents_ref, node4, {1, 2});
		compare(&BaseNode::parents_ref, node5, {1, 2});
	}

	SECTION("Children"){
		compare(&BaseNode::children_ref, node0, {1, 2});
		compare(&BaseNode::children_ref, node1, {3, 4, 5});
		compare(&BaseNode::children_ref, node2, {3, 4, 5});
		compare(&BaseNode::children_ref, node3, {});
		compare(&BaseNode::children_ref, node4, {});
		compare(&BaseNode::children_ref, node5, {});
	}
}

TEST_CASE( "Graph execution order is correct.", "[manager]" ) {
	std::cout << "Graph execution order is correct.\n";

	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);

	auto assert_order = [&](std::vector<int> expected_order){
		m.execute(2);
		REQUIRE(m.execution_order() == expected_order);
	};
	
	SECTION("First Graph"){
		node0 >> (node1, node2);
		assert_order({0, 2, 1});
	}
	
	SECTION("Second Graph"){
		// define node3 before FIRST GRAPH
		// will lead to multi-graph
		auto& node3 = m.append_node(3, fun0);
		node0 >> (node1, node2) >> node3;
		assert_order({0, 2, 1, 3});
	}
}

TEST_CASE( "Max Thread.", "[manager]" ) {
	std::cout << "Max Thread.\n";

	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	auto& node3 = m.append_node(3, fun0);

	node0 >> (node1, node2) >> node3;

	auto assert_order = [&](int threads, std::vector<int> expected_order){
		m.execute(threads);
		REQUIRE(m.execution_order() == expected_order);
	};
	
	SECTION("Single Thread"){
		assert_order(1, {0, 1, 2, 3});
	}

	SECTION("Multi-thread"){
		assert_order(2, {0, 2, 1, 3});
	}
}

TEST_CASE( "Multiple Graphs", "[manager]") {
	std::cout << "Multiple Graphs.\n";

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

	// Define multiple graphs
	node0 >> (node1, node2);

	node3 >> node4;

	// lone ranger
	node5;

	(node6, node7) >> node8 >> (node9, node10);

	std::set<int> cmpl_nodes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto assert_func = [&](int threads){
		m.execute(threads);
		auto exec_order = m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		REQUIRE(cmpl_nodes == executed_nodes);
	};

	// Each section checks if all the nodes
	// were executed or not.
	SECTION("Default Threads"){
		int threads = std::thread::hardware_concurrency();
		assert_func(threads);
	}

	
	SECTION("1 Thread"){
		assert_func(1);
	}

	SECTION("2 Thread"){
		assert_func(2);
	}
	
	SECTION("3 Thread"){
		assert_func(3);
	}

	SECTION("4 Thread"){
		assert_func(4);
	}
	
	SECTION("5 Thread"){
		assert_func(5);
	}
}

TEST_CASE( "For Loop", "[manager]") {
	std::cout << "For Loop.\n";

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

	// Define multiple graphs
	node0 >> (node1, node2);

	node3 >> node4;

	// lone ranger
	node5;

	(node6, node7) >> node8 >> (node9, node10);

	std::set<int> cmpl_nodes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto assert_func = [&](int threads){
		for (int i = 0; i < 5; i++){
			m.execute(threads);
			auto exec_order = m.execution_order();
			std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
			REQUIRE(cmpl_nodes == executed_nodes);
		}
	};

	// Each section checks if all the nodes
	// were executed or not.
	SECTION("Default Threads"){
		int threads = std::thread::hardware_concurrency();
		assert_func(threads);
	}

	
	SECTION("1 Thread"){
		assert_func(1);
	}

	SECTION("2 Thread"){
		assert_func(2);
	}
	
	SECTION("3 Thread"){
		assert_func(3);
	}

	SECTION("4 Thread"){
		assert_func(4);
	}
	
	SECTION("5 Thread"){
		assert_func(5);
	}
}

TEST_CASE( "Nested Manager", "[manager]") {
	std::cout << "Nested Manager.\n";

	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };
	auto nested_mang = []() {
		Manager nested_m;
		auto fun0 = []() {};
		auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };
		auto& node0 = nested_m.append_node(0, fun0);
		auto& node1 = nested_m.append_node(1, fun1);
		auto& node2 = nested_m.append_node(2, fun1);
		node0 >> node1, node2;
		nested_m.execute();
		auto exec_order = nested_m.execution_order();
		std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
		std::set<int> cmpl_nodes = {0, 1, 2};
		// Do not use Catch2 in a multi-threaded context.
		//REQUIRE(executed_nodes == cmpl_nodes);
		assert(executed_nodes == cmpl_nodes);
	};

	Manager m;

	auto& node0 = m.append_node(0, nested_mang);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, nested_mang);
	auto& node3 = m.append_node(3, nested_mang);
	auto& node4 = m.append_node(4, fun1);

	node0 >> node1 >> (node2, node3) >> node4;
	m.execute();
	auto exec_order = m.execution_order();
	std::set<int> executed_nodes(exec_order.begin(), exec_order.end());
	std::set<int> cmpl_nodes = {0, 1, 2, 3, 4};
	REQUIRE(executed_nodes == cmpl_nodes);
}
