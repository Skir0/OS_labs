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

            long long result = num * 7;
            if (num > 0 && result < 0) {
                std::cerr << "Warning: overflow when multiplying " << num << " by 7" << std::endl;
            } else if (num < 0 && result > 0) {
                std::cerr << "Warning: overflow when multiplying " << num << " by 7" << std::endl;
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
                std::cout << (dnum * 7.0);
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