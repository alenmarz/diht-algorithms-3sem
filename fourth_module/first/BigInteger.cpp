#include"BigInteger.h"
#include<vector>
#include<string>

BigInteger BigInteger::ZERO = BigInteger(0);
BigInteger BigInteger::ONE  = BigInteger(1);
BigInteger BigInteger::TWO  = BigInteger(2);
BigInteger BigInteger::TEN  = BigInteger(10);

BigInteger::BigInteger() {
    number_.clear();
    sign_ = true;
}

BigInteger::BigInteger(int init_value) {
    number_.clear();
    if (init_value >= 0)
        sign_ = true;
    else {
        sign_ = false;
        init_value = -init_value;
    }
    if (init_value == 0) {
        number_.push_back(0);
    }
    while (init_value > 0) {
        number_.push_back(init_value % 10);
        init_value /= 10;
    }
}

BigInteger::BigInteger(const std::string & init_value) {
    number_.clear();
    sign_ = true;
    int sign_symbol = 0; //появляется, если число отрицательное
    if (init_value[0] == '-') {
        sign_ = false;
        sign_symbol = 1;
    }
    for (int index = init_value.length() - 1; index > 0 + sign_symbol; --index) {
        number_.push_back(init_value[index] - '0');
    }
    number_.push_back(init_value[0 + sign_symbol] - '0');
}

BigInteger BigInteger::operator =(const BigInteger & init_value) {
    number_.clear();
    sign_ = init_value.sign_;
    for (std::vector<int>::size_type index = 0; index < init_value.number_.size(); ++index) {
        number_.push_back(init_value.number_[index]);
    }
    return *this;
}

BigInteger BigInteger::operator =(const std::string & init_value) {
    return BigInteger(init_value);
}

BigInteger operator +(const BigInteger & left, const BigInteger & right) {
    BigInteger result(left);
    result += right;
    return result;
}

BigInteger operator -(const BigInteger & left, const BigInteger & right) {
    BigInteger result(left);
    result -= right;
    return result;
}

BigInteger operator *(const BigInteger & left, const BigInteger & right) {
    BigInteger result(left);
    result *= right;
    return result;
}

BigInteger operator /(const BigInteger & left, const BigInteger & right) {
    BigInteger result(left);
    result /= right;
    return result;
}

BigInteger operator %(const BigInteger & left, const BigInteger & right) {
    BigInteger result(left);
    result %= right;
    return result;
}

BigInteger & BigInteger::operator +=(const BigInteger & init_value) {
    if (sign_ == init_value.sign_) {
        while (number_.size() <= init_value.number_.size()) {
            number_.push_back(0);
        }
        int carry = 0;
        std::vector<int>::size_type index;
        for (index = 0; index < init_value.number_.size(); ++index) {
            number_.at(index) += carry + init_value.number_.at(index);
            if (number_[index] >= 10) {
                carry = 1;
                number_[index] -= 10;
            } else {
                carry = 0;
            }
        }
        for ( ; index < number_.size(); ++index) {
            number_.at(index) += carry + 0;
            if (number_[index] >= 10) {
                carry = 1;
                number_[index] -= 10;
            } else {
                carry = 0;
            }
        }
    } else {
        if (sign_ == true && init_value.sign_ == false) {
            BigInteger tmp(init_value);
            tmp.sign_ = true;
            *this -= tmp;
        } else {
            BigInteger tmp(*this);
            tmp.sign_ = true;
            *this = init_value - tmp;
        }
    }
    trim();
    return *this;
}

BigInteger & BigInteger::operator -=(const BigInteger & init_value) {
    BigInteger tmp;
    if (*this < init_value) {
        tmp = *this;
        *this = init_value;
        this->sign_ = false;
    } else {
        tmp = init_value;
    }
    int bor = 0;
    std::vector<int>::size_type index;
    for (index = 0; index < tmp.number_.size(); ++index) {
        number_[index] -= bor + tmp.number_[index];
        if (number_[index] < 0) {
            bor = 1;
            number_[index] += 10;
        } else {
            bor = 0;
        }
    }
    for ( ; index < number_.size(); ++index) {
        number_[index] -= bor;
        if (number_[index] < 0) {
            bor = 1;
            number_[index] += 10;
        } else {
            bor = 0;
        }
    }
    trim();
    return *this;
}

BigInteger & BigInteger::operator *=(const BigInteger & init_value) {
    BigInteger result;
    if (this->sign_ == init_value.sign_) {
        result.sign_ = true;
    } else {
        result.sign_ = false;
    }
    for (std::vector<int>::size_type global_index = 0; global_index < number_.size(); ++global_index) {
        for (std::vector<int>::size_type index = 0; index < init_value.number_.size(); ++index) {
            if (global_index + index < result.number_.size()) {
                result.number_[global_index + index] += number_[global_index] * init_value.number_[index];
            } else {
                result.number_.push_back(number_[global_index] * init_value.number_[index]);
            }
        }
    }
    for (std::vector<int>::size_type index = 0; index < result.number_.size(); ++index) {
        if (index + 1 < result.number_.size()) {
            result.number_[index + 1] += result.number_[index] / 10;
        } else if (result.number_[index] >= 10) {
            result.number_.push_back(result.number_[index] / 10);
        } else {
            break;
        }
        result.number_[index] %= 10;
    }
    result.trim();
    *this = result;
    return *this;
}

BigInteger& BigInteger::operator /=(const BigInteger& init_value) {
    if (*this == init_value) {
        *this = BigInteger::ONE;
        return *this;
    }

    BigInteger result(0);
    if (sign_ == init_value.sign_) {
        result.sign_ = true;
    } else {
        result.sign_ = false;
    }

    BigInteger divider(init_value);

    this->sign_ = true;
    divider.sign_ = true;

    if (*this < divider) {
        *this = BigInteger::ZERO;
        return *this;
    }

    int counter = 0;
    while (*this > BigInteger::ZERO) {
        if (*this >= divider) {
            *this -= divider;
            result += pow(BigInteger::TEN, counter);
            divider *= BigInteger::TEN;
            counter++;
        } else {
            divider = init_value;
            counter = 0;
            if (*this < divider) {
                result.trim();
                *this = result;
                return *this;
            }
        }
        divider.sign_ = true;
    }

    result.trim();
    *this = result;
    return *this;
}

BigInteger::operator bool() const {
    return *this != BigInteger::ZERO;
}

std::string BigInteger::toString() const {
    std::string result;

    result.reserve(number_.size() + 1);

    size_t offset = 0;

    if (! sign_) {
        result[0] = '-';
        offset = 1;
    }

    std::string tmp;

    for (size_t index = 0; index < number_.size(); ++index) {
        result[index + offset] = number_[number_.size() - index - 1] + '0';
    }

    return result;
}

BigInteger pow(const BigInteger & i_value, const BigInteger & i_exp) {
    if (i_exp == BigInteger::ZERO || i_value == BigInteger::ONE) {
        return BigInteger::ONE;
    }
    if (i_value == BigInteger::ZERO) {
        return BigInteger::ZERO;
    }

    BigInteger i(0);
    BigInteger result(i_value);
    BigInteger exp(i_exp);

    exp -= BigInteger::ONE;

    while (i < exp){
        result *= i_value;
        ++i;
    }

    return result;
}

BigInteger pow(const BigInteger & value, const int i_exp) {
    BigInteger exp(i_exp);
    return pow(value, exp);
}

BigInteger & BigInteger::operator %=(const BigInteger & init_value) {
    *this = *this - (*this / init_value) * init_value;
    return *this;
}

BigInteger& BigInteger::operator ++() {
    *this += BigInteger::ONE;
    return *this;
}

BigInteger & BigInteger::operator --() {
    *this -= BigInteger::ONE;
    return *this;
}

const BigInteger BigInteger::operator ++(int) {
    BigInteger temp = *this;
    ++(*this);
    return BigInteger(temp);
}

const BigInteger BigInteger::operator --(int) {
    BigInteger temp = *this;
    --(*this);
    return BigInteger(temp);
}

BigInteger & BigInteger::operator +() {
    return *this;
}

BigInteger & BigInteger::operator -() {
    this->sign_ = !(this->sign_);
    return *this;
}

bool operator >(const BigInteger & left, const BigInteger & right) {
    return (left != right) && (right < left);
}

bool operator >=(const BigInteger & left, const BigInteger & right) {
    return !(left < right);
}

bool operator <(const BigInteger & left, const BigInteger & right) {
    if (left.sign_ == false && right.sign_ == true) {
        return true;
    }
    if (left.sign_ == true && right.sign_ == false) {
        return false;
    }
    if (left.sign_) {
        if (left.number_.size() < right.number_.size()) {
            return true;
        }
        else if (left.number_.size() > right.number_.size()) {
            return false;
        }
    } else {
        if (left.number_.size() < right.number_.size()) {
            return false;
        }
        else if (left.number_.size() > right.number_.size()) {
            return true;
        }
    }
    for (std::vector<int>::size_type index = left.number_.size() - 1; index > 0; --index) {
        if (left.number_[index] < right.number_[index]) {
            if (left.sign_) {
                return true;
            } else {
                return false;
            }
        } else if (left.number_[index] > right.number_[index]) {
            if (left.sign_) {
                return false;
            } else {
                return true;
            }
        }
    }
    if (left.number_[0] < right.number_[0]) {
        if (left.sign_) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool operator <=(const BigInteger & left, const BigInteger & right) {
    return !(left > right);
}

bool operator ==(const BigInteger & left, const BigInteger & right) {
    if (left.sign_ != right.sign_ || left.number_.size() != right.number_.size()) {
        return false;
    }
    for (std::vector<int>::size_type index = 0; index < left.number_.size(); index++) {
        if (left.number_.at(index) != right.number_.at(index)) {
            return false;
        }
    }
    return true;
}

bool operator !=(const BigInteger & left, const BigInteger & right) {
    return !(left == right);
}


std::istream & operator >>(std::istream & in, BigInteger & value) {
    std::string tmp;
    in >> tmp;
    value = tmp;
    return in;
}

std::ostream& operator <<(std::ostream & out, const BigInteger & value) {
    if (value.number_.size() == 0) {
        out << "0";
    } else {
        if (value.sign_ == false) {
            out << '-';
        }
        for (std::vector<int>::size_type index = value.number_.size() - 1; index > 0; --index) {
            out << value.number_[index];
        }
        out << value.number_[0];
    }
    return out;
}

void BigInteger::trim() {
    while (number_.size() > 1 && number_[number_.size() - 1] == 0) {
        number_.pop_back();
    }
}
