#include "gtest/gtest.h"

#include <SparseMatrix.h>

using namespace zap;


DOKSparseMatrix RandomDOKSparseMatrixFromSeed(int M, int N, double sparsity, int seed) {
    std::srand(seed);

    DOKSparseMatrix mat{M, N};
    for (int e = 0; e < (int)(sparsity * M * N); e++) {
        int i = std::rand() % M;
        int j = std::rand() % N;
        double v =  (double) std::rand() / RAND_MAX - 0.5;
        mat.addEntry(i, j, v);
    }
    return mat;
}

TEST(DOKSparseMatrix, Constructor) {
    int N = 3;
    int M = 5;

    DOKSparseMatrix A{N, M};

    ASSERT_EQ(A.nRows(), N);
    ASSERT_EQ(A.nCols(), M);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            ASSERT_EQ(A.getEntry(i, j), 0);
        }
    }
}

TEST(DOKSparseMatrix, addEntry) {
    int N = 3;
    int M = 5;

    DOKSparseMatrix A{N, M};
    A.addEntry(0, 0, 2.0);
    A.addEntry(0, 4, 3.0);
    A.addEntry(2, 2, 4.0);

    ASSERT_EQ(A.getEntry(0, 0), 2.0);
    ASSERT_EQ(A.getEntry(0, 4), 3.0);
    ASSERT_EQ(A.getEntry(2, 2), 4.0);
}

TEST(DOKSparseMatrix, transposed) {
    int N = 3;
    int M = 5;

    DOKSparseMatrix A = RandomDOKSparseMatrixFromSeed(M, N, 0.2, 42);
    DOKSparseMatrix A_T = A.transposed();

    for (int i = 0; i < A.nRows(); i++) {
        for (int j = 0; j < A.nCols(); j++) {
            ASSERT_EQ(A.getEntry(i, j), A_T.getEntry(j, i));
        }
    }
}

TEST(CSRSparseMatrix, CSRSparseMatrix) {
    DOKSparseMatrix A{3, 3};
    A.addEntry(0, 0, 2.0);
    A.addEntry(1, 1, 2.0);
    A.addEntry(2, 2, 2.0);

    CSRSparseMatrix B{A};
    ASSERT_EQ(B.nRows(), 3);
    ASSERT_EQ(B.nCols(), 3);

    ASSERT_EQ(B.getEntry(0, 0), 2.0);
    ASSERT_EQ(B.getEntry(1, 1), 2.0);
    ASSERT_EQ(B.getEntry(2, 2), 2.0);
}

TEST(CSRSparseMatrix, multiply) {
    DOKSparseMatrix A{3, 3};
    A.addEntry(0, 0, 2.0);
    A.addEntry(1, 1, 2.0);
    A.addEntry(2, 2, 2.0);

    CSRSparseMatrix B{A};
    Vector v {1, 2, 3};
    Vector w = B * v;

    ASSERT_EQ(w, (Vector{2.0, 4.0, 6.0}));
}

TEST(CSRSparseMatrix, transposed) {
    int N = 3;
    int M = 5;

    DOKSparseMatrix A = RandomDOKSparseMatrixFromSeed(M, N, 0.2, 42);
    CSRSparseMatrix B(A);
    CSRSparseMatrix B_T = B.transposed();

    for (int i = 0; i < A.nRows(); i++) {
        for (int j = 0; j < A.nCols(); j++) {
            ASSERT_EQ(B.getEntry(i, j), B_T.getEntry(j, i));
        }
    }
}