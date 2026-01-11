#include <iostream>
#include <string>
#include <sstream>

int main() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cerr << "Error reading input" << std::endl;
        return 1;
    }

    std::stringstream ss(line);
    double x;
    double sum = 0.0;

    while (ss >> x) {
        sum += x;
    }

    if (ss.fail() && !ss.eof()) {
        std::cerr << "Warning: Some values could not be parsed as numbers" << std::endl;
    }

    std::cout << sum << std::endl;
    return 0;
}