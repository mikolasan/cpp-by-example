#include <iostream>
#include <thread>

using namespace std;

void doSomething() { cout << "Inside doSomething " << endl; }
void doSomethingElse() { cout << "Inside doSomethingElse " << endl; }

int main(void)
{
    // Using LAMBDA expressions to call the functions.
    thread my_thread([](){ doSomething(); doSomethingElse(); });
    //my_thread.join(); ---------------> 1

    return 0;
}