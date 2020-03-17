#ifndef ZOP_RANDOM_H
#define ZOP_RANDOM_H

#include <SparseMatrix.h>

namespace zop::random {

    DOKSparseMatrix DOKSparseUpperTriangular(int M, int N, double sparsity, long seed=0) {
        std::srand(seed);

        DOKSparseMatrix mat{M, N};
        for (int e = 0; e < (int)(sparsity * M * N); e++) {
            int i = std::rand() % M;
            int j = std::rand() % N;
            double v = (double) std::rand() / RAND_MAX - 0.5;
            if (j >= i) {
                mat.setEntry(i, j, v);
            }
        }
        assert(mat.isUpperTriangular());
        return mat;
    }

    DOKSparseMatrix DOKSparseLowerTriangular(int M, int N, double sparsity, long seed=0) {
        std::srand(seed);

        DOKSparseMatrix mat{M, N};
        for (int e = 0; e < (int)(sparsity * M * N); e++) {
            int i = std::rand() % M;
            int j = std::rand() % N;
            double v =  (double) std::rand() / RAND_MAX - 0.5;
            if (j <= i) {
                mat.setEntry(i, j, v);
            }
        }
        assert(mat.isLowerTriangular());
        return mat;
    }
};

#endif /* ZOP_RANDOM_H */