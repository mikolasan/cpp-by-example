# Files

## Get file content

```cpp
std::ifstream in(filename, std::ios::in | std::ios::binary);
if (!in) {
    std::cerr << "could not open " << filename << std::endl;
    return 1;
}
std::ostringstream contents;
contents << in.rdbuf();
std::string ogg_data = contents.str();
```