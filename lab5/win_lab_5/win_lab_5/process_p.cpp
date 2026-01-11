#include <iostream>
#include <sstream>
#include <string>

int main()
{
    std::string line;
    bool first = true;
    bool numbers_processed = false;

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        long long num;

        while (iss >> num)
        {
            if (!first) {
                std::cout << " ";
            }

            long long cube = num;
            if (num > 1290 || num < -1290) {
                cube = num * num;
                if (cube > 0 && num > 0 && (cube > 9223372036854775807LL / num)) {
                    std::cerr << "Warning: possible overflow calculating cube of " << num << std::endl;
                }
                cube *= num;
            } else {
                cube = num * num * num;
            }

            std::cout << cube;
            first = false;
            numbers_processed = true;
        }

        if (!numbers_processed && !line.empty()) {
            std::istringstream iss2(line);
            double dnum;
            while (iss2 >> dnum) {
                if (!first) {
                    std::cout << " ";
                }
                double cube = dnum * dnum * dnum;
                std::cout << cube;
                first = false;
                numbers_processed = true;
            }
        }
    }

    if (numbers_processed) {
        std::cout << std::endl;
    } else {
        std::cerr << "No numbers were processed" << std::endl;
        return 1;
    }

    return 0;
}