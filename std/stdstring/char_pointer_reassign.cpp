#include <stdint.h>
#include <string.h>

#include <string>

char* in_file_name = 0;
char* out_file_name = 0;

int main(int argc, char const *argv[])
{
    uint32_t size = 16;
    {
        const char file_name[] = "input";
        size_t name_size = strlen(file_name) + 1;
        in_file_name = (char*)malloc(name_size);
        strncpy(in_file_name, file_name, name_size);
        // in_file_name = std::string("input").data();
    }
    {
        FILE * f = fopen(in_file_name, "w+b");
        if (f) {
            uint32_t data[1] = { 0 };
            for (uint32_t n = 0; n < size; n++) {
                data[0] = n;
                fwrite(data, 4, 1, f);
            }
            fclose(f);
        } else {
            return 2;
        }
    }
    
    {
        char file_name[] = "output";
        size_t name_size = strlen(file_name) + 1;
        out_file_name = (char*)malloc(name_size);
        strncpy(out_file_name, file_name, name_size);
        // out_file_name = std::string("input").data();
    }
    {
        FILE * f = fopen(out_file_name, "rb");
        if (f) {
            uint32_t data[1] = { 0 };
            for (uint32_t n = 0; n < size; n++) {
                fread(data, 4, 1, f);
                if (data[0] != n) {
                    return 1;
                }
            }
            fclose(f);
        } else {
            return 2;
        }
    }
    return 0;
}
