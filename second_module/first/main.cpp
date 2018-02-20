#include <iostream>
#include "SuffixArray.h"

int main() {
    
    std::string text;
    std::cin >> text;
    
    SuffixArray suffix_array(text);
    std::cout << suffix_array.number_of_different_substrings() << std::endl;

    return 0;
}
