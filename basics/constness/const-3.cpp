int main(int argc, char const *argv[])
{
    char* const const_variable = new char[5];
    const_variable[0] = 'H';
    const_variable[1] = 'e';
    const_variable[2] = 'l';
    const_variable[3] = 'l';
    const_variable[4] = 'o';
    delete const_variable;
    const_variable = nullptr; // error: expression must be a modifiable lvalue
    return 0;
}
