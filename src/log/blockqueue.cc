#include "blockqueue.h"
template <class T>
void BlockQueue<T>::Close() {
  {
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
    isClose_ = true;
  }
  condConsumer_.notify_all();
  conProducer_.notify_all();
}
template <class T>
void BlockQueue<T>::flush() {
  condConsumer_.notify_one();
}

template <class T>
void BlockQueue<T>::clear() {
  std::lock_guard<std::mutex> locker(mtx_);
  deq_.clear();
}
template <class T>
T BlockQueue<T>::back() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.back();
}
template <class T>
T BlockQueue<T>::front() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.front();
}
template <class T>
size_t BlockQueue<T>::size() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.size();
}

template <class T>
size_t BlockQueue<T>::capacity() {
  std::lock_guard<std::mutex> locker(mtx_);
  return capacity_;
}

template <class T>
void BlockQueue<T>::push_back(const T& item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.size() >= capacity_) {
    conProducer_.wait(locker);
  }
  deq_.push_back(item);
  condConsumer_.notify_one();
}

template <class T>
void BlockQueue<T>::push_front(const T& item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.size() >= capacity_) {
    conProducer_.wait(locker);
  }
  deq_.push_front(item);
  condConsumer_.notify_one();
}
template <class T>
bool BlockQueue<T>::empty() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.empty();
}
template <class T>
bool BlockQueue<T>::pop(T& item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.empty()) {
    condConsumer_.wait(locker);
    if (isClose_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  conProducer_.notify_one();
  return true;
}

template <class T>
bool BlockQueue<T>::pop(T& item, int timeout) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.empty()) {
    if (condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) ==
        std::cv_status::timeout) {
      return false;
    }
    if (isClose_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  conProducer_.notify_one();
  return true;
}