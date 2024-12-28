#include <iostream>

template <typename T>
class Deque;

template <bool IsConst, typename T, typename F>
struct Conditional {
  using type = F;
};

template <typename T, typename F>
struct Conditional<true, T, F> {
  using type = T;
};

template <bool IsConst, typename T, typename F>
using conditional_t = typename Conditional<IsConst, T, F>::type;

// ============================================================ //
// ===================== REVERSEITERATOR ====================== //
// ============================================================ //

namespace rev_iter {
template <typename Iterator>
struct ReverseIterator {
 public:
  ReverseIterator(const Iterator& iter) : it_(iter) {}

  using value_type = typename Iterator::value_type;
  using pointer = typename Iterator::pointer;
  using iterator_category = std::random_access_iterator_tag;
  using reference = typename Iterator::reference;
  using difference_type = std::ptrdiff_t;

  reference operator*() const { return *it_; }
  pointer operator->() const { return it_.operator->(); }

  ReverseIterator& operator++() {
    --it_;
    return *this;
  }

  ReverseIterator operator++(int) {
    ReverseIterator copy = *this;
    ++it_;
    return copy;
  }

  ReverseIterator& operator--() {
    ++it_;
    return *this;
  }

  ReverseIterator operator--(int) {
    ReverseIterator copy = *this;
    --it_;
    return copy;
  }

  ReverseIterator& operator+=(int64_t number) {
    it_ -= number;
    return *this;
  }

  ReverseIterator& operator-=(int64_t number) {
    it_ += number;
    return *this;
  }

  ReverseIterator operator+(int64_t number) const {
    ReverseIterator copy = *this;
    copy += number;
    return copy;
  }

  ReverseIterator operator-(int64_t number) const {
    ReverseIterator copy = *this;
    copy -= number;
    return copy;
  }

  int64_t operator-(const ReverseIterator& other) const {
    return other.it_ - it_;
  }

  bool operator<(const ReverseIterator& other) const {
    return (it_ - other.it_) > 0;
  }

  bool operator>(const ReverseIterator& other) const { return other < *this; }

  bool operator<=(const ReverseIterator& other) const {
    return !(*this > other);
  }

  bool operator>=(const ReverseIterator& other) const {
    return !(*this < other);
  }

  bool operator==(const ReverseIterator& other) const {
    return !(*this < other) && !(*this > other);
  }

  bool operator!=(const ReverseIterator& other) const {
    return !(*this == other);
  }

 private:
  Iterator it_;
};
}  // namespace rev_iter

template <typename T>
class Deque {
 public:
  // ============================================================ //
  // ======================= CONSTRUCTORS ======================= //
  // ============================================================ //

