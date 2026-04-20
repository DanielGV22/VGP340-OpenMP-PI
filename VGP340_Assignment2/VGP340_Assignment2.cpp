#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <omp.h>

#include "Matrix.h"

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int rowsA, colsA, colsB;
    int numThreads;

    std::cout << "Enter rows for Matrix A: ";
    std::cin >> rowsA;

    std::cout << "Enter columns for Matrix A / rows for Matrix B: ";
    std::cin >> colsA;

    std::cout << "Enter columns for Matrix B: ";
    std::cin >> colsB;

    std::cout << "Enter number of threads: ";
    std::cin >> numThreads;

    omp_set_num_threads(numThreads);

    Matrix A(rowsA, colsA);
    Matrix B(colsA, colsB);
    Matrix serialResult(rowsA, colsB);
    Matrix parallelResult(rowsA, colsB);

    A.init();
    B.init();

    auto serialStart = std::chrono::high_resolution_clock::now();
    Matrix::Mult(A, B, &serialResult);
    auto serialEnd = std::chrono::high_resolution_clock::now();

    auto parallelStart = std::chrono::high_resolution_clock::now();
    Matrix::MultParallel(A, B, &parallelResult);
    auto parallelEnd = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> serialDuration = serialEnd - serialStart;
    std::chrono::duration<double> parallelDuration = parallelEnd - parallelStart;

    std::cout << "\nFirst 5x5 of Serial Result:\n";
    serialResult.printPartial();

    std::cout << "First 5x5 of Parallel Result:\n";
    parallelResult.printPartial();

    bool same = CompareMatrices(serialResult, parallelResult);
    std::cout << "Comparison Result: " << (same ? "MATCH" : "DO NOT MATCH") << "\n\n";

    std::cout << "Serial Time: " << serialDuration.count() << " seconds\n";
    std::cout << "Parallel Time: " << parallelDuration.count() << " seconds\n";

    if (parallelDuration.count() > 0.0)
    {
        std::cout << "Speedup: " << serialDuration.count() / parallelDuration.count() << "\n";
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}