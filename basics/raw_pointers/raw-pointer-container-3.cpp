#include <algorithm>
#include <cstdlib> // at_exit
#include <iostream>
#include <map>
#include <vector>

class GoodData {
public:
    GoodData(int id) : _id(id) {
        std::cout << "data [" << _id << "] constructor" << std::endl;
    }
    ~GoodData() {
        std::cout << "data [" << _id << "] destructor" << std::endl;
    }
    int _id;
};

// `simple_insert_iterator` based on ideas from this article
// https://www.fluentcpp.com/2017/03/17/smart-iterators-for-inserting-into-sorted-container/

template <typename Container>
class simple_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
protected:
    Container* container_;
public:
    typedef Container container_type;
    explicit simple_insert_iterator (Container& container)
        : container_(&container) {}
    simple_insert_iterator<Container>& operator= (const typename Container::value_type& value)
    {
        container_->insert(value);
        return *this;
    }
    simple_insert_iterator<Container>& operator* () { return *this; }
    simple_insert_iterator<Container>& operator++ () { return *this; }
    simple_insert_iterator<Container> operator++ (int) { return *this; }
};

template <typename Container>
simple_insert_iterator<Container> simple_inserter(Container& container)
{
    return simple_insert_iterator<Container>(container);
}

int main(int argc, char const *argv[])
{
    const int n_data = 5;
    std::vector<GoodData*> array_of_good_data(n_data);
    std::generate(array_of_good_data.begin(),
        array_of_good_data.end(),
        [i = 0]() mutable { return new GoodData(i++); });
    std::map<int, GoodData*> map_of_good_data;

    std::transform(array_of_good_data.begin(),
        array_of_good_data.end(),
        simple_inserter(map_of_good_data),
        [](GoodData* d) -> std::map<int, GoodData*>::value_type { // BTW: `(const GoodData* d)` will not work!
            return {d->_id, d};
    });
    
    for (const auto [i, p] : map_of_good_data) {
        delete p;
    }
    for (const auto p : array_of_good_data) {
        delete p;
    }

    return 0;
}
