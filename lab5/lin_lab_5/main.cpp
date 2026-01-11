#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>
#include <vector>
#include <string>

void CreateProcessWithPipes(const std::string& processName, int readFd, int writeFd) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed for " << processName << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        if (readFd != STDIN_FILENO) {
            if (dup2(readFd, STDIN_FILENO) == -1) {
                std::cerr << "dup2 input failed for " << processName << ": " << strerror(errno) << std::endl;
                _exit(EXIT_FAILURE);
            }
            close(readFd);
        }

        if (writeFd != STDOUT_FILENO) {
            if (dup2(writeFd, STDOUT_FILENO) == -1) {
                std::cerr << "dup2 output failed for " << processName << ": " << strerror(errno) << std::endl;
                _exit(EXIT_FAILURE);
            }
            close(writeFd);
        }

        execl(processName.c_str(), processName.c_str(), nullptr);
        std::cerr << "execl failed for " << processName << ": " << strerror(errno) << std::endl;
        _exit(EXIT_FAILURE);
    }
}

int main() {
    char currentDir[1024];
    if (getcwd(currentDir, sizeof(currentDir)) == nullptr) {
        std::cerr << "Failed to get current directory: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    std::string baseDir = currentDir;

    std::string processMPath = baseDir + "/ProcessM";
    std::string processAPath = baseDir + "/ProcessA";
    std::string processPPath = baseDir + "/ProcessP";
    std::string processSPath = baseDir + "/ProcessS";
    std::string inputFilePath = baseDir + "/input.txt";

    std::cout << "Current directory: " << baseDir << std::endl;
    std::cout << "Looking for files:" << std::endl;
    std::cout << "  ProcessM: " << processMPath << std::endl;
    std::cout << "  ProcessA: " << processAPath << std::endl;
    std::cout << "  ProcessP: " << processPPath << std::endl;
    std::cout << "  ProcessS: " << processSPath << std::endl;
    std::cout << "  Input file: " << inputFilePath << std::endl;

    if (access(processMPath.c_str(), X_OK) != 0 ||
        access(processAPath.c_str(), X_OK) != 0 ||
        access(processPPath.c_str(), X_OK) != 0 ||
        access(processSPath.c_str(), X_OK) != 0) {
        std::cerr << "One or more process executables not found or not executable." << std::endl;
        std::cerr << "Make sure to compile all .cpp files first!" << std::endl;
        return EXIT_FAILURE;
    }

    int fileIn = open(inputFilePath.c_str(), O_RDONLY);
    if (fileIn < 0) {
        std::cerr << "Error opening input file " << inputFilePath << ": " << strerror(errno) << std::endl;
        std::cerr << "Make sure input.txt exists in the same directory as the executable." << std::endl;
        return EXIT_FAILURE;
    }

    int pipe1[2], pipe2[2], pipe3[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        std::cerr << "Error creating pipes: " << strerror(errno) << std::endl;
        close(fileIn);
        return EXIT_FAILURE;
    }

    std::cout << "Starting pipeline..." << std::endl;

    CreateProcessWithPipes(processMPath, fileIn, pipe1[1]);
    close(fileIn);
    close(pipe1[1]);

    CreateProcessWithPipes(processAPath, pipe1[0], pipe2[1]);
    close(pipe1[0]);
    close(pipe2[1]);

    CreateProcessWithPipes(processPPath, pipe2[0], pipe3[1]);
    close(pipe2[0]);
    close(pipe3[1]);

    CreateProcessWithPipes(processSPath, pipe3[0], STDOUT_FILENO);
    close(pipe3[0]);

    int status;
    pid_t pid;
    while ((pid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                std::cerr << "Process " << pid << " exited with status " << WEXITSTATUS(status) << std::endl;
            }
        } else if (WIFSIGNALED(status)) {
            std::cerr << "Process " << pid << " killed by signal " << WTERMSIG(status) << std::endl;
        }
    }

    return EXIT_SUCCESS;
}