#include <iostream>
#include "Number/Number.h"
#include "Vector/Vector.h"

int main() {
    Number n1(1.1), n2(2.2);
    std::cout << n1 << " + " << n2 << " = " << (n1 + n2) << "\n";
    Number n11(1), n12(2), n21(-2), n22(1);
    Vector v1(n11, n12), v2(n21, n22);
    std::cout << v1 << " + " << v2 << " = " << (v1 + v2) << "\n";
    std::cout << v1 << " to polar: angle = " << v1.getAngle() <<
    ", dist = " << v1.getDistance() << "\n";
}
