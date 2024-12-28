#pragma once
#include <iostream>
#include <vector>

class BigInt {
 public:
  BigInt();

  // =========================================================================
  // =========================== CONSTRUCTOR STRING ==========================
  // =========================================================================

  BigInt(const std::string& number);

  // =========================================================================
  // =========================== CONSTRUCTOR INT64_T =========================
  // =========================================================================

  BigInt(int64_t number);

  // =========================================================================
  // ============================= COPY CONSTRUCTOR ==========================
  // =========================================================================
  // // //

  BigInt(const BigInt& other);

  // =========================================================================
  // ============================= OPERATOR "=" ==============================
  // =========================================================================

  BigInt& operator=(const BigInt& other);

  // =========================================================================
  // ============================= OPERATOR "+=" =============================
  // =========================================================================
  BigInt& operator+=(const BigInt& other);

  // =========================================================================
  // ============================= OPERATOR "-=" =============================
  // =========================================================================

  BigInt operator-=(const BigInt& other);

  // =========================================================================
  // ============================= OPERATOR "*=" =============================
  // =========================================================================

  BigInt& operator*=(const BigInt& other);

  // =========================================================================
  // ============================= OPERATOR "/=" =============================
  // =========================================================================

  BigInt& operator/=(const BigInt& other);

  // =========================================================================
  // ============================= OPERATOR "%=" =============================
  // =========================================================================

  BigInt& operator%=(const BigInt& other);

  // =========================================================================
  // ============================ PREFIX "++" ================================
  // =========================================================================

  BigInt& operator++();

  // =========================================================================
  // ============================== POSTFIX "++" =============================
  // =========================================================================

  BigInt operator++(int);

  // =========================================================================
  // =============================== PREFIX "--" =============================
  // =========================================================================

  BigInt& operator--();

  // =========================================================================
  // ============================== POSTFIX "--" =============================
  // =========================================================================
  BigInt operator--(int);

  // =========================================================================
  // ============================== UNARY "-" ================================
  // =========================================================================

  BigInt& operator-();

  // =========================================================================
  // ========================= COMPARATIVE OPERATORS =========================
  // =========================================================================

  bool operator<(const BigInt& other) const;

  bool operator<=(const BigInt& other) const;

  bool operator>(const BigInt& other) const;

  bool operator>=(const BigInt& other) const;

  bool operator==(const BigInt& other) const;

  bool operator!=(const BigInt& other) const;

  // =========================================================================
  // ================================ DESTRUCTOR =============================
  // =========================================================================

  ~BigInt();

 private:
  std::vector<int> number_;
  bool is_negative_ = false;
  const int kBaseDelim = 10;

  // =========================================================================
  // =========================== INT_TO_STR_CONVERTING =======================
  // =========================================================================

  std::vector<int> IntToVector(int64_t number) {
    std::vector<int> digits;

    if (number < 0) {
      is_negative_ = true;
    }

    while (number != 0) {
      digits.push_back(number % kBaseDelim);
      number /= kBaseDelim;
    }
    if (is_negative_) {
      for (int i = 0; i < static_cast<int>(digits.size()) - 1; ++i) {
        digits[i] *= -1;
      }
    }

    return digits;
  }

  // =========================================================================
  // ========================= ADD NULLS TO THE END ==========================
  // =========================================================================

  void static AddNullsToEnd(std::vector<int>& number, size_t times) {
    for (size_t i = 0; i < times; ++i) {
      number.push_back(0);
    }
  }

  // =========================================================================
  // ========================= ADD NULLS TO THE START ========================
  // =========================================================================

  void static AddNullsToStart(std::vector<int>& number, size_t times) {
    std::vector<int> copy = number;

    number.resize(copy.size() + times);

    for (size_t i = 0; i < times; ++i) {
      number[i] = 0;
    }

    for (size_t i = times; i < number.size(); ++i) {
      number[i] = copy[i - times];
    }
  }

  // =========================================================================
  // =========================== DELETE LEAD NULLS ===========================
  // =========================================================================

