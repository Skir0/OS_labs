#include <iostream>
#include <sstream>
#include <string>
#define N 11

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

            long long result = num + N;
            if (num > 0 && result < 0) {
                std::cerr << "Warning: overflow when adding " << N << " to " << num << std::endl;
            }

            std::cout << result;
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
                std::cout << (dnum + N);
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