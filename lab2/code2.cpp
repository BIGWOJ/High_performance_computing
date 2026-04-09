#include <cstdio>
#include <cmath>
#include <chrono>
#include <iostream>
#include <omp.h>

struct Color
{
    char red;
    char green;
    char blue;
};

// screen ( integer) coordinate
const int iXmax = 2000;
const int iYmax = 2000;

Color pixmap[iYmax][iXmax];

int main()
{
    // world (double) coordinate = parameter plane
    double Cx, Cy;

    const double CxMin = -2.5;
    const double CxMax = 1.5;
    const double CyMin = -2.0;
    const double CyMax = 2.0;

    const double PixelWidth = (CxMax - CxMin) / iXmax;
    const double PixelHeight = (CyMax - CyMin) / iYmax;

    const int IterationMax = 20000;

    // bail-out value , radius of circle ;
    const double EscapeRadius = 2;

    const double ER2 = EscapeRadius * EscapeRadius;

    Color black;
    black.red = 0;
    black.green = 0;
    black.blue = 0;
    Color white;
    white.red = 255;
    white.green = 255;
    white.blue = 255;

    Color red;
    red.red = 255;
    red.green = 0;
    red.blue = 0;
    Color red_shade;
    red_shade.red = 255;
    red_shade.green = 128;
    red_shade.blue = 128;

    Color green;
    green.red = 0;
    green.green = 255;
    green.blue = 0;
    Color green_shade;
    green_shade.red = 128;
    green_shade.green = 255;
    green_shade.blue = 128;
    Color blue;
    blue.red = 0;
    blue.green = 0;
    blue.blue = 255;
    Color blue_shade;
    blue_shade.red = 128;
    blue_shade.green = 128;
    blue_shade.blue = 255;

    // create new file,give it a name and open it in binary mode
    FILE *fp = std::fopen("mandelbrot.ppm", "wb"); // b - binary mode

    // color component ( R or G or B) is coded from 0 to 255
    // it is 24 bit color RGB file
    const int MaxColorComponentValue = 255;

    const char *comment = "# "; // comment should start with #

    // write ASCII header to the file
    std::fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);

    int tests_num = 5;
    int mean_time = 0;
    for (int test_num = 1; test_num <= tests_num; test_num++)
    {
        std::cout << "\n=====" << test_num << "/" << tests_num << "=====" << std::endl;
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

// compute and write image data bytes to the file
#pragma omp parallel for schedule(dynamic, 250) collapse(2)
        for (int iY = 0; iY < iYmax; ++iY)
        {
            for (int iX = 0; iX < iXmax; ++iX)
            {
                double Cy = CyMin + iY * PixelHeight;

                if (fabs(Cy) < PixelHeight / 2)
                    Cy = 0.0; // Main antenna

                const double Cx = CxMin + iX * PixelWidth;
                // initial value of orbit = critical point Z = 0

                // Z = Zx+Zy*i; Z0 = 0
                double Zx = 0.0;
                double Zy = 0.0;
                // Zx2 = Zx*Zx; Zy2 = Zy*Zy
                double Zx2 = Zx * Zx;
                double Zy2 = Zy * Zy;

                int Iteration = 0;
                for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); ++Iteration)
                {
                    // for(volatile int d = 0; d < 100; d++);
                    Zy = 2 * Zx * Zy + Cy;
                    Zx = Zx2 - Zy2 + Cx;
                    Zx2 = Zx * Zx;
                    Zy2 = Zy * Zy;
                }

                int thread_id = omp_get_thread_num();
                pixmap[iY][iX] = (Iteration == IterationMax) ? (thread_id % 3 == 0 ? red : (thread_id % 3 == 1 ? green : blue)) : (thread_id % 3 == 0 ? red_shade : (thread_id % 3 == 1 ? green_shade : blue_shade));

                // pixmap[iY][iX] = (Iteration == IterationMax) ? black : white;

                // // compute pixel color (24 bit = 3 bytes)
                // if (Iteration == IterationMax)
                // {
                //     std::fwrite(&black.red, 1, 1, fp);
                //     std::fwrite(&black.green, 1, 1, fp);
                //     std::fwrite(&black.blue, 1, 1, fp);
                // }
                // else
                // {
                //     std::fwrite(&white.red, 1, 1, fp);
                //     std::fwrite(&white.green, 1, 1, fp);
                //     std::fwrite(&white.blue, 1, 1, fp);
                // }
            }
        }

        for (int iY = 0; iY < iYmax; ++iY)
        {
            for (int iX = 0; iX < iXmax; ++iX)
            {
                std::fwrite(&pixmap[iY][iX].red, 1, 1, fp);
                std::fwrite(&pixmap[iY][iX].green, 1, 1, fp);
                std::fwrite(&pixmap[iY][iX].blue, 1, 1, fp);
            }
        }

        std::fclose(fp);
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
        mean_time += duration.count();
        std::cout << "Time taken: " << duration.count() << " ms" << std::endl;
    }
    std::cout << "\nMean time: " << mean_time / tests_num << " ms" << std::endl;
}
