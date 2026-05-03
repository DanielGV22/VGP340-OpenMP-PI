#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include <atomic>
#include <limits>

struct Point
{
    float x = 0.0f;
    float y = 0.0f;
};

std::atomic_bool done{ false };
std::mutex coutMutex;

int GetQuarter(const Point& p)
{
    if (p.x >= 0.0f && p.y >= 0.0f) return 1;
    if (p.x < 0.0f && p.y >= 0.0f) return 2;
    if (p.x < 0.0f && p.y < 0.0f) return 3;
    return 4;
}

float Distance(const Point& a, const Point& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

struct CircularBuffer
{
    Point* buf;
    int capacity;
    int frontIdx;
    int rearIdx;
    int count;

    std::mutex m;
    std::condition_variable notEmpty;
    std::condition_variable notFull;

    CircularBuffer(int cap)
        : capacity(cap), frontIdx(0), rearIdx(0), count(0)
    {
        buf = new Point[cap];
    }

    ~CircularBuffer()
    {
        delete[] buf;
    }

    void push(const Point& p)
    {
        std::unique_lock<std::mutex> lk(m);

        notFull.wait(lk, [this]()
            {
                return count != capacity;
            });

        buf[rearIdx] = p;
        rearIdx = (rearIdx + 1) % capacity;
        ++count;

        lk.unlock();
        notEmpty.notify_all();
    }

    bool pop(Point& p)
    {
        std::unique_lock<std::mutex> lk(m);

        notEmpty.wait(lk, [this]()
            {
                return count > 0 || done;
            });

        if (count == 0 && done)
        {
            return false;
        }

        p = buf[frontIdx];
        frontIdx = (frontIdx + 1) % capacity;
        --count;

        lk.unlock();
        notFull.notify_one();

        return true;
    }
};

void PrintClosestForQuarter(int quarter, const std::vector<Point>& points)
{
    if (points.size() < 2)
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "quarter " << quarter
            << ": not enough points. Total number of points in this quarter is "
            << points.size() << ".\n";
        return;
    }

    float closestDistance = std::numeric_limits<float>::max();
    Point closestA;
    Point closestB;

    for (size_t i = 0; i < points.size(); ++i)
    {
        for (size_t j = i + 1; j < points.size(); ++j)
        {
            float d = Distance(points[i], points[j]);

            if (d < closestDistance)
            {
                closestDistance = d;
                closestA = points[i];
                closestB = points[j];
            }
        }
    }

    std::lock_guard<std::mutex> lock(coutMutex);

    std::cout << "quarter " << quarter
        << ": closest points are ("
        << closestA.x << ", " << closestA.y << ") and ("
        << closestB.x << ", " << closestB.y << ") and their distance is "
        << closestDistance
        << ". Total number of points in this quarter is "
        << points.size() << ".\n";
}

void consumer(CircularBuffer& buf, int quarter)
{
    std::vector<Point> localPoints;

    Point p;

    while (buf.pop(p))
    {
        if (GetQuarter(p) == quarter)
        {
            localPoints.push_back(p);
        }
    }

    PrintClosestForQuarter(quarter, localPoints);
}

void producer(CircularBuffer& buf)
{
    std::mt19937 generator(
        static_cast<unsigned int>(
            std::chrono::system_clock::now().time_since_epoch().count()
            )
    );

    std::uniform_real_distribution<float> distribution(-1000.0f, 1000.0f);

    for (int i = 0; i < 10000; ++i)
    {
        Point p;
        p.x = distribution(generator);
        p.y = distribution(generator);

        buf.push(p);

        if (i % 100 == 0)
        {
            std::cout << "Generated " << i << " points...\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    done = true;
    buf.notEmpty.notify_all();
}

int main()
{
    CircularBuffer dataBuf(400);

    auto q1 = std::async(std::launch::async, consumer, std::ref(dataBuf), 1);
    auto q2 = std::async(std::launch::async, consumer, std::ref(dataBuf), 2);
    auto q3 = std::async(std::launch::async, consumer, std::ref(dataBuf), 3);
    auto q4 = std::async(std::launch::async, consumer, std::ref(dataBuf), 4);

    producer(dataBuf);

    q1.get();
    q2.get();
    q3.get();
    q4.get();

    std::cout << "Finished generating and processing 10,000 points.\n";

    return 0;
}