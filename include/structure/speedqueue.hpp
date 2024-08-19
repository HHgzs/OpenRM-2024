#ifndef __OPENRM_STRUCTURE_SPEEDQUEUE_HPP__
#define __OPENRM_STRUCTURE_SPEEDQUEUE_HPP__
#include <iostream>
#include <deque>
#include <vector>
namespace rm {

template <class T>
class SpeedQueue {
public:
    SpeedQueue() : length_(3ull), init_((T)0) {deque_.resize(3ull, 0); percent_.resize(3ull, 1.0 / 3.0);}
    SpeedQueue(size_t length, T init) : length_(length), init_(init) {
        deque_.resize(length_, init); percent_.resize(length_, 1.0 / (double)length_);}
    SpeedQueue(size_t length, T init, std::vector<double> pct) : length_(length), init_(init) {
        deque_.resize(length_, init);
        percent_.resize(length_, 0.0);
        double pct_sum = std::accumulate(pct.begin(), pct.end(), 0.0);
        for(int i = 0; i < length_; i++) percent_[length_ - i - 1] = pct[i] / pct_sum;
    }
    ~SpeedQueue() {}
    void push(T num) {
        T last = deque_.back();
        if(last == num) return;
        deque_.pop_front();
        deque_.push_back(num);
    }
    T pop() {
        T avg = (T)0;
        for (int i = 0; i < length_; i++) avg += deque_[i] * percent_[i];
        return avg;
    }
    T back() {
        return deque_.back();
    }

    void clear() {
        deque_.clear();
        deque_.resize(length_, init_);
    }


private:
    std::deque<T> deque_;
    size_t length_;
    std::vector<double> percent_;
    T init_ = 0.0;
};

}
#endif