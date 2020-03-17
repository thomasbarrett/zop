
#include "gtest/gtest.h"
#include <Vector.h>
using namespace zop;

TEST(VectorTest, Constructor) {
    Vector v(5);
    for (int i = 0; i < v.dim(); i++) {
        ASSERT_EQ(v[i], 0);
    }
}

TEST(VectorTest,Add) {
    Vector v(5);
    Vector w(5);
    for (int i = 0; i < v.dim(); i++) {
        v[i] = i;
        w[i] = 2 * i;
    }

    Vector x = v + w;
    for (int i = 0; i < x.dim(); i++) {
        ASSERT_EQ(x[i], 3 * i);
    }
}

TEST(VectorTest, Subtract) {
    Vector v(5);
    Vector w(5);
    for (int i = 0; i < v.dim(); i++) {
        v[i] = 3 * i;
        w[i] = 2 * i;
    }

    Vector x = v - w;
    for (int i = 0; i < x.dim(); i++) {
        ASSERT_EQ(x[i], i);
    }
}


TEST(VectorTest, Multiply) {
    Vector v(5);
    Vector w(5);
    for (int i = 0; i < v.dim(); i++) {
        v[i] = 2 * i;
        w[i] = 2;
    }

    Vector x = v * w;
    for (int i = 0; i < x.dim(); i++) {
        ASSERT_EQ(x[i], 4 * i);
    }
}

TEST(VectorTest, Divide) {
    Vector v(5);
    Vector w(5);
    for (int i = 0; i < v.dim(); i++) {
        v[i] = 4;
        w[i] = 2;
    }

    Vector x = v / w;
    for (int i = 0; i < x.dim(); i++) {
        ASSERT_EQ(x[i], 2);
    }

}
