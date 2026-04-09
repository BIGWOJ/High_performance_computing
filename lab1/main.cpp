// -fopenmp flag required to compile with OpenMP support

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include "matrix_file.h"
using namespace std;

const int I = 800;
const int J = 800;
const int K = 800;
double A[I][K];
double B[K][J];
double C[I][J];
int NUM_THREADS;

void multiply_openmp()
{
#pragma omp parallel for num_threads(NUM_THREADS)
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
            const int i_end = parallelize_i_loop ? end : I;
            const int j_start = parallelize_i_loop ? 0 : start;
            const int j_end = parallelize_i_loop ? J : end;

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

double test_sequential_parallel()
{
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    multiply_sequential();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    double Ts_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Sequential multiplication time (Ts): " << Ts_time << " ms" << endl;

    start = chrono::steady_clock::now();
    multiply_parallel(true);
    end = chrono::steady_clock::now();
    double Tr_std_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Parallel multiplication time, I loop parallelized (Tr): " << Tr_std_time << " ms" << endl;

    // start = chrono::steady_clock::now();
    // multiply_parallel(false);
    // end = chrono::steady_clock::now();
    // cout << "Parallel multiplication time, J loop parallelized (Tr): " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

    return Ts_time, Tr_std_time;
}

double test_openmp()
{
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    multiply_openmp();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    double Tr_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Parallel OpenMP multiplication time: " << Tr_time << " ms" << endl;
    return Tr_time;
}

int main()
{
    cout << "Enter number of threads: ";
    cin >> NUM_THREADS;
    cout << thread::hardware_concurrency() << " concurrent threads are supported." << endl;
    int tests_num = 5;
    double mean_amdahl = 0;
    double amdahl, mean_parallel_time, mean_program_time, mean_std_time;
    cout << "Number of threads: " << NUM_THREADS << endl;

    create_save_matrix("matrixA.txt", I, K);
    create_save_matrix("matrixB.txt", K, J);

    for (int test_num = 1; test_num <= tests_num; test_num++)
    {
        cout << "\n=====" << test_num << "/" << tests_num << "=====" << endl;

        chrono::steady_clock::time_point start_overall = chrono::steady_clock::now();

        chrono::steady_clock::time_point start = chrono::steady_clock::now();
        load_matrix_from_file("matrixA.txt", A);
        load_matrix_from_file("matrixB.txt", B);
        double Ts_time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
        cout << "Sequential matrix loading time (Ts): " << Ts_time << " ms" << endl;
        double Tr_std_time;
        Ts_time, Tr_std_time = test_sequential_parallel();
        double Tr_time = test_openmp();

        double T_time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_overall).count();
        cout << "Overall time (T): " << T_time << " ms" << endl;
        amdahl = 1.0 / (Ts_time / T_time + (1 - Ts_time / T_time) / NUM_THREADS);
        mean_amdahl += amdahl;
        mean_parallel_time += Tr_time;
        mean_program_time += T_time;
        mean_std_time += Tr_std_time;

        cout << "Potential speedup for whole program (Amdahl's law): " << amdahl << endl;
        cout << "Multiplication speedup - OpenMP parallelization vs sequential: " << Ts_time / Tr_time << endl;
    }
    cout << "Mean potential speedup using " << NUM_THREADS << " threads (Amdahl's law): " << (double)mean_amdahl / tests_num << endl;
    cout << "Mean parallel time: " << (double)mean_parallel_time / tests_num << " ms" << endl;
    cout << "Mean program time: " << (double)mean_program_time / tests_num << " ms" << endl;
    cout << "Mean std::thread multiplication time: " << (double)mean_std_time / tests_num << " ms" << endl;
    return 0;
}
