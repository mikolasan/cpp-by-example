#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex g_display_mutex;

void foo()
{
    std::thread::id this_id = std::this_thread::get_id();
    static std::atomic_int var{0};
    g_display_mutex.lock();
    var++;
    std::cout << this_id << ": " << var << std::endl;
    g_display_mutex.unlock();
}

int main(int argc, char const *argv[])
{
    std::thread t1(foo);
    std::thread t2(foo);
    std::thread t3(foo);
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
