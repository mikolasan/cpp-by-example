#include "easywsclient.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <deque>
#include <thread>
#include <chrono>
#include <atomic>

// a simple, thread-safe queue with (mostly) non-blocking reads and writes
namespace non_blocking {
template <class T>
class Queue {
    mutable std::mutex m;
    std::deque<T> data;
public:
    void push(T const &input) { 
        std::lock_guard<std::mutex> L(m);
        data.push_back(input);
    }

    bool pop(T &output) {
        std::lock_guard<std::mutex> L(m);
        if (data.empty())
            return false;
        output = data.front();
        data.pop_front();
        return true;
    }
};
}

// eastwsclient isn't thread safe, so this is a really simple
// thread-safe wrapper for it.
class Ws {
    std::thread runner;
    non_blocking::Queue<std::string> outgoing;
    non_blocking::Queue<std::string> incoming;
    std::atomic<bool> running { true };

public:
    void send(std::string const &s) { outgoing.push(s); }
    bool recv(std::string &s) { return incoming.pop(s); }

    Ws(std::string url) {
        using easywsclient::WebSocket;

        runner = std::thread([=] {
            std::unique_ptr<WebSocket> ws(WebSocket::from_url(url));
            if (!ws) {
                std::cerr << "no ws" << std::endl;
                close();
                return;
            }
            while (running) {
                if (ws->getReadyState() == WebSocket::CLOSED)
                    break;
                std::string data;
                if (outgoing.pop(data))
                    ws->send(data);
                ws->poll();
                ws->dispatch([&](const std::string & message) {
                    incoming.push(message);
                });
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            ws->close();
            ws->poll();
        });
    }
    void close() { running = false; }
    ~Ws() { if (runner.joinable()) runner.join(); }
};

int main(int argc, char const *argv[])
{
    Ws socket("ws://localhost:9000/api/v3/ws");

    std::atomic<bool> run{true};
    auto receiver = std::thread([&] {
        std::string s;
        while (run) {
            if (socket.recv(s))
                std::cout << s << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::string line;
    while (std::getline(std::cin, line)) {
        socket.send(line);
    }
    run = false;
    receiver.join();
    socket.close();

    return 0;
}
