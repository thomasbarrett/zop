#ifndef POLYGON_H
#define POLYGON_H

#include <array>
#include <vector>
#include <cmath>
#include <Vector.h>

namespace zop {

class SimplePolygon {
    std::vector<Vector> vertices_;
public:

    const Vector& vertex(int i) const {
        return vertices_.at(i % vertexCount());
    }

    bool isConvex() const {
        for (int  i = 0; i < )
        Vector a = (vertex(i + 1) - vertex(i)).normalize();
        Vector b = (vertex(i) - vertex(i - 1)).normalize();
    }

    int vertexCount() const {
        return vertices_.size();
    }

    double edgeLength(int i) const {
        const Vector &a = vertex(i);
        const Vector &b = vertex(i + 1);
        return (b - a).norm();
    }

    double perimeter() const {
        double acc = 0.0;
        for (int i = 0; i < vertexCount(); i++) {
            acc += edgeLength(i);
        }
        return acc;
    }

    double interiorAngle(int i) const {
        Vector a = (vertex(i + 1) - vertex(i)).normalize();
        Vector b = (vertex(i) - vertex(i - 1)).normalize();
        double angle = acos(a.dot(b));
    }
};

};

#endif /* POLYGON_H */