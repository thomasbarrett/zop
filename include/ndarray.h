
#ifndef NDARRAY_H
#define NDARRAY_H

#include <vector>

namespace zap {

template <typename T> class ndarray {
private:
    std::vector<T> data_;
    std::vector<int> shape_;
public:
    
    ndarray(std::vector<int> shape): shape_{shape} {
        int size = 0;
        for (int s: shape) size += s;
        data_.resize(size);
    }

};

}


#endif /* NDARRAY_H */
