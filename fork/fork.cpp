#include <iostream>
#include <sys/wait.h> // wait
#include <unistd.h> // fork, exec

const char* const long_argv[] = {"./long_process.sh", NULL};

int main(int argc, char const *argv[])
{
    if (fork() == 0) {
        std::cout << "child process" << std::endl;
        execvp(long_argv[0], const_cast<char* const *>(long_argv));
    } else {
        std::cout << "parent process" << std::endl;
    }
    int status;
    wait(&status);
    std::cout << "parent done" << std::endl;

    return 0;
}
