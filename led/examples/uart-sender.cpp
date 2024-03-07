#include <chrono>
#include <iostream>
#include <string>
#include <string_view>
#include <string.h>
#include <thread>

#include <ncurses.h>
#undef timeout // this collides with the timeout from the serial library

#include "uart_sender.h"


int main(int argc, char const *argv[])
{
    std::string device{"/dev/ttyACM0"};
    const uint64_t baudrate = 19200;

    const char start_seq[] = "Woop woop woop";
    size_t start_seq_length = strlen(start_seq);
    const size_t crc_length = 2;
    size_t msg_length = 5;
    std::vector<uint8_t> msg(start_seq_length + 1 + msg_length + crc_length);

    std::copy(std::begin(start_seq), std::end(start_seq), msg.begin());

    msg[start_seq_length] = msg_length;

    msg[start_seq_length + 1] = 'h';
    msg[start_seq_length + 2] = 'e';
    msg[start_seq_length + 3] = 'l';
    msg[start_seq_length + 4] = 'l';
    msg[start_seq_length + 5] = 'o';

    std::cout << "crc " << std::hex << crc16(
            reinterpret_cast<const uint8_t*>(msg.data() + start_seq_length + 1),
            msg_length)
        << std::endl;
    UartSender::add_crc(
        msg, 
        start_seq_length + 1,
        msg_length,
        start_seq_length + 1 + msg_length
    );

    // Initialize ncurses
    // initscr();
    // cbreak();  // Line buffering disabled, Pass on everything
    // // noecho();  // Don't echo() while we do getch
    // // keypad(stdscr, TRUE);  // Enable special keys
    // refresh();

    UartSender sender;
    sender.set_destination(device, baudrate);
    bool key_pressed = false;
    while (!key_pressed) {
        int n_bytes = sender.send(msg);
        std::cout << std::dec << n_bytes << " bytes sent" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // int ch = getch();
        // if (ch == 'c' || ch == 'C') {
        //     key_pressed = true;
        // }

        std::string_view response = sender.read();
        if (!response.empty()) {
            std::cout << std::dec;
            for (const auto c : response) {
                std::cout << (char)c;
            }
            std::cout << std::endl;
        }
    }

    // Cleanup ncurses
    // endwin();
    return 0;
}