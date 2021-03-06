#ifndef ZAP_VECTOR_H
#define ZAP_VECTOR_H

#include <vector>
#include <functional>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <cassert>

namespace zop {

/**
 * This class implements an n-dimensional mathmatical vector as well as common
 * vector operations such as vector addition, dot product, cross product, and 
 * normalization, and other various methods.
 */
class Vector {
private:
    std::vector<double> data_;
    int dim_ = 0;

public:

    
    Vector(int dim): dim_{dim} {
        data_.resize(dim);
    }

    Vector(std::initializer_list<double> data) {
        dim_ = data.size();
        data_.resize(dim_);

        int i = 0;
        for (double element: data) {
            data_[i] = element;
            i += 1;
        }
    }

    int dim() const {
        return dim_;
    }


    double norm() const {
        return sqrt(this->dot(*this));
    }

    Vector normalize() const {
        return (*this) / norm();
    }

    Vector map(std::function<double(double)> f) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res[i] = f(data_[i]);
        }
        return res;
    }

    Vector reduce(std::function<double(double, double)> f, double initial) const {
        double acc = initial;
        for (int i = 0; i < dim(); i++) {
            acc = f(acc, data_[i]);
        }
        return acc;
    }

    int argmin() const {
        int imin = 0;
        double min = data_.front();
        for (int i = 1; i < dim(); i++) {
            if (data_[i] < min) {
                min = data_[i];
                imin = i;
            }
        }
        return imin;
    }

    int argmax() const {
        int imax = 0;
        double min = data_.front();
        for (int i = 1; i < dim(); i++) {
            if (data_[i] > min) {
                min = data_[i];
                imax = i;
            }
        }
        return imax;
    }

    int max() const {
        return data_[argmax()];
    }

    int min() const {
        return data_[argmin()];
    }

    double sum() const {
        double acc = 0.0;
        for (double e: data_) {
            acc += e;
        }
        return acc;
    }

    double mean() const {
        return sum() / dim();
    }
    //=----------------------- Operator Overloads --------------------------=//

    double& operator[](size_t i) {
        assert(i >= 0 &&  i < dim());
        return data_[i];
    }

    const double& operator[](size_t i) const {
        assert(i >= 0 &&  i < dim());
        return data_[i];
    }

    friend bool operator==(const Vector &a, const Vector &b) {
        if (a.dim() != b.dim()) return false;
        for (int i = 0; i < a.dim(); i++) {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    Vector operator+(const Vector &b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] + b.data_[i];
        }
        return res;
    }

    Vector operator+(double b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] + b;
        }
        return res;
    }

    Vector operator-(const Vector &b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] - b.data_[i];
        }
        return res;
    }

    Vector operator-(double b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] - b;
        }
        return res;
    }

    Vector operator*(const Vector &b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] * b.data_[i];
        }
        return res;
    }


    Vector operator*(double b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] * b;
        }
        return res;
    }

    Vector operator/(const Vector &b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] / b.data_[i];
        }
        return res;
    }

    Vector operator/(double b) const {
        Vector res(dim());
        for (int i = 0; i < dim(); i++) {
            res.data_[i] = data_[i] / b;
        }
        return res;
    }

    double dot(const Vector &b) const {
        if (dim() != b.dim()) {
            throw std::runtime_error("dim a != dim b");
        }
        double res = 0.0;
        for (int i = 0; i < dim(); i++) {
            res += (*this)[i] * b[i];
        }
        return res;
    }

    Vector cross(const Vector &b) const {
        if (dim() != 3 || b.dim() != 3) {
            throw std::runtime_error("dim a != dim b != 3");
        }

        const Vector &a = *this;

        return {
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        }; 
    }

    friend std::ostream& operator<<(std::ostream &os, const Vector &v) {
        os << "[";
        for (int i = 0; i < v.dim(); i++) {
            if (i != 0) os << ", ";
            os << v[i];
        }
        os << "]";
        return os;
    }
};

}

#endif /* ZAP_VECTOR_H */