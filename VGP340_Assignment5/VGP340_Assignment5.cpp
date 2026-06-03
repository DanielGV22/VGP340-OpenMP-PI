// VGP340_Assignment5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <limits>
#include <omp.h>
#include <chrono>
#include <random>

double PI_MonteCarlo()
{
    const long long samples = 100000000;
    long long inside = 0;

#pragma omp parallel
    {
        int threadID = omp_get_thread_num();

        std::mt19937 rng(
            (unsigned int)std::chrono::steady_clock::now()
            .time_since_epoch()
            .count() + threadID);

        std::uniform_real_distribution<double> dist(0.0, 1.0);

#pragma omp for reduction(+:inside)
        for (long long i = 0; i < samples; i++)
        {
            double x = dist(rng);
            double y = dist(rng);

            if (x * x + y * y <= 1.0)
            {
                inside++;
            }
        }
    }

    return 4.0 * inside / samples;
}

double PI_Integral()
{
    long num_steps = 100000000;
    double step = 1.0 / (double)num_steps;
    double sum = 0.0;

#pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < num_steps; i++)
    {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    return sum * step;
}

int main()
{
    std::cout.precision(15);

    auto start1 = std::chrono::steady_clock::now();
    double piIntegral = PI_Integral();
    auto end1 = std::chrono::steady_clock::now();

    auto start2 = std::chrono::steady_clock::now();
    double piMonteCarlo = PI_MonteCarlo();
    auto end2 = std::chrono::steady_clock::now();

    auto integralTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end1 - start1).count();

    auto monteTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end2 - start2).count();

    std::cout << "\nIntegral Method\n";
    std::cout << "PI = " << piIntegral << '\n';
    std::cout << "Time = " << integralTime << " ms\n";

    std::cout << "\nMonte Carlo Method\n";
    std::cout << "PI = " << piMonteCarlo << '\n';
    std::cout << "Time = " << monteTime << " ms\n";
}