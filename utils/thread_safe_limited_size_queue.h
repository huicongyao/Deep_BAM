//
// Created by yaohc on 2024/7/26.
//

#ifndef BAMCLASS_THREAD_SAFE_LIMITED_SIZE_QUEUE_H
#define BAMCLASS_THREAD_SAFE_LIMITED_SIZE_QUEUE_H
#include <mutex>
#include <queue>
#include <condition_variable>

namespace Yao {
// implement a thread safe, limit size queue
    template <typename T>
    class lock_baseed_queue {
    public:
        explicit lock_baseed_queue(size_t maxSize) : maxSize_(maxSize) {}
        lock_baseed_queue() : maxSize_(2) {}

        void push(const T& value) {
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [this] { return queue_.size() < maxSize_; });
            queue_.push_back(value);
            lock.unlock();
            cond_.notify_all();
        }

        // template method to construct elements in place
        template<class... Args>
        decltype(auto) emplace(Args&&... args) {
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [this] { return queue_.size() < maxSize_; });
            queue_.emplace(std::forward<Args>(args)...);
            lock.unlock();
            cond_.notify_all();
        }

        void pop() {
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [this] { return !queue_.empty(); });
            queue_.pop();
            lock.unlock();
            cond_.notify_all();
        }

        T& front() { // 可能是front 有问题
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [this] { return !queue_.empty(); });
            return queue_.front();
        }

        const T& front() const {
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [this] { return !queue_.empty(); });
            return queue_.front();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return queue_.empty();
        }

        size_t size() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return queue_.size();
        }

        size_t maxSize() const {
            return maxSize_;
        }

    private:
        std::queue<T> queue_;
        size_t maxSize_;
        mutable std::mutex mtx_;
        std::condition_variable cond_;
    };

} // Yao

#endif //BAMCLASS_THREAD_SAFE_LIMITED_SIZE_QUEUE_H