  void static DeleteLeadNulls(std::vector<int>& number) {
    for (size_t i = number.size() - 1; i > 0; --i) {
      if (number[i] == 0) {
        number.pop_back();
      } else {
        break;
      }
    }
  }

  // =========================================================================
  // ================================ ABS ====================================
  // =========================================================================

  BigInt static Abs(const BigInt& number) {
    BigInt copy = number;
    if (copy.number_[number.number_.size() - 1] < 0) {
      copy.number_[number.number_.size() - 1] *= -1;
    }
    copy.is_negative_ = false;
    return copy;
  }

  // =========================================================================
  // ============================= MAX NUMBER ================================
  // =========================================================================

  const static BigInt& MaxOfTwo(const BigInt& number1, const BigInt& number2) {
    if (number1 >= number2) {
      return number1;
    }
    return number2;
  }

  // =========================================================================
  // ============================= MIN NUMBER ================================
  // =========================================================================

  const static BigInt& MinOfTwo(const BigInt& number1, const BigInt& number2) {
    if (number1 <= number2) {
      return number1;
    }
    return number2;
  }

  // =========================================================================
  // ======================== SUM FOR POSITIVE ===============================
  // =========================================================================

  std::vector<int> SumForPositive(std::vector<int> number1,
                                  std::vector<int> number2) const {
    std::vector<int> result;
    result.resize(std::max(number1.size(), number2.size()) + 1, 0);

    if (number1.size() > number2.size()) {
      AddNullsToEnd(number2, number1.size() - number2.size());
    } else if (number1.size() < number2.size()) {
      AddNullsToEnd(number1, number2.size() - number1.size());
    }

    int add_point = 0;
    for (size_t i = 0; i < number1.size(); ++i) {
      int result_in_column = number1[i] + number2[i] + add_point;
      if (result_in_column >= kBaseDelim) {
        add_point = 1;
      } else {
        add_point = 0;
      }
      result[i] = result_in_column % kBaseDelim;
    }
    result[result.size() - 1] += add_point;

    DeleteLeadNulls(result);
    return result;
  }

  // =========================================================================
  // ======================== MINUS FOR POSITIVE =============================
  // =========================================================================

  std::vector<int> MinusForPositive(std::vector<int> number1,
                                    std::vector<int> number2) const {
    BigInt copy1 = 0;
    copy1.number_ = number1;
    BigInt copy2 = 0;
    copy2.number_ = number2;
    std::vector<int> max_abs;
    std::vector<int> min_abs;
    std::vector<int> result;
    if (copy1 >= copy2) {
      max_abs = number1;
      min_abs = number2;
    } else {
      max_abs = number2;
      min_abs = number1;
    }
    AddNullsToEnd(min_abs, max_abs.size() - min_abs.size());
    int del_point = 0;
    for (size_t i = 0; i < max_abs.size(); ++i) {
      if (max_abs[i] + del_point < min_abs[i]) {
        max_abs[i] += kBaseDelim;
        result.push_back(max_abs[i] - min_abs[i] + del_point);
        del_point = -1;
      } else {
        result.push_back(max_abs[i] - min_abs[i] + del_point);
        del_point = 0;
      }
    }
    DeleteLeadNulls(result);
    return result;
  }

  // =========================================================================
  // ============== MULTIPLY ON SIMPLE NUMBER FOR POSITIVE ===================
  // =========================================================================

  BigInt SimpleMultiply(std::vector<int> number, int digit) const {
    std::vector<int> result;
    result.resize(number.size() + 1, 0);

    int add_point = 0;
    for (size_t i = 0; i < number.size(); ++i) {
      int result_in_column = number[i] * digit + add_point;
      if (result_in_column >= kBaseDelim) {
        add_point = (number[i] * digit + add_point) / kBaseDelim;
      } else {
        add_point = 0;
      }
      result[i] = result_in_column % kBaseDelim;
    }

    result[result.size() - 1] += add_point;

    DeleteLeadNulls(result);

    BigInt result_number = 0;
    result_number.number_ = result;
    return result_number;
  }

