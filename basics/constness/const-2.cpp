int main(int argc, char const *argv[])
{
    char* const const_variable = nullptr;
    // const_variable = new char[5]; // error: expression must be a modifiable lvalue
    const_variable[0] = 'H';
    delete[] const_variable;
    return 0;
}
