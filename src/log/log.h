#ifndef LOG_H
#define LOG_H
#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <memory>
#include <string>
#include <thread>

#include "blockqueue.h"
#include "buffer.h"
class Log {
 public:
  void init(int32_t level, const char* path = "./log",
            const char* suffix = ".log", int32_t maxQueueCapacity = 1024);
  static std::shared_ptr<Log> Instance();
  static void FlushLogThread();
  void write(int level, const char* format, ...);
  void flush();
  int GetLevel();
  void SetLevel(int32_t level);
  bool IsOpen() { return isOpen_; }

 private:
  Log();
  void AppendLogLevelTitle_(int level);
  virtual ~Log();
  void AsyncWrite();

 private:
  static const int LOG_PATH_LEN = 256;
  static const int LOG_MAX_LEN = 256;
  static const int MAX_LENS = 50000;
  const char* path_;
  const char* suffix_;
  int MAX_LENS_;
  int linecount_;
  int toDay_;
  bool isOpen_ :

      Buffer buff_;
  int level_;
  bool isAync_;

  FILE* fp_;
  std::unique_ptr<BlockQueue<std::string>> deque_;
  std::unique_ptr<std::thread> writeThreads_;
  std::mutex mtx_;
};
#endif