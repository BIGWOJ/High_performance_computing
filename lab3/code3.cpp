#include <cstdio>
#include <cmath>
#include <chrono>
#include <iostream>
#include <omp.h>
using namespace std;

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

int main()
{
    chrono::steady_clock::time_point start = chrono::steady_clock::now();


}