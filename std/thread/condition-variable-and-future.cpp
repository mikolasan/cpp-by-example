#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <thread>
 
template <typename R>
bool is_future_ready(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

class A {
public:
    int blocking_thing()
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        _cv.notify_all();
        return 42;
    }
public:
    std::condition_variable _cv;
};

class B {
public:
    int another_blocking_thing()
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        _cv.notify_all();
        return 123;
    }
public:
    std::condition_variable _cv;
};

int main()
{
    std::mutex cv_m;
    std::future<int> future;
    A a;
    B b;
    while (true) {
        if (!future.valid()) {
            future = std::async(std::launch::async, &A::blocking_thing, &a);
            std::unique_lock<std::mutex> lk(cv_m);
            a._cv.wait(lk, [&future]{ return is_future_ready(future); });
            std::cerr << "Wait is over\n";
        }

        if (!is_future_ready(future)) {
            std::cerr << "But the future is not ready!!\n";
            continue;
        }

        std::cerr << future.get() << "\n";
        break;
    }
}