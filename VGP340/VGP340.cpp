// VGP340.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <omp.h>
#include <iostream>

int main()
{
	int testValue = 0;
	omp_set_num_threads(4);
    #pragma omp parallel
	{
		int id = omp_get_thread_num();
		++testValue;
		printf("Hello(% d)", id, testValue);
		printf("World(%d)\n", id, testValue);
	}
}

#define NUM_THREADS 4
static long numSteps = 100000;
double step = 0.0;

void Excersise2()
{
	int numThreads = 0;
	double x = 0.0;
	double pi = 0.0;
	double sum[NUM_THREADS] = { 0.0 }; // causes false sharing
	step = 1.0 / (double)numSteps;
	omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
	{
		int i = 0;
		int id = omp_get_thread_num();
		int nThreads = omp_get_num_threads();
		if (id == 0)
		{
			numThreads = nThreads;
		}
		for (i = id; i < numSteps; i += nThreads)
		{
			x = (i + 0.5) * step;
			sum[id] += (4.0 / (1.0 + x * x));
		}
	}
	for (int i = 0; i < numThreads; ++i)
	{
		std::cout << "ID: " << i << " Sum:" << sum[i] << "\n";
		pi += step * sum[i];
	}

	std::cout << "PI: " << pi << "\n";

}

int main()
{
	double x = 0.0;
	double pi = 0.0;
	double sum = 0.0;
	step = 1.0 / (double)numSteps;
	omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel for private (x) reduction(+:sum)
	{
		for (int i = 0; i < numSteps; ++i)
		{
			x = (i + 0.5) * step;
			sum += (4.0 / (1.0 + x * x));
		}
			
	}
	pi = sum * step;
	std::cout << "PI: " << pi << "\n";

	return 0;
}