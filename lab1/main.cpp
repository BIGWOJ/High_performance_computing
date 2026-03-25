#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include "matrix_file.h"
using namespace std;

const int I = 512;
const int J = 512;
const int K = 512;
int NUM_THREADS;
double A[I][K];
double B[K][J];
double C[I][J];

void multiply_openmp()
{
#pragma omp parallel for
    for (int i = 0; i < I; i++)
    {
        for (int j = 0; j < J; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < K; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiply_sequential()
{
    for (int i = 0; i < I; i++)
    {
        for (int j = 0; j < J; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < K; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiply_parallel(bool parallelize_i_loop = true)
{
    vector<thread> threads;
    threads.reserve(NUM_THREADS);

    const int size = parallelize_i_loop ? I : J;
    const int thread_chunk = (size + NUM_THREADS - 1) / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; t++)
    {
        const int start = t * thread_chunk;
        const int end = min(start + thread_chunk, size);
        if (start >= end)
        {
            break;
        }
        threads.emplace_back([start, end, parallelize_i_loop]()
                             {
            const int i_start = parallelize_i_loop ? start : 0;
            const int i_end   = parallelize_i_loop ? end   : I;
            const int j_start = parallelize_i_loop ? 0     : start;
            const int j_end   = parallelize_i_loop ? J     : end;

            for (int i = i_start; i < i_end; i++)
            {
                for (int j = j_start; j < j_end; j++)
                {
                    C[i][j] = 0;
                    for (int k = 0; k < K; k++)
                    {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            } });
    }
    for (thread &t : threads)
    {
        t.join();
    }
}

void test_sequential_parallel(chrono::steady_clock::time_point start_overall, double Ts_time)
{
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    multiply_sequential();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Sequential multiplication time (Ts): " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

    start = chrono::steady_clock::now();
    multiply_parallel(true);
    end = chrono::steady_clock::now();
    cout << "Parallel multiplication time, I loop parallelized (Tr): " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

    double T_time = chrono::duration_cast<chrono::milliseconds>(end - start_overall).count();
    cout << "Overall time (T): " << T_time << " ms" << endl;

    // start = chrono::steady_clock::now();
    // multiply_parallel(false);
    // end = chrono::steady_clock::now();
    // cout << "Parallel multiplication time, J loop parallelized (Tr): " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    cout << "Potential speedup for " << NUM_THREADS << " threads (Amdahl's law): " << 1.0 / (Ts_time / T_time + (1 - Ts_time / T_time) / NUM_THREADS) << endl;
}

void test_openmp()
{
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    multiply_openmp();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "OpenMP parallel multiplication time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

int main()
{

    // g++ main.cpp -fopenmp -o main uruchamiać

    cout << "Enter number of threads: ";
    cin >> NUM_THREADS;
    // cout << thread::hardware_concurrency() << " concurrent threads are supported." << endl;

    for (int test_num = 1; test_num <= 10; test_num++)
    {
        cout << test_num << "/10" << endl;

        create_save_matrix("matrixA.txt", I, K);
        create_save_matrix("matrixB.txt", K, J);

        chrono::steady_clock::time_point start_overall = chrono::steady_clock::now();

        chrono::steady_clock::time_point start = chrono::steady_clock::now();
        load_matrix_from_file("matrixA.txt", A);
        load_matrix_from_file("matrixB.txt", B);
        chrono::steady_clock::time_point end = chrono::steady_clock::now();
        double Ts_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        // cout << "Matrix sequential loading time (Ts): " << Ts_time << " ms" << endl;

        test_sequential_parallel(start_overall, Ts_time);
        test_openmp();
    }
    return 0;
}
