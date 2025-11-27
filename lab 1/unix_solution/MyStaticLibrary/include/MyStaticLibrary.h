#ifndef NUMBERLIBRARY_H
#define NUMBERLIBRARY_H

class Number {
private:
    double value;

public:

    static const Number ZERO;
    static const Number ONE;

    Number();
    Number(double value);

    double getValue() const;
    void setValue(double newValue);

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;
    Number operator-() const;

    bool operator==(const Number& other) const;
    bool operator!=(const Number& other) const;
};


Number createNumber(double value);

#endif //NUMBERLIBRARY_H