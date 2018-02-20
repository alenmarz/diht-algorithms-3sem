#include <iostream>
#include <stdio.h>
#include "ShortestDistanceBetweenTwoSegments.h"

int main() {
    
    Segment first, second;
    
    std::cin >> first.first.x >> first.first.y >> first.first.z;
    std::cin >> first.second.x >> first.second.y >> first.second.z;
    std::cin >> second.first.x >> second.first.y >> second.first.z;
    std::cin >> second.second.x >> second.second.y >> second.second.z;
    
    ShortestDistanceBetweenTwoSegments distance(first, second);
    
    printf("%.9lf\n", distance.shortest_distance);
    
    return 0;
}
