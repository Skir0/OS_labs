#include "MyStaticLibrary.h"
#include <stdexcept>

const Number Number::ZERO = Number(0.0);
const Number Number::ONE = Number(1.0);

Number::Number() : value(0.0)
{
}

Number::Number(double value)
{
    this->value = value;
}

double Number::getValue() const {
    return value;
}

void Number::setValue(double newValue) {
    this->value = newValue;
}


Number Number::operator+(const Number& other) const {
    return Number(this->value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(this->value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(this->value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0) {
        throw std::runtime_error("Division by zero");
    }
    return Number(this->value / other.value);
}

Number Number::operator-() const {
    return Number(-this->value);
}

bool Number::operator==(const Number& other) const {
    return this->value == other.value;
}

bool Number::operator!=(const Number& other) const {
    return !(*this == other);
}

Number createNumber(double value) {
    return Number(value);
}