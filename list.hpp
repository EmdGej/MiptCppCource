#pragma once
#include <iostream>

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

template <typename Iterator>
class ReverseIterator {
 public:
  using value_type = typename Iterator::value_type;
  using pointer = typename Iterator::pointer;
  using iterator_category = std::bidirectional_iterator_tag;
  using reference = typename Iterator::reference;
  using difference_type = std::ptrdiff_t;

  ReverseIterator(const Iterator& iter) : it_(iter) {}

  reference operator*() const { return *it_; }
  pointer operator->() const { return it_.operator->(); }

  ReverseIterator& operator++() {
    --it_;
    return *this;
  }

  ReverseIterator operator++(int) {
    ReverseIterator copy = *this;
    --it_;
    return copy;
  }

  ReverseIterator& operator--() {
    ++it_;
    return *this;
  }

  ReverseIterator operator--(int) {
    ReverseIterator copy = *this;
    ++it_;
    return copy;
  }

  bool operator==(const ReverseIterator& other) const {
    return it_ == other.it_;
  }

  bool operator!=(const ReverseIterator& other) const {
    return !(*this == other);
  }

 private:
  Iterator it_;
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode;
  struct Node;

 public:
  using value_type = T;
  using allocator_type = Allocator;

  template <bool IsConst>
  class CommonIterator {
   public:
    using value_type = conditional_t<IsConst, const T, T>;
    using pointer = conditional_t<IsConst, const T*, T*>;
    using iterator_category = std::bidirectional_iterator_tag;
    using reference = conditional_t<IsConst, const T&, T&>;
    using difference_type = std::ptrdiff_t;

    CommonIterator(const BaseNode* ptr) : ptr_(const_cast<BaseNode*>(ptr)) {}

    reference operator*() const { return static_cast<Node*>(ptr_)->value; }
    pointer operator->() const { return &static_cast<Node*>(ptr_)->value; }

    CommonIterator& operator++() {
      ptr_ = ptr_->next;
      return *this;
    }

    CommonIterator operator++(int) {
      CommonIterator copy = *this;
      ptr_ = ptr_->next;
      return copy;
    }

    CommonIterator& operator--() {
      ptr_ = ptr_->prev;
      return *this;
    }

    CommonIterator operator--(int) {
      CommonIterator copy = *this;
      ptr_ = ptr_->prev;
      return copy;
    }

    bool operator==(const CommonIterator& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const CommonIterator& other) const {
      return !(*this == other);
    }

   private:
    BaseNode* ptr_ = nullptr;
  };

  List() {
    fake_node_.next = &fake_node_;
    fake_node_.prev = &fake_node_;
  }

  explicit List(const Allocator& allocator) : allocator_(allocator) {
    fake_node_.next = &fake_node_;
    fake_node_.prev = &fake_node_;
  }

  explicit List(size_t count, const Allocator& allocator = Allocator())
      : allocator_(allocator), size_(count) {
    if (count == 0) {
      fake_node_.next = &fake_node_;
      fake_node_.prev = &fake_node_;
      return;
    }

    size_t i_var = 0;
    BaseNode* current_node_for_exception = &fake_node_;
    try {
      BaseNode* current_tail_ptr = &fake_node_;

      for (; i_var < count; ++i_var) {
        Node* current_node_ptr = node_allocator_traits::allocate(allocator_, 1);
        current_node_for_exception = current_node_ptr;
        node_allocator_traits::construct(allocator_, current_node_ptr);

        current_node_ptr->next = &fake_node_;
        current_node_ptr->prev = current_tail_ptr;

        current_tail_ptr->next = current_node_ptr;
        fake_node_.prev = current_node_ptr;
        current_tail_ptr = current_node_ptr;
      }
    } catch (...) {
      destroy_list(i_var, current_node_for_exception);
    }
  }

  explicit List(size_t count, const T& value,
                const Allocator& allocator = Allocator())
      : allocator_(allocator), size_(count) {
    create_list(count, value);
  }

  List(std::initializer_list<T> init, const Allocator& allocator = Allocator())
      : allocator_(allocator), size_(init.size()) {
    if (init.size() == 0) {
      fake_node_.next = &fake_node_;
      fake_node_.prev = &fake_node_;
      return;
    }

    size_t i_var = 0;
    BaseNode* current_node_for_exception = &fake_node_;
    try {
      BaseNode* current_tail_ptr = &fake_node_;

      for (; i_var < init.size(); ++i_var) {
        Node* current_node_ptr = node_allocator_traits::allocate(allocator_, 1);
        current_node_for_exception = current_node_ptr;
        node_allocator_traits::construct(allocator_, current_node_ptr,
                                         *(init.begin() + i_var), &fake_node_,
                                         current_tail_ptr);

        current_tail_ptr->next = current_node_ptr;
        fake_node_.prev = current_node_ptr;
        current_tail_ptr = current_node_ptr;
      }
    } catch (...) {
      destroy_list(i_var, current_node_for_exception);
    }
  }

