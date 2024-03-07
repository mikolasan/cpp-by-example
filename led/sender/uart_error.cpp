#include <stdexcept>
#include <string>
#include <string.h> // strerror

#include "uart_sender.h"

uart_error::uart_error(const std::string& function_name, int code) {
    message = "[";
    message += function_name;
    message += "]: ";
    message += strerror(code);
}

const char* uart_error::what() const noexcept {
    return message.c_str();
}
