#include "MyStaticLibrary.h" // Подключаем наш новый, правильный заголовочный файл
#include <stdexcept>

// Определяем статические члены, объявленные в заголовочном файле
const Number Number::ZERO = Number(0.0);
const Number Number::ONE = Number(1.0);

// Конструктор по умолчанию. Инициализирует значением 0.0
Number::Number() : value(0.0)
{
}

// Конструктор с параметром
Number::Number(double value)
{
    this->value = value;
}

double Number::getValue() const {
    return value;
}

// ИСПРАВЛЕНО: Метод setValue теперь работает
void Number::setValue(double newValue) {
    this->value = newValue;
}

// --- Реализация операторов ---

Number Number::operator+(const Number& other) const {
    return Number(this->value + other.value);
}

// ИСПРАВЛЕНО: Оператор вычитания теперь выполняет вычитание
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
    return !(*this == other); // Лучше реализовать через ==
}

// Реализация вспомогательной функции
Number createNumber(double value) {
    return Number(value);
}