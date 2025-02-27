#include <pthread.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <cstdlib>
#include <memory>
#include <print>

constexpr size_t WORK_QUEUE_CAPACITY = 16;

struct MatmulArg {
  float* ma;
  float* mb;
  float* mz;
  size_t n;
  size_t k;
  size_t m;
};

void matmul(void* arg, [[maybe_unused]] void* result) {
  auto arg_ = (MatmulArg*)(arg);
  for (auto ni = 0; ni < arg_->n; ++ni) {
    for (auto mi = 0; mi < arg_->m; ++mi) {
      auto zi = ni * arg_->m + mi;
      arg_->mz[zi] = 0.0;
      auto ai_base = ni * arg_->k;
      for (auto ki = 0; ki < arg_->k; ++ki) {
        auto ai = ai_base + ki;
        auto bi = ki * arg_->m + mi;
        arg_->mz[zi] += arg_->ma[ai] * arg_->mb[bi];
      }
    }
  }
}

void simple_sleep(void* arg, [[maybe_unused]] void* result) {
  auto arg_ = (float*)(arg);
  SPDLOG_INFO("simple sleep for {} seconds", *arg_);
  sleep(*arg_);
}

enum Cmd { NORMAL = 0, EXIT = 1, MAX = 2 };

struct Task {
  Cmd cmd;
  void (*func)(void*, void*);
  void* arg;
  void* result;
};

template <size_t CAPACITY>
class WorkQueue {
 private:
  Task tasks_[CAPACITY];
  size_t size_;
  size_t read_ptr_;
  size_t write_ptr_;

  pthread_mutex_t mutex_;
  pthread_cond_t cond_;

 public:
  WorkQueue()
      : tasks_(), size_(0), read_ptr_(0), write_ptr_(0), mutex_(), cond_() {}

  int push(Cmd cmd, void (*func)(void*, void*), void* arg, void* result) {
    if (size_ == CAPACITY) {
      return ENOBUFS;
    }
    pthread_mutex_lock(&mutex_);
    tasks_[write_ptr_].cmd = cmd;
    tasks_[write_ptr_].func = func;
    tasks_[write_ptr_].arg = arg;
    tasks_[write_ptr_].result = result;
    write_ptr_ = (write_ptr_ + 1) % CAPACITY;
    size_ += 1;
    pthread_cond_signal(&cond_);
    pthread_mutex_unlock(&mutex_);
    return 0;
  }
  int pop(Task* task) {
    if (size_ == 0) {
      return ENODATA;
    }
    while (size_ == 0) {
      pthread_cond_wait(&cond_, &mutex_);
    }
    memcpy(task, &tasks_[read_ptr_], sizeof(Task));
    read_ptr_ = (read_ptr_ + 1) % CAPACITY;
    size_ -= 1;
    pthread_mutex_unlock(&mutex_);
    return 0;
  }
};

struct WorkerArg {
  size_t uid;
  WorkQueue<WORK_QUEUE_CAPACITY>* q;
};

void* worker_routine(void* arg) {
  auto arg_ = (WorkerArg*)(arg);
  SPDLOG_INFO("Worker {} starts", arg_->uid);
  while (true) {
    Task t;
    arg_->q->pop(&t);
    if (t.cmd == EXIT) {
      break;
    }
    if (t.cmd == NORMAL) {
      t.func(t.arg, t.result);
    }
  }
  SPDLOG_INFO("Worker {} finishes", arg_->uid);
  return nullptr;
}

int main() {
  SPDLOG_INFO("start");
  // constexpr size_t n = 2;
  // constexpr size_t m = 3;
  // constexpr size_t k = 4;
  // float ma[n * k] = {0};
  // float mb[k * m] = {0};
  // float mz[n * m] = {0};

  // for (auto i = 0; i < (n * k); ++i) {
  //   ma[i] = i * 1.0;
  // }
  // for (auto i = 0; i < (k * m); ++i) {
  //   mb[i] = i * 1.0;
  // }

  // MatmulArg marg;

  WorkQueue<WORK_QUEUE_CAPACITY> work_queue;
  float f1p0 = 1.0;
  float f2p0 = 2.0;
  float f3p0 = 3.0;
  work_queue.push(NORMAL, simple_sleep, &f1p0, nullptr);
  work_queue.push(NORMAL, simple_sleep, &f1p0, nullptr);
  work_queue.push(NORMAL, simple_sleep, &f2p0, nullptr);
  work_queue.push(NORMAL, simple_sleep, &f2p0, nullptr);
  work_queue.push(NORMAL, simple_sleep, &f3p0, nullptr);
  work_queue.push(NORMAL, simple_sleep, &f3p0, nullptr);
  work_queue.push(EXIT, nullptr, nullptr, nullptr);
  work_queue.push(EXIT, nullptr, nullptr, nullptr);
  work_queue.push(EXIT, nullptr, nullptr, nullptr);
  work_queue.push(EXIT, nullptr, nullptr, nullptr);

  constexpr size_t NUM_WORKER = 4;
  pthread_t tids[NUM_WORKER];
  WorkerArg args[NUM_WORKER];

  for (auto i = 0; i < NUM_WORKER; ++i) {
    args[i].uid = i;
    args[i].q = &work_queue;
    pthread_create(&tids[i], NULL, worker_routine, &args[i]);
  }

  for (auto i = 0; i < NUM_WORKER; ++i) {
    pthread_join(tids[i], nullptr);
  }
  SPDLOG_INFO("return");
  return 0;
}