#ifndef VECTORLIBRARY_H
#define VECTORLIBRARY_H


#if defined(_WIN32)
    #ifdef VECTOR_EXPORTS
        #define VECTOR_API __declspec(dllexport)
    #else
        #define VECTOR_API __declspec(dllimport)
    #endif
#else
    #define VECTOR_API __attribute__((visibility("default")))
#endif
#include "MyStaticLibrary.h"
// -----------------------------------------


class VECTOR_API Vector {
private:
    Number x;
    Number y;

public:
    Vector();
    Vector(Number dx, Number dy);

    Number getX() const;
    Number getY() const ;
    void setX(Number dx);
    void setY(Number dy);

    Vector operator+(const Vector& other) const;

    Number length() const;
    Number angle() const;

    Vector operator-(const Vector& other) const;
    Vector operator*(Number scalar) const;
    Vector operator/(Number scalar) const;
};


extern VECTOR_API const Vector ZeroVector;
extern VECTOR_API const Vector UnitVector;


#endif //VECTORLIBRARY_H