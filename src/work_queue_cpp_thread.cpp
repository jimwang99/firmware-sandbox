#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdlib>
#include <functional>
#include <thread>
#include <vector>

static size_t worker_uid = 0;

enum class Cmd {
  NORMAL,
  EXIT,
};

struct Task {
  Cmd cmd;
  std::function<void(void*, void*)> func;
  void* arg;
  void* result;
};

class WorkQueue {
 private:
  std::queue<Task> q_;
  WorkQueue() : q_() {}

  std::mutex q_mutex_;
  std::condition_variable q_cond_;

 public:
  WorkQueue(const WorkQueue&) = delete;
  WorkQueue& operator=(const WorkQueue&) = delete;
  static WorkQueue& get_instance() {
    static WorkQueue instance;
    return instance;
  }

  void push(std::function<void(void*, void*)> func, void* arg, void* result,
            Cmd cmd = Cmd::NORMAL) {
    Task t({.cmd = cmd, .func = func, .arg = arg, .result = result});
    {
      std::scoped_lock lck(q_mutex_);
      q_.push(t);
      q_cond_.notify_one();
    }
  }
  Task pop() {
    std::unique_lock lck(q_mutex_);
    q_cond_.wait(lck, [&] { return !q_.empty(); });
    auto t = q_.front();
    q_.pop();
    lck.unlock();
    return t;
  }
};

class Worker {
 private:
  size_t uid_;
  WorkQueue& work_queue_;
  std::thread thread_;

  void run() {
    SPDLOG_INFO("Worker {} starts to run", uid_);
    while (true) {
      auto t = work_queue_.pop();
      SPDLOG_INFO("Worker {} gets task", uid_);
      if (t.cmd == Cmd::EXIT) {
        break;
      }
      if (t.cmd == Cmd::NORMAL) {
        t.func(t.arg, t.result);
      }
    }
    SPDLOG_INFO("Worker {} finishes", uid_);
  }

 public:
  Worker()
      : uid_(worker_uid++),
        work_queue_(WorkQueue::get_instance()),
        thread_(&Worker::run, this) {
    SPDLOG_INFO("Worker {} is created", uid_);
  }
  ~Worker() { SPDLOG_INFO("Worker {} is destroyed", uid_); }
  void join() { thread_.join(); }
};

void simple_sleep(void* arg, [[maybe_unused]] void* result) {
  auto second = *reinterpret_cast<float*>(arg);
  SPDLOG_INFO("task: sleep for {} seconds", second);
  std::chrono::duration<float> duration(second);
  std::this_thread::sleep_for(duration);
}

int main() {
  SPDLOG_INFO("start");

  constexpr size_t NUM_WORKER = 4;
  std::array<Worker, NUM_WORKER> workers;

  // tasks
  WorkQueue& work_queue = WorkQueue::get_instance();
  float f1p0 = 1.0;
  float f2p0 = 2.0;
  float f3p0 = 3.0;
  work_queue.push(simple_sleep, &f1p0, nullptr);
  work_queue.push(simple_sleep, &f1p0, nullptr);
  work_queue.push(simple_sleep, &f2p0, nullptr);
  work_queue.push(simple_sleep, &f2p0, nullptr);
  work_queue.push(simple_sleep, &f3p0, nullptr);
  work_queue.push(simple_sleep, &f3p0, nullptr);
  for (auto i = 0; i < NUM_WORKER; ++i) {
    work_queue.push(nullptr, nullptr, nullptr, Cmd::EXIT);
  }

  // wait
  for (auto i = 0; i < NUM_WORKER; ++i) {
    workers[i].join();
  }
  SPDLOG_INFO("return");
  return 0;
}