#include <iostream>

class ProtectedDestructor {
public:
    ProtectedDestructor() {
        std::cout << "[ProtectedDestructor] constructor" << std::endl;
    }
protected:
    virtual ~ProtectedDestructor() {
        std::cout << "[ProtectedDestructor] destructor" << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    ProtectedDestructor* obj = new ProtectedDestructor();
    // delete dynamic_cast<PleaseLetMeDelete*>(obj);
    delete obj;
    return 0;
}
