#pragma once

#include <cstdint>
#include <cstring>

class IPAddress {
private:
    union {
        uint8_t bytes[4];  // IPv4 address
        uint32_t dword;
    } _address;

public:
    // Constructors
    IPAddress() { _address.dword = 0; }
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
        _address.bytes[0] = first_octet;
        _address.bytes[1] = second_octet;
        _address.bytes[2] = third_octet;
        _address.bytes[3] = fourth_octet;
    }
    IPAddress(uint32_t address) { _address.dword = address; }
    IPAddress(const uint8_t *address) { memcpy(_address.bytes, address, sizeof(_address.bytes)); }

    bool operator==(const IPAddress& addr) const { return _address.dword == addr._address.dword; };
    bool operator!=(const IPAddress& addr) const { return _address.dword != addr._address.dword; };
    
    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](int index) const { return _address.bytes[index]; };
    uint8_t& operator[](int index) { return _address.bytes[index]; };
};