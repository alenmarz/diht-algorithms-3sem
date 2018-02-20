#include "SuffixArray.h"

SuffixArray::SuffixArray(const std::string & text)
    : text_(text + '#')
{
        suffixes_sort();
}
    
long_type SuffixArray::number_of_different_substrings() {
        
    long_type sum = 0;
    for (size_t index = 0; index < text_.length(); ++index) {
        sum += (text_.length() - 1) - sorted_by_second_part_[index];
    }
    return sum - to_sum_up_lcp(build_lcp());
}
std::vector<int> SuffixArray::counting_sort() {
    
    std::vector<int> counter(ALPHABET_SIZE, 0);
    std::vector<int> sorted_array_of_symbols(text_.length(), 0);
    
    for (size_t index = 0; index < text_.length(); ++index) {
        ++counter[text_[index]];
    }
    for (size_t index = 1; index < ALPHABET_SIZE; ++index) {
        counter[index] += counter[index - 1];
    }
    for (size_t index = 0; index < text_.length(); ++index) {
        sorted_array_of_symbols[--counter[text_[index]]] = index;
    }
    
    return sorted_array_of_symbols;
}

void SuffixArray::symbols_sort() {
    
    std::vector<int> sorted_symbols = counting_sort();

    std::vector<size_t> labels(text_.length(), 0);
    labels[sorted_symbols[0]] = 0;

    classes_ = 1;
    
    for (size_t index = 1; index < text_.length(); ++index) {
        if (text_[sorted_symbols[index]] != text_[sorted_symbols[index - 1]])  
            ++classes_;
        labels[sorted_symbols[index]] = classes_ - 1;
    }

    sorted_by_second_part_ = sorted_symbols;
    labels_ = labels;

}

void SuffixArray::second_parts_sort(size_t binary_number) {    
    
    std::vector<int> now_sorting(text_.length());

    for (size_t index = 0; index < text_.length(); ++index) {     
        now_sorting[index] = sorted_by_second_part_[index] - (1<<binary_number);
        
        if (now_sorting[index] < 0) {
            now_sorting[index] += text_.length();
        }
    }

    std::vector<int> counter(classes_, 0);

    for (size_t index = 0; index < text_.length(); ++index) {
        ++counter[labels_[now_sorting[index]]];
    }
    for (size_t index = 1; index < classes_; ++index) {
        counter[index] += counter[index - 1];
    }

    for (int index = text_.length() - 1; index >= 0; --index) {
        sorted_by_second_part_[--counter[labels_[now_sorting[index]]]] = now_sorting[index];
    }
}


void SuffixArray::set_new_labels(size_t binary_number) {

    std::vector<size_t> labels_new(text_.length());
    
    labels_new[sorted_by_second_part_[0]] = 0;
    classes_ = 1;
    
    for (size_t index = 1; index < text_.length(); ++index) {
        
        size_t first_pair_middle = (sorted_by_second_part_[index] + (1<<binary_number)) % text_.length();
        size_t second_pair_middle = (sorted_by_second_part_[index - 1] + (1<<binary_number)) % text_.length();
        
        if (
            labels_[sorted_by_second_part_[index]] != labels_[sorted_by_second_part_[index - 1]] 
            || labels_[first_pair_middle] != labels_[second_pair_middle]
        ) {
                ++classes_;
        }
        
        labels_new[sorted_by_second_part_[index]] = classes_ - 1;
    }

    labels_ = labels_new;
}

void SuffixArray::suffixes_sort() {
    
    symbols_sort();
    
    for (size_t binary_number = 0; (1<<binary_number) < text_.length(); ++binary_number) {
                
        second_parts_sort(binary_number);
        set_new_labels(binary_number);
    }
}

std::vector<int> SuffixArray::build_suffixes_positions_array() {
    
    std::vector<int> positions(text_.length());

    for (size_t index = 0; index < text_.length(); ++index) {
        positions[sorted_by_second_part_[index]] = index;
    }

    return positions;
}

std::vector<int> SuffixArray::build_lcp() {

    std::vector<int> suffix_position = build_suffixes_positions_array();
    std::vector<int> lcp(text_.length(), 0);
    
    for (size_t index = 0, prefix_length = 0; index < text_.length(); ++index) {
        if (prefix_length > 0) {
            --prefix_length;
        }
        if (suffix_position[index] == text_.length() - 1) {
        
            lcp[text_.length() - 1] = -1;
            prefix_length = 0;
        
        } else {
        
            size_t next_suffix = sorted_by_second_part_[suffix_position[index] + 1];
            while (
                    std::max(index + prefix_length, next_suffix + prefix_length) < text_.length()
                    && text_[index + prefix_length] == text_[next_suffix + prefix_length]
            ) {
                ++prefix_length;
            }
            
            lcp[suffix_position[index]] = prefix_length;
        }
    }
    return lcp;
}

long_type SuffixArray::to_sum_up_lcp(const std::vector<int> & lcp) {
    long_type lcp_sum = 0;
    for (size_t index = 0; index + 1 < text_.length(); ++index) {
        lcp_sum += lcp[index];
    }
    return lcp_sum;
}