  // =========================================================================
  // ============================= REVERSE NUMBER ============================
  // =========================================================================

  BigInt static ReverseNumber(const BigInt& number) {
    BigInt reversed_number = number;
    for (size_t i = 0; i < number.number_.size(); ++i) {
      reversed_number.number_[i] =
          number.number_[number.number_.size() - i - 1];
    }

    return reversed_number;
  }

  // =========================================================================
  // =========================== DIV AlGORITHM ===============================
  // =========================================================================

  std::vector<int> static DivAlgorithm(BigInt& tmp, const BigInt& number1,
                                       const BigInt& right) {
    std::vector<int> result;
    size_t nulls_to_push = 0;
    size_t pos = tmp.number_.size() - 1;
    while (pos < number1.number_.size()) {
      if (ReverseNumber(tmp) < right && pos + 1 < number1.number_.size()) {
        if (tmp == 0) {
          tmp.number_.clear();
        }
        tmp.number_.push_back(number1.number_[pos + 1]);
        if (nulls_to_push != 0) {
          result.push_back(0);
        }
        ++nulls_to_push;
        ++pos;
      } else {
        int times = 0;
        while (ReverseNumber(tmp) >= right) {
          tmp = ReverseNumber(ReverseNumber(tmp) -= right);
          ++times;
        }
        result.push_back(times);
        nulls_to_push = 0;
        if (pos == number1.number_.size() - 1) {
          break;
        }
      }
    }
    return result;
  }

  // =========================================================================
  // ======================== DIV FOR POSITIVE ===============================
  // =========================================================================

  std::vector<int> static DivForPositive(const BigInt& left,
                                         const BigInt& right) {
    BigInt number1 = ReverseNumber(left);
    BigInt number2 = ReverseNumber(right);

    BigInt tmp_number;

    for (size_t i = 0; i < number2.number_.size(); ++i) {
      tmp_number.number_.push_back(number1.number_[i]);
    }

    return DivAlgorithm(tmp_number, number1, right);
  }

  // =========================================================================
  // ============================ IS LESS ====================================
  // =========================================================================

  bool static IsLess(const BigInt& number1, const BigInt& number2,
                     bool& flag_for_negative) {
    bool is_less = false;

    for (int i = number1.number_.size() - 1; i >= 0; --i) {
      if (number1.number_[i] > number2.number_[i]) {
        is_less = false;
        flag_for_negative = false;
        break;
      }
      if (number1.number_[i] < number2.number_[i]) {
        is_less = true;
        flag_for_negative = false;
        break;
      }
      flag_for_negative = true;
      is_less = false;
    }

    return is_less;
  }

  friend std::ostream& operator<<(std::ostream& out, const BigInt& number);
  friend std::istream& operator>>(std::istream& ins, BigInt& number);
};

// ========================================================================= //
// ============================= OPERATOR "+" ============================== //
// ========================================================================= //

BigInt operator+(const BigInt& number1, const BigInt& number2);

// ========================================================================= //
// ============================= OPERATOR "-" ============================== //
// ========================================================================= //

BigInt operator-(const BigInt& number1, const BigInt& number2);

// ========================================================================= //
// ============================= OPERATOR "*" ============================== //
// ========================================================================= //

BigInt operator*(const BigInt& number1, const BigInt& number2);

// ========================================================================= //
// ============================= OPERATOR "/" ============================== //
// ========================================================================= //

BigInt operator/(const BigInt& number1, const BigInt& number2);

// ========================================================================= //
// ============================= OPERATOR "%" ============================== //
// ========================================================================= //

BigInt operator%(const BigInt& number1, const BigInt& number2);

// ========================================================================= //
// ============================= INPUT / OUTPUT ============================ //
// ========================================================================= //

std::ostream& operator<<(std::ostream& out, const BigInt& number);

std::istream& operator>>(std::istream& ins, BigInt& number);
