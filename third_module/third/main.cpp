#include <iostream>
#include "MinkowskiSum.h"

int main() {
    
    int size_A, size_B;

    std::cin >> size_A;
    std::vector<Point> polygon_A(size_A);
    for (int i = 0; i < size_A; ++i) {
        std::cin >> polygon_A[i].x >> polygon_A[i].y;
    }

    std::cin >> size_B;
    std::vector<Point> polygon_B(size_B);
    for (int i = 0; i < size_B; ++i) {
        std::cin >> polygon_B[i].x >> polygon_B[i].y;
    }
    
    
    MinkowskiSum polygon(polygon_A, polygon_B);

    auto & final_polygon = polygon.get_final_polygon();
    
    std::cout << polygon.has_zero() << std::endl;

    return 0;
}
