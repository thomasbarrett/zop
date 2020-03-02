#ifndef ZAP_VECTOR_H
#define ZAP_VECTOR_H

#include <vector>
#include <functional>
#include <cmath>

namespace loki {

class Vector {
private:
    std::vector<double> data_;
    int dim_ = 0;
public:

    //=--------------------------- Constructors ----------------------------=//

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

    //=------------------------- Instance Methods --------------------------=//

    int dim() const {
        return dim_;
    }

    double norm() const {
        return sqrt(Vector::dot(*this, *this));
    }

    Vector normalize() const {
        return Vector::divide(*this, norm());
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

    //=------------------------- Class Functions ---------------------------=//

    static Vector add(const Vector &a, const Vector &b) {
        return a + b;
    }

    static Vector add(const Vector &a, double b) {
        return a + b;
    }

    static Vector subtract(const Vector &a, const Vector &b) {
        return a - b;
    }

    static Vector subtract(const Vector &a, double b) {
        return a - b;
    }

    static Vector multiply(const Vector &a, const Vector &b) {
        return a * b;
    }

    static Vector multiply(const Vector &a, double b) {
        return a * b;
    }

    static Vector divide(const Vector &a, const Vector &b) {
        return a / b;
    }

    static Vector divide(const Vector &a, double b) {
        return a / b;
    }

    static double dot(const Vector &a, const Vector &b) {
        if (a.dim() != b.dim()) {
            throw std::runtime_error("dim a != dim b");
        }
        double res = 0.0;
        for (int i = 0; i < a.dim(); i++) {
            res += a[i] * b[i];
        }
        return res;
    }

    static Vector cross(const Vector &a, const Vector &b) {
        if (a.dim() != 3 || b.dim() != 3) {
            throw std::runtime_error("dim a != dim b != 3");
        }
        
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