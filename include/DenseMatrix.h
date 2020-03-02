#ifndef ZAP_DENSE_MATRIX_H
#define ZAP_DENSE_MATRIX_H

/**
 * @file DenseMatrix.h
 * @author Thomas Barrett
 * 
 * This file contains definitions for working with dense matrices.
 */

#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <initializer_list>
#include <cassert>

namespace loki {


class DenseMatrix {
private:
    int nRows_ = 0;
    int nCols_ = 0;
    std::vector<double> data_;

protected:
public:

    DenseMatrix(int rows, int cols):
        nRows_{rows},
        nCols_{cols} {
        assert(rows > 0 && cols > 0);
        data_.resize(rows * cols);
    }

    DenseMatrix(std::initializer_list<std::initializer_list<double>> data) {
        nRows_ = data.size();
        nCols_ = data.begin()->size();
        data_.resize(nRows_ * nCols_);
        int i = 0;

        for (auto &row: data) {
            if (row.size() != nCols_) {
                throw (std::runtime_error("ragged row vectors"));
            }

            for (double element: row) {
                data_[i] = element;
                i += 1;
            }
        }
    }

    int nRows() const {
        return nRows_;
    }

    int nCols() const {
        return nCols_;
    }

    DenseMatrix transposed() const {
        DenseMatrix res{nCols(), nRows()};
        for (int i = 0; i < nRows(); i++) {
            for (int j = 0; j < nCols(); j++) {
                res[j][i] = (*this)[i][j];
            }
        }
        return res;
    };

    bool isLowerTriangular() const {

        for (int i = 0; i < nRows(); i++) {
            for (int j = i + 1; j < nCols(); j++) {
                if ((*this)[i][j] != 0) return false;
            }
        }
        return true;
    }


    bool isUpperTriangular() const {

        for (int i = 0; i < nRows(); i++) {
            for (int j = 0; j < i && j < nCols(); j++) {
                if ((*this)[i][j] != 0) return false;
            }
        }
        return true;
    }

    bool isSymmetric() const {
        if (nRows() != nCols()) return false;

        for (int i = 0; i < nRows(); i++) {
            for (int j = i; j < nRows(); j++) {
                if ((*this)[i][j] != (*this)[j][i]) return false;
            }
        }

        return true;
    }

    DenseMatrix minor(int d) const {
        DenseMatrix res{nRows(), nCols()};
        for (int i = 0; i < d; i++) {
            res[i][i] = 1;
        }
        for (int i = d; i < nRows(); i++) {
            for (int j = d; j < nCols(); j++) {
                res[i][j] = (*this)[i][j];
            }
        }
        return res;
    }

    static DenseMatrix Identity(int rows, int cols) {
        int nEntries = rows < cols? rows: cols;
        DenseMatrix mat{rows, cols};
        for (int i = 0; i < nEntries; i++) {
            mat[i][i] = 1;
        }
        return mat;
    }

    static DenseMatrix Cholesky(const DenseMatrix &A) {
        DenseMatrix L{A.nRows(), A.nCols()};

        if (!A.isSymmetric()) {
            throw std::runtime_error("matrix must by symmetric");
        }

        for (int i = 0; i < A.nRows(); i++) {
            for (int j = 0; j < (i+1); j++) {
                double s = 0;
                for (int k = 0; k < j; k++) {
                    s += L[i][k] * L[j][k];
                }

                if (i == j) {
                    double p = A[i][i] - s;
                    if (p < 0) throw std::runtime_error("matrix must be positive semidefinite");
                    L[i][j] = sqrt(p);
                } else {
                    L[i][j] = (1.0 / L[j][j] * (A[i][j] - s));
                }           
            }
        }
        return L;
    }
 
    static std::pair<DenseMatrix, DenseMatrix> LU(const DenseMatrix &a) {
        if (a.nRows() != a.nCols()) {
            throw std::runtime_error("matrix must be square");
        }
        int n = a.nRows();
        DenseMatrix l{a.nRows(), a.nRows()};
        DenseMatrix u{a.nRows(), a.nRows()};
        
        int i = 0, j = 0, k = 0;
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if (j < i)
                l[j][i] = 0;
                else {
                    l[j][i] = a[j][i];
                    for (k = 0; k < i; k++) {
                    l[j][i] = l[j][i] - l[j][k] * u[k][i];
                    }
                }
            }
            for (j = 0; j < n; j++) {
                if (j < i)
                    u[i][j] = 0;
                else if (j == i)
                    u[i][j] = 1;
                else {
                    u[i][j] = a[i][j] / l[i][i];
                    for (k = 0; k < i; k++) {
                    u[i][j] = u[i][j] - ((l[i][k] * u[k][j]) / l[i][i]);
                    }
                }
            }
    }
        return {std::move(l), std::move(u)};
    }

    double* operator[](int i) {
        return &data_.at(i * nCols_);
    }

    const double* operator[](int i) const {
        return &data_.at(i * nCols_);
    }
    
    friend DenseMatrix operator+(const DenseMatrix &A, const DenseMatrix &B) {
        if (A.nRows() != B.nRows() || A.nCols() != B.nCols()) {
            throw std::runtime_error("error: dimension mismatch");
        }

        DenseMatrix res{A.nRows(), A.nCols()};
        for (int i = 0; i < A.nRows(); i++) {
            for (int j = 0; j < A.nCols(); j++) {
                res[i][j] = A[i][j] + B[i][j];
            }
        }
        return res;
    }

    friend bool operator==(const DenseMatrix &A, const DenseMatrix &B) {
        if (A.nRows() != B.nRows() || A.nCols() != B.nCols()) {
            return false;
        }
        for (int i = 0; i < A.nRows(); i++) {
            for (int j = 0; j < A.nCols(); j++) {
                if(A[i][j] != B[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    friend bool operator!=(const DenseMatrix &A, const DenseMatrix &B) {
        return !(A == B);
    }

    friend DenseMatrix operator*(const DenseMatrix &A, const DenseMatrix &B) {
        if (A.nCols() != B.nRows()) {
            throw std::runtime_error("error: dimension mismatch");
        }

        DenseMatrix res{A.nRows(), B.nCols()};
        for (int i = 0; i < A.nRows(); i++) {
            for (int j = 0; j < B.nCols(); j++) {
                for (int k = 0; k < A.nCols(); k++) {
                    res[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return res;
    }

    friend std::ostream& operator<<(std::ostream& s, const DenseMatrix &m) {
        s << "DenseMatrix [" << m.nRows() << ", " << m.nCols() << "]" << std::endl;
        s <<  std::fixed << std::setprecision(3);
        s << "[";
        for (int i = 0; i < m.nRows(); i++) {
            if (i > 4 && i < m.nRows() - 5) {
                s << std::string(37, ' ') << "..." << std::endl;
                i = m.nRows() - 5;
            }
            if (i != 0) s << " ";
            s << "[";
            for (int j = 0; j < m.nCols(); j++) {
                if (j > 4 && j < m.nCols() - 5) {
                    s << "... , ";
                    j = m.nCols() - 5;
                }
                s << m[i][j];
                if (j != m.nCols() - 1) s << ", ";
            }
            s << "]";
            if (i != m.nRows() - 1) s << "," << std::endl;
        }
        s << "]" << std::endl;

        return s;
    }
};

}

#endif /* ZAP_DENSE_MATRIX_H */