#pragma once

#include <string.h>

#include <iostream>
#include <vector>

class String {
 public:
  // =========================================================================
  // ============================== CONSTRUCTORS =============================
  // =========================================================================

  // default constructor - creates empty string
  String();

  // constructor with params: size, character
  explicit String(size_t size, char character);

  // constructor with params: string
  String(const char* string);

  // copy constructor
  String(const String& other);

  // =========================================================================
  // ============================== OPERATOR "=" =============================
  // =========================================================================

  String& operator=(const String& other);

  // =========================================================================
  // ============================ [] OPERATOR [] =============================
  // =========================================================================

  char& operator[](size_t index);

  const char& operator[](size_t index) const;

  // =========================================================================
  // ========================== COMPARISON OPERATORS =========================
  // =========================================================================

  bool operator<(const String& other) const;
  bool operator<=(const String& other) const;
  bool operator>(const String& other) const;
  bool operator>=(const String& other) const;
  bool operator==(const String& other) const;
  bool operator!=(const String& other) const;

  // =========================================================================
  // ============================== CONCATENATE "+=" =========================
  // =========================================================================

  String& operator+=(const String& other);

  // =========================================================================
  // ============================= MULTIPLY "*=" =============================
  // =========================================================================

  String& operator*=(size_t number);

  // =========================================================================
  // ============================ GET-VALUE METHODS ==========================
  // =========================================================================

  void Clear();

  bool Empty() const;

  size_t Size() const;

  size_t Capacity() const;

  const char* Data() const;

  char* Data();

  // =========================================================================
  // ========================== GET/SET FIRST/LAST ===========================
  // =========================================================================

  char& Front();

  const char& Front() const;

  char& Back();

  const char& Back() const;

  // =========================================================================
  // ============================ PUSH/POP METHODS ===========================
  // =========================================================================

  void PushBack(char character);
  void PopBack();

  // =========================================================================
  // ====================== SIZE / CAPACITY CHANGE METHODS ===================
  // =========================================================================

  // Default Resize - without filling
  void Resize(size_t new_size);
  // Resize - with filling
  void Resize(size_t new_size, char character);

  void Reserve(size_t new_cap);

  void ShrinkToFit();

  // =========================================================================
  // =================================== SWAP ================================
  // =========================================================================

  void Swap(String& other);
  // =========================================================================
  // ================================== SPLIT ================================
  // =========================================================================

  std::vector<String> Split(const String& delim = " ");

  // =========================================================================
  // ================================== JOIN =================================
  // =========================================================================

  String Join(const std::vector<String>& strings) const;

  // =========================================================================
  // =============================== DESTRUCTOR ==============================
  // =========================================================================

  ~String();

 private:
  // =========================================================================
  // ========================== PRIVATE VARIABLES ============================
  // =========================================================================

  char* string_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;
  const int kGrowthRate = 2;

  // =========================================================================
  // ============================= NEW BUFFER ================================
  // =========================================================================

  void CreateNewBuffer() {
    String temp_copy = *this;
    delete[] string_;

    if (capacity_ != 0) {
      capacity_ *= kGrowthRate;
    } else {
      capacity_ = 1;
    }

    string_ = new char[capacity_ + 1];

    memcpy(string_, temp_copy.string_, size_);
  }

  void CreateNewBuffer(int new_capacity) {
    String temp_copy = *this;
    delete[] string_;

    capacity_ = new_capacity;
    string_ = new char[capacity_ + 1];

    memcpy(string_, temp_copy.string_, size_);
  }

  // =========================================================================
  // ============================== SUBSTRING ================================
  // =========================================================================

  String Substring(size_t start, size_t end) {
    String str;
    if (end - start > 1) {
      for (size_t i = start; i < end; ++i) {
        str.PushBack(string_[i]);
      }
    } else if (end - start == 1) {
      str.Resize(1);
      str[0] = string_[start];
    }
    return str;
  }

  // =========================================================================
  // ============================= SWAP FIELDS ===============================
  // =========================================================================

  template <typename T>
  void SwapFields(T& field1, T& field2) {
    T tmp_field = field1;
    field1 = field2;
    field2 = tmp_field;
  }
};

String operator+(const String& str1, const String& str2);

String operator*(const String& str, size_t number);

std::ostream& operator<<(std::ostream& out, const String& str);

std::istream& operator>>(std::istream& in, String& str);
