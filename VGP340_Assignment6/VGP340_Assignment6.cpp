#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

const int NUM_EVAL_RUNS = 2;
const int N = 10000000;
const unsigned int L = 5000;

void merge(int* array, unsigned int left, unsigned int mid, unsigned int right);

unsigned int GetMaxDepth()
{
    unsigned int threads = thread::hardware_concurrency();
    unsigned int depth = 0;

    while (threads > 1)
    {
        threads /= 2;
        depth++;
    }

    return depth;
}

void sequential_merge_sort(int* array, unsigned int left, unsigned int right)
{
    if (left >= right)
        return;

    if ((right - left + 1) <= L)
    {
        sort(array + left, array + right + 1);
        return;
    }

    unsigned int mid = (left + right) / 2;

    sequential_merge_sort(array, left, mid);
    sequential_merge_sort(array, mid + 1, right);

    merge(array, left, mid, right);
}

void parallel_merge_sort_helper(int* array, unsigned int left, unsigned int right, unsigned int depth)
{
    if (left >= right)
        return;

    if ((right - left + 1) <= L)
    {
        sort(array + left, array + right + 1);
        return;
    }

    unsigned int mid = (left + right) / 2;

    if (depth > 0)
    {
        thread leftThread(parallel_merge_sort_helper, array, left, mid, depth - 1);

        parallel_merge_sort_helper(array, mid + 1, right, depth - 1);

        leftThread.join();
    }
    else
    {
        sequential_merge_sort(array, left, mid);
        sequential_merge_sort(array, mid + 1, right);
    }

    merge(array, left, mid, right);
}

void parallel_merge_sort(int* array, unsigned int left, unsigned int right)
{
    parallel_merge_sort_helper(array, left, right, GetMaxDepth());
}

void merge(int* arr, unsigned int left, unsigned int mid, unsigned int right)
{
    unsigned int num_left = mid - left + 1;
    unsigned int num_right = right - mid;

    auto array_left = make_shared<int[]>(num_left);
    auto array_right = make_shared<int[]>(num_right);

    copy(&arr[left], &arr[mid + 1], array_left.get());
    copy(&arr[mid + 1], &arr[right + 1], array_right.get());

    unsigned int index_left = 0;
    unsigned int index_right = 0;
    unsigned int index_insert = left;

    while ((index_left < num_left) || (index_right < num_right))
    {
        if ((index_left < num_left) && (index_right < num_right))
        {
            if (array_left[index_left] <= array_right[index_right])
            {
                arr[index_insert] = array_left[index_left];
                index_left++;
            }
            else
            {
                arr[index_insert] = array_right[index_right];
                index_right++;
            }
        }
        else if (index_left < num_left)
        {
            arr[index_insert] = array_left[index_left];
            index_left++;
        }
        else if (index_right < num_right)
        {
            arr[index_insert] = array_right[index_right];
            index_right++;
        }

        index_insert++;
    }
}

int partition_array(int* array, int left, int right)
{
    int pivot = array[(left + right) / 2];

    while (left <= right)
    {
        while (array[left] < pivot)
            left++;

        while (array[right] > pivot)
            right--;

        if (left <= right)
        {
            swap(array[left], array[right]);
            left++;
            right--;
        }
    }

    return left;
}

void sequential_quick_sort(int* array, int left, int right)
{
    if (left >= right)
        return;

    if ((right - left + 1) <= (int)L)
    {
        sort(array + left, array + right + 1);
        return;
    }

    int index = partition_array(array, left, right);

    sequential_quick_sort(array, left, index - 1);
    sequential_quick_sort(array, index, right);
}

void parallel_quick_sort_helper(int* array, int left, int right, unsigned int depth)
{
    if (left >= right)
        return;

    if ((right - left + 1) <= (int)L)
    {
        sort(array + left, array + right + 1);
        return;
    }

    int index = partition_array(array, left, right);

    if (depth > 0)
    {
        thread leftThread(parallel_quick_sort_helper, array, left, index - 1, depth - 1);

        parallel_quick_sort_helper(array, index, right, depth - 1);

        leftThread.join();
    }
    else
    {
        sequential_quick_sort(array, left, index - 1);
        sequential_quick_sort(array, index, right);
    }
}

