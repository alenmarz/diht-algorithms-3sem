//#include <istream>
//#include <ostream>
//#include <fstream>
#include <iostream>

#include <vector>
#include <string>


const size_t BUFFER_SIZE = 1024;


class PrefixFunctionSearcher {
public:
    PrefixFunctionSearcher(const std::string & pattern)
        : pattern_(pattern + '#'),
          prefix_function_(pattern_.size(), 0),
          position_(0)
    {
        for (size_t index = 1; index < pattern_.size(); ++index) {
            advance(pattern_[index]);
            prefix_function_[index] = position_;
        }
    }

    void search(std::basic_istream<char> & input, std::basic_ostream<char> & output) {
        size_t chars_read;
        size_t global_index = 0;
        char buffer[BUFFER_SIZE];

        do {
            input.read(buffer, BUFFER_SIZE);
            chars_read = input.gcount();

            for (size_t index = 0; index < chars_read; ++index, ++global_index) {
                advance(buffer[index]);

                if (position_ + 1 == pattern_.size()) {
                    output << global_index - position_ + 1 << ' ';
                }
            }
        } while (chars_read > 0);

        output << std::endl;
    }

private:
    void advance(char symbol) {
        while (position_ > 0 && pattern_[position_] != symbol) {
            position_ = prefix_function_[position_ - 1];
        }

        position_ += pattern_[position_] == symbol;
    }

    std::string pattern_;
    std::vector<size_t> prefix_function_;
    size_t position_;
};

