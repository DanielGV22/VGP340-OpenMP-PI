#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace std;

const int NUM_PHILOSOPHERS = 5;
const int RUN_TIME_SECONDS = 60;

mutex forks[NUM_PHILOSOPHERS];
mutex coutMutex;

class Philosopher
{
public:
    int id;
    int leftFork;
    int rightFork;
    int eatCount = 0;

    Philosopher(int i)
    {
        id = i;
        leftFork = i;
        rightFork = (i + 1) % NUM_PHILOSOPHERS;
    }

    void eat()
    {
        auto endTime = chrono::steady_clock::now() + chrono::seconds(RUN_TIME_SECONDS);

        while (chrono::steady_clock::now() < endTime)
        {
            think();

            int firstFork = min(leftFork, rightFork);
            int secondFork = max(leftFork, rightFork);

            {
                lock_guard<mutex> lock1(forks[firstFork]);
                lock_guard<mutex> lock2(forks[secondFork]);

                eatCount++;

                {
                    lock_guard<mutex> coutLock(coutMutex);
                    cout << "Philosopher " << id
                        << " is eating using forks "
                        << leftFork << " and " << rightFork
                        << " | Eat count: " << eatCount << endl;
                }

                this_thread::sleep_for(chrono::seconds(1));
            }

            this_thread::sleep_for(chrono::seconds(4));
        }
    }

    void think()
    {
        lock_guard<mutex> coutLock(coutMutex);
        cout << "Philosopher " << id << " is thinking" << endl;
    }
};

int main()
{
    vector<thread> threads;
    vector<Philosopher> philosophers;

    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        philosophers.emplace_back(i);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        threads.emplace_back(&Philosopher::eat, &philosophers[i]);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    cout << "\nFinal Eating Results:\n";

    for (const Philosopher& philosopher : philosophers)
    {
        cout << "Philosopher " << philosopher.id
            << " ate " << philosopher.eatCount
            << " times." << endl;
    }

    cout << "\nSimulation finished." << endl;

    return 0;
}