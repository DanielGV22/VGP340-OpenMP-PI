#include "Matrix.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <algorithm>
#include <omp.h>

Matrix::Matrix()
{
}

Matrix::Matrix(int r, int c) : nRows(r), nCols(c)
{
    data = new long long* [nRows];
    for (int i = 0; i < nRows; ++i)
    {
        data[i] = new long long[nCols];
        for (int j = 0; j < nCols; ++j)
        {
            data[i][j] = 0;
        }
    }
}

Matrix::~Matrix()
{
    if (data == nullptr)
        return;

    for (int i = 0; i < nRows; ++i)
    {
        delete[] data[i];
    }
    delete[] data;
}

void Matrix::init()
{
    for (int i = 0; i < nRows; ++i)
    {
        for (int j = 0; j < nCols; ++j)
        {
            data[i][j] = std::rand() % 20 - 10;
        }
    }
}

void Matrix::print()
{
    std::cout << "Matrix:\n";
    for (int i = 0; i < nRows; ++i)
    {
        for (int j = 0; j < nCols; ++j)
        {
            std::cout << data[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Matrix::printPartial(int maxRows, int maxCols) const
{
    int rows = std::min(maxRows, nRows);
    int cols = std::min(maxCols, nCols);

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            std::cout << data[i][j] << "\t";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Matrix::Mult(const Matrix& A, const Matrix& B, Matrix* result)
{
    assert(A.nCols == B.nRows && "Matrices num of Cols and Rows do not match!");

    if (A.nCols != B.nRows)
    {
        std::cout << "A and B dimensions do not match! "
            << "A.nCols=" << A.nCols
            << ", B.nRows=" << B.nRows << std::endl;
        return;
    }

    for (int i = 0; i < A.nRows; ++i)
    {
        for (int j = 0; j < B.nCols; ++j)
        {
            long long res = 0;
            for (int k = 0; k < A.nCols; ++k)
            {
                res += A.data[i][k] * B.data[k][j];
            }
            result->data[i][j] = res;
        }
    }
}

void Matrix::MultParallel(const Matrix& A, const Matrix& B, Matrix* result)
{
    assert(A.nCols == B.nRows && "Matrices num of Cols and Rows do not match!");

    if (A.nCols != B.nRows)
    {
        std::cout << "A and B dimensions do not match! "
            << "A.nCols=" << A.nCols
            << ", B.nRows=" << B.nRows << std::endl;
        return;
    }

#pragma omp parallel for schedule(static)
    for (int i = 0; i < A.nRows; ++i)
    {
        for (int j = 0; j < B.nCols; ++j)
        {
            long long res = 0;
            for (int k = 0; k < A.nCols; ++k)
            {
                res += A.data[i][k] * B.data[k][j];
            }
            result->data[i][j] = res;
        }
    }
}

bool CompareMatrices(const Matrix& A, const Matrix& B)
{
    if (A.nRows != B.nRows || A.nCols != B.nCols)
        return false;

    for (int i = 0; i < A.nRows; ++i)
    {
        for (int j = 0; j < A.nCols; ++j)
        {
            if (A.data[i][j] != B.data[i][j])
            {
                return false;
            }
        }
    }

    return true;
}