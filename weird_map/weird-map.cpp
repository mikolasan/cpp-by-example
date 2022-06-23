#include <iostream>
#include <string>
#include <unordered_map>

class InterestingData {
public:
    InterestingData() = delete;
    InterestingData(int id) : _id(id) {
        std::cout << "constructor" << _id << std::endl;
    }
    ~InterestingData() {
        std::cout << "destructor" << _id << std::endl;
    }
    InterestingData(const InterestingData& other) : _id(other._id) {
        std::cout << "copy constructor" << _id << std::endl;
    }
    InterestingData& operator=(const InterestingData& other) {
        _id = other._id;
        std::cout << "copy assignment" << _id << std::endl;
        return *this;
    }

    int _id;
};

int main(int argc, char const *argv[])
{
    std::unordered_map<std::string, InterestingData> map;
    InterestingData data1(1), data2(2);
    map["test"] = data1;
    map["test"] = data2;
    map.clear();
    std::cout << "BOOM!" << std::endl;
    return 0;
}
