#ifndef __OPENRM_STRUCTURE_SWAP_BUFFER_HPP__
#define __OPENRM_STRUCTURE_SWAP_BUFFER_HPP__
#include <memory>
#include <mutex>
#include <array>

namespace rm {

template <class T>
class SwapBuffer {

public: 
    SwapBuffer() :
        buffer_index_(0),
        buffer_available_{false, false},
        buffer_{std::make_shared<T>(), std::make_shared<T>()},
        buffer_mutex_{std::make_shared<std::mutex>(), std::make_shared<std::mutex>()} {}
    ~SwapBuffer() {};

    void push(std::shared_ptr<T> data) {
        int push_index = !buffer_index_;

        std::unique_lock<std::mutex> lock(*buffer_mutex_[push_index]);

        buffer_[push_index] = data;

        buffer_available_[push_index] = true;
        
        buffer_index_ = push_index;
    }

    std::shared_ptr<T> pop() {
        int pop_index = buffer_index_;

        std::unique_lock<std::mutex> lock(*buffer_mutex_[pop_index]);

        if (!buffer_available_[pop_index]) {
            return nullptr;
        }
        
        std::shared_ptr<T> ret_data = buffer_[pop_index];
        buffer_[pop_index] = std::make_shared<T>();
        
        buffer_available_[pop_index] = false;
        
        return ret_data;
    }

private:   
    int buffer_index_;
    int buffer_available_[2];
    std::shared_ptr<T> buffer_[2];
    std::array<std::shared_ptr<std::mutex>, 2> buffer_mutex_;
};

}
#endif