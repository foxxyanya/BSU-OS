#include "Number.h"
#include <cmath>

Number::Number(): num(0) {}

Number::Number(double num): num(num) {}

Number Number::operator+(const Number& n) const{
    return Number(this->num + n.num);
}

Number Number::operator-(const Number& n) const{
    return Number(this->num - n.num);
}

Number Number::operator*(const Number& n) const{
    return Number(this->num * n.num);
}

Number Number::operator/(const Number& n) const{
    return Number(this->num / n.num);
}

std::ostream& operator<<(std::ostream& os, const Number& n){
    return os << n.num << "N";
}

Number Number::operator=(const Number& n) {
    this->num = n.num;	
    return *this; 
}

bool Number::operator==(const Number& n) const{
    return this->num == n.num;
}


Number atan(const Number& n) {
    return Number(atan(n.num));
}

Number pow(const Number& n1, const double & n2) {
    return Number(pow(n1.num, n2));
}

namespace Constants{
     extern const Number ZERO(0);
     extern const Number ONE(1);
}


