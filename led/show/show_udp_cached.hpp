#pragma once

#include <iostream>
#include <memory>

#include "Artnet.h"
#include "udp_sender.h"
#include "show_cached.hpp"
#include "test_stream.hpp"
#include "sequential_stream.hpp"

class ShowUdpCached : public ShowCached {
public:
    ShowUdpCached(
        const std::string& codename,
        const std::string& filename
    ) :
        ShowCached(codename, filename),
        last_sequence(0)
    {
        artnet.begin();
    }

    void set_sender(const std::shared_ptr<UdpSender>& sender) {
        SequentialStream& s = artnet.getStream();
        s.udp_sender = sender;
    }

    void send() override {
        play(); // --> do_send
    }

    std::string send_blocking() {
        send();
        return codename;
    }

    void send_async() {
        std::thread t([this](){
            send();
        });
        t.detach();
    }

protected:
    size_t do_send(const std::string_view& data) override {
        size_t sent_bytes = 0;

        std::string data_copy(data.data(), data.size());
        
        artnet.getStream().open(data_copy);
        auto code = artnet.parse();
        uint8_t universe = artnet.universe();
        
        if (code == arx::artnet::OpCode::Dmx) {
            uint8_t sequence = artnet.Receiver_<SequentialStream>::sequence();
            if (sequence != last_sequence) {
                last_sequence = sequence;
                ++new_sequence;
                artnet.set_sequence(new_sequence);
            }
            artnet.Sender_<SequentialStream>::send(
                hostname,
                static_cast<uint32_t>(universe),
                artnet.Receiver_<SequentialStream>::data(),
                artnet.Receiver_<SequentialStream>::size());
        } else {
            if (auto p = udp_sender.lock()) {
                try {
                    sent_bytes = p->send(data);
                } catch (const network_error& e) {
                    std::cerr << e.what() << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                }
            } else {
                std::cout << "\tdo_send() is unable to lock weak_ptr<>\n";
            }
        }

        return sent_bytes;
    }

public:
    std::string hostname;
    std::weak_ptr<UdpSender> udp_sender;
protected:
    uint8_t last_sequence;
    static uint8_t new_sequence;
private:
    using ArtnetManager = arx::artnet::Manager<SequentialStream>;
    ArtnetManager artnet;
    // using ArtnetReceiver = arx::artnet::Receiver<TestStream>;
    // ArtnetReceiver artnet_parser;
    // using ArtnetSender = arx::artnet::Sender<SequentialStream>;
    // ArtnetSender artnet_sender;
};
