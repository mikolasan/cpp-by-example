# Common Operations


## Get content as string

```cpp
std::ifstream in(filename, std::ios::in | std::ios::binary);
if (!in) {
    std::cerr << "could not open " << filename << std::endl;
    return 1;
}
std::ostringstream contents;
contents << in.rdbuf();
std::string data = contents.str();
```

## Get binary content as buffer

```cpp
bool load_file(const char* filename, unsigned char** buf, int& size)
{
    if (std::ifstream file_stream{filename, std::ios::binary | std::ios::ate}) {
        size = file_stream.tellg();
        std::string str(size, '\0');
        file_stream.seekg(0);
        if (file_stream.read(&str[0], size)) {
            *buf = static_cast<unsigned char*>(malloc(size));
            str.copy(reinterpret_cast<char*>(*buf), size);
            return true;
        }
        std::cerr << "Reading of file " << filename << " failed" << std::endl;
        return false;
    }
    std::cerr << "Can't open file " << filename << std::endl;
    return false;
}
```
    
## Get file size

