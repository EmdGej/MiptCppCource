#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
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

template <typename T, typename Allocator>
class Deque {
 public:
  Deque() : pointers_array_size_(kCreationCf) {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, kCreationCf);
    for (size_t i = 0; i < kCreationCf; ++i) {
      pointers_array_[i] =
          block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
    }
  }

  Deque(const Allocator& allocator)
      : pointers_array_size_(kCreationCf),
        block_alloc_(allocator),
        ptr_to_blocks_alloc_(allocator) {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, kCreationCf);
    for (size_t i = 0; i < kCreationCf; ++i) {
      pointers_array_[i] =
          block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
    }
  }

  Deque(size_t count, const Allocator& allocator = Allocator())
      : pointers_array_size_(kCreationCf *
                             ((count / kBlockArraySize) +
                              ((count % kBlockArraySize) == 0 ? 0 : 1))),
        deque_size_(count),
        end_i_(pointers_array_size_ / 2 - 1),
        end_j_(count % kBlockArraySize == 0 ? kBlockArraySize - 1
                                            : count % kBlockArraySize - 1),
        block_alloc_(allocator),
        ptr_to_blocks_alloc_(allocator) {
    fill_deque();
  }

  Deque(size_t count, const T& value, const Allocator& allocator = Allocator())
      : pointers_array_size_(kCreationCf *
                             ((count / kBlockArraySize) +
                              ((count % kBlockArraySize) == 0 ? 0 : 1))),
        deque_size_(count),
        end_i_(pointers_array_size_ / 2 - 1),
        end_j_(count % kBlockArraySize == 0 ? kBlockArraySize - 1
                                            : count % kBlockArraySize - 1),
        block_alloc_(allocator),
        ptr_to_blocks_alloc_(allocator) {
    fill_deque(value);
  }

  Deque(std::initializer_list<T> init, const Allocator& allocator = Allocator())
      : pointers_array_size_(kCreationCf *
                             ((init.size() / kBlockArraySize) +
                              ((init.size() % kBlockArraySize) == 0 ? 0 : 1))),
        deque_size_(init.size()),
        end_i_(pointers_array_size_ / 2 - 1),
        end_j_(init.size() % kBlockArraySize == 0
                   ? kBlockArraySize - 1
                   : init.size() % kBlockArraySize - 1),
        block_alloc_(allocator),
        ptr_to_blocks_alloc_(allocator) {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, pointers_array_size_);
    int64_t i_var = begin_i_;
    size_t j_var = begin_j_;
    size_t counter_i_for_init = 0;
    size_t counter_j_for_init = 0;

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
    }

    try {
      for (; i_var <= end_i_; ++i_var, ++counter_i_for_init) {
        for (; i_var == end_i_ ? j_var <= end_j_ : j_var < kBlockArraySize;
             ++j_var, ++counter_j_for_init) {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[i_var] + j_var,
              std::move(
                  *(init.begin() + counter_i_for_init + counter_j_for_init)));
        }
        j_var = 0;
        --counter_j_for_init;
      }
    } catch (...) {
      for (int64_t k_var = begin_i_; k_var <= i_var; ++k_var) {
        for (size_t l_var = 0;
             k_var == i_var ? l_var < j_var : l_var < kBlockArraySize;
             ++j_var) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[i_var] + j_var);
        }
      }

      for (size_t i_var = 0; i_var < pointers_array_size_; ++i_var) {
        block_alloc_traits::deallocate(block_alloc_, pointers_array_[i_var],
                                       kBlockArraySize);
      }

      ptr_alloc_traits::deallocate(ptr_to_blocks_alloc_, pointers_array_,
                                   pointers_array_size_);
      throw;
    }
  }

  Deque(const Deque& other)
      : kBlockArraySize(other.kBlockArraySize),
        pointers_array_size_(other.pointers_array_size_),
        deque_size_(other.deque_size_),
        begin_i_(other.begin_i_),
        begin_j_(other.begin_j_),
        end_i_(other.end_i_),
        end_j_(other.end_j_),
        block_alloc_(block_alloc_traits::select_on_container_copy_construction(
            other.block_alloc_)),
        ptr_to_blocks_alloc_(
            ptr_alloc_traits::select_on_container_copy_construction(
                other.ptr_to_blocks_alloc_)) {
    if (pointers_array_size_ > 0) {
      copy_deque(other);
    }
  }

  Deque(Deque&& other) noexcept
      : pointers_array_size_(other.pointers_array_size_),
        deque_size_(other.deque_size_),
        begin_i_(other.begin_i_),
        begin_j_(other.begin_j_),
        end_i_(other.end_i_),
        end_j_(other.end_j_),
        block_alloc_(std::move(other.block_alloc_)),
        ptr_to_blocks_alloc_(std::move(other.ptr_to_blocks_alloc_)) {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, kCreationCf);
    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] = other.pointers_array_[i];
      other.pointers_array_[i] = nullptr;
    }
    other.pointers_array_size_ = 0;
    other.deque_size_ = 0;
    other.begin_i_ = 0;
    other.begin_j_ = 0;
    other.end_i_ = 0;
    other.end_j_ = 0;
  }

  Deque& operator=(const Deque& other) {
    Deque copy = other;
    swap_deque(copy);

    if (block_alloc_traits::propagate_on_container_copy_assignment::value &&
        block_alloc_ != other.block_alloc_) {
      block_alloc_ = other.block_alloc_;
    }

    if (ptr_alloc_traits::propagate_on_container_copy_assignment::value &&
        ptr_to_blocks_alloc_ != other.ptr_to_blocks_alloc_) {
      ptr_to_blocks_alloc_ = other.ptr_to_blocks_alloc_;
    }

    return *this;
  }

  Deque& operator=(Deque&& other) noexcept {
    Deque copy = std::move(other);
    swap_deque(copy);

    if (block_alloc_traits::propagate_on_container_copy_assignment::value &&
        block_alloc_ != other.block_alloc_) {
      block_alloc_ = copy.block_alloc_;
    }

    if (ptr_alloc_traits::propagate_on_container_copy_assignment::value &&
        ptr_to_blocks_alloc_ != other.ptr_to_blocks_alloc_) {
      ptr_to_blocks_alloc_ = copy.ptr_to_blocks_alloc_;
    }

    return *this;
  }

  size_t size() const { return deque_size_; }

  bool empty() const { return deque_size_ == 0; }

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

  void push_back(const T& value) {
    if (pointers_array_size_ == 0) {
      reserve(kCreationCf, true);
    }

    if (deque_size_ == 0) {
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[end_i_],
                                      value);
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_]);
        throw;
      }
      ++deque_size_;
      return;
    }

    if (end_i_ == static_cast<int64_t>(pointers_array_size_ - 1) &&
        end_j_ == kBlockArraySize - 1) {
      reserve(pointers_array_size_ * kCreationCf);
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[end_i_ + 1],
                                      value);
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_ + 1]);
        throw;
      }
      ++end_i_;
      end_j_ = 0;
    } else {
      if (end_j_ == kBlockArraySize - 1) {
        try {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[end_i_ + 1], value);
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[end_i_ + 1]);
          throw;
        }
        ++end_i_;
        end_j_ = 0;
      } else {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[end_i_] + end_j_ + 1, value);
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[end_i_] + end_j_ + 1);
          throw;
        }
        ++end_j_;
      }
    }
    ++deque_size_;
  }

  void push_back(T&& value) {
    if (pointers_array_size_ == 0) {
      reserve(kCreationCf, true);
    }

    if (deque_size_ == 0) {
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[end_i_],
                                      std::move(value));
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_]);
        throw;
      }
      ++deque_size_;
      return;
    }

    if (end_i_ == static_cast<int64_t>(pointers_array_size_ - 1) &&
        end_j_ == kBlockArraySize - 1) {
      reserve(pointers_array_size_ * kCreationCf);
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[end_i_ + 1],
                                      std::move(value));
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_ + 1]);
        throw;
      }
      ++end_i_;
      end_j_ = 0;
    } else {
      if (end_j_ == kBlockArraySize - 1) {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[end_i_ + 1], std::move(value));
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[end_i_ + 1]);
          throw;
        }
        ++end_i_;
        end_j_ = 0;
      } else {
        try {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[end_i_] + end_j_ + 1,
                                        std::move(value));
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[end_i_] + end_j_ + 1);
          throw;
        }
        ++end_j_;
      }
    }
    ++deque_size_;
  }

  void push_front(const T& value) {
    if (pointers_array_size_ == 0) {
      reserve(kCreationCf, true);
    }

    if (deque_size_ == 0) {
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[begin_i_],
                                      value);
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[begin_i_]);
      }

      ++deque_size_;
      return;
    }
    if (begin_i_ == 0 && begin_j_ == 0) {
      reserve(pointers_array_size_ * kCreationCf);

      try {
        block_alloc_traits::construct(
            block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1,
            value);
      } catch (...) {
        block_alloc_traits::destroy(
            block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1);
      }

      --begin_i_;
      begin_j_ = kBlockArraySize - 1;
    } else {
      if (begin_j_ == 0) {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1,
              value);
        } catch (...) {
          block_alloc_traits::destroy(
              block_alloc_,
              pointers_array_[begin_i_ - 1] + kBlockArraySize - 1);
        }

        --begin_i_;
        begin_j_ = kBlockArraySize - 1;
      } else {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[begin_i_] + begin_j_ - 1, value);
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[begin_i_] + begin_j_ - 1);
        }

        --begin_j_;
      }
    }
    ++deque_size_;
  }

  void push_front(T&& value) {
    if (pointers_array_size_ == 0) {
      reserve(kCreationCf, true);
    }

    if (deque_size_ == 0) {
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[begin_i_],
                                      std::move(value));
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[begin_i_]);
      }

      ++deque_size_;
      return;
    }
    if (begin_i_ == 0 && begin_j_ == 0) {
      reserve(pointers_array_size_ * kCreationCf);

      try {
        block_alloc_traits::construct(
            block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1,
            std::move(value));
      } catch (...) {
        block_alloc_traits::destroy(
            block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1);
      }

      --begin_i_;
      begin_j_ = kBlockArraySize - 1;
    } else {
      if (begin_j_ == 0) {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1,
              std::move(value));
        } catch (...) {
          block_alloc_traits::destroy(
              block_alloc_,
              pointers_array_[begin_i_ - 1] + kBlockArraySize - 1);
        }

        --begin_i_;
        begin_j_ = kBlockArraySize - 1;
      } else {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[begin_i_] + begin_j_ - 1,
              std::move(value));
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[begin_i_] + begin_j_ - 1);
        }

        --begin_j_;
      }
    }
    ++deque_size_;
  }

  void pop_back() {
    block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_] + end_j_);
    if (end_j_ == 0) {
      --end_i_;
      end_j_ = kBlockArraySize - 1;
    } else {
      --end_j_;
    }
    --deque_size_;
  }

  void pop_front() {
    block_alloc_traits::destroy(block_alloc_,
                                pointers_array_[begin_i_] + begin_j_);
    if (begin_j_ == kBlockArraySize - 1) {
      ++begin_i_;
      begin_j_ = 0;
    } else {
      ++begin_j_;
    }
    --deque_size_;
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (pointers_array_size_ == 0) {
      reserve(kCreationCf, true);
    }

    if (deque_size_ == 0) {
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[end_i_],
                                      std::forward<Args>(args)...);
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_]);
        throw;
      }
      ++deque_size_;
      return;
    }

    if (end_i_ == static_cast<int64_t>(pointers_array_size_ - 1) &&
        end_j_ == kBlockArraySize - 1) {
      reserve(pointers_array_size_ * kCreationCf);
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[end_i_ + 1],
                                      std::forward<Args>(args)...);
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[end_i_ + 1]);
        throw;
      }
      ++end_i_;
      end_j_ = 0;
    } else {
      if (end_j_ == kBlockArraySize - 1) {
        try {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[end_i_ + 1],
                                        std::forward<Args>(args)...);
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[end_i_ + 1]);
          throw;
        }
        ++end_i_;
        end_j_ = 0;
      } else {
        try {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[end_i_] + end_j_ + 1,
                                        std::forward<Args>(args)...);
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[end_i_] + end_j_ + 1);
          throw;
        }
        ++end_j_;
      }
    }
    ++deque_size_;
  }

  template <typename... Args>
  void emplace_front(Args&&... args) {
    if (pointers_array_size_ == 0) {
      reserve(kCreationCf, true);
    }

    if (deque_size_ == 0) {
      try {
        block_alloc_traits::construct(block_alloc_, pointers_array_[begin_i_],
                                      std::forward<Args>(args)...);
      } catch (...) {
        block_alloc_traits::destroy(block_alloc_, pointers_array_[begin_i_]);
      }

      ++deque_size_;
      return;
    }
    if (begin_i_ == 0 && begin_j_ == 0) {
      reserve(pointers_array_size_ * kCreationCf);

      try {
        block_alloc_traits::construct(
            block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1,
            std::forward<Args>(args)...);
      } catch (...) {
        block_alloc_traits::destroy(
            block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1);
      }

      --begin_i_;
      begin_j_ = kBlockArraySize - 1;
    } else {
      if (begin_j_ == 0) {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[begin_i_ - 1] + kBlockArraySize - 1,
              std::forward<Args>(args)...);
        } catch (...) {
          block_alloc_traits::destroy(
              block_alloc_,
              pointers_array_[begin_i_ - 1] + kBlockArraySize - 1);
        }

        --begin_i_;
        begin_j_ = kBlockArraySize - 1;
      } else {
        try {
          block_alloc_traits::construct(
              block_alloc_, pointers_array_[begin_i_] + begin_j_ - 1,
              std::forward<Args>(args)...);
        } catch (...) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[begin_i_] + begin_j_ - 1);
        }

        --begin_j_;
      }
    }
    ++deque_size_;
  }
  ~Deque() {
    if (deque_size_ != 0) {
      for (int64_t i = begin_i_; i <= end_i_; ++i) {
        for (size_t j = (i == begin_i_ ? begin_j_ : 0);
             i == end_i_ ? j <= end_j_ : j < kBlockArraySize; ++j) {
          block_alloc_traits::destroy(block_alloc_, pointers_array_[i] + j);
        }
      }
    }

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      block_alloc_traits::deallocate(block_alloc_, pointers_array_[i],
                                     kBlockArraySize);
    }

    ptr_alloc_traits::deallocate(ptr_to_blocks_alloc_, pointers_array_,
                                 pointers_array_size_);
  }

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

  void insert(const iterator& iterator, T&& value) {
    if (iterator == end()) {
      push_back(std::move(value));
      return;
    }

    Deque copy = *this;
    *iterator = std::move(value);

    auto iter_copy = copy.begin() + (iterator - begin()) + 1;

    for (auto iter = iterator + 1; iter < end(); ++iter, ++iter_copy) {
      *iter = *(iter_copy - 1);
    }
    push_back(*(copy.end() - 1));
  }

  template <typename... Args>
  void emplace(const iterator& iterator, Args&&... args) {
    if (iterator == end()) {
      emplace_back(std::forward<Args>(args)...);
      return;
    }

    Deque copy = *this;
    *iterator = std::move(T(std::forward<Args>(args)...));

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

  Allocator get_allocator() { return block_alloc_; }

 private:
  T** pointers_array_ = nullptr;

  const size_t kBlockArraySize = 1000;
  const size_t kCreationCf = 2;
  size_t pointers_array_size_ = 0;
  size_t deque_size_ = 0;

  int64_t begin_i_ = 0;
  size_t begin_j_ = 0;
  int64_t end_i_ = 0;
  size_t end_j_ = 0;

  using block_alloc_traits = std::allocator_traits<Allocator>;
  using ptr_to_block_alloc =
      typename std::allocator_traits<Allocator>::template rebind_alloc<T*>;
  using ptr_alloc_traits = std::allocator_traits<ptr_to_block_alloc>;

  Allocator block_alloc_;
  ptr_to_block_alloc ptr_to_blocks_alloc_;

  void fill_deque(const T& value) {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, pointers_array_size_);
    int64_t i_var = begin_i_;
    size_t j_var = begin_j_;

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
    }

    try {
      for (; i_var <= end_i_; ++i_var) {
        for (; i_var == end_i_ ? j_var <= end_j_ : j_var < kBlockArraySize;
             ++j_var) {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[i_var] + j_var, value);
        }
        j_var = 0;
      }
    } catch (...) {
      for (int64_t k_var = begin_i_; k_var <= i_var; ++k_var) {
        for (size_t l_var = 0;
             k_var == i_var ? l_var < j_var : l_var < kBlockArraySize;
             ++l_var) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[k_var] + l_var);
        }
      }

      for (size_t i_var = 0; i_var < pointers_array_size_; ++i_var) {
        block_alloc_traits::deallocate(block_alloc_, pointers_array_[i_var],
                                       kBlockArraySize);
      }

      ptr_alloc_traits::deallocate(ptr_to_blocks_alloc_, pointers_array_,
                                   pointers_array_size_);
      throw;
    }
  }

  void fill_deque() {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, pointers_array_size_);
    int64_t i_var = begin_i_;
    size_t j_var = begin_j_;

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
    }

    try {
      for (; i_var <= end_i_; ++i_var) {
        for (; i_var == end_i_ ? j_var <= end_j_ : j_var < kBlockArraySize;
             ++j_var) {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[i_var] + j_var);
        }
        j_var = 0;
      }
    } catch (...) {
      for (int64_t k_var = begin_i_; k_var <= i_var; ++k_var) {
        for (size_t l_var = 0;
             k_var == i_var ? l_var < j_var : l_var < kBlockArraySize;
             ++l_var) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[k_var] + l_var);
        }
      }

      for (size_t i_var = 0; i_var < pointers_array_size_; ++i_var) {
        block_alloc_traits::deallocate(block_alloc_, pointers_array_[i_var],
                                       kBlockArraySize);
      }

      ptr_alloc_traits::deallocate(ptr_to_blocks_alloc_, pointers_array_,
                                   pointers_array_size_);
      throw;
    }
  }

  void copy_deque(const Deque& other) {
    pointers_array_ =
        ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, pointers_array_size_);

    for (size_t i = 0; i < pointers_array_size_; ++i) {
      pointers_array_[i] =
          block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
    }

    int64_t i_var = begin_i_;
    size_t j_var = begin_j_;
    try {
      for (; i_var <= end_i_; ++i_var) {
        for (; i_var == end_i_ ? j_var <= end_j_ : j_var < kBlockArraySize;
             ++j_var) {
          block_alloc_traits::construct(block_alloc_,
                                        pointers_array_[i_var] + j_var,
                                        other.pointers_array_[i_var][j_var]);
        }
        j_var = 0;
      }
    } catch (...) {
      for (int64_t k_var = begin_i_; k_var <= i_var; ++k_var) {
        for (size_t l_var = 0;
             k_var == i_var ? l_var < j_var : l_var < kBlockArraySize;
             ++l_var) {
          block_alloc_traits::destroy(block_alloc_,
                                      pointers_array_[k_var] + l_var);
        }
      }

      for (size_t i_var = 0; i_var < pointers_array_size_; ++i_var) {
        block_alloc_traits::deallocate(block_alloc_, pointers_array_[i_var],
                                       kBlockArraySize);
      }

      ptr_alloc_traits::deallocate(ptr_to_blocks_alloc_, pointers_array_,
                                   pointers_array_size_);
      throw;
    }
  }

  void swap_deque(Deque& other) {
    std::swap(pointers_array_, other.pointers_array_);
    std::swap(pointers_array_size_, other.pointers_array_size_);
    std::swap(deque_size_, other.deque_size_);
    std::swap(begin_i_, other.begin_i_);
    std::swap(begin_j_, other.begin_j_);
    std::swap(end_i_, other.end_i_);
    std::swap(end_j_, other.end_j_);
  }

  void reserve(size_t size, bool is_empty = false) {
    if (is_empty) {
      pointers_array_ = ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, size);
      for (size_t i = 0; i < size; ++i) {
        pointers_array_[i] =
            block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
      }
      pointers_array_size_ = size;
      return;
    }

    T** new_array = ptr_alloc_traits::allocate(ptr_to_blocks_alloc_, size);
    for (size_t i = 0; i < size; ++i) {
      if (i < pointers_array_size_ / 2 ||
          static_cast<int64_t>(i) >
              static_cast<int64_t>(pointers_array_size_ / 2) + end_i_ -
                  begin_i_) {
        new_array[i] =
            block_alloc_traits::allocate(block_alloc_, kBlockArraySize);
      }
    }

    for (int64_t i = 0; i < begin_i_; ++i) {
      block_alloc_traits::deallocate(block_alloc_, pointers_array_[i],
                                     kBlockArraySize);
    }

    for (int64_t i = end_i_ + 1; i < static_cast<int64_t>(pointers_array_size_);
         ++i) {
      block_alloc_traits::deallocate(block_alloc_, pointers_array_[i],
                                     kBlockArraySize);
    }

    for (int64_t i = 0; i <= end_i_ - begin_i_; ++i) {
      new_array[i + static_cast<int64_t>(pointers_array_size_ / 2)] =
          pointers_array_[begin_i_ + i];
    }

    ptr_alloc_traits::deallocate(ptr_to_blocks_alloc_, pointers_array_,
                                 pointers_array_size_);

    begin_i_ = pointers_array_size_ / 2;
    end_i_ =
        begin_i_ + (deque_size_ + kBlockArraySize - 1) / kBlockArraySize - 1;

    pointers_array_size_ = size;
    pointers_array_ = new_array;
  }
};