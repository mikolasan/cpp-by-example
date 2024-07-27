int main(int argc, char const *argv[])
{
    const char* const const_variable = new char[5];
    // const_variable[0] = 'H'; // error: assignment of read-only location
    delete const_variable;
    // const_variable = nullptr; error: assignment of read-only variable
    return 0;
}
