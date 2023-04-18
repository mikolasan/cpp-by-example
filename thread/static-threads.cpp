#include <chrono>
#include <iostream>
#include <thread>

static int result = 0;

void bar() {
    ++result;
}

void foo() {
    static std::thread t(bar);
    t.detach();
}

int main(int argc, char const *argv[])
{
    foo();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "result: " << result << std::endl;
    // terminate called without an active exception
    return 0;
}
