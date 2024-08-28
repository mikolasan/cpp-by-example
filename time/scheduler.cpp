#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <thread>

class Task {
public:
  virtual ~Task() = default;
  virtual void run() = 0;
};

class SimpleTask : public Task {
  void run() {
    using std::chrono::operator""ms;
    std::this_thread::sleep_for(100ms);
  }
};

class Scheduler {
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::milliseconds;

public:
  Scheduler() {}
  ~Scheduler() {}

  void main_loop() {
    bool running = true;
    TimePoint prev_now = Clock::now();
    uint64_t step = 0;
    using std::chrono::operator""ms;
    auto step_delay = 1000ms;

    while (running) {
      TimePoint now = Clock::now();
      auto drift = std::chrono::duration_cast<Duration>(
              now - prev_now - step_delay).count();
      std::cout << "-- drift "  << drift << "ms " << std::endl;
      prev_now = now;

      run_tasks();

      TimePoint after_tasks = Clock::now();
      auto task_time = std::chrono::duration_cast<Duration>(
              after_tasks - now).count();
      std::cout << "-- task time "  << task_time << "ms " << std::endl;
      
      ++step;

      auto expected_time = now + step_delay;
      std::this_thread::sleep_until(expected_time);
    }
  }

  void add_task(const std::shared_ptr<Task>& task) {
    tasks.emplace_back(task);
  }

private:
  void run_tasks() {
    for (const auto& task : tasks) {
      task->run();
    }
  }

  std::list<std::shared_ptr<Task>> tasks;
};

int main(int argc, char const *argv[])
{
  std::unique_ptr<Scheduler> scheduler = std::make_unique<Scheduler>();
  scheduler->add_task(std::make_shared<SimpleTask>());
  scheduler->main_loop();

  return 0;
}
