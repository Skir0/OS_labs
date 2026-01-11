#include <iostream>
#include <sstream>
#include <string>

int main()
{
    std::string line;
    long long sum = 0;
    bool numbers_read = false;

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        long long num;

        while (iss >> num)
        {
            sum += num;
            numbers_read = true;
        }

        if (!line.empty() && !numbers_read) {
            std::istringstream iss2(line);
            double dnum;
            while (iss2 >> dnum) {
                sum += static_cast<long long>(dnum);
                numbers_read = true;
            }
        }
    }

    if (numbers_read) {
        std::cout << sum << std::endl;
    } else {
        std::cerr << "No numbers were read from input" << std::endl;
        return 1;
    }

    return 0;
}