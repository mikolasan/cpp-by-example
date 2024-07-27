# Strings

## Strip whitespaces from the right

```cpp
std::string line;
if (std::getline(file, line)) {
    line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), line.end());
}
```