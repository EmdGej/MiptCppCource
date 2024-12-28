#include "string.hpp"

// ========================================================================= //
// ============================== CONSTRUCTORS ============================= //
// ========================================================================= //

String::String() {
  size_ = capacity_ = 0;
  string_ = new char[size_];  // size_ == 0
}

// constructor with params: size, character
String::String(size_t size, char character) {
  size_ = capacity_ = size;
  string_ = new char[capacity_ + 1];

  memset(string_, character, size);
  string_[size_] = '\0';
}

// constructor with params: string
String::String(const char* string) {
  size_ = capacity_ = strlen(string);
  string_ = new char[capacity_ + 1];

  memcpy(string_, string, size_);
  string_[size_] = '\0';
}

// copy constructor
String::String(const String& other) {
  size_ = other.size_;
  capacity_ = other.capacity_;

  string_ = new char[capacity_ + 1];
  memcpy(string_, other.string_, size_);
  string_[size_] = '\0';
}
// ========================================================================= //
// ============================== OPERATOR "=" ============================= //
// ========================================================================= //

String& String::operator=(const String& other) {
  String copy = other;
  Swap(copy);
  return *this;
}

// ========================================================================= //
// ============================ [] OPERATOR [] ============================= //
// ========================================================================= //

char& String::operator[](size_t index) { return string_[index]; }

const char& String::operator[](size_t index) const { return string_[index]; }

// ========================================================================= //
// ========================== COMPARISON OPERATORS ========================= //
// ========================================================================= //

bool String::operator<(const String& other) const {
  size_t min_size = std::min(size_, other.size_);
  for (size_t i = 0; i < min_size; ++i) {
    if (string_[i] < other.string_[i]) {
      return true;
    }
    if (string_[i] > other.string_[i]) {
      return false;
    }
  }
  return size_ < other.size_;
}

bool String::operator<=(const String& other) const {
  return (*this < other) || (!(*this < other) && !(other < *this));
}

bool String::operator>(const String& other) const { return !(*this <= other); }

bool String::operator>=(const String& other) const { return !(*this < other); }

bool String::operator==(const String& other) const {
  return (*this <= other) && (other <= *this);
}

bool String::operator!=(const String& other) const { return !(*this == other); }

// ========================================================================= //
// ============================== CONCATENATE "+=" ========================= //
// ========================================================================= //

String& String::operator+=(const String& other) {
  size_t tmp_size = size_;
  if (capacity_ < size_ + other.size_) {
    Resize(capacity_ + other.capacity_);
  }

  memcpy(string_ + tmp_size, other.string_, other.size_);
  string_[size_] = '\0';

  return *this;
}

// ========================================================================= //
// ============================= MULTIPLY "*=" ============================= //
// ========================================================================= //

String& String::operator*=(size_t number) {
  String copy = *this;
  Resize(size_ * number);
  for (size_t i = 0; i < number; ++i) {
    memcpy(string_ + copy.size_ * i, copy.string_, copy.size_);
  }
  string_[size_] = '\0';
  return *this;
}

// ========================================================================= //
// ============================ GET-VALUE METHODS ========================== //
// ========================================================================= //

void String::Clear() {
  size_ = 0;
  string_[0] = '\0';
}

bool String::Empty() const { return size_ == 0; }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

const char* String::Data() const { return string_; }

char* String::Data() { return string_; }

// ========================================================================= //
// ========================== GET/SET FIRST/LAST =========================== //
// ========================================================================= //

char& String::Front() { return string_[0]; }

const char& String::Front() const { return string_[0]; }

char& String::Back() { return string_[size_ - 1]; }

const char& String::Back() const { return string_[size_ - 1]; }

// ========================================================================= //
// ============================ PUSH/POP METHODS =========================== //
// ========================================================================= //

void String::PushBack(char character) {
  if (size_ == capacity_) {
    CreateNewBuffer();
  }

  string_[size_] = character;
  string_[size_ + 1] = '\0';
  ++size_;
}

void String::PopBack() {
  if (size_ > 0) {
    --size_;
    string_[size_] = '\0';
  }
}

// ========================================================================= //
// ====================== SIZE / CAPACITY CHANGE METHODS =================== //
// ========================================================================= //

// Default Resize - without filling
void String::Resize(size_t new_size) {
  if (new_size > capacity_) {
    CreateNewBuffer(new_size);
    memset(string_ + size_, '\0', new_size - size_);
  }
  size_ = new_size;
  string_[size_] = '\0';
}

// Resize - with filling
void String::Resize(size_t new_size, char character) {
  if (new_size > capacity_) {
    CreateNewBuffer(new_size);
  }
  if (new_size > size_) {
    memset(string_ + size_, character, new_size - size_);
  }
  size_ = new_size;
  string_[size_] = '\0';
}

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    CreateNewBuffer(new_cap);
  }
  string_[size_] = '\0';
}

void String::ShrinkToFit() {
  if (capacity_ > size_) {
    CreateNewBuffer(size_);
  }
  string_[size_] = '\0';
}

// ========================================================================= //
// =================================== SWAP ================================ //
// ========================================================================= //

void String::Swap(String& other) {
  SwapFields(size_, other.size_);
  SwapFields(capacity_, other.capacity_);
  SwapFields(string_, other.string_);
}

// ========================================================================= //
// ================================== SPLIT ================================ //
// ========================================================================= //

std::vector<String> String::Split(const String& delim) {
  size_t i = 0;
  std::vector<size_t> positions;
  std::vector<String> strings;
  if (delim == "") {
    strings.push_back(string_);
    return strings;
  }

  while (i + delim.size_ <= size_) {
    if (this->Substring(i, i + delim.size_) == delim) {
      positions.push_back(i);
      positions.push_back(i + delim.size_ - 1);

      i += delim.size_;
      continue;
    }
    ++i;
  }
  if (positions.empty()) {
    strings.push_back(string_);
  } else {
    strings.push_back(Substring(0, positions[0]));
    for (size_t i = 1; i < positions.size() - 1; i += 2) {
      strings.push_back(Substring(positions[i] + 1, positions[i + 1]));
    }
    strings.push_back(Substring(positions[positions.size() - 1] + 1, size_));
  }
  return strings;
}

// ========================================================================= //
// ================================== JOIN ================================= //
// ========================================================================= //

String String::Join(const std::vector<String>& strings) const {
  if (strings.empty()) {
    return "";
  }
  String str;
  for (size_t i = 0; i < strings.size() - 1; ++i) {
    str += strings[i];
    str += *this;
  }
  str += strings[strings.size() - 1];
  str[str.Size()] = '\0';

  return str;
}

// ========================================================================= //
// =============================== DESTRUCTOR ============================== //
// ========================================================================= //

String::~String() {
  delete[] string_;
  // string_ = nullptr;
}

// ========================================================================= //
// ============================== CONCATENATE "+"  ========================= //
// ========================================================================= //

String operator+(const String& str1, const String& str2) {
  String copy = str1;
  copy += str2;
  return copy;
}

// ========================================================================= //
// ============================== MULTIPLY "*"  ============================ //
// ========================================================================= //

String operator*(const String& str, size_t number) {
  String new_str = str;
  new_str *= number;

  return new_str;
}

// ========================================================================= //
// ============================= INPUT / OUTPUT ============================ //
// ========================================================================= //
std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.Size(); ++i) {
    out << str[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& str) {
  char input_string;
  while (in.get(input_string)) {
    if (input_string == '\n') {
      break;
    }
    str.PushBack(input_string);
  }
  str[str.Size()] = '\0';

  return in;
}
