#pragma once

#include <chrono>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <optional>
#include <pcap/pcap.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include <arpa/inet.h> // inet_ntop
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "ipaddress.h"
#include "show.h"

namespace fs = std::filesystem;

class PcapStream {
public:
    PcapStream() : dumper(NULL), has_packets(false) {}
    PcapStream(const std::string& fname) : dumper(NULL) {
        open(fname);
    }
    PcapStream(const fs::path& fname) : dumper(NULL) {
        open(fname.string());
    }
    ~PcapStream() {
        if (handle != NULL) {
            pcap_close(handle);
        }
        if (dumper != NULL) {
            int result = pcap_dump_flush(dumper);
            if (result == PCAP_ERROR) {
                std::cerr << pcap_geterr(handle) << std::endl;
            }
            pcap_dump_close(dumper);
        }
    }

    void open(const std::string& fname) {
        char errbuf[PCAP_ERRBUF_SIZE];
        handle = pcap_open_offline(fname.c_str(), errbuf);
        if (handle == NULL) {
            throw std::runtime_error(errbuf);            
        }
        has_packets = true;
    }

    size_t parsePacket() {
        int result = pcap_next_ex(handle, &header, &packet);
        if (result == PCAP_ERROR) {
            throw std::runtime_error(pcap_geterr(handle));
        } else if (result == PCAP_ERROR_BREAK) {
            has_packets = false;
            return 0;
        }

        const Clock::duration duration = std::chrono::microseconds(header->ts.tv_sec * 1000000 + header->ts.tv_usec);
        const TimePoint time_point(duration);
        if (last_packet_point) {
            wait_duration_usec = std::chrono::duration_cast<Duration>(time_point - last_packet_point.value());
        } else {
            wait_duration_usec = std::chrono::microseconds(0);
        }
        // std::cout << wait_duration_usec.count() << " us\n";
        last_packet_point = time_point;
        // Ethernet layer
        const struct ether_header* ethernet_header = reinterpret_cast<const struct ether_header*>(packet);
        if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IP) {
            // IP level
            const struct ip* ip_header = (struct ip*)(packet + sizeof(struct ether_header));
            char source_ip[INET_ADDRSTRLEN];
            char dest_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ip_header->ip_src), source_ip, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(ip_header->ip_dst), dest_ip, INET_ADDRSTRLEN);
            //remote_ip = dest_ip;
            if (ip_header->ip_p == IPPROTO_UDP) {
                // UDP level
                const struct udphdr* udp_header = (struct udphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
                u_int source_port = ntohs(udp_header->source);
                u_int dest_port = ntohs(udp_header->dest);
                remote_port = dest_port;
                const char* udp_payload = reinterpret_cast<const char*>(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));
                size_t length = header->len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));
                data = {udp_payload, length};
                return length;                
            }
        }
        return 0;
    }

    void output(const std::string& fname) {
        dumper = pcap_dump_open(handle, fname.c_str());
        if (dumper == NULL) {
            throw std::runtime_error(pcap_geterr(handle));
        }
    }

    void dumpPacket() {
        pcap_dump(reinterpret_cast<u_char*>(dumper), header, packet);
    }

    template<typename C>
    void read(C d, size_t size) {
        std::memcpy(d, data.data(), size);
    }

    template<typename C>
    void read_all(C d) {
        std::memcpy(d, data.data(), data.size());
    }

    void wait() const {
        std::this_thread::sleep_for(wait_duration_usec);
    }

    IPAddress remoteIP() const { return remote_ip; }
    uint16_t remotePort() const { return remote_port; }
    bool hasPackets() const { return has_packets; }
    Data getData() const { return data; }
    Duration getDuration() const { return wait_duration_usec; }

private:
    pcap_t* handle;
    pcap_dumper_t* dumper;
    struct pcap_pkthdr* header;
    const u_char* packet;
    IPAddress remote_ip;
    uint16_t remote_port;
    Data data;
    bool has_packets;
    std::optional<TimePoint> last_packet_point;
    Duration wait_duration_usec;
};
