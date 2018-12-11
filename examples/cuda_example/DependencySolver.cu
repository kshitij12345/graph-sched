#include "Manager.h"
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

/********** Functions to Run *****************/
void printer0(){
	std::cout << "Hello from 0\n";
	return;
}

int printer1(){
	std::this_thread::sleep_for(std::chrono::microseconds(500));
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

#define COLUMNS 3
#define ROWS 2

__global__ void add(int *a, int *b, int *c)
{
 int x = blockIdx.x;
 int y = blockIdx.y;
 int i = (COLUMNS*y) + x;
 c[i] = a[i] + b[i];
}

void cuda_func(){

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
	
	for (int y = 0; y < ROWS; y++) // Output Arrays
	{
		for (int x = 0; x < COLUMNS; x++)
		{
			printf("[%d][%d]=%d ",y,x,c[y][x]);
		}
		printf("\n");
	}
}
/*********************************************/

int main()
{
	Manager m;

	auto& node0 = m.append_node(0, printer0);
	auto& node1 = m.append_node(1, printer1);
	auto& node2 = m.append_node(2, printer2);
	auto& node3 = m.append_node(3, printer3);
	auto& node4 = m.append_node(4, cuda_func);

	// Dependeny Declaration
	// Node >> Node_1 implies Node is parent of Node_1
	// Node << Node_1 implies Node is child of Node_1

	node0 >> node1 >> node2;

	node3 << node1 << node2;

	node4 << node3;

	m.execute();

	// Expected Order of Execution
	std::vector<int> expected_order = {0, 2, 1, 3, 4};

	assert(expected_order == m.execution_order() && "Graph didn't execute in expected order!!");
	
	return 0;
}
