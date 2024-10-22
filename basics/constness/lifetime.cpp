#include <new>
#include <cstddef>

template<typename T>
const T* start_lifetime_as(const void* p) noexcept {
    const auto mp = const_cast<void*>(p);
    const auto bytes = new(mp) std::byte[sizeof(T)];
    const auto ptr = reinterpret_cast<const T*>(bytes);
    (void)*ptr; // ?
    return ptr;
}

int main() {
    const int x = 42;
    const int* px = &x;
    const unsigned int* z = start_lifetime_as<unsigned int>(reinterpret_cast<const void *>(px));
    return *z;
}