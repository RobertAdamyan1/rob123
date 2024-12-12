#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctime>
void do_command(char** argv) {
    clock_t start_time = clock();

    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Fork failed!" << std::endl;
        exit(1);
    } else if (pid == 0) {
        execvp(argv[0], argv);
        std::cerr << "Execvp failed!" << std::endl;
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        clock_t end_time = clock();

        double duration = double(end_time - start_time) / CLOCKS_PER_SEC;

        if (WIFEXITED(status)) {
            std::cout << "Command completed with " 
                      << WEXITSTATUS(status) << " exit code and took "
                      << duration << " seconds." << std::endl;
        } else {
            std::cerr << "Command failed to execute properly." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./do-command <command> [arguments]" << std::endl;
        return 1;
    }

    std::vector<char*> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    args.push_back(nullptr);
    char** command_args = args.data();

    do_command(command_args);

    return 0;
}
