#include <iostream>
#include <string>
#include <unordered_map>

class InterestingData {
public:
    InterestingData() {} // Ok, fine, here you go
    InterestingData(int* id) : _id(id) {
        std::cout << "constructor" << *_id << std::endl;
    }
    ~InterestingData() {
        std::cout << "destructor" << *_id << std::endl;
        delete _id;
    }
    InterestingData(const InterestingData& other) : _id(new int(*other._id)) {
        std::cout << "copy constructor" << *_id << std::endl;
    }
    InterestingData& operator=(const InterestingData& other) {
        _id = new int(*other._id);
        std::cout << "copy assignment" << *_id << std::endl;
        return *this;
    }

    int* _id;
};

int main(int argc, char const *argv[])
{
    std::unordered_map<std::string, InterestingData> map;
    InterestingData data1(new int(1)), data2(new int(2));
    map.insert_or_assign("test", data1);
    map.erase("test");
    map.insert_or_assign("test", data2); // LEAK?
    map.clear();
    std::cout << "BOOM!" << std::endl;
    return 0;
}
