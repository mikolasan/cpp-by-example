#include <iostream>
#include <chrono>
#include <thread>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::milliseconds;

int main(int argc, char const *argv[])
{
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

    // do the thing
    std::this_thread::sleep_for(100ms);

    TimePoint after_tasks = Clock::now();
    auto task_time = std::chrono::duration_cast<Duration>(
            after_tasks - now).count();
    std::cout << "-- task time "  << task_time << "ms " << std::endl;
    
    ++step;

    auto expected_time = now + step_delay;
    std::this_thread::sleep_until(expected_time);
  }

  return 0;
}
