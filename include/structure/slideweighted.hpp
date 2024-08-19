#ifndef __OPENRM_STRUCTURE_SLIDE_WEIGHTED_HPP__
#define __OPENRM_STRUCTURE_SLIDE_WEIGHTED_HPP__
#include <deque>
#include <cmath>
#include <algorithm>

namespace rm{

template<typename T>
class SlideWeightedAvg {

public:
    SlideWeightedAvg() : size_((size_t)20) {}
    SlideWeightedAvg(int size) : size_((size_t)size) {}
    ~SlideWeightedAvg() {}

    void push(T value, T weight) {
        if (values_.size() < size_) {
            values_.push_back(value);
            weights_.push_back(weight);
        } else {
            sum_ -= values_.front() * weights_.front();
            weight_sum_ -= weights_.front();
            values_.pop_front();
            weights_.pop_front();
            values_.push_back(value);
            weights_.push_back(weight);
        }
        sum_ += value * weight;
        weight_sum_ += weight;
        avg_ = sum_ / weight_sum_;
    }
    T getAvg() {return avg_;};
    size_t getSize() {return values_.size();};
    void clear() {
        values_.clear();
        weights_.clear();
        sum_ = (T)0;
        weight_sum_ = (T)0;
        avg_ = (T)0;
    }

private:
    std::deque<T> values_;
    std::deque<T> weights_;
    size_t size_;
    T sum_ = (T)0;
    T weight_sum_ = (T)0;
    T avg_ = (T)0;

};

}

#endif