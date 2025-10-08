#ifndef __OPENRM_STRUCTURE_CYCLEQUEUE_HPP__
#define __OPENRM_STRUCTURE_CYCLEQUEUE_HPP__
#include <deque>
#include <vector>
#include <cmath>
#include <algorithm>

namespace rm {

template<typename T>
class CycleQueue {

public:
    CycleQueue() : size_((size_t)5), sum_((T)0) {}
    CycleQueue(int size) : size_((size_t)size), sum_((T)0) {}
    ~CycleQueue() {}

    void push(T value) {
        if (values_.size() < size_) {
            values_.push_back(value); 
        } else {
            sum_ -= values_.front();
            values_.pop_front();
            values_.push_back(value);
        }
        sum_ = sum_ + value;
        avg_ = sum_ / values_.size();
    }
    T pop() {return values_.front();};
    T getAvg() {return avg_;};
    T getSum() {return sum_;};
    std::vector<T> getVec() {
        std::vector<T> vec;
        for (auto it = values_.begin(); it != values_.end(); it++) {
            vec.push_back(*it);
        }
        return vec;
    }
    void clear() {
        values_.clear();
        sum_ = (T)0;
    }

private:
    std::deque<T> values_;
    size_t size_;
    T sum_;
    T avg_;
};

}

#endif