  List(const List& other)
      : allocator_(node_allocator_traits::select_on_container_copy_construction(
            other.allocator_)),
        size_(other.size_) {
    if (size_ == 0) {
      fake_node_.next = &fake_node_;
      fake_node_.prev = &fake_node_;
      return;
    }

    size_t i_var = 0;
    BaseNode* current_node_for_exception = &fake_node_;
    try {
      BaseNode* current_tail_ptr = &fake_node_;
      BaseNode* current_node_to_copy = other.fake_node_.next;

      for (; i_var < size_; ++i_var) {
        Node* current_node_ptr = node_allocator_traits::allocate(allocator_, 1);
        current_node_for_exception = current_node_ptr;
        node_allocator_traits::construct(
            allocator_, current_node_ptr,
            static_cast<Node*>(current_node_to_copy)->value, &fake_node_,
            current_tail_ptr);

        current_tail_ptr->next = current_node_ptr;
        fake_node_.prev = current_node_ptr;

        current_tail_ptr = current_node_ptr;
        current_node_to_copy = current_node_to_copy->next;
      }
    } catch (...) {
      destroy_list(i_var, current_node_for_exception);
    }
  }

  List(List&& other)
      : fake_node_(other.fake_node_),
        allocator_(std::move(other.allocator_)),
        size_(other.size_) {
    other.fake_node_.next = &other.fake_node_;
    other.fake_node_.prev = &other.fake_node_;
    other.size_ = 0;
  }

  List& operator=(const List& other) {
    List copy = other;
    swap_lists(copy);

    if (node_allocator_traits::propagate_on_container_copy_assignment::value &&
        allocator_ != other.allocator_) {
      allocator_ = other.allocator_;
    }

    return *this;
  }

  List& operator==(List&& other) noexcept {
    List copy = std::move(other);
    swap_lists(copy);

    if (node_allocator_traits::propagate_on_container_copy_assignment::value &&
        allocator_ != other.allocator_) {
      allocator_ = std::move(other.allocator_);
    } else {
      node_allocator temporary = std::move(copy.allocator_);
    }
    return *this;
  }

  void push_back(const T& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    node_allocator_traits::construct(allocator_, new_node);

    BaseNode* current_tail_ptr = fake_node_.prev;

    try {
      new_node->value = value;
      new_node->next = &fake_node_;
      new_node->prev = fake_node_.prev;

      fake_node_.prev->next = new_node;
      fake_node_.prev = new_node;
    } catch (...) {
      node_allocator_traits::destroy(allocator_, new_node);
      node_allocator_traits::deallocate(allocator_, new_node, 1);

      current_tail_ptr->next = &fake_node_;
      fake_node_.prev = current_tail_ptr;

      throw;
    }

    ++size_;
  }

  void push_back(T&& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    node_allocator_traits::construct(allocator_, new_node);

    BaseNode* current_tail_ptr = fake_node_.prev;

    try {
      new_node->value = std::move(value);
      new_node->next = &fake_node_;
      new_node->prev = fake_node_.prev;

      fake_node_.prev->next = new_node;
      fake_node_.prev = new_node;
    } catch (...) {
      node_allocator_traits::destroy(allocator_, new_node);
      node_allocator_traits::deallocate(allocator_, new_node, 1);

      current_tail_ptr->next = &fake_node_;
      fake_node_.prev = current_tail_ptr;

      throw;
    }

    ++size_;
  }

  void push_front(const T& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    node_allocator_traits::construct(allocator_, new_node);

    BaseNode* current_begin_ptr = fake_node_.next;

    try {
      new_node->value = value;
      new_node->next = fake_node_.next;
      new_node->prev = &fake_node_;

      fake_node_.next->prev = new_node;
      fake_node_.next = new_node;
    } catch (...) {
      node_allocator_traits::destroy(allocator_, new_node);
      node_allocator_traits::deallocate(allocator_, new_node, 1);

      fake_node_.next = current_begin_ptr;
      current_begin_ptr->prev = &fake_node_;

      throw;
    }

    ++size_;
  }

  void push_front(T&& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    node_allocator_traits::construct(allocator_, new_node);

    BaseNode* current_begin_ptr = fake_node_.next;

    try {
      new_node->value = std::move(value);
      new_node->next = fake_node_.next;
      new_node->prev = &fake_node_;

      fake_node_.next->prev = new_node;
      fake_node_.next = new_node;
    } catch (...) {
      node_allocator_traits::destroy(allocator_, new_node);
      node_allocator_traits::deallocate(allocator_, new_node, 1);

      fake_node_.next = current_begin_ptr;
      current_begin_ptr->prev = &fake_node_;

      throw;
    }

    ++size_;
  }

  void pop_back() {
    BaseNode* node_to_rebind = fake_node_.prev->prev;

    node_allocator_traits::destroy(allocator_,
                                   static_cast<Node*>(fake_node_.prev));
    node_allocator_traits::deallocate(allocator_,
                                      static_cast<Node*>(fake_node_.prev), 1);

    node_to_rebind->next = &fake_node_;
    fake_node_.prev = node_to_rebind;

    --size_;
  }

