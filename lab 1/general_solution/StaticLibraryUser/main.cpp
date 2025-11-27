#include <iostream>
#include <MyStaticLibrary.h>

int main() {
    std::cout << "Zero: " << Number::ZERO.getValue() << std::endl;
    std::cout << "One: " << Number::ONE.getValue() << std::endl;

    Number a = createNumber(10.5);
    Number b = createNumber(2.5);

    Number sum = a + b;
    Number diff = a - b;
    Number product = a * b;
    Number quotient = a / b;

    std::cout << "a + b = " << sum.getValue() << std::endl;
    std::cout << "a - b = " << diff.getValue() << std::endl;
    std::cout << "a * b = " << product.getValue() << std::endl;
    std::cout << "a / b = " << quotient.getValue() << std::endl;

    Number negative = -a;
    std::cout << "-a = " << negative.getValue() << std::endl;

    return 0;
}