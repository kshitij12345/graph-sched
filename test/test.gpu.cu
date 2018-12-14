#include <cassert>
#include <stdio.h>

#include "Manager.h"
#include "node_dsl.hpp"


#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>


#define COLUMNS 3
#define ROWS 2

__global__ void add(int *a, int *b, int *c)
{
	int x = blockIdx.x;
	int y = blockIdx.y;
	int i = (COLUMNS*y) + x;
	c[i] = a[i] + b[i];
}

void cuda_func() {
	int a[ROWS][COLUMNS], b[ROWS][COLUMNS], c[ROWS][COLUMNS];
	int *dev_a, *dev_b, *dev_c;
	
	cudaMalloc((void **) &dev_a, ROWS*COLUMNS*sizeof(int));
	cudaMalloc((void **) &dev_b, ROWS*COLUMNS*sizeof(int));
	cudaMalloc((void **) &dev_c, ROWS*COLUMNS*sizeof(int));
	
	for (int y = 0; y < ROWS; y++){// Fill Arrays
		for (int x = 0; x < COLUMNS; x++)
		{
			a[y][x] = x;
			b[y][x] = y;
		}
	}

	cudaMemcpy(dev_a, a, ROWS*COLUMNS*sizeof(int),
	cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, b, ROWS*COLUMNS*sizeof(int),
	cudaMemcpyHostToDevice);
	
	dim3 grid(COLUMNS,ROWS);
	add<<<grid,1>>>(dev_a, dev_b, dev_c);
	
	cudaMemcpy(c, dev_c, ROWS*COLUMNS*sizeof(int),
	cudaMemcpyDeviceToHost);
}

TEST_CASE( "CUDA Graph execution order is correct.", "[CUDA-manager]" ) {
	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(500)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	auto& node3 = m.append_node(3, fun0);
	auto& node4 = m.append_node(4, cuda_func);

	node0 >> (node1, node2) >> node3 >> node4;
	m.execute();

	std::vector<int> expected_order = {0, 2, 1, 3, 4};
	REQUIRE(m.execution_order() == expected_order);
}
