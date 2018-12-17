#include <cassert>
#include <stdio.h>

#include "Manager.h"
#include "node_dsl.hpp"
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/sequence.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/replace.h>
#include <thrust/functional.h>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

int cuda_func()
{
  // allocate three device_vectors with 10 elements
  thrust::device_vector<int> X(10);
  thrust::device_vector<int> Y(10);
  thrust::device_vector<int> Z(10);

  // initialize X to 0,1,2,3, ....
  thrust::sequence(X.begin(), X.end());

  // compute Y = -X
  thrust::transform(X.begin(), X.end(), Y.begin(), thrust::negate<int>());

  // fill Z with twos
  thrust::fill(Z.begin(), Z.end(), 2);

  // compute Y = X mod 2
  thrust::transform(X.begin(), X.end(), Z.begin(), Y.begin(), thrust::modulus<int>());

  // replace all the ones in Y with tens
  thrust::replace(Y.begin(), Y.end(), 1, 10);
 
  return 0; 
}

TEST_CASE( "CUDA Graph execution order is correct.", "[CUDA-manager]" ) {
	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	auto& node3 = m.append_node(3, fun0);
	auto& node4 = m.append_node(4, cuda_func);

	node0 >> (node1, node2) >> node3 >> node4;
	m.execute(0);

	std::vector<int> expected_order = {0, 2, 1, 3, 4};
	if (std::thread::hardware_concurrency() == 1){
		expected_order = {0, 1, 2, 3, 4};
	}
	REQUIRE(m.execution_order() == expected_order);
}
