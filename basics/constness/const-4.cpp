int main(int argc, char const *argv[])
{
    const char* const const_variable = new char[5];
    const_variable[0] = 'H'; // error: expression must be a modifiable lvalue
    delete const_variable;
    const_variable = nullptr; // error: expression must be a modifiable lvalue
    return 0;
}
