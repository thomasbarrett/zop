#ifndef MATRIX_H
#define MATRIX_H

#include <type_traits>
#include <cmath>
#include <Vector.h>

namespace zop {

/**
 * This exception is thrown when a matrix operation is invalid due to the
 * dimensions of the matrix. The exact meaning depends on the context
 * For example, the hadamard product (element-wise multiplication) requires
 * that matrix dimensions be identical, while a matrix product only requires
 * that the left matrices number of columns is equivelant to the right matrices
 * number of rows.
 */
class DimensionMismatchException: public std::exception {
public:
    virtual const char* what() const throw() {
        return "dimension mismatch";
    }
};

/**
 * AbstractMatrix contains an general implementation of many useful matrix
 * operations. A subclass can override these methods with a more
 * specialized implementation. In order for a subclass to utilize these
 * methods, it must provide some combination of the following methods:
 * 
 * * `Derived::Builder(nRows, nCols)`
 * * `Derived::Derived(const Derived::Builder &)`
 * * `Derived::getEntry(int i, int j)`
 * * `Derived::setEntry(int i, int j, double e)` 
 * * `Derived::nRows()`
 * * `Derived::nCols()`
 * * `Derived::row(int i)`
 * * `Derived::Row::dot(const Derived::Row&, const Derived::Row&)`
 * 
 * This base class follows the Curiously Recurring Template Pattern (CRTP).
 * This design choice was made for a number of reasons. First of all, 
 * CRTP provides compile time polymorphism so that there is no overhead
 * of virtual calls that would dramatically slow down matrix operations.
 * Also, by virtue of the SFINAE principal, subclasses need not implement
 * every method. 
 */
template <class Derived> class AbstractMatrix {
public:

    static Derived Identity(int rows, int cols) {
        int nEntries = rows < cols? rows: cols;
        typename Derived::Builder mat{rows, cols};
        for (int i = 0; i < nEntries; i++) {
            mat.setEntry(i, i, 1.0);
        }
        return Derived(std::move(mat));
    }

    static Derived AffineScale(double r) {
        typename Derived::Builder mat = Derived::Builder::Identity(4, 4);
        for (int i = 0; i < 3; i++) {
            mat.setEntry(i, i, r);
        }
        return Derived(std::move(mat));
    }

    static Derived AffineTranslation(double x, double y, double z) {
        typename Derived::Builder mat = Derived::Builder::Identity(4, 4);
        mat.setEntry(0, 3, x);
        mat.setEntry(1, 3, y);
        mat.setEntry(2, 3, z);
        return Derived(std::move(mat));
    }

    static Derived AffineRotationX(double r) {
        typename Derived::Builder mat = Derived::Builder::Identity(4, 4);
        double c = cos(r);
        double s = sin(r);
        mat.setEntry(1, 1, c);
        mat.setEntry(1, 2, -s);
        mat.setEntry(2, 1, s);
        mat.setEntry(2, 2, c);
        return Derived(std::move(mat));
    }

    static Derived AffineRotationY(double r) {
        typename Derived::Builder mat = Derived::Builder::Identity(4, 4);
        double c = cos(r);
        double s = sin(r);
        mat.setEntry(0, 0, c);
        mat.setEntry(0, 2, s);
        mat.setEntry(2, 0, -s);
        mat.setEntry(2, 2, c);
        return Derived(std::move(mat));
    }

    static Derived AffineRotationZ(double r) {
        typename Derived::Builder mat = Derived::Builder::Identity(4, 4);
        double c = cos(r);
        double s = sin(r);
        mat.setEntry(0, 0, c);
        mat.setEntry(0, 1, -s);
        mat.setEntry(1, 0, s);
        mat.setEntry(1, 1, c);
        return Derived(std::move(mat));
    }
 
    bool isSymmetric() const {
        const Derived *self = static_cast<const Derived *>(this);
        int M = self->nRows();
        int N = self->nCols();

        if (M != N) return false;
        for (int i = 0; i < M; i++) {
            for (int j = i; j < N; j++) {
                if (self->getEntry(i, j) != self->getEntry(j, i)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool isLowerTriangular() const {
        const Derived *self = static_cast<const Derived *>(this);
        int M = self->nRows();
        int N = self->nCols();

        for (int i = 0; i < M; i++) {
            for (int j = i + 1; j < N; j++) {
                if (self->getEntry(i, j) != 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool isUpperTriangular() const {
        const Derived *self = static_cast<const Derived *>(this);
        int M = self->nRows();
        int N = self->nCols();

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < i && j < N; j++) {
                if (self->getEntry(i, j) != 0) {
                    return false;
                }
            }
        }
        return true;
    }

    Derived cholesky() const {
        const Derived *self = static_cast<const Derived *>(this);
        int M = self->nRows();
        int N = self->nCols();

        typename Derived::Builder L{M, N};

        if (!self->isSymmetric()) {
            throw std::runtime_error("matrix must by symmetric");
        }

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < (i+1); j++) {
                double s = 0;
                for (int k = 0; k < j; k++) {
                    s += L.getEntry(i, k) * L.getEntry(j, k);
                }

                if (i == j) {
                    double p = self->getEntry(i, i) - s;
                    if (p < 0) throw std::runtime_error("matrix must be positive semidefinite");
                    L.setEntry(i, j, sqrt(p));
                } else {
                    L.setEntry(i, j, (1.0 / L.getEntry(j, j) * (self->getEntry(i, j) - s)));
                }           
            }
        }

        return Derived(std::move(L));
    }
 
    std::pair<Derived, Derived> LU() const {
        const Derived *self = static_cast<const Derived *>(this);
        int M = self->nRows();
        int N = self->nCols();

        if (M != N) {
            throw std::runtime_error("matrix must be square");
        }

        typename Derived::Builder lower{N, N};
        typename Derived::Builder upper{N, N};
        
        for (int i = 0; i < N; i++) { 
  
            // Upper Triangular 
            for (int k = i; k < N; k++) { 
    
                double sum = 0; 
                for (int j = 0; j < i; j++) 
                    sum += lower.getEntry(i, j) * upper.getEntry(j, k); 
    
                upper.setEntry(i, k, self->getEntry(i, k) - sum);
            } 
    
            // Lower Triangular 
            for (int k = i; k < N; k++) { 
                if (i == k) 
                    lower.setEntry(i, i, 1.0); // Diagonal as 1 
                else { 
    
                    double sum = 0; 
                    for (int j = 0; j < i; j++) 
                        sum += lower.getEntry(k, j) * upper.getEntry(j, i); 

                    assert(upper.getEntry(i, i) != 0.0);
                    lower.setEntry(k, i, (self->getEntry(k, i) - sum) / upper.getEntry(i, i)); 
                }
            } 
        } 

        return {Derived(std::move(lower)), Derived(std::move(upper))};
    }

    Vector operator*(const Vector &v) const {
        const Derived *self = static_cast<const Derived *>(this);
        if (v.dim() != self->nCols()) throw DimensionMismatchException{};
        Vector res(v.dim());
        for (int i = 0; i < self->nRows(); i++) {
           res[i] = self->row(i).dot(v);
        }
        return res;
    }

    Derived operator*(const Derived &B) const {
        const Derived *self = static_cast<const Derived *>(this);

        if (self->nCols() != B.nRows()) {
            throw std::runtime_error("error: dimension mismatch");
        }

        Derived B_T = B.transposed();
        typename Derived::Builder res{self->nRows(), B.nCols()};
        for (int i = 0; i < self->nRows(); i++) {
            for (int j = 0; j < B_T.nRows(); j++) {
                double v = self->row(i).dot(B_T.row(j));
                if (v != 0) res.setEntry(i, j, v);
            }
        }
        return Derived(std::move(res));
    }

    Derived operator+(const Derived &B) {
        const Derived *self = static_cast<const Derived *>(this);

        if (self->nCols() != B.nRows()) {
            throw std::runtime_error("error: dimension mismatch");
        }

        typename Derived::Builder res{self->nRows(), B.nCols()};
        for (int i = 0; i < self->nRows(); i++) {
            for (int j = 0; j < self->nCols(); j++) {
                res.setEntry(i, j, self->getEntry(i, j) + B.getEntry(i, j));
            }
        }

        return Derived(std::move(res));
    }

    Derived operator-(const Derived &B) {
        const Derived *self = static_cast<const Derived *>(this);

        if (self->nCols() != B.nRows()) {
            throw std::runtime_error("error: dimension mismatch");
        }

        typename Derived::Builder res{self->nRows(), B.nCols()};
        for (int i = 0; i < self->nRows(); i++) {
            for (int j = 0; j < self->nCols(); j++) {
                res.setEntry(i, j, self->getEntry(i, j) - B.getEntry(i, j));
            }
        }

        return Derived(std::move(res));
    }

    friend bool operator==(const Derived &A, const Derived &B) {
        if (A.nRows() != B.nRows() || A.nCols() != B.nCols()) {
            return false;
        }
        for (int i = 0; i < A.nRows(); i++) {
            for (int j = 0; j < A.nCols(); j++) {
                if(A.getEntry(i, j) != B.getEntry(i, j)) {
                    return false;
                }
            }
        }
        return true;
    }

    friend bool operator!=(const Derived &A, const Derived &B) {
        return !(A == B);
    }

};

}
#endif /* MATRIX_H */