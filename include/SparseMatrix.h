#ifndef ZAP_SPARSE_MATRIX_H
#define ZAP_SPARSE_MATRIX_H

/**
 *  \file SparseMatrix.h
 *  \author Thomas Barrett 
 */

#include <map>
#include <cassert>
#include <vector>
#include <exception>

#include <Vector.h>

namespace loki {

class DimensionMismatchException: public std::exception {
public:
    virtual const char* what() const throw() {
        return "dimension mismatch";
    }
};

class DOKSparseMatrix {
private:
    int nRows_ = 0;
    int nCols_ = 0;
    std::map<std::pair<int, int>, double> entries_;
public:
    DOKSparseMatrix(int nRows, int nCols) {
        nRows_ = nRows;
        nCols_ = nCols;
    }
    
    using const_iterator = decltype(entries_)::const_iterator;

    int nRows() const { return nRows_; }
    int nCols() const { return nCols_; }

    DOKSparseMatrix transposed() const {
        DOKSparseMatrix res{nCols(), nRows()};
        for (auto &[loc, v]: entries_) {
            res.addEntry(loc.second, loc.first, v);
        }
        return res;
    }

    void addEntry(int i, int j, double v) {
        assert(0 <= i && i < nRows_);
        assert(0 <= j && j < nCols_);
        entries_.emplace(std::make_pair(i, j), v);
    }

    double getEntry(int i, int j) {
        assert(0 <= i && i < nRows_);
        assert(0 <= j && j < nCols_);
        auto it = entries_.find({i, j});
        return it == entries_.end() ? 0.0: it->second;
    }

    const_iterator begin() const {
        return entries_.begin();
    }

    const_iterator end() const {
        return entries_.end();
    }
};

/**
 * This class represents a sparse matrix in compressed sparse row format.
 * This is optimal for matrix-vector multiplication. A CSR matrix
 * representation stores the following information:
 * 
 * 1. a vector of non-zero values
 * 2. a vector of column indices
 * 3. a vector of row start indices
 * 
 * Note that the non-zero values and column indices are parallel arrays.
 * Thus, the column of the ith non-zero value can be found at the ith
 * position of the column vector. The vector of row-start indices stores
 * the index of the first non-zero value in each row.
 */
class CSRSparseMatrix {
private:
    int nRows_ = 0;
    int nCols_ = 0;
    std::vector<double> values_;
    std::vector<int> column_indices_;
    std::vector<int> row_indices_;

    std::pair<int, int> getRowIndices(int i) const {
        assert(i >= 0 && i < nRows());
        if (i >= row_indices_.size()) {
            return {values_.size(), values_.size()};
        } else if (i == row_indices_.size() - 1) {
            return {row_indices_[i], values_.size()};
        } else {
            return {row_indices_[i], row_indices_[i + 1]};
        }
    }

    struct Row {
        int count;
        const double* values;
        const int* indices;

        static double dot(const Row &A, const Row &B) {\
            double acc = 0;
            int j = 0;
            for (int i = 0; i < A.count; i++) {
                while (A.indices[i] > B.indices[j] && j < B.count) j += 1;
                if (A.indices[i] == B.indices[j]) {
                    acc += A.values[i] * B.values[j];
                }
            }
            return acc;
        }

        static double dot(const Row &A, const Vector &B) {\
            double acc = 0;
            for (int i = 0; i < A.count; i++) {
                acc += A.values[i] * B[A.indices[i]];
            }
            return acc;
        }

    };


    Row row(int i) const {
        auto [a, b] = getRowIndices(i);
        return {b - a, &values_[a], &column_indices_[a]};
    }

public:
    CSRSparseMatrix(int nRows, int nCols): nRows_{nRows}, nCols_{nCols} {};
    CSRSparseMatrix(const DOKSparseMatrix &M) {
        nRows_ = M.nRows();
        nCols_ = M.nCols();
        int row = -1;
        int a = 0;
        for (auto &[loc, val]: M) {
            auto [i, j] = loc;
            values_.push_back(val);
            column_indices_.push_back(j);
            while (row != i) {
                row_indices_.push_back(a);
                row += 1;
            }
            a += 1;
        }
    };

    int nRows() const { return nRows_; }
    int nCols() const { return nCols_; }

    CSRSparseMatrix transposed() const {
        DOKSparseMatrix mat{nCols(), nRows()};
        for (int i = 0; i < nRows(); i++) {
            auto [a, b] = getRowIndices(i);
            while (a < b) {
                int j = column_indices_[a];
                double v = values_[a];
                mat.addEntry(j, i, v);
                a += 1;
            }
        }
        return CSRSparseMatrix(mat);
    }

    Vector operator*(const Vector &v) {
        if (v.dim() != nCols()) throw DimensionMismatchException{};
        Vector res(v.dim());
        for (int i = 0; i < nRows(); i++) {
           res[i] = Row::dot(row(i), v);
        }
        return res;
    };

    friend CSRSparseMatrix operator*(const CSRSparseMatrix &A, const CSRSparseMatrix &B) {
        if (A.nCols() != B.nRows()) {
            throw std::runtime_error("error: dimension mismatch");
        }

        CSRSparseMatrix B_T = B.transposed();
        DOKSparseMatrix res{A.nRows(), B.nCols()};
        for (int i = 0; i < A.nRows(); i++) {
            for (int j = 0; j < B.nRows(); j++) {
                double v = Row::dot(A.row(i), B.row(j));
                if (v != 0) res.addEntry(i, j, v);
            }
        }
        return CSRSparseMatrix(res);
    }

    double getEntry(int i, int j) {
        assert(0 <= i && i < nRows_);
        assert(0 <= j && j < nCols_);
        auto [a, b] = getRowIndices(i);
        while (a < b) {
            int col = column_indices_[a];
            if (j == col) return values_[a];
            a += 1;
        }
        return 0.0;
    }

};

}

#endif /* ZAP_SPARSE_MATRIX_H */