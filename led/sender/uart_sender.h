#pragma once

#include <stdint.h>
#include <string>
#include <string_view>

#include <serial/serial.h>

class uart_error : public std::exception {
public:
    uart_error(const std::string& function_name, int code);
    const char* what() const noexcept override;
private:
    std::string message;
};

uint16_t crc16(const uint8_t* s, size_t len, uint16_t crcval = 0);

class UartSender {
public:
    UartSender();
    ~UartSender() noexcept(false);

    void set_destination(const std::string& device, uint64_t baudrate);
    
    template <typename VecType>
    size_t send(const VecType& data) {
        ssize_t n_bytes = _serial.write(reinterpret_cast<const uint8_t*>(data.data()), data.size());
        if (n_bytes == -1) {
            throw uart_error("write", errno);
        }
        _serial.flush();
        return n_bytes;
    }
    std::string_view read();

    template <typename VecType>
    static void add_crc(VecType& data, size_t msg_start_pos, size_t msg_length, size_t crc_pos) {
        uint16_t crc = crc16(reinterpret_cast<const uint8_t*>(data.data() + msg_start_pos), msg_length);
        data.at(crc_pos) = static_cast<uint8_t>((crc >> 8) & 0xFF);
        data.at(crc_pos + 1) = static_cast<uint8_t>(crc & 0xFF);
    }

private:
    serial::Serial _serial;
};