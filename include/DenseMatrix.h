#ifndef ZAP_DENSE_MATRIX_H
#define ZAP_DENSE_MATRIX_H

/**
 * @file DenseMatrix.h
 * @author Thomas Barrett
 * 
 * This file contains definitions for working with dense matrices.
 */

#include <Matrix.h>
#include <vector>
#include <cmath>
#include <initializer_list>
#include <cassert>
#include <Vector.h>

namespace zop {

/**
 * This is a dense matrix
 */
class DenseMatrix: public AbstractMatrix<DenseMatrix> {
private:
    int nRows_ = 0;
    int nCols_ = 0;
    std::vector<Vector> data_;

protected:
public:
    using Row = Vector;
    using Builder = DenseMatrix;

    DenseMatrix(int rows, int cols):
        nRows_{rows},
        nCols_{cols} {
        assert(rows > 0 && cols > 0);
        for (int i = 0; i < rows; i++) {
            data_.push_back(Vector(cols));
        }
    }

    DenseMatrix(std::initializer_list<std::initializer_list<double>> data) {
        nRows_ = data.size();
        nCols_ = data.begin()->size();
        for (auto &row: data) {
            if (row.size() != nCols_) {
                throw (std::runtime_error("ragged row vectors"));
            }
            data_.push_back({row});
        }
    }

    DenseMatrix transposed() const {
        DenseMatrix res{nCols(), nRows()};
        for (int i = 0; i < nRows(); i++) {
            for (int j = 0; j < nCols(); j++) {
                res.setEntry(j, i, this->getEntry(i, j));
            }
        }
        return res;
    };

    int nRows() const {
        return nRows_;
    }

    int nCols() const {
        return nCols_;
    }

    Row& row(int i) {
        return data_.at(i);
    }

    const Row& operator[](int i) {
        return row(i);
    }
    
    const Row& row(int i) const {
        return data_.at(i);
    }

    double getEntry(int i, int j) const {
        return data_.at(i)[j];
    }

    void setEntry(int i, int j, double e) {
        data_.at(i)[j] = e;
    }


};

}

#endif /* ZAP_DENSE_MATRIX_H */