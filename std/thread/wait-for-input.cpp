// #include <chrono>
// #include <cstdio>
// #include <iostream>
// #include <thread>

// int main(int argc, char const *argv[])
// {
//     using namespace std::chrono_literals;

// 	std::thread t([]{
// 		while(true) {
// 			std::this_thread::sleep_for(250ms);
//             int c = std::getchar();
//             std::cout << c << std::endl;
// 		}
// 	});
// 	t.join();

//     return 0;
// }

#include <iostream>
#include <ncurses.h>
#include <future>

char get_keyboard_input();

int main(int argc, char *argv[])
{
    initscr();
    raw();
    // noecho();
    keypad(stdscr,true);

    auto f = std::async(std::launch::async, get_keyboard_input);
    while (f.wait_for(std::chrono::milliseconds(20)) != std::future_status::ready)
    {
        // do some work
    }

    endwin();
    std::cout << "returned: " << f.get() << std::endl;
    return 0;
}

char get_keyboard_input()
{
    char input = '0';
    while(input != 'q')
    {
        input = getch();
    }
    return input;
}