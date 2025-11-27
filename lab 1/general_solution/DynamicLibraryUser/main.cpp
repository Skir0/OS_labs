#include <iostream>
#include <cmath>
#include "MyDynamicLibrary.h"
#include "MyStaticLibrary.h"

int main() {
    std::cout << "--- Testing Dynamic Library: Vector ---" << std::endl;

    std::cout << "\n1. Testing Global Variables" << std::endl;
    std::cout << "Global ZeroVector: (" << ZeroVector.getX().getValue() << ", " << ZeroVector.getY().getValue() << ")" << std::endl;
    std::cout << "Global UnitVector: (" << UnitVector.getX().getValue() << ", " << UnitVector.getY().getValue() << ")" << std::endl;

    Vector v1(Number(3.0), Number(4.0));
    std::cout << "\n2. Testing Constructor and Getters" << std::endl;
    std::cout << "Vector v1 created as: (" << v1.getX().getValue() << ", " << v1.getY().getValue() << ")" << std::endl;

    std::cout << "\n3. Testing Vector Addition" << std::endl;
    Vector v2 = v1 + UnitVector;
    std::cout << "v1 + UnitVector = (" << v2.getX().getValue() << ", " << v2.getY().getValue() << ")" << std::endl;

    std::cout << "\n4. Testing Polar Coordinates for v1(3, 4)" << std::endl;
    Number len = v1.length();
    Number ang = v1.angle();
    std::cout << "Length: " << len.getValue() << std::endl;
    std::cout << "Angle (radians): " << ang.getValue() << std::endl;

    std::cout << "\n5. Testing Setters" << std::endl;
    v1.setX(Number(5.0));
    v1.setY(Number(12.0));
    std::cout << "Vector v1 after setters: (" << v1.getX().getValue() << ", " << v1.getY().getValue() << ")" << std::endl;
    std::cout << "New length for v1(5, 12): " << v1.length().getValue() << std::endl;

    Vector v3(Number(1.0), Number(0.0));
    std::cout << "\n6. Testing Edge Cases for Angle" << std::endl;
    std::cout << "Angle for vector (1, 0) should be 0: " << v3.angle().getValue() << std::endl;
    v3.setX(Number(-1.0));
    std::cout << "Angle for vector (-1, 0) should be PI: " << v3.angle().getValue() << std::endl;

    return 0;
}