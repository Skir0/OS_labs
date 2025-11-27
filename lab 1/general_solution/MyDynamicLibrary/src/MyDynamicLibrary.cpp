#include "MyDynamicLibrary.h"
#include <cmath>
#include "MyStaticLibrary.h"
#include <stdexcept>


const Vector ZeroVector(Number(0.0), Number(0.0));
const Vector UnitVector(Number(1.0), Number(1.0));
// -----------------------------------------
Number Vector::getX() const
{
    return x;
}

Number Vector::getY() const
{
    return y;
}

void Vector::setX(Number dx) {
    this->x = dx;
}

void Vector::setY(Number dy) {
    this->y = dy;
}

Vector::Vector() : x(0.0), y(0.0) {}

Vector::Vector(Number dx, Number dy) : x(dx), y(dy) {}

Vector Vector::operator+(const Vector& other) const
{
    return Vector(x + other.x, y + other.y);
}

Vector Vector::operator-(const Vector& other) const
{
    return Vector(x - other.x, y - other.y);
}

Vector Vector::operator*(Number scalar) const
{
    return Vector(x * scalar, y * scalar);
}

Vector Vector::operator/(Number scalar) const
{
    if (scalar.getValue() == 0.0) {
        throw std::runtime_error("Деление вектора на ноль");
    }
    return Vector(x / scalar, y / scalar);
}

Number Vector::length() const
{
    double val = sqrt(x.getValue() * x.getValue() + y.getValue() * y.getValue());
    return Number(val);
}

Number Vector::angle() const
{

    double angle_rad = std::atan2(y.getValue(), x.getValue());
    return Number(angle_rad);
}