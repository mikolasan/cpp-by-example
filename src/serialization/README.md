# Serialization

## JSON

```cpp
{{#include ../../serialization/json/json.cpp}}
```

Another example, how to prepare a map for a printable string that can be parsed by another application

```cpp
std::string variablesToJson(const std::map<std::string, std::string>& map)
{
    using json = nlohmann::json;
    json jsonObject = json::object();
    for (const auto& [name, value] : map) {
        jsonObject[name] = value;
    }
    return " '" + jsonObject.dump() + "'";
}
```

## YAML

## Binary