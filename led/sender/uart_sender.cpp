#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <string_view>

#include "uart_sender.h"

class hexostream {
public:
    template<typename T>
    std::ostream& operator<<(T to_print) {
        std::cout << std::setfill('0') << std::setw(2) << std::uppercase << std::hex
            << to_print;
        return std::cout;
    }
};

hexostream cout_hex;

template<typename T>
int pretty_byte(T b) {
    return (static_cast<int>(b) & 0xff);
}

// cout_hex << pretty_byte(c) << " ";

uint16_t crc16(const uint8_t* s, size_t len, uint16_t crcval) {
  uint32_t c, q;
  for (; len; len--) {
    c = *s++;
    q = (crcval ^ c) & 017;
    crcval = (crcval >> 4) ^ (q * 010201);
    q = (crcval ^ (c >> 4)) & 017;
    crcval = (crcval >> 4) ^ (q * 010201);
  }
  return (crcval);
}


UartSender::UartSender() {

}

UartSender::~UartSender() noexcept(false) {
    _serial.close();
}

void UartSender::set_destination(const std::string& port, uint64_t baudrate) {
    _serial.setPort(port);
    _serial.setBaudrate(baudrate);
    serial::Timeout timeout = serial::Timeout::simpleTimeout(10);
    _serial.setTimeout(timeout);
    _serial.setBytesize(serial::bytesize_t::eightbits);
    _serial.setParity(serial::parity_t::parity_mark);
    _serial.setStopbits(serial::stopbits_t::stopbits_one);
    _serial.setFlowcontrol(serial::flowcontrol_t::flowcontrol_none);

    _serial.open();
}

std::string_view UartSender::read() {
  size_t n_bytes_available = _serial.available();
  // std::cout << "n bytes available " << n_bytes_available << std::endl;
  std::vector<uint8_t> buffer;
  auto n_bytes = _serial.read(buffer, n_bytes_available);
  // for (const auto c : buffer) {
  //   cout_hex << pretty_byte(c) << " ";
  // }
  // std::cout << std::endl;

  char* tmp = (char*)std::malloc(buffer.size());
  std::memcpy(tmp, buffer.data(), buffer.size());
  return std::string_view(tmp, buffer.size());
}