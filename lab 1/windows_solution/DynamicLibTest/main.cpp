#include <iostream>
#include <cmath>
#include "..\DynamicLib\VectorLibrary.h"
#include "..\StaticLib\NumberLibrary.h"

int main() {

    std::cout << "Global ZeroVector: (" << ZeroVector.getX().getValue() << ", " << ZeroVector.getY().getValue() << ")" << std::endl;
    std::cout << "Global UnitVector: (" << UnitVector.getX().getValue() << ", " << UnitVector.getY().getValue() << ")" << std::endl;

    Vector v1(Number(3.0), Number(4.0));
    std::cout << "Vector v1 created as: (" << v1.getX().getValue() << ", " << v1.getY().getValue() << ")" << std::endl;

    Vector v2 = v1 + UnitVector;
    std::cout << "v1 + UnitVector = (" << v2.getX().getValue() << ", " << v2.getY().getValue() << ")" << std::endl;

    Number len = v1.length();
    Number ang = v1.angle();
    std::cout << "Length: " << len.getValue() << std::endl;
    std::cout << "Angle (radians): " << ang.getValue() << std::endl;

    v1.setX(Number(5.0));
    v1.setY(Number(12.0));
    std::cout << "Vector v1 after setters: (" << v1.getX().getValue() << ", " << v1.getY().getValue() << ")" << std::endl;
    std::cout << "New length for v1(5, 12): " << v1.length().getValue() << std::endl;

    Vector v3(Number(1.0), Number(0.0));
    std::cout << "Angle for vector (1, 0) should be 0: " << v3.angle().getValue() << std::endl;
    v3.setX(Number(-1.0));
    std::cout << "Angle for vector (-1, 0) should be PI: " << v3.angle().getValue() << std::endl;

    return 0;
}