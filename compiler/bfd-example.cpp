#include <iostream>
#include <memory>

#define PACKAGE "bfd"
#define PACKAGE_VERSION "x.y.z"
#include <bfd.h>
#include <dlfcn.h>

class WindowsLibraryLoader {
public:
    WindowsLibraryLoader(const char* filename)
        : filename_(filename), bfd_(nullptr), entry_point_(nullptr), lib_handle_(nullptr)
    {
        bfd_ = bfd_openr(filename_, NULL);
        if (bfd_ == nullptr) {
            std::cerr << "Could not open " << filename_ << std::endl;
            throw std::runtime_error("Failed to open library");
        }
        
        if (!bfd_check_format(bfd_, bfd_object)) {
            std::cerr << filename_ << " is not a valid object file" << std::endl;
            throw std::runtime_error("Invalid object file");
        }

        // Get the address of the library entry point
        entry_point_ = dlsym(RTLD_DEFAULT, "WinMain");
        if (entry_point_ == nullptr) {
            std::cerr << "Could not find WinMain in " << filename_ << std::endl;
            throw std::runtime_error("Failed to find entry point");
        }

        // Load the library
        lib_handle_ = dlopen(filename_, RTLD_LAZY);
        if (lib_handle_ == nullptr) {
            std::cerr << "Could not load " << filename_ << ": " << dlerror() << std::endl;
            throw std::runtime_error("Failed to load library");
        }

        std::cout << "Library loaded successfully!" << std::endl;
    }

    ~WindowsLibraryLoader() {
        dlclose(lib_handle_);
        bfd_close(bfd_);
    }

    template<typename F>
    F callMyFunction(const char* func_name) {
        // Call a function in the library
        F my_func = reinterpret_cast<F>(dlsym(lib_handle_, func_name));
        if (my_func == nullptr) {
            std::cerr << "Could not find '" << func_name << "'" << filename_ << ": " << dlerror() << std::endl;
            throw std::runtime_error("Failed to find function");
        }
        return my_func;
    }

private:
    const char* filename_;
    bfd* bfd_;
    void* entry_point_;
    void* lib_handle_;
};

int main(int argc, char const *argv[]) {
    try {
        std::unique_ptr<WindowsLibraryLoader> loader(new WindowsLibraryLoader(argv[1]));
        typedef int (*my_func_t)();
        std::cout << "my_func returned " << loader->callMyFunction<my_func_t>("my_func") << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
