#ifndef __OPENRM_STRUCTURE_SLIDE_STD_HPP__
#define __OPENRM_STRUCTURE_SLIDE_STD_HPP__
#include <deque>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace rm {

template<typename T>
class SlideStd {
    
public:
    SlideStd() : size_((size_t)20), sum_((T)0) {}
    SlideStd(int size) : size_((size_t)size), sum_((T)0) {}
    ~SlideStd() {}

    void push(T value) {
        if (values_.size() < size_) {
            values_.push_back(value); 
        } else {
            sum_ -= values_.front();
            values_.pop_front();
            values_.push_back(value);
        }
        sum_ += value;
        T average = sum_ / values_.size();
        avg_ = average;
        var_ = std::accumulate(values_.begin(), values_.end(), 0.0, [average](double acc, T value) { 
                            return acc + std::pow(value - average, 2); 
                        } ) / values_.size();
        std_ = sqrt(var_);
    }
    double getStd() {return std_;};
    double getVar() {return var_;};
    double getAvg() {return avg_;};
    size_t getSize() {return values_.size();};
    void clear() {
        values_.clear();
        sum_ = (T)0;
        avg_ = (T)0;
    }

private:
    std::deque<T> values_;
    size_t size_;
    T sum_;
    T avg_;
    double var_;
    double std_;
};

template<typename T>
class SlideAvg {
public:
    SlideAvg() : size_((size_t)20) {}
    SlideAvg(int size) : size_((size_t)size) {}
    ~SlideAvg() {}

    void push(T value) {
        if (values_.size() < size_) {
            values_.push_back(value); 
        } else {
            sum_ -= values_.front();
            values_.pop_front();
            values_.push_back(value);
        }
        sum_ += value;
        avg_ = sum_ / values_.size();
    }
    double getAvg() {return avg_;};
    size_t getSize() {return values_.size();};
    void clear() {
        values_.clear();
        sum_ = (T)0;
        avg_ = (T)0;
    }

private:
    std::deque<T> values_;
    size_t size_;
    T sum_ = (T)0;
    T avg_ = (T)0;
};



}

#endif