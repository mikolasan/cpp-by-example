#pragma once

#include <iostream>
#include <memory>

#include "udp_sender.h"
#include "show_cached.hpp"

const uint16_t port = 6454;


class ShowUdpCached : public ShowCached {
public:
    ShowUdpCached(
        const std::string& codename,
        const std::string& filename,
        const std::string& hostname
    ) :
        ShowCached(codename, filename),
        hostname(hostname)
    {}

    void send() override {
        std::cout << "SEND " << codename << std::endl;
        udp_sender = std::make_unique<UdpSender>();
        udp->set_destination(hostname, port);
        if (hostname.ends_with("255")) {
            udp->enable_broadcast();
        }
        
        play(); // --> do_send
    }

protected:
    size_t do_send(const std::string_view& data) override {
        size_t sent_bytes = 0;
        if (!udp_sender) return sent_bytes;
        
        try {
            udp_sender->send(std::get<0>(*current_packet));
        } catch (const network_error& e) {
            std::cerr << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        return sent_bytes;
    }

    // void do_apply_config(const ryml::NodeRef & config) override {
    //     cache(filename)
    // }

protected:
    std::string hostname;
    std::unique_ptr<UdpSender> udp_sender;
};
