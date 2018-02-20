#include <bitset>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


const size_t ALPHABET_SIZE = 26;
const char FIRST_LETTER = 'a';


class PrefixFunction {
public:
    PrefixFunction() {}

    PrefixFunction(const std::string & pattern)
        : pattern_(pattern),
          value_(pattern.size(), 0),
          length_(0) 
    {
        for (size_t index = 1; index < pattern_.size(); ++index) {
            advance(pattern_[index]);
            update(index, length_);
        }

        // reset position after construction
        length_ = 0;
    }

    friend std::basic_istream<char> &
    operator >> (std::basic_istream<char> & input, PrefixFunction & function) {
        std::string line;
        getline(input, line);

        std::stringstream stream(line);

        size_t item = 0;

        while (stream >> item) {
            function.value_.push_back(item);
        }

        function.pattern_ = function.ToString();
        function.length_ = 0;

        return input;
    }

    friend std::basic_ostream<char> &
    operator << (
        std::basic_ostream<char> & output, const PrefixFunction & function
    ) {
        for (size_t index = 0; index < function.value_.size(); ++index) {
            output << function.value_[index] << ' ';
        }

        output << std::endl;

        return output;
    }

    static PrefixFunction FromString(const std::string & pattern) {
        return PrefixFunction(pattern);
    }

    std::string ToString() const {
        std::string result(value_.size(), '?');
        result[0] = FIRST_LETTER;

        // letters which cannot be used at each step
        std::bitset<ALPHABET_SIZE> forbidden_letters;

        for (size_t index = 1; index < value_.size(); ++index) {
            if (value_[index] > 0) {
                result[index] = result[value_[index] - 1];
                continue;
            }

            forbidden_letters.reset();
            size_t update_index = index;

            // mark all forbidden letters (using prefix rule)
            do {
                update_index = value_[update_index - 1];
                forbidden_letters.set(result[update_index] - FIRST_LETTER);
            } while (update_index > 0);

            // find index of first unused letter
            size_t letter_index = 0;
            while (
                letter_index < forbidden_letters.size() && 
                forbidden_letters[letter_index]
            ) {
                ++letter_index;
            }

            result[index] = FIRST_LETTER + letter_index;
        }

        return result;
    }

    size_t advance(char symbol) {
        while (length_ > 0 && pattern_[length_] != symbol) {
            length_ = value_[length_ - 1];
        }

        length_ += pattern_[length_] == symbol;

        return length_;
    }

    size_t size() const {
        return value_.size();
    }

private:
    void update(size_t index, size_t length) {
        value_[index] = length;
    }

    // original pattern
    std::string pattern_;
    // values of prefix function
    std::vector<size_t> value_;
    // length of current prefix
    size_t length_;
};


class ZFunction {
public:
    ZFunction() {}

    ZFunction(const std::string & pattern)
        : pattern_(pattern),
          value_(pattern.size(), 0)
    {
        size_t left = 0, right = 0;
        for (size_t index = 1; index < pattern_.size(); ++index) {
            if (index <= right) {
                value_[index] = std::min(right - index + 1, value_[index - left]);
            }

            while (
                    substring_end_index(index) < pattern_.size() &&
                    pattern_[value_[index]] == pattern_[substring_end_index(index)]
            ) {

                ++value_[index];
            }
            
            if (substring_end_index(index) - 1 > right) {
                left = index;
                right = substring_end_index(index);
            }
        }
    }

    friend std::basic_istream<char> &
    operator >> (std::basic_istream<char> & input, ZFunction & function) {
        std::string line;
        getline(input, line);

        std::stringstream stream(line);

        size_t item = 0;

        while (stream >> item) {
            function.value_.push_back(item);
        }

        function.pattern_ = function.ToString();

        return input;
    }

    friend std::basic_ostream<char> &
    operator << (
        std::basic_ostream<char> & output, const ZFunction & function
    ) {
        for (size_t index = 0; index < function.value_.size(); ++index) {
            output << function.value_[index] << ' ';
        }

        output << std::endl;

        return output;
    }

    static ZFunction FromString(const std::string & pattern) {
        return ZFunction(pattern);
    }
    
    std::string ToString() const {
        std::string result(value_.size(), '?');

        char biggest_letter = FIRST_LETTER;
        result[0] = FIRST_LETTER;

        std::bitset<ALPHABET_SIZE> forbidden_letters;
        
        size_t current_max_substring_length = 0, current_letter_index = 0;

        for (size_t global_index = 1; global_index < value_.size(); ++global_index) {
            if (value_[global_index] == 0 && current_max_substring_length == 0) {
                forbidden_letters.reset();
                forbidden_letters.set(0);
                
                for (size_t index = 0; index < global_index; ++index) {
                    if (value_[index] > 0 && substring_end_index(index) == global_index) {
                        forbidden_letters.set(result[value_[index]] - FIRST_LETTER);
                    }
                }

                // find index of first unused letter
                size_t letter_index = 0;
                while (
                    letter_index < forbidden_letters.size() && 
                    forbidden_letters[letter_index]
                ) {
                    ++letter_index;
                }

                result[global_index] = FIRST_LETTER + letter_index;

            } else {
                if (value_[global_index] > current_max_substring_length) {
                    current_max_substring_length = value_[global_index];
                    current_letter_index = 0;
                } 
                result[global_index] = result[current_letter_index];
                --current_max_substring_length;
                ++current_letter_index;
            }
        }

        return result;
    }
    
    //returns index after substring
    size_t substring_end_index(size_t index) const{
        return value_[index] + index;
    }

    size_t size() const {
        return value_.size();
    }

private:
    // original pattern
    std::string pattern_;
    // values of z function
    std::vector<size_t> value_;
};

void z_from_prefix(
    std::basic_ostream<char> & output, std::basic_istream<char> & input
) {
    PrefixFunction prefix_function;
    input >> prefix_function;
    
    ZFunction z_function(prefix_function.ToString());

    output << z_function;
}


void prefix_from_z(
    std::basic_ostream<char> & output, std::basic_istream<char> & input
) {
    ZFunction z_function;
    input >> z_function;
    
    PrefixFunction prefix_function(z_function.ToString());

    output << prefix_function;
}


int main() {
    std::basic_istream<char> & input = std::cin;
    std::basic_ostream<char> & output = std::cout;

    // prefix_from_z(output, input);
    // z_from_prefix(ouput, input);

    // PrefixFunction one_function;
    ZFunction one_function;

    input >> one_function;

    //std::string line;
    //getline(input, line);

    output << one_function.ToString() << std::endl;
    // output << PrefixFunction::FromString(line); 
    // output << ZFunction::FromString(line);    

    return 0;
}
