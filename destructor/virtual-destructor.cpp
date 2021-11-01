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

class PleaseLetMeDelete : public ProtectedDestructor {
public:
    PleaseLetMeDelete() {
        std::cout << "[PleaseLetMeDelete] constructor" << std::endl;
    }
    ~PleaseLetMeDelete() {
        std::cout << "[PleaseLetMeDelete] destructor" << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    ProtectedDestructor* obj = new PleaseLetMeDelete();
    delete dynamic_cast<PleaseLetMeDelete*>(obj);
    return 0;
}
