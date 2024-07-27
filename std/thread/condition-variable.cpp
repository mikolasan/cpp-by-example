#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>
 
std::condition_variable cv;
std::mutex cv_m; // This mutex is used for three purposes:
                 // 1) to synchronize accesses to i
                 // 2) to synchronize accesses to std::cerr
                 // 3) for the condition variable cv
int i = 0;
 
void waits()
{
    std::unique_lock<std::mutex> lk(cv_m);
    std::cerr << "Waiting... \n";
    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    cv.wait_until(lk, now + std::chrono::seconds(2), []{ return i == 1; });
    std::cerr << "...finished waiting. i == 1\n";
}
 
void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 2;
        std::cerr << "Predicate is NOT true...\n";
    }
    cv.notify_all();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        std::cerr << "NOT Notifying...\n";
    }
    // cv.notify_all();
}
 
int main()
{
    std::thread t1(signals), t2(waits);
    t1.join(); 
    t2.join(); 
}