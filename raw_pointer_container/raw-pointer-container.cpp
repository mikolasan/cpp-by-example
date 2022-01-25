#include <algorithm>
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

int main(int argc, char const *argv[])
{
    const int n_data = 5;
    {
        std::vector<GoodData*> array_of_good_data(n_data);
        std::generate(array_of_good_data.begin(),
            array_of_good_data.end(),
            [i = 0]() mutable { return new GoodData(i++); });
    }
    // leak?
    return 0;
}
