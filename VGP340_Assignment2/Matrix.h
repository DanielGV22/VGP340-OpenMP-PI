#pragma once

struct Matrix
{
    int nRows = 0;
    int nCols = 0;
    long long** data = nullptr;

    Matrix();
    Matrix(int r, int c);
    ~Matrix();

    void init();
    void print();
    void printPartial(int maxRows = 5, int maxCols = 5) const;

    static void Mult(const Matrix& A, const Matrix& B, Matrix* result);
    static void MultParallel(const Matrix& A, const Matrix& B, Matrix* result);
};

bool CompareMatrices(const Matrix& A, const Matrix& B);