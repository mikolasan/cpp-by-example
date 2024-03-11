#include <stdexcept>
#include <string.h> // strerror

#include "udp_sender.h"

network_error::network_error(const std::string& function_name, int code) {
    message = "[";
    message += function_name;
    message += "]: ";
    message += strerror(code);
}

const char* network_error::what() const noexcept {
    return message.c_str();
}
