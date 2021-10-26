#ifndef NUMBER_H
#define NUMBER_H

#include <iostream>

class Number{
private:
    double num;
public:
    explicit Number(double num);
    Number();
    Number operator+(const Number& n) const;
    Number operator-(const Number& n) const;
    Number operator*(const Number& n) const;
    Number operator/(const Number& n) const;
    Number operator=(const Number& n);
    bool operator==(const Number& n) const;
    friend std::ostream& operator<<(std::ostream& os,const Number& n);
    friend Number atan(const Number& n);
    friend Number pow(const Number& n1, const double& n2);
};

namespace Constants{
    extern const Number ZERO;
    extern const Number ONE;
}

#endif //NUMBER_H
