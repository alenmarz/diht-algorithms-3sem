#ifndef __SUFFIXARRAY_H__
#define __SUFFIXARRAY_H__

#include <vector>
#include <string>

typedef unsigned long long long_type;

const size_t ALPHABET_SIZE = 256;

class SuffixArray {
private:
    std::string text_;
    std::vector<int> sorted_by_second_part_;
    std::vector<size_t> labels_;
    size_t classes_;

public:
    SuffixArray(const std::string & text);
    long_type number_of_different_substrings();

private:
    std::vector<int> counting_sort();
    void symbols_sort();
    void second_parts_sort(size_t binary_number);
    void set_new_labels(size_t binary_number);
    void suffixes_sort();    
    std::vector<int> build_suffixes_positions_array();
    std::vector<int> build_lcp();
    long_type to_sum_up_lcp(const std::vector<int> & lcp);
};

#endif