  void pop_front() {
    BaseNode* node_to_rebind = fake_node_.next->next;

    node_allocator_traits::destroy(allocator_,
                                   static_cast<Node*>(fake_node_.next));
    node_allocator_traits::deallocate(allocator_,
                                      static_cast<Node*>(fake_node_.next), 1);

    node_to_rebind->prev = &fake_node_;
    fake_node_.next = node_to_rebind;

    --size_;
  }

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }

  T& front() { return static_cast<Node*>(fake_node_.next)->value; }
  const T& front() const { return static_cast<Node*>(fake_node_.next)->value; }

  T& back() { return static_cast<Node*>(fake_node_.prev)->value; }
  const T& back() const { return static_cast<Node*>(fake_node_.prev)->value; }

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = ReverseIterator<iterator>;
  using const_reverse_iterator = ReverseIterator<const_iterator>;

  iterator begin() { return iterator(fake_node_.next); }
  iterator end() { return iterator(&fake_node_); }

  const_iterator begin() const { return const_iterator(fake_node_.next); }
  const_iterator end() const { return const_iterator(&fake_node_); }

  const_iterator cbegin() const { return const_iterator(fake_node_.next); }
  const_iterator cend() const { return const_iterator(&fake_node_); }

  reverse_iterator rbegin() {
    return reverse_iterator(iterator(fake_node_.prev));
  }
  reverse_iterator rend() { return reverse_iterator(iterator(&fake_node_)); }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(const_iterator(fake_node_.prev));
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(const_iterator(&fake_node_));
  }

  const_reverse_iterator rcbegin() const {
    return const_reverse_iterator(const_iterator(fake_node_.prev));
  }
  const_reverse_iterator rcend() const {
    return const_reverse_iterator(const_iterator(&fake_node_));
  }

  Allocator get_allocator() { return allocator_; }

  ~List() {
    BaseNode* node_to_delete = fake_node_.next;

    for (size_t i = 0; i < size_; ++i) {
      BaseNode* next_node = node_to_delete->next;

      node_allocator_traits::destroy(allocator_,
                                     static_cast<Node*>(node_to_delete));
      node_allocator_traits::deallocate(allocator_,
                                        static_cast<Node*>(node_to_delete), 1);

      node_to_delete = next_node;
    }
  }

 private:
  struct BaseNode {
    BaseNode() = default;
    BaseNode(BaseNode* next, BaseNode* prev) : next(next), prev(prev) {}
    BaseNode* next = nullptr;
    BaseNode* prev = nullptr;
  };

  struct Node : BaseNode {
    Node() = default;
    Node(const T& value, BaseNode* next, BaseNode* prev)
        : BaseNode(next, prev), value(value) {}
    T value;
  };

  using node_allocator =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using node_allocator_traits = std::allocator_traits<node_allocator>;

  BaseNode fake_node_;
  node_allocator allocator_;
  size_t size_ = 0;

  void create_list(size_t count, const T& value = T()) {
    if (count == 0) {
      fake_node_.next = &fake_node_;
      fake_node_.prev = &fake_node_;
      return;
    }

    size_t i_var = 0;
    BaseNode* current_node_for_exception = &fake_node_;
    try {
      BaseNode* current_tail_ptr = &fake_node_;

      for (size_t i = 0; i < count; ++i) {
        Node* current_node_ptr = node_allocator_traits::allocate(allocator_, 1);
        current_node_for_exception = current_node_ptr;
        node_allocator_traits::construct(allocator_, current_node_ptr, value,
                                         &fake_node_, current_tail_ptr);

        current_tail_ptr->next = current_node_ptr;
        fake_node_.prev = current_node_ptr;
        current_tail_ptr = current_node_ptr;
      }
    } catch (...) {
      BaseNode* node_to_delete = fake_node_.next;

      for (size_t k = 0; k < i_var; ++k) {
        BaseNode* next_node = node_to_delete->next;

        node_allocator_traits::destroy(allocator_,
                                       static_cast<Node*>(node_to_delete));
        node_allocator_traits::deallocate(
            allocator_, static_cast<Node*>(node_to_delete), 1);

        node_to_delete = next_node;
      }
      node_allocator_traits::deallocate(
          allocator_, static_cast<Node*>(current_node_for_exception), 1);
      throw;
    }
  }

  void swap_lists(List& other) {
    std::swap(size_, other.size_);
    std::swap(fake_node_, other.fake_node_);
  }

  void destroy_list(size_t i_var, BaseNode* current_node_for_exception) {
    BaseNode* node_to_delete = fake_node_.next;

    for (size_t k = 0; k < i_var; ++k) {
      BaseNode* next_node = node_to_delete->next;

      node_allocator_traits::destroy(allocator_,
                                     static_cast<Node*>(node_to_delete));
      node_allocator_traits::deallocate(allocator_,
                                        static_cast<Node*>(node_to_delete), 1);

      node_to_delete = next_node;
    }
    node_allocator_traits::deallocate(
        allocator_, static_cast<Node*>(current_node_for_exception), 1);
    throw;
  }
};