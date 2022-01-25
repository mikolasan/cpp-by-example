#include <algorithm>
#include <cstdlib> // at_exit
#include <iostream>
#include <vector>

class GoodData {
public:
    GoodData(int id) : _id(id) {
        std::cout << "data [" << _id << "] constructor" << std::endl;
    }
    ~GoodData() {
        std::cout << "data [" << _id << "] destructor" << std::endl;
    }
private:
    int _id;
};

const int n_data = 5;
std::vector<GoodData*> array_of_good_data(n_data);

int main(int argc, char const *argv[])
{
    std::atexit([](){
        for (const auto p : array_of_good_data) {
            delete p;
        }
    });

    std::generate(array_of_good_data.begin(),
        array_of_good_data.end(),
        [i = 0]() mutable { return new GoodData(i++); });

    exit(0);
}
