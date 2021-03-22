#ifndef VECTOR_H
#define VECTOR_H

#include "../Number/Number.h"

class Vector{
private:
    Number x;
    Number y;
public:
    Vector(Number x, Number y);
    Vector operator+(const Vector& v);
    Number getAngle();
    Number getDistance();
    friend std::ostream& operator<<(std::ostream& os,const Vector& v);
};

#endif //VECTOR_H
