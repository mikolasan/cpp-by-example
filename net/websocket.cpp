#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "easywsclient.hpp"
#include "json.hpp"
#include "queue.hpp"


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
