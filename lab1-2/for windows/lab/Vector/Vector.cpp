#include "Vector.h"

Vector::Vector(Number x, Number y): x(x), y(y) {}

Vector Vector::operator+(const Vector& v) {
    return {this->x + v.x, this->y + v.y};
}

Number Vector::getAngle() {
    return !(x == Number(0)) ? atan(this->y / this->x) : Number(0);
}

Number Vector::getDistance() {
    return pow(pow(this->x, 2) + pow(this->y, 2), 0.5);
}

std::ostream& operator<<(std::ostream& os,const Vector& v){
    return os << "(" << v.x << ", " << v.y << ")";
}
