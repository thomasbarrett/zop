#include "gtest/gtest.h"

#include <DenseMatrix.h>
#include <random>

using namespace zop;

DenseMatrix RandomMatrixFromSeed(int M, int N, int seed) {
    std::srand(seed);

    DenseMatrix mat{M, N};
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            mat.setEntry(i, j, (double) std::rand() / RAND_MAX - 0.5);
        }
    }
    return mat;
}

TEST(DenseMatrixTest, Constructor) {
    int N = 3;
    int M = 5;

    DenseMatrix A{N, M};

    ASSERT_EQ(A.nRows(), N);
    ASSERT_EQ(A.nCols(), M);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            ASSERT_EQ(A.getEntry(i, j), 0);
        }
    }
}

TEST(DenseMatrixTest, Add) {
    const int N = 3;
    const int M = 3;
    const int SEED1 = 42;
    const int SEED2 = 16;

    DenseMatrix A = RandomMatrixFromSeed(M, N, SEED1);
    DenseMatrix B = RandomMatrixFromSeed(M, N, SEED2);
    DenseMatrix C = A + B;

    ASSERT_EQ(C.nRows(), N);
    ASSERT_EQ(C.nCols(), M);

    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            ASSERT_EQ(C.getEntry(i, j), A.getEntry(i, j) + B.getEntry(i, j));
        }
    }
}


TEST(DenseMatrixTest, Multiply) {
    const int L = 3;
    const int M = 4;
    const int N = 5;
    const int SEED1 = 42;
    const int SEED2 = 16;

    DenseMatrix A = RandomMatrixFromSeed(L, M, SEED1);
    DenseMatrix B = RandomMatrixFromSeed(M, N, SEED2);
    DenseMatrix C = A * B;

    ASSERT_EQ(C.nRows(), L);
    ASSERT_EQ(C.nCols(), N);

    ASSERT_ANY_THROW(B * A);
}

TEST(DenseMatrixTest, Transpose) {
    const int M = 4;
    const int N = 5;
    const int SEED1 = 42;

    DenseMatrix A = RandomMatrixFromSeed(M, N, SEED1);
    DenseMatrix A_T = A.transposed();


    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            ASSERT_EQ(A.getEntry(i, j), A_T.getEntry(j, i));
        }
    }
}

TEST(DenseMatrixTest, isLowerTriangular) {
    
    const DenseMatrix A {
        {4.0,   12.0,   -16.0},
        {12.0,  37.0,   -43.0},
        {-16.0, -43.0,  98.0}
    };

    ASSERT_TRUE(!A.isLowerTriangular());

    const DenseMatrix B {
        {2.0,   0.0,    0.0},
        {6.0,   1.0,    0.0},
        {-8.0,  5.0,    3.0}
    };

    ASSERT_TRUE(B.isLowerTriangular());
    ASSERT_TRUE(!B.isUpperTriangular());
}

TEST(DenseMatrixTest, isUpperTriangular) {
    
    const DenseMatrix A {
        {4.0,   12.0,   -16.0},
        {12.0,  37.0,   -43.0},
        {-16.0, -43.0,  98.0}
    };

    ASSERT_TRUE(!A.isUpperTriangular());

    const DenseMatrix B {
        {1.0,   4.0,    7.0},
        {0.0,   1.0,    9.0},
        {0.0,   0.0,    3.0}
    };

    ASSERT_TRUE(B.isUpperTriangular());
    ASSERT_TRUE(!B.isLowerTriangular());

}

TEST(DenseMatrixTest, Cholesky) {

    const DenseMatrix A {
        {4.0,   12.0,   -16.0},
        {12.0,  37.0,   -43.0},
        {-16.0, -43.0,  98.0}
    };

    const DenseMatrix L = A.cholesky();
    ASSERT_TRUE(L.isLowerTriangular());
    ASSERT_EQ(A, L * L.transposed());

    // non-symmetric matrix
    const DenseMatrix B {
        {4.0,  12.0,   -16.0},
        {0.0,  37.0,   -43.0},
        {10.0, -43.0,  98.0}
    };

    ASSERT_ANY_THROW(B.cholesky());

    // non-semi-postive-definite matrix
    const DenseMatrix C {
        {-2.0,  0.0,   0.0},
        {0.0,   -4.0,  0.0},
        {0.0,   0.0,   0.0}
    };

    ASSERT_ANY_THROW(C.cholesky());
}


TEST(DenseMatrixTest, LU) {

    const DenseMatrix A {
        {1.0, 1.0, 0.0},
        {2.0, 1.0, 3.0},
        {3.0, 1.0, 1.0}
    };

    const auto [L, U] = A.LU();
    ASSERT_EQ(A, L * U);
}

int main(int argc, char** argv) { 
    testing::InitGoogleTest(&argc, argv); 
    (void) RUN_ALL_TESTS(); 
}