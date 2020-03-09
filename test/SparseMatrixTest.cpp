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
        mat.setEntry(i, j, v);
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

TEST(DOKSparseMatrix, setEntry) {
    int N = 3;
    int M = 5;

    DOKSparseMatrix A{N, M};
    A.setEntry(0, 0, 2.0);
    A.setEntry(0, 4, 3.0);
    A.setEntry(2, 2, 4.0);

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
    A.setEntry(0, 0, 2.0);
    A.setEntry(1, 1, 2.0);
    A.setEntry(2, 2, 2.0);

    CSRSparseMatrix B{A};
    ASSERT_EQ(B.nRows(), 3);
    ASSERT_EQ(B.nCols(), 3);

    ASSERT_EQ(B.getEntry(0, 0), 2.0);
    ASSERT_EQ(B.getEntry(1, 1), 2.0);
    ASSERT_EQ(B.getEntry(2, 2), 2.0);
}

TEST(CSRSparseMatrix, multiply) {
    DOKSparseMatrix A{3, 3};
    A.setEntry(0, 0, 2.0);
    A.setEntry(1, 1, 2.0);
    A.setEntry(2, 2, 2.0);

    CSRSparseMatrix B{A};
    Vector v {1, 2, 3};
    Vector w = B * v;

    ASSERT_EQ(w, (Vector{2.0, 4.0, 6.0}));
}

TEST(CSRSparseMatrix, transposed) {
    int N = 4;
    int M = 4;

    DOKSparseMatrix A = RandomDOKSparseMatrixFromSeed(M, N, 0.2, 42);
    CSRSparseMatrix B(A);
    CSRSparseMatrix B_T = B.transposed();

    for (int i = 0; i < A.nRows(); i++) {
        for (int j = 0; j < A.nCols(); j++) {
            ASSERT_EQ(B.getEntry(i, j), B_T.getEntry(j, i));
        }
    }
}

TEST(CSRSparseMatrix, isSymmetric) {

    DOKSparseMatrix B{3, 3};
    B.setEntry(0, 0, 4.0);
    B.setEntry(0, 1, 12.0);
    B.setEntry(0, 2, -16.0);
    B.setEntry(1, 1, 37.0);
    B.setEntry(1, 2, -43.0);
    B.setEntry(2, 2, 98.0);

    ASSERT_TRUE(!CSRSparseMatrix(B).isSymmetric());

}

TEST(CSRSparseMatrix, isLowerTriangular) {
    
    DOKSparseMatrix A{3, 3};
    A.setEntry(0, 0, 4.0);
    A.setEntry(0, 1, 12.0);
    A.setEntry(0, 2, -16.0);
    A.setEntry(1, 0, 12.0);
    A.setEntry(1, 1, 37.0);
    A.setEntry(1, 2, -43.0);
    A.setEntry(2, 0, -16.0);
    A.setEntry(2, 1, -43.0);
    A.setEntry(2, 2, 98.0);

    ASSERT_TRUE(!CSRSparseMatrix(A).isLowerTriangular());

    DOKSparseMatrix B{3, 3};
    B.setEntry(0, 0, 2.0);
    B.setEntry(1, 0, 6.0);
    B.setEntry(1, 1, 1.0);
    B.setEntry(2, 0, -8.0);
    B.setEntry(2, 1, 5.0);
    B.setEntry(2, 2, 3.0);

    ASSERT_TRUE(CSRSparseMatrix(B).isLowerTriangular());
    ASSERT_TRUE(!CSRSparseMatrix(B).isUpperTriangular());
}

TEST(CSRSparseMatrix, isUpperTriangular) {
   
    DOKSparseMatrix A{3, 3};
    A.setEntry(0, 0, 4.0);
    A.setEntry(0, 1, 12.0);
    A.setEntry(0, 2, -16.0);
    A.setEntry(1, 0, 12.0);
    A.setEntry(1, 1, 37.0);
    A.setEntry(1, 2, -43.0);
    A.setEntry(2, 0, -16.0);
    A.setEntry(2, 1, -43.0);
    A.setEntry(2, 2, 98.0);

    ASSERT_TRUE(!CSRSparseMatrix(A).isUpperTriangular());

    DOKSparseMatrix B{3, 3};
    B.setEntry(0, 0, -8.0);
    B.setEntry(0, 1, 5.0);
    B.setEntry(0, 2, 3.0);
    B.setEntry(1, 1, 6.0);
    B.setEntry(1, 2, 1.0);
    B.setEntry(2, 2, 2.0);

    ASSERT_TRUE(!CSRSparseMatrix(B).isLowerTriangular());
    ASSERT_TRUE(CSRSparseMatrix(B).isUpperTriangular());

}

TEST(CSRSparseMatrix, Cholesky) {

    DOKSparseMatrix A{3, 3};
    A.setEntry(0, 0, 4.0);
    A.setEntry(0, 1, 12.0);
    A.setEntry(0, 2, -16.0);
    A.setEntry(1, 0, 12.0);
    A.setEntry(1, 1, 37.0);
    A.setEntry(1, 2, -43.0);
    A.setEntry(2, 0, -16.0);
    A.setEntry(2, 1, -43.0);
    A.setEntry(2, 2, 98.0);

    const CSRSparseMatrix L = CSRSparseMatrix(A).cholesky();
    ASSERT_TRUE(L.isLowerTriangular());

    ASSERT_EQ(CSRSparseMatrix(A), L.AbstractMatrix<CSRSparseMatrix>::operator*(L.transposed()));

    DOKSparseMatrix B{3, 3};
    B.setEntry(0, 0, 4.0);
    B.setEntry(0, 1, 12.0);
    B.setEntry(0, 2, -16.0);
    B.setEntry(1, 1, 37.0);
    B.setEntry(1, 2, -43.0);
    B.setEntry(2, 2, 98.0);

    ASSERT_ANY_THROW(CSRSparseMatrix(B).cholesky());

    // non-semi-postive-definite matrix

    DOKSparseMatrix C{3, 3};
    C.setEntry(0, 0, -4.0);
    C.setEntry(1, 1, 37.0);
    C.setEntry(2, 2, 98.0);

    ASSERT_ANY_THROW(CSRSparseMatrix(C).cholesky());

}