void parallel_quick_sort(int* array, int left, int right)
{
    parallel_quick_sort_helper(array, left, right, GetMaxDepth());
}

bool verify_same(const vector<int>& a, const vector<int>& b)
{
    return a == b;
}

int main()
{
    mt19937 generator((unsigned int)chrono::system_clock::now().time_since_epoch().count());

    vector<int> original_array;
    vector<int> sequential_merge_result;
    vector<int> parallel_merge_result;
    vector<int> sequential_quick_result;
    vector<int> parallel_quick_result;

    original_array.reserve(N);

    for (int i = 0; i < N; i++)
    {
        original_array.push_back(generator() % 2000000 - 1000000);
    }

    cout << "Array size N = " << N << endl;
    cout << "Cutoff L = " << L << endl;
    cout << "Hardware threads = " << thread::hardware_concurrency() << endl << endl;

    chrono::duration<double> sequential_merge_time(0);
    chrono::duration<double> parallel_merge_time(0);
    chrono::duration<double> sequential_quick_time(0);
    chrono::duration<double> parallel_quick_time(0);

    cout << "Evaluating Sequential Merge Sort...\n";

    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        sequential_merge_result = original_array;

        auto start_time = chrono::high_resolution_clock::now();
        sequential_merge_sort(&sequential_merge_result[0], 0, N - 1);
        sequential_merge_time += chrono::high_resolution_clock::now() - start_time;
    }

    sequential_merge_time /= NUM_EVAL_RUNS;

    cout << "Evaluating Parallel Merge Sort...\n";

    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        parallel_merge_result = original_array;

        auto start_time = chrono::high_resolution_clock::now();
        parallel_merge_sort(&parallel_merge_result[0], 0, N - 1);
        parallel_merge_time += chrono::high_resolution_clock::now() - start_time;
    }

    parallel_merge_time /= NUM_EVAL_RUNS;

    cout << "Evaluating Sequential Quick Sort...\n";

    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        sequential_quick_result = original_array;

        auto start_time = chrono::high_resolution_clock::now();
        sequential_quick_sort(&sequential_quick_result[0], 0, N - 1);
        sequential_quick_time += chrono::high_resolution_clock::now() - start_time;
    }

    sequential_quick_time /= NUM_EVAL_RUNS;

    cout << "Evaluating Parallel Quick Sort...\n";

    for (int i = 0; i < NUM_EVAL_RUNS; i++)
    {
        parallel_quick_result = original_array;

        auto start_time = chrono::high_resolution_clock::now();
        parallel_quick_sort(&parallel_quick_result[0], 0, N - 1);
        parallel_quick_time += chrono::high_resolution_clock::now() - start_time;
    }

    parallel_quick_time /= NUM_EVAL_RUNS;

    cout << endl;

    cout << "Verification Results:\n";
    cout << "Parallel Merge Sort Correct: "
        << (verify_same(sequential_merge_result, parallel_merge_result) ? "YES" : "NO") << endl;

    cout << "Sequential Quick Sort Correct: "
        << (verify_same(sequential_merge_result, sequential_quick_result) ? "YES" : "NO") << endl;

    cout << "Parallel Quick Sort Correct: "
        << (verify_same(sequential_merge_result, parallel_quick_result) ? "YES" : "NO") << endl;

    cout << endl;

    cout << "Average Sequential Merge Sort Time: "
        << sequential_merge_time.count() * 1000 << " ms\n";

    cout << "Average Parallel Merge Sort Time: "
        << parallel_merge_time.count() * 1000 << " ms\n";

    cout << "Merge Sort Speedup: "
        << sequential_merge_time / parallel_merge_time << endl;

    cout << "Merge Sort Efficiency: "
        << 100.0 * (sequential_merge_time / parallel_merge_time) / thread::hardware_concurrency()
        << "%\n\n";

    cout << "Average Sequential Quick Sort Time: "
        << sequential_quick_time.count() * 1000 << " ms\n";

    cout << "Average Parallel Quick Sort Time: "
        << parallel_quick_time.count() * 1000 << " ms\n";

    cout << "Quick Sort Speedup: "
        << sequential_quick_time / parallel_quick_time << endl;

    cout << "Quick Sort Efficiency: "
        << 100.0 * (sequential_quick_time / parallel_quick_time) / thread::hardware_concurrency()
        << "%\n";

    return 0;
}