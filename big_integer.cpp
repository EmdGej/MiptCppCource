#include "big_integer.hpp"

BigInt::BigInt() = default;

// ========================================================================= //
// =========================== CONSTRUCTOR STRING ========================== //
// ========================================================================= //

BigInt::BigInt(const std::string& number) {
  if (number.empty()) {
    return;
  }

  size_t nulls_counter = 0;
  for (size_t i = number.size() - 1; i > 0; --i) {
    if (number[i] - '0' == 0) {
      ++nulls_counter;
    }
    number_.push_back(number[i] - '0');
  }

  if (number[0] == '-' && nulls_counter != number.size() - 1) {
    is_negative_ = true;

    for (int i = number_.size() - 1; i >= 0; --i) {
      if (number_[i] != 0) {
        number_[i] *= -1;
        break;
      }
    }
  } else if (number[0] != '-') {
    number_.push_back(number[0] - '0');
  }

  DeleteLeadNulls(number_);
}

// ========================================================================= //
// =========================== CONSTRUCTOR INT64_T ========================= //
// ========================================================================= //

BigInt::BigInt(int64_t number) {
  if (number == 0) {
    number_.push_back(0);
  } else {
    is_negative_ = number < 0;
    number_ = IntToVector(number);
  }
}

// ========================================================================= //
// ============================= COPY CONSTRUCTOR ========================== //
// ========================================================================= //

BigInt::BigInt(const BigInt& other) {
  is_negative_ = other.is_negative_;
  number_ = other.number_;
}

// ========================================================================= //
// ============================= OPERATOR "=" ============================== //
// ========================================================================= //

BigInt& BigInt::operator=(const BigInt& other) {
  is_negative_ = other.is_negative_;
  number_ = other.number_;

  return *this;
}

// ========================================================================= //
// ============================= OPERATOR "+=" ============================= //
// ========================================================================= //

BigInt& BigInt::operator+=(const BigInt& other) {
  BigInt number_copy1 = *this;
  BigInt number_copy2 = other;

  if (!is_negative_ && !other.is_negative_) {
    number_ = SumForPositive(number_, other.number_);
  } else if (is_negative_ && other.is_negative_) {
    number_copy1.number_[number_copy1.number_.size() - 1] *= -1;
    number_copy2.number_[number_copy2.number_.size() - 1] *= -1;

    number_ = SumForPositive(number_copy1.number_, number_copy2.number_);
    number_[number_.size() - 1] *= -1;
  } else if ((is_negative_ && !other.is_negative_)) {
    number_ = MinusForPositive(Abs(*this).number_, Abs(other).number_);

    if (Abs(number_copy1) > Abs(number_copy2)) {
      number_[number_.size() - 1] *= -1;
    } else {
      is_negative_ = false;
    }
  } else {
    number_ = MinusForPositive(Abs(*this).number_, Abs(other).number_);

    if (Abs(number_copy1) < Abs(number_copy2)) {
      number_[number_.size() - 1] *= -1;
      is_negative_ = true;
    }
  }

  return *this;
}

// ========================================================================= //
// ============================= OPERATOR "-=" ============================= //
// ========================================================================= //

BigInt BigInt::operator-=(const BigInt& other) {
  BigInt copy = other;

  if (is_negative_ && other.is_negative_) {
    *this += Abs(other);
  } else if (!is_negative_ && other.is_negative_) {
    *this += Abs(other);
  } else if (is_negative_ && !other.is_negative_) {
    copy.number_[copy.number_.size() - 1] *= -1;
    copy.is_negative_ = true;

    *this += copy;
  } else {
    copy.number_[copy.number_.size() - 1] *= -1;
    copy.is_negative_ = true;

    *this += copy;
  }

  return *this;
}

// ========================================================================= //
// ============================= OPERATOR "*=" ============================= //
// ========================================================================= //

BigInt& BigInt::operator*=(const BigInt& other) {
  if (*this == 0) {
    return *this;
  }
  BigInt result = 0;
  BigInt tmp_result = 0;

  BigInt max_abs = MaxOfTwo(Abs(*this), Abs(other));
  BigInt min_abs = MinOfTwo(Abs(*this), Abs(other));

  size_t times_to_repeat = min_abs.number_.size();

  for (size_t i = 0; i < times_to_repeat; ++i) {
    tmp_result = SimpleMultiply(max_abs.number_, min_abs.number_[i]);
    AddNullsToStart(tmp_result.number_, i);

    result += tmp_result;
  }

  number_ = result.number_;

  if ((is_negative_ && other.is_negative_) ||
      (!is_negative_ && !other.is_negative_)) {
    is_negative_ = false;
  } else {
    is_negative_ = true;
    number_[number_.size() - 1] *= -1;
  }

  return *this;
}

// ========================================================================= //
// ============================= OPERATOR "/=" ============================= //
// ========================================================================= //

BigInt& BigInt::operator/=(const BigInt& other) {
  if (Abs(*this) < Abs(other) || *this == 0) {
    *this = 0;
    return *this;
  }

  std::vector<int> result = DivForPositive(Abs(*this), Abs(other));

  if ((is_negative_ && other.is_negative_) ||
      (!is_negative_ && !other.is_negative_)) {
    is_negative_ = false;
    number_ = result;
  } else {
    is_negative_ = true;
    result[0] *= -1;
    number_ = result;
  }

  *this = ReverseNumber(*this);
  return *this;
}

// ========================================================================= //
// ============================= OPERATOR "%=" ============================= //
// ========================================================================= //

BigInt& BigInt::operator%=(const BigInt& other) {
  if (Abs(*this) < Abs(other)) {
    return *this;
  }

  BigInt copy1 = *this;
  *this -= (copy1 /= other) *= other;

  return *this;
}

// ========================================================================= //
// ============================ PREFIX "++" ================================ //
// ========================================================================= //

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}

// ========================================================================= //
// ============================== POSTFIX "++" ============================= //
// ========================================================================= //

BigInt BigInt::operator++(int) {
  BigInt copy = *this;
  ++*this;

  return copy;
}

// ========================================================================= //
// =============================== PREFIX "--" ============================= //
// ========================================================================= //

BigInt& BigInt::operator--() {
  *this -= 1;
  return *this;
}

// ========================================================================= //
// ============================== POSTFIX "--" ============================= //
// ========================================================================= //

BigInt BigInt::operator--(int) {
  BigInt copy = *this;
  --*this;

  return copy;
}

// ========================================================================= //
// ============================== UNARY "-" ================================ //
// ========================================================================= //

BigInt& BigInt::operator-() {
  *this *= -1;

  return *this;
}

// ========================================================================= //
// ========================= COMPARATIVE OPERATORS ========================= //
// ========================================================================= //

bool BigInt::operator<(const BigInt& other) const {
  if (is_negative_ && !other.is_negative_) {
    return true;
  }
  if (!is_negative_ && other.is_negative_) {
    return false;
  }
  if (is_negative_ && other.is_negative_ &&
      number_.size() != other.number_.size()) {
    return number_.size() >= other.number_.size();
  }
  if (!is_negative_ && !other.is_negative_ &&
      number_.size() != other.number_.size()) {
    return number_.size() < other.number_.size();
  }
  bool flag_for_negative = false;
  bool is_less = IsLess(*this, other, flag_for_negative);

  if (!is_negative_ && !other.is_negative_) {
    return is_less;
  }
  if (!flag_for_negative) {
    return !is_less;
  }
  return is_less;
}

bool BigInt::operator<=(const BigInt& other) const {
  return *this < other || (!(*this < other) && !(other < *this));
}

bool BigInt::operator>(const BigInt& other) const { return other < *this; }

bool BigInt::operator>=(const BigInt& other) const { return !(*this < other); }

bool BigInt::operator==(const BigInt& other) const {
  return !(*this < other) && !(other < *this);
}

bool BigInt::operator!=(const BigInt& other) const {
  return !(!(*this < other) && !(other < *this));
}

// ========================================================================= //
// ================================ DESTRUCTOR ============================= //
// ========================================================================= //

BigInt::~BigInt() = default;

// ========================================================================= //
// ============================= OPERATOR "+" ============================== //
// ========================================================================= //

BigInt operator+(const BigInt& number1, const BigInt& number2) {
  BigInt copy = number1;
  copy += number2;

  return copy;
}

// ========================================================================= //
// ============================= OPERATOR "-" ============================== //
// ========================================================================= //

BigInt operator-(const BigInt& number1, const BigInt& number2) {
  BigInt copy = number1;

  copy -= number2;
  return copy;
}

// ========================================================================= //
// ============================= OPERATOR "*" ============================== //
// ========================================================================= //

BigInt operator*(const BigInt& number1, const BigInt& number2) {
  BigInt copy = number1;
  copy *= number2;

  return copy;
}

// ========================================================================= //
// ============================= OPERATOR "/" ============================== //
// ========================================================================= //

BigInt operator/(const BigInt& number1, const BigInt& number2) {
  BigInt copy = number1;
  copy /= number2;

  return copy;
}

// ========================================================================= //
// ============================= OPERATOR "%" ============================== //
// ========================================================================= //

BigInt operator%(const BigInt& number1, const BigInt& number2) {
  BigInt copy = number1;
  copy %= number2;

  return copy;
}

// ========================================================================= //
// ============================= INPUT / OUTPUT ============================ //
// ========================================================================= //

std::ostream& operator<<(std::ostream& out, const BigInt& number) {
  size_t size = number.number_.size();
  for (size_t i = 0; i < size; ++i) {
    out << number.number_[size - i - 1];
  }

  return out;
}

std::istream& operator>>(std::istream& ins, BigInt& number) {
  std::string str;
  char input_string;
  while (ins.get(input_string)) {
    if (input_string == '\n' || input_string == ' ') {
      break;
    }
    str.push_back(input_string);
  }
  BigInt tmp(str);
  number = tmp;

  return ins;
}
