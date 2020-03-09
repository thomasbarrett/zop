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

#include <Matrix.h>
#include <Vector.h>

namespace zap {


class DOKSparseMatrix: public AbstractMatrix<DOKSparseMatrix>  {
private:
    int nRows_ = 0;
    int nCols_ = 0;
    std::map<std::pair<int, int>, double> entries_;
public:

    using Builder = DOKSparseMatrix;
   

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
            res.setEntry(loc.second, loc.first, v);
        }
        return res;
    }

    void setEntry(int i, int j, double v) {
        assert(0 <= i && i < nRows_);
        assert(0 <= j && j < nCols_);
        if (v != 0.0) {
            entries_.emplace(std::make_pair(i, j), v);
        }
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
class CSRSparseMatrix: public AbstractMatrix<CSRSparseMatrix> {
private:
    int nRows_ = 0;
    int nCols_ = 0;
    std::vector<double> values_;
    std::vector<int> column_indices_;
    std::vector<int> row_indices_;

public:

    using Builder = DOKSparseMatrix;
    
    /**
     * A convenience view to a single row in a CSRSparseMatrix.
     * 
     * Since a CSRSparseMatrix is row-oriented, many row operations are
     * extremely efficient with respect to the number of non-zero entries.
     * This convenience class encompasses the operations that can be 
     * performed on a row.
     */
    class Row {
    private:
        int a_ = 0;
        int b_ = 0;
        const CSRSparseMatrix *mat_ = nullptr;

        static int binary_search(const int *arr, int a, int b, int i) {
            assert(b >= a);
            if (b - a > 1) {  
                int mid = (a + b) / 2;
                if (arr[mid] == i) return mid;
                if (arr[mid] > i) return binary_search(arr, a, mid, i);
                return binary_search(arr, mid, b, i);         
            } else {
                if (arr[a] == i) return a;
                else return -1;
            }
        }
        
    public:
        Row(int a, int b, const CSRSparseMatrix *mat): a_{a}, b_{b}, mat_{mat} {}

        struct iterator {
            int i = 0;
            const Row *row;
            void operator++() {i++; }
            bool operator!=(const iterator &it) const { return i != it.i; }
            std::pair<int, double> operator*() const {
                return {row->indices(i), row->values(i)};
            }
        };

        double values(int i) const {
            return mat_->values_[a_ + i]; 
        }

        int indices(int i) const {
            return mat_->column_indices_[a_ + i]; 
        }

        int count() const {
            return b_ - a_;
        }

        /**
         * Returns the value at the ith column. If there is no non-zero entry
         * in that column, then 0.0 is returned.
         * 
         * Note that this is a O(log(N)) operation, and thus should only be
         * used when absolutely necessary. For example, if no sparse optimized
         * implementation is defined for a certain operation, this allows the
         * dense algorithm to be used.
         */
        double operator[](int i) {
            if (a_ == b_) return 0.0;
            int j = binary_search(mat_->column_indices_.data(), a_, b_, i);
            if (j == -1) return 0.0;
            else return mat_->values_[j];
        }

        /**
         * Return an iterator to the start of the row.
         */
        const iterator begin() const {
            return {0, this};
        }

        /**
         * Return an iterator to the end of the row.
         */
        const iterator end() const {
            return {count(), this};
        }

        /**
         * Return the dot product between two sparse rows.
         * 
         * This operation is sparse optimized and runs in O(N) with respect
         * to the number of non-zero elements in the rows.
         */
        static double dot(const Row &A, const Row &B) {
            double acc = 0;
            int idx = 0;
            for (auto [i, e]: A) {
                while (i > B.indices(idx) && idx < B.count()) {
                    idx += 1;
                }
                if (i == B.indices(idx)) {
                    acc += e * B.values(idx);
                }
            }
            return acc;
        }

        /**
         * Return the dot product between a sparse row and a dense vector.
         * 
         * This operation is sparse optimized and runs in O(N) with respect
         * to the number of non-zero elements in the row.
         */
        static double dot(const Row &A, const Vector &B) {
            double acc = 0;
            for (auto [i, e]: A) {
                acc += e * B[i];
            }
            return acc;
        }

    };

    CSRSparseMatrix(const DOKSparseMatrix &M) {
        row_indices_.resize(M.nRows() + 1);
        nRows_ = M.nRows();
        nCols_ = M.nCols();
        int row = -1;
        int a = 0;
        for (auto &[loc, val]: M) {
            auto [i, j] = loc;
            values_.push_back(val);
            column_indices_.push_back(j);
            while (row != i) {
                row += 1;
                row_indices_[row]= a;
            }
            a += 1;
        }

        for (int r = row + 1; r <= M.nRows(); r++) {
            row_indices_[r] = a;
        }
    };


    int nRows() const { return nRows_; }
    int nCols() const { return nCols_; }
    double getEntry(int i, int j) const { return row(i)[j]; }

    /**
     * Return a view to the ith row in the matrix. 
     */
    Row row(int i) const {
        int a = row_indices_[i];
        int b = row_indices_[i + 1];
        return {a, b, this};
    }

    /**
     * Return the transpose of the original matrix.
     */
    CSRSparseMatrix transposed() const {
        DOKSparseMatrix mat{nCols(), nRows()};
        for (int i = 0; i < nRows(); i++) {
            for (auto [j, e]: row(i)) {
                mat.setEntry(j, i, e);
            }
        }
        return CSRSparseMatrix(mat);
    } 
    
};

}

#endif /* ZAP_SPARSE_MATRIX_H */