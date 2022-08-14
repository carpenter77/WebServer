#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H
#include <sys/time.h>

#include <condition_variable>
#include <deque>
#include <mutex>
template <class T>
class BlockQueue {
 public:
  explicit BlockQueue(size_t MAXCapacity = 1000);
  ~BlockQueue();
  void clear();
  bool full();
  void Close();
  size_t size();
  size_t capacity();
  T front();
  T back();
  void push_back(const T& item);
  void push_front(const T& item);
  void pop_fornt();
  bool empty();
  bool pop(T& item);
  bool pop(T& item, int timeout);
  void flush();

 private:
  std::deque<T> deq_;
  size_t capacity_;
  std::mutex mtx_;
  bool isClose_;
  std::condition_variable condConsumer_;
  std::condition_variable conProducer_;
};
#endif