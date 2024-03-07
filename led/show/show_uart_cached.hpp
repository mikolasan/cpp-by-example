#pragma once

#include <atomic>
#include <iostream>
#include <iterator> // distance
#include <list>
#include <tuple>

#include "Artnet.h"
#include "ipaddress.h"
#include "uart_sender.h"
#include "show_cached.hpp"

const uint64_t baudrate = 115200;


class TestStream {
public:
    TestStream() {}

    void open(const std::string& data) {
        this->data = data;
    }
    bool hasPackets() {
        return data.has_value();
    }
    size_t parsePacket() {
        if (data) {
            return data.value().size();
        } else {
            return 0;
        }
    }
    
    template<typename C>
    void read(C d, size_t size) {
        if (!data) return;
        std::memcpy(d, data.value().data(), size);
        data.reset();
    }

    IPAddress remoteIP() const { return IPAddress(0U); }
    uint16_t remotePort() const { return 0; }
    
private:
    std::optional<std::string> data;
};

using ArtnetSender = arx::artnet::Sender<TestStream>;


class ShowUartCached : public ShowCached {
public:
    ShowUartCached(
        const std::string& codename,
        const std::string& filename
    ) :
        ShowCached(codename, filename)
    {}

    ShowUartCached(
        const std::string& codename,
        const std::string& filename,
        const std::string& port
    ) :
        ShowCached(codename, filename),
        port(port)
    {}

    void send() override {
        std::cout << "SEND " << codename << std::endl;
        uart_sender = std::make_unique<UartSender>();
        uart_sender->set_destination(port, baudrate);

        play();
    }

protected:
    size_t do_send(const std::string_view& data) override {
        size_t sent_bytes = 0;
        if (!uart_sender) return sent_bytes;
        
        using ArtnetReceiver = arx::artnet::Receiver<TestStream>;
        ArtnetReceiver artnet;

        std::string data_copy(data.data(), data.size());
        artnet.open(data_copy);
        artnet.subscribe([this, &sent_bytes] (const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
            // TODO: testing 1 unsiverse
            if (universe != 0) return;

            const char start_seq[] = "Woop woop woop";
            size_t start_seq_length = strlen(start_seq);
            const size_t length_value_length = 2;
            const size_t crc_length = 2;
            size_t msg_length = size;

            std::vector<uint8_t> msg(start_seq_length + length_value_length + msg_length + crc_length);
            // start
            std::copy(std::begin(start_seq), std::end(start_seq), msg.begin());
            // length
            msg[start_seq_length] = (uint8_t)((msg_length >> 8) & 0xFF);
            msg[start_seq_length + 1] = (uint8_t)(msg_length & 0xFF);
            // message
            for (int i = 0; i < size; ++ i) {
                msg[start_seq_length + length_value_length + i] = data[i];
            }
            // CRC
            UartSender::add_crc(
                msg, 
                start_seq_length + length_value_length,
                msg_length,
                start_seq_length + length_value_length + msg_length
            );

            sent_bytes += uart_sender->send(msg);
            // std::cout << std::dec << sent_bytes << " bytes sent" << std::endl;

            // std::cout << "UNIVERSE " << universe
            //     << " size " << std::dec << (size / 3) << std::endl;
            // for (int i = 0; i < size / 3; ++ i) {
            //     std::cout << "[ ";
            //     std::cout << static_cast<int>(data[3*i]) << " ";
            //     std::cout << static_cast<int>(data[3*i + 1]) << " ";
            //     std::cout << static_cast<int>(data[3*i + 2]) << " ";
            //     std::cout << "] ";
            // }
            // std::cout << std::endl;

        });
        do {
            artnet.parse();
        } while (artnet.hasPackets());

        bool ready = false;
        while (!ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::string_view response = uart_sender->read();
            std::string response_copy(response.data(), response.size());
            if (response_copy.compare("ready")) {
                ready = true;
                // std::cout << "ready" << std::endl;
            } else {
                std::cout << "something else" << std::endl;
            }
        }

        return sent_bytes;
    }

private:
    std::string port;
    std::unique_ptr<UartSender> uart_sender;
};
