#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>

bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

int nth_prime(int n) {
    int count = 0;
    int num = 1;
    while (count < n) {
        num++;
        if (is_prime(num)) {
            count++;
        }
    }
    return num;
}

int main(void) {
    int pipeParentToChild[2], pipeChildToParent[2];
    pid_t pid;
    std::string input;

    if (pipe(pipeParentToChild) == -1 || pipe(pipeChildToParent) == -1) {
        std::cerr << "Pipe creation failed" << std::endl;
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        return 1;
    }

    if (pid == 0) {
        close(pipeParentToChild[1]);
        close(pipeChildToParent[0]);

        int m;
        read(pipeParentToChild[0], &m, sizeof(int));

        int result = nth_prime(m);
        write(pipeChildToParent[1], &result, sizeof(int));

        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);
        exit(0);
    } else {
        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);

        while (true) {
            std::cout << "[Parent] Please enter the number: ";
            std::getline(std::cin, input);

            if (input == "exit") {
                std::cout << "[Parent] Exiting..." << std::endl;
                break;
            }

            try {
                int m = std::stoi(input);
                write(pipeParentToChild[1], &m, sizeof(int));

                int result;
                read(pipeChildToParent[0], &result, sizeof(int));
                std::cout << "[Parent] Received calculation result of prime(" << m << ") = " << result << "..." << std::endl;

            } catch (const std::invalid_argument&) {
                std::cerr << "[Parent] Invalid input, please enter a valid integer or 'exit' to quit." << std::endl;
            }
        }

        close(pipeParentToChild[1]);
        close(pipeChildToParent[0]);

        wait(NULL);
    }

    return 0;
}
