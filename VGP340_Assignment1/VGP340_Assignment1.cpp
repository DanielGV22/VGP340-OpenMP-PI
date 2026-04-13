// VGP340_Assignment1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <omp.h>
#include <chrono>
#include <iostream>

double SerialPI_Integration(int n)
{
    double step = 1.0 / (double)n;
    double sum = 0.0;

    for (int i = 0; i < n; ++i)
    {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    return sum * step;
}

double ParallelPI_Integration(int n)
{
    double step = 1.0 / (double)n;
    double sum = 0.0;

#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; ++i)
    {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    return sum * step;
}

void TestSerial(int n)
{
    auto start = std::chrono::steady_clock::now();
    double pi = SerialPI_Integration(n);
    auto end = std::chrono::steady_clock::now();

    std::cout << "PI_serial_" << n << " = " << pi << "\n";
    std::cout << "Elapsed time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n\n";
}

void TestParallel(int n)
{
    auto start = std::chrono::steady_clock::now();
    double pi = ParallelPI_Integration(n);
    auto end = std::chrono::steady_clock::now();

    std::cout << "PI_parallel_" << n << " = " << pi << "\n";
    std::cout << "Elapsed time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n\n";
}

void CompareResults(int n)
{
    double piSerial = SerialPI_Integration(n);
    double piParallel = ParallelPI_Integration(n);

    std::cout << "Comparison for n = " << n << "\n";
    std::cout << "Serial PI   : " << piSerial << "\n";
    std::cout << "Parallel PI : " << piParallel << "\n";
    std::cout << "Difference  : " << (piSerial - piParallel) << "\n";
    std::cout << "-----------------------------\n\n";
}

int main()
{
    omp_set_num_threads(4);

    TestSerial(10000);
    TestSerial(100000);
    TestSerial(1000000);

    TestParallel(10000);
    TestParallel(100000);
    TestParallel(1000000);

    CompareResults(10000);
    CompareResults(100000);
    CompareResults(1000000);

    return 0;
}