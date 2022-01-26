#include <vector>

#define MACRO_NONCOPYABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
public:
    MACRO_NONCOPYABLE(NonCopyable)
};

class TestClass : public NonCopyable {
public:
    TestClass() = default;
    ~TestClass() = default;
};

int main(int argc, char const *argv[])
{
    std::vector<TestClass> v;
    TestClass t;
    v.push_back(std::move(t)); // <-- compile error here
    return 0;
}