  Deque() : pointers_array_size_(kCreationCf) {
    pointers_array_ = new T*[kCreationCf];

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          reinterpret_cast<T*>(new int8_t[kBlockArraySize * sizeof(T)]);
    }
  }

  Deque(size_t count)
      : pointers_array_size_(
            count == 0
                ? 0
                : kCreationCf * ((count / kBlockArraySize) +
                                 ((count % kBlockArraySize) == 0 ? 0 : 1))),
        deque_size_(count),
        end_i_(count == 0 ? 0 : (pointers_array_size_ / 2 - 1)),
        end_j_(count == 0                     ? 0
               : count % kBlockArraySize == 0 ? kBlockArraySize - 1
                                              : count % kBlockArraySize - 1) {
    pointers_array_ = new T*[pointers_array_size_];
    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          reinterpret_cast<T*>(new int8_t[kBlockArraySize * sizeof(T)]);
    }

    for (int64_t i = begin_i_; i <= end_i_; ++i) {
      for (size_t j = 0; i == end_i_ ? j <= end_j_ : j < kBlockArraySize; ++j) {
        new (pointers_array_[i] + j) T();
      }
    }
  }

  Deque(size_t count, const T& value)
      : pointers_array_size_(
            count == 0
                ? 0
                : kCreationCf * ((count / kBlockArraySize) +
                                 ((count % kBlockArraySize) == 0 ? 0 : 1))),
        deque_size_(count),
        end_i_(count == 0 ? 0 : (pointers_array_size_ / 2 - 1)),
        end_j_(count == 0 ? 0
                          : (count % kBlockArraySize == 0
                                 ? kBlockArraySize - 1
                                 : count % kBlockArraySize - 1)) {
    pointers_array_ = new T*[pointers_array_size_];
    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          reinterpret_cast<T*>(new int8_t[kBlockArraySize * sizeof(T)]);
    }

    fill_deque(value);
  }

  Deque(const Deque& other)
      : kBlockArraySize(other.kBlockArraySize),
        pointers_array_size_(other.pointers_array_size_),
        deque_size_(other.deque_size_),
        begin_i_(other.begin_i_),
        begin_j_(other.begin_j_),
        end_i_(other.end_i_),
        end_j_(other.end_j_) {
    pointers_array_ = new T*[pointers_array_size_];
    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          reinterpret_cast<T*>(new int8_t[kBlockArraySize * sizeof(T)]);
    }

    copy_deque(other);
  }

  // ============================================================ //
  // ======================= OPERATOR "=" ======================= //
  // ============================================================ //

  Deque<T>& operator=(Deque other) {
    swap_deque(other);
    return *this;
  }

  // ============================================================ //
  // =========================== SIZE =========================== //
  // ============================================================ //

  size_t size() const { return deque_size_; }

  // ============================================================ //
  // ========================== EMPTY =========================== //
  // ============================================================ //

  bool empty() const { return deque_size_ == 0; }

  // ============================================================ //
  // ======================= OPERATOR [] ======================== //
  // ============================================================ //

  T& operator[](size_t index) {
    return pointers_array_[begin_i_ + static_cast<int64_t>((begin_j_ + index) /
                                                           kBlockArraySize)]
                          [(begin_j_ + index) % kBlockArraySize];
  }
  const T& operator[](size_t index) const {
    return pointers_array_[begin_i_ + static_cast<int64_t>((begin_j_ + index) /
                                                           kBlockArraySize)]
                          [(begin_j_ + index) % kBlockArraySize];
  }

  // ============================================================ //
  // ======================= OPERATOR AT ======================== //
  // ============================================================ //

  T& at(size_t index) {
    if (index >= deque_size_) {
      throw std::out_of_range("");
    }
    return pointers_array_[begin_i_ + static_cast<int64_t>((begin_j_ + index) /
                                                           kBlockArraySize)]
                          [(begin_j_ + index) % kBlockArraySize];
  }

  const T& at(size_t index) const {
    if (index >= deque_size_) {
      throw std::out_of_range("");
    }
    return pointers_array_[begin_i_ + static_cast<int64_t>((begin_j_ + index) /
                                                           kBlockArraySize)]
                          [(begin_j_ + index) % kBlockArraySize];
  }

  // ============================================================ //
  // ========================= PUSHBACK ========================= //
  // ============================================================ //

  void push_back(const T& value) {
    if (deque_size_ == 0) {
      new (pointers_array_[end_i_]) T(value);
      ++deque_size_;
      return;
    }
    if (end_i_ == static_cast<int64_t>(pointers_array_size_ - 1) &&
        end_j_ == kBlockArraySize - 1) {
      reserve(pointers_array_size_ * kCreationCf);
      new (pointers_array_[end_i_ + 1]) T(value);
      ++end_i_;
      end_j_ = 0;
    } else {
      if (end_j_ == kBlockArraySize - 1) {
        new (pointers_array_[end_i_ + 1]) T(value);
        ++end_i_;
        end_j_ = 0;
      } else {
        new (pointers_array_[end_i_] + end_j_ + 1) T(value);
        ++end_j_;
      }
    }
    ++deque_size_;
  }

  // ============================================================ //
  // ========================= PUSHFRONT ======================== //
  // ============================================================ //

  void push_front(const T& value) {
    if (deque_size_ == 0) {
      new (pointers_array_[begin_i_]) T(value);
      ++deque_size_;
      return;
    }
    if (begin_i_ == 0 && begin_j_ == 0) {
      reserve(pointers_array_size_ * kCreationCf);
      new (pointers_array_[begin_i_ - 1] + kBlockArraySize - 1) T(value);
      --begin_i_;
      begin_j_ = kBlockArraySize - 1;
    } else {
      if (begin_j_ == 0) {
        new (pointers_array_[begin_i_ - 1] + kBlockArraySize - 1) T(value);
        --begin_i_;
        begin_j_ = kBlockArraySize - 1;
      } else {
        new (pointers_array_[begin_i_] + begin_j_ - 1) T(value);
        --begin_j_;
      }
    }
    ++deque_size_;
  }

  // ============================================================ //
  // ========================= POPBACK ========================== //
  // ============================================================ //

  void pop_back() {
    (pointers_array_[end_i_] + end_j_)->~T();
    if (end_j_ == 0) {
      --end_i_;
      end_j_ = kBlockArraySize - 1;
    } else {
      --end_j_;
    }
    --deque_size_;
  }

  // ============================================================ //
  // ======================== POPFRONT ========================== //
  // ============================================================ //

  void pop_front() {
    (pointers_array_[begin_i_] + begin_j_)->~T();
    if (begin_j_ == kBlockArraySize - 1) {
      ++begin_i_;
      begin_j_ = 0;
    } else {
      ++begin_j_;
    }
    --deque_size_;
  }
  // ============================================================ //
  // ======================== DESTRUCTOR ======================== //
  // ============================================================ //

  ~Deque() {
    if (deque_size_ != 0) {
      for (int64_t i = begin_i_; i <= end_i_; ++i) {
        for (size_t j = (i == begin_i_ ? begin_j_ : 0);
             i == end_i_ ? j <= end_j_ : j < kBlockArraySize; ++j) {
          (pointers_array_[i] + j)->~T();
        }
      }
    }

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      delete[] reinterpret_cast<int8_t*>(pointers_array_[i]);
    }

    delete[] pointers_array_;
  }

  // ============================================================ //
  // ======================== ITERATOR ========================== //
  // ============================================================ //

  template <bool IsConst>
  class CommonIterator {
   public:
    CommonIterator(T* ptr, T** pointers_array, int64_t current_i,
                   size_t current_j)
        : ptr_(ptr),
          pointers_array_(pointers_array),
          current_i_(current_i),
          current_j_(current_j) {}

    CommonIterator(const CommonIterator& other)
        : ptr_(other.ptr_),
          pointers_array_(other.pointers_array_),
          current_i_(other.current_i_),
          current_j_(other.current_j_) {}

    using value_type = conditional_t<IsConst, const T, T>;
    using pointer = conditional_t<IsConst, const T*, T*>;
    using iterator_category = std::random_access_iterator_tag;
    using reference = conditional_t<IsConst, const T&, T&>;
    using difference_type = std::ptrdiff_t;

    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }

    CommonIterator& operator=(CommonIterator other) {
      swap(other);
      return *this;
    }

    CommonIterator& operator++() {
      if (current_j_ < kBlockArraySize - 1) {
        ++ptr_;
        ++current_j_;
      } else {
        ptr_ = &pointers_array_[++current_i_][0];
        current_j_ = 0;
      }
      return *this;
    }

    CommonIterator operator++(int) {
      CommonIterator copy = *this;
      ++*this;
      return copy;
    }

    CommonIterator& operator--() {
      if (current_j_ > 0) {
        --ptr_;
        --current_j_;
      } else {
        ptr_ = &pointers_array_[--current_i_][kBlockArraySize - 1];
        current_j_ = kBlockArraySize - 1;
      }
      return *this;
    }

    CommonIterator operator--(int) {
      CommonIterator copy = *this;
      --*this;
      return copy;
    }

    CommonIterator& operator+=(int64_t number) {
      if (number < 0) {
        *this -= number;
      } else {
        current_i_ += (current_j_ + number) / kBlockArraySize;
        current_j_ = (current_j_ + number) % kBlockArraySize;

        ptr_ = &pointers_array_[current_i_][current_j_];
      }
      return *this;
    }

    CommonIterator& operator-=(int64_t number) {
      if (number < 0) {
        *this += number;
      } else {
        current_i_ -=
            (kBlockArraySize - current_j_ - 1 + number) / kBlockArraySize;
        current_j_ =
            kBlockArraySize - 1 -
            (kBlockArraySize - current_j_ - 1 + number) % kBlockArraySize;

        ptr_ = &pointers_array_[current_i_][current_j_];
      }

      return *this;
    }

    CommonIterator operator+(int64_t number) const {
      CommonIterator copy = *this;
      copy += number;
      return copy;
    }

    CommonIterator operator-(int64_t number) const {
      CommonIterator copy = *this;
      copy -= number;
      return copy;
    }

    int64_t operator-(const CommonIterator& other) const {
      int64_t current_j_this = static_cast<int64_t>(current_j_);
      int64_t current_j_other = static_cast<int64_t>(other.current_j_);

      if (current_i_ == other.current_i_) {
        return current_j_this - current_j_other;
      }
      if (current_i_ - other.current_i_ == 1) {
        return current_j_this +
               (static_cast<int64_t>(kBlockArraySize) - 1 - current_j_this);
      }
      if (current_i_ - other.current_i_ == -1) {
        return -(static_cast<int64_t>(kBlockArraySize) - current_j_this) -
               current_j_other;
      }
      if (current_i_ > other.current_i_) {
        return (current_i_ - other.current_i_ - 1) * kBlockArraySize +
               current_j_this +
               (static_cast<int64_t>(kBlockArraySize) - 1 - current_j_other);
      }
      return -(-current_i_ + other.current_i_ - 1) * kBlockArraySize -
             (other.current_j_ +
              (static_cast<int64_t>(kBlockArraySize) - 1 - current_j_));
    }

    bool operator<(const CommonIterator& other) const {
      return (*this - other) < 0;
    }

    bool operator>(const CommonIterator& other) const { return other < *this; }

    bool operator<=(const CommonIterator& other) const {
      return !(*this > other);
    }

    bool operator>=(const CommonIterator& other) const {
      return !(*this < other);
    }

    bool operator==(const CommonIterator& other) const {
      return !(*this < other) && !(*this > other);
    }

    bool operator!=(const CommonIterator& other) const {
      return !(*this == other);
    }

   private:
    T* ptr_ = nullptr;
    T** pointers_array_;

    int64_t current_i_;
    size_t current_j_;

    const size_t kBlockArraySize = 1000;

    void swap(CommonIterator& other) {
      std::swap(ptr_, other.ptr_);
      std::swap(pointers_array_, other.pointers_array_);
      std::swap(current_i_, other.current_i_);
      std::swap(current_j_, other.current_j_);
    }
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = rev_iter::ReverseIterator<iterator>;
  using const_reverse_iterator = rev_iter::ReverseIterator<const_iterator>;

  iterator begin() {
    return iterator(&pointers_array_[begin_i_][begin_j_], pointers_array_,
                    begin_i_, begin_j_);
  }
  const_iterator begin() const {
    return const_iterator(&pointers_array_[begin_i_][begin_j_], pointers_array_,
                          begin_i_, begin_j_);
  }

  iterator end() {
    if (deque_size_ == 0) {
      return iterator(&pointers_array_[begin_i_][begin_j_], pointers_array_,
                      begin_i_, begin_j_);
    }

    return iterator(&pointers_array_[end_i_][end_j_ + 1], pointers_array_,
                    end_i_, end_j_ + 1);
  }
  const_iterator end() const {
    if (deque_size_ == 0) {
      return const_iterator(&pointers_array_[begin_i_][begin_j_],
                            pointers_array_, begin_i_, begin_j_);
    }

    return const_iterator(&pointers_array_[end_i_][end_j_ + 1], pointers_array_,
                          end_i_, end_j_ + 1);
  }

  const_iterator cbegin() const {
    return const_iterator(&pointers_array_[begin_i_][begin_j_], pointers_array_,
                          begin_i_, begin_j_);
  }
  const_iterator cend() const {
    if (deque_size_ == 0) {
      return const_iterator(&pointers_array_[begin_i_][begin_j_],
                            pointers_array_, begin_i_, begin_j_);
    }

    return const_iterator(&pointers_array_[end_i_][end_j_ + 1], pointers_array_,
                          end_i_, end_j_ + 1);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(iterator(&pointers_array_[end_i_][end_j_],
                                     pointers_array_, end_i_, end_j_));
  }

  reverse_iterator rend() {
    if (deque_size_ == 0) {
      return reverse_iterator(iterator(&pointers_array_[end_i_][end_j_],
                                       pointers_array_, end_i_, end_j_));
    }

    return reverse_iterator(iterator(&pointers_array_[begin_i_][begin_j_ - 1],
                                     pointers_array_, begin_i_, begin_j_ - 1));
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(const_iterator(
        &pointers_array_[end_i_][end_j_], pointers_array_, end_i_, end_j_));
  }
  const_reverse_iterator rend() const {
    if (deque_size_ == 0) {
      return reverse_iterator(const_iterator(&pointers_array_[end_i_][end_j_],
                                             pointers_array_, end_i_, end_j_));
    }

    return const_reverse_iterator(
        const_iterator(&pointers_array_[begin_i_][begin_j_ - 1],
                       pointers_array_, begin_i_, begin_j_ - 1));
  }

  const_reverse_iterator rcbegin() const {
    return const_reverse_iterator(const_iterator(
        &pointers_array_[end_i_][end_j_], pointers_array_, end_i_, end_j_));
  }

  const_reverse_iterator rcend() const {
    if (deque_size_ == 0) {
      return reverse_iterator(const_iterator(&pointers_array_[end_i_][end_j_],
                                             pointers_array_, end_i_, end_j_));
    }

    return const_reverse_iterator(
        const_iterator(&pointers_array_[begin_i_][begin_j_ - 1],
                       pointers_array_, begin_i_, begin_j_ - 1));
  }

  void insert(const iterator& iterator, const T& value) {
    if (iterator == end()) {
      push_back(value);
      return;
    }

    Deque copy = *this;
    *iterator = value;

    auto iter_copy = copy.begin() + (iterator - begin()) + 1;

    for (auto iter = iterator + 1; iter < end(); ++iter, ++iter_copy) {
      *iter = *(iter_copy - 1);
    }
    push_back(*(copy.end() - 1));
  }

  void erase(const iterator& iterator) {
    Deque copy = *this;
    auto iter_copy = copy.begin() + (iterator - begin());

    for (auto iter = iterator; iter < end() - 1; ++iter, ++iter_copy) {
      *iter = *(iter_copy + 1);
    }
    pop_back();
  }

 private:
  // structure of deque:
  // [ pointers_array_0 ] -> [ elements ]
  // [ pointers_array_1 ] -> [ elements ]
  // [ pointers_array_2 ] -> [ elements ]
  // [ pointers_array_3 ] -> [ elements ]
  // [ pointers_array_4 ] -> [ elements ]

  // pointers_array_ - main array which includes arrays of elements
  T** pointers_array_ = nullptr;

  // kBlockArraySize - size of arrays with elements
  const size_t kBlockArraySize = 1000;
  // kCreationCf - const value which is used for reserving memory
  const size_t kCreationCf = 2;
  size_t pointers_array_size_ = 0;
  size_t deque_size_ = 0;

  int64_t begin_i_ = 0;
  size_t begin_j_ = 0;
  int64_t end_i_ = 0;
  size_t end_j_ = 0;

  // ============================================================ //
  // ======================== fill_deque ======================== //
  // ============================================================ //

  void fill_deque(const T& value) {
    // i_var and j_var - variables to move through elements of deque (standart
    // realization of filling with exceptions)
    int64_t i_var = begin_i_;
    size_t j_var = begin_j_;
    try {
      for (; i_var <= end_i_; ++i_var) {
        for (; i_var == end_i_ ? j_var <= end_j_ : j_var < kBlockArraySize;
             ++j_var) {
          new (pointers_array_[i_var] + j_var) T(value);
        }
        j_var = 0;
      }
    } catch (...) {
      for (int64_t k_var = begin_i_; k_var <= i_var; ++k_var) {
        for (size_t l_var = 0;
             k_var == i_var ? l_var < j_var : l_var < kBlockArraySize;
             ++j_var) {
          (pointers_array_[i_var] + j_var)->~T();
        }
      }

      for (size_t i_var = 0; i_var < pointers_array_size_; ++i_var) {
        delete[] reinterpret_cast<int8_t*>(pointers_array_[i_var]);
      }

      delete[] pointers_array_;
      throw;
    }
  }

  // ============================================================ //
  // ======================== copy_deque ======================== //
  // ============================================================ //

  void copy_deque(const Deque& other) {
    int64_t i_var = begin_i_;
    size_t j_var = begin_j_;
    try {
      for (; i_var <= end_i_; ++i_var) {
        for (; i_var == end_i_ ? j_var <= end_j_ : j_var < kBlockArraySize;
             ++j_var) {
          new (pointers_array_[i_var] + j_var)
              T(other.pointers_array_[i_var][j_var]);
        }
        j_var = 0;
      }
    } catch (...) {
      for (int64_t k_var = begin_i_; k_var <= i_var; ++k_var) {
        for (size_t l_var = 0;
             k_var == i_var ? l_var < j_var : l_var < kBlockArraySize;
             ++j_var) {
          (pointers_array_[i_var] + j_var)->~T();
        }
      }

      for (size_t i_var = 0; i_var < pointers_array_size_; ++i_var) {
        delete[] reinterpret_cast<int8_t*>(pointers_array_[i_var]);
      }
      delete[] pointers_array_;
      throw;
    }
  }

  // ============================================================ //
  // ======================== SWAPDEQUE ========================= //
  // ============================================================ //

  void swap_deque(Deque& other) {
    std::swap(pointers_array_, other.pointers_array_);
    std::swap(pointers_array_size_, other.pointers_array_size_);
    std::swap(deque_size_, other.deque_size_);
    std::swap(begin_i_, other.begin_i_);
    std::swap(begin_j_, other.begin_j_);
    std::swap(end_i_, other.end_i_);
    std::swap(end_j_, other.end_j_);
  }

  // ============================================================ //
  // ======================== RESERVE =========================== //
  // ============================================================ //

  void reserve(size_t size) {
    T** new_array = new T*[size];
    for (size_t i = 0; i < size; ++i) {
      if (i < pointers_array_size_ / 2 ||
          static_cast<int64_t>(i) >
              static_cast<int64_t>(pointers_array_size_ / 2) + end_i_ -
                  begin_i_) {
        new_array[i] =
            reinterpret_cast<T*>(new int8_t[kBlockArraySize * sizeof(T)]);
      }
    }

    for (int64_t i = 0; i < begin_i_; ++i) {
      delete[] reinterpret_cast<int8_t*>(pointers_array_[i]);
    }

    for (int64_t i = end_i_ + 1; i < static_cast<int64_t>(pointers_array_size_);
         ++i) {
      delete[] reinterpret_cast<int8_t*>(pointers_array_[i]);
    }

    for (int64_t i = 0; i <= end_i_ - begin_i_; ++i) {
      new_array[i + static_cast<int64_t>(pointers_array_size_ / 2)] =
          pointers_array_[begin_i_ + i];
    }

    delete[] pointers_array_;

    begin_i_ = pointers_array_size_ / 2;
    end_i_ =
        begin_i_ + (deque_size_ + kBlockArraySize - 1) / kBlockArraySize - 1;

    pointers_array_size_ = size;
    pointers_array_ = new_array;
  }
};