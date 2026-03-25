#include <iostream>
#include <cstdlib>
#include <fstream>
#include <random>
using namespace std;

void create_save_matrix(const string &file_name, int rows, int cols)
{
    ofstream out(file_name, ios::out | ios::trunc);
    if (!out.is_open())
    {
        cerr << "Nie mozna otworzyc pliku: " << file_name << endl;
        return;
    }

    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 9);

    out << rows << " " << cols << "\n";
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            out << dist(gen);
            if (j + 1 < cols)
            {
                out << " ";
            }
        }
        out << "\n";
    }
}

template <size_t ROWS, size_t COLS>
bool load_matrix_from_file(const string &file_name, double (&matrix)[ROWS][COLS])
{
    ifstream in(file_name);

    size_t rows_in_file = 0;
    size_t cols_in_file = 0;
    in >> rows_in_file >> cols_in_file;

    for (size_t i = 0; i < ROWS; i++)
    {
        for (size_t j = 0; j < COLS; j++)
        {
            in >> matrix[i][j];
            if (!in)
            {
                cerr << "Blad odczytu danych macierzy z pliku: " << file_name << endl;
                return false;
            }
        }
    }

    return true;
}
