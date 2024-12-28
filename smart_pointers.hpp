#include <iostream>
#include <memory>

struct BaseControlBlock {
  size_t shared_count = 0;
  size_t weak_count = 0;
  bool made_directly = true;

  BaseControlBlock(size_t shared_count, size_t weak_count, bool made_directly)
      : shared_count(shared_count),
        weak_count(weak_count),
        made_directly(made_directly) {}

  virtual void destroy_object() = 0;
  virtual void deallocate_memory() = 0;
  virtual ~BaseControlBlock() = default;
};

template <typename T>
class SharedPtr {
 public:
  SharedPtr() noexcept = default;
  SharedPtr(std::nullptr_t) noexcept {};

  template <typename Y>
  SharedPtr(Y* ptr) : object_(ptr) {
    using control_block_allocloc = std::allocator<ControlBlockMakeDirect<Y>>;
    control_block_allocloc control_block_alloc;

    auto cb_ptr = std::allocator_traits<control_block_allocloc>::allocate(
        control_block_alloc, 1);
    std::allocator_traits<control_block_allocloc>::construct(
        control_block_alloc, cb_ptr, ptr, 1, 0, true);

    control_block_ = cb_ptr;
  }

  template <typename Y, typename Deleter>
  SharedPtr(Y* ptr, const Deleter& deleter) : object_(ptr) {
    using control_block_allocloc =
        std::allocator<ControlBlockMakeDirect<Y, Deleter>>;
    control_block_allocloc control_block_alloc;

    auto cb_ptr = std::allocator_traits<control_block_allocloc>::allocate(
        control_block_alloc, 1);
    std::allocator_traits<control_block_allocloc>::construct(
        control_block_alloc, cb_ptr, ptr, 1, 0, true, deleter);

    control_block_ = cb_ptr;
  }

  template <typename Y, typename Deleter, typename Alloc>
  SharedPtr(Y* ptr, const Deleter& deleter, const Alloc& alloc) : object_(ptr) {
    using control_block_allocloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<
            ControlBlockMakeDirect<Y, Deleter, Alloc>>;
    control_block_allocloc control_block_alloc = alloc;

    auto cb_ptr = std::allocator_traits<control_block_allocloc>::allocate(
        control_block_alloc, 1);
    std::allocator_traits<control_block_allocloc>::construct(
        control_block_alloc, cb_ptr, ptr, 1, 0, true, deleter, alloc);

    control_block_ = cb_ptr;
  }

  SharedPtr(const SharedPtr& other) noexcept
      : control_block_(other.control_block_), object_(other.object_) {
    ++control_block_->shared_count;
  }

  template <typename Y>
  SharedPtr(const SharedPtr<Y>& other) noexcept
      : control_block_(dynamic_cast<BaseControlBlock*>(other.control_block_)),
        object_(other.object_) {
    ++control_block_->shared_count;
  }

  SharedPtr(SharedPtr&& other) noexcept
      : control_block_(other.control_block_), object_(other.object_) {
    other.control_block_ = nullptr;
    other.object_ = nullptr;
  }

  template <typename Y>
  SharedPtr(SharedPtr<Y>&& other) noexcept
      : control_block_(dynamic_cast<BaseControlBlock*>(other.control_block_)),
        object_(other.object_) {
    other.control_block_ = nullptr;
    other.object_ = nullptr;
  }

  SharedPtr& operator=(const SharedPtr& other) noexcept {
    SharedPtr copy = other;
    swap(copy);
    return *this;
  }

  template <typename Y>
  SharedPtr& operator=(const SharedPtr<Y>& other) noexcept {
    SharedPtr copy = other;
    swap(copy);
    return *this;
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    SharedPtr copy = std::move(other);
    swap(copy);
    return *this;
  }

  template <typename Y>
  SharedPtr& operator=(SharedPtr<Y>&& other) noexcept {
    SharedPtr copy = std::move(other);
    swap(copy);
    return *this;
  }

  T* get() const noexcept { return object_; }
  size_t use_count() const noexcept {
    if (control_block_ == nullptr) {
      return 0;
    }
    return control_block_->shared_count;
  }

  T& operator*() const noexcept { return *get(); }
  T* operator->() const noexcept { return get(); }

  void reset() {
    if (control_block_ == nullptr) {
      return;
    }

    --control_block_->shared_count;
    if (control_block_->shared_count == 0) {
      control_block_->destroy_object();
      if (control_block_->weak_count == 0) {
        control_block_->deallocate_memory();
      }
    }
    control_block_ = nullptr;
    object_ = nullptr;
  }

  ~SharedPtr() {
    if (control_block_ == nullptr) {
      return;
    }

    --control_block_->shared_count;
    if (control_block_->shared_count == 0) {
      control_block_->destroy_object();

      if (control_block_->weak_count == 0) {
        control_block_->deallocate_memory();
      }
    }
  }

 private:
  template <typename U, typename Alloc = std::allocator<U>>
  struct ControlBlockMakeShared : BaseControlBlock {
    alignas(U) char object[sizeof(U)];
    Alloc alloc;

    template <typename... Args>
    ControlBlockMakeShared(size_t shared_count, size_t weak_count,
                           bool made_directly, const Alloc& allocator,
                           Args&&... args)
        : BaseControlBlock(shared_count, weak_count, made_directly),
          alloc(allocator) {
      std::allocator<U> obj_alloc;
      std::allocator_traits<std::allocator<U>>::construct(
          obj_alloc, reinterpret_cast<U*>(object), std::forward<Args>(args)...);
    }

    void destroy_object() override { reinterpret_cast<U*>(object)->~U(); }

    void deallocate_memory() override {
      using control_block_allocloc = typename std::allocator_traits<
          Alloc>::template rebind_alloc<ControlBlockMakeShared<U, Alloc>>;
      control_block_allocloc control_block_alloc = alloc;

      std::allocator_traits<control_block_allocloc>::destroy(
          control_block_alloc, this);
      std::allocator_traits<control_block_allocloc>::deallocate(
          control_block_alloc, this, 1);
    }

    ~ControlBlockMakeShared() {}
  };

  template <typename U, typename Deleter = std::default_delete<U>,
            typename Alloc = std::allocator<U>>
  struct ControlBlockMakeDirect : BaseControlBlock {
    U* object;
    Deleter deleter;
    Alloc alloc;

    ControlBlockMakeDirect(U* object, size_t shared_count, size_t weak_count,
                           bool made_directly,
                           const Deleter& deleter = Deleter(),
                           const Alloc& allocator = Alloc())
        : BaseControlBlock(shared_count, weak_count, made_directly),
          object(object),
          deleter(deleter),
          alloc(allocator) {}

    void destroy_object() override { deleter(object); }

    void deallocate_memory() override {
      using control_block_allocloc =
          typename std::allocator_traits<Alloc>::template rebind_alloc<
              ControlBlockMakeDirect<U, Deleter, Alloc>>;
      control_block_allocloc control_block_alloc = alloc;

      std::allocator_traits<control_block_allocloc>::destroy(
          control_block_alloc, this);
      std::allocator_traits<control_block_allocloc>::deallocate(
          control_block_alloc, this, 1);
    }

    ~ControlBlockMakeDirect() {}
  };

  BaseControlBlock* control_block_ = nullptr;
  T* object_ = nullptr;

  template <typename Y, typename Alloc>
  SharedPtr(ControlBlockMakeShared<Y, Alloc>* ptr)
      : control_block_(ptr), object_(reinterpret_cast<Y*>(ptr->object)) {}

  template <typename Y>
  SharedPtr(BaseControlBlock* ptr, Y* object)
      : control_block_(ptr), object_(object) {}

  void swap(SharedPtr& other) {
    std::swap(control_block_, other.control_block_);
    std::swap(object_, other.object_);
  }

  template <typename Y>
  friend class SharedPtr;

  template <typename Y, typename Alloc, typename... Args>
  friend SharedPtr<Y> AllocateShared(const Alloc& alloc, Args&&... args);

  template <typename Y>
  friend class WeakPtr;
};

template <typename T>
class WeakPtr {
 public:
  WeakPtr() noexcept = default;

  template <typename Y>
  WeakPtr(const SharedPtr<Y>& shared) noexcept
      : control_block_(dynamic_cast<BaseControlBlock*>(shared.control_block_)),
        object_(shared.object_) {
    ++control_block_->weak_count;
  }

  WeakPtr(const WeakPtr& other) noexcept
      : control_block_(other.control_block_), object_(other.object_) {
    ++control_block_->weak_count;
  }

  template <typename Y>
  WeakPtr(const WeakPtr<Y>& other) noexcept
      : control_block_(dynamic_cast<BaseControlBlock*>(other.control_block_)),
        object_(other.object_) {
    ++control_block_->weak_count;
  }

  WeakPtr(WeakPtr&& other) noexcept
      : control_block_(other.control_block_), object_(other.object_) {
    other.control_block_ = nullptr;
    other.object_ = nullptr;
  }

  template <typename Y>
  WeakPtr(WeakPtr<Y>&& other) noexcept
      : control_block_(dynamic_cast<BaseControlBlock*>(other.control_block_)),
        object_(other.object_) {
    other.control_block_ = nullptr;
    other.object_ = nullptr;
  }

  WeakPtr& operator=(const WeakPtr& other) {
    WeakPtr copy = other;
    swap(copy);
    return *this;
  }

  template <typename Y>
  WeakPtr& operator=(const WeakPtr<Y>& other) {
    WeakPtr copy = other;
    swap(copy);
    return *this;
  }

  WeakPtr& operator=(const WeakPtr&& other) {
    WeakPtr copy = std::move(other);
    swap(copy);
    return *this;
  }

  template <typename Y>
  WeakPtr& operator=(const WeakPtr<Y>&& other) {
    WeakPtr copy = std::move(other);
    swap(copy);
    return *this;
  }

  bool expired() const { return control_block_->shared_count == 0; }

  SharedPtr<T> lock() const {
    ++control_block_->shared_count;
    return SharedPtr<T>(control_block_, object_);
  }

  ~WeakPtr() {
    if (control_block_ == nullptr) {
      return;
    }
    --control_block_->weak_count;

    if (control_block_->weak_count == 0 && control_block_->shared_count == 0) {
      control_block_->deallocate_memory();
    }
  }

 private:
  BaseControlBlock* control_block_ = nullptr;
  T* object_ = nullptr;

  void swap(WeakPtr& other) {
    std::swap(control_block_, other.control_block_);
    std::swap(object_, other.object_);
  }

  template <typename Y>
  friend class WeakPtr;
};

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> AllocateShared(const Alloc& alloc, Args&&... args) {
  using control_block_allocloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<
          typename SharedPtr<T>::template ControlBlockMakeShared<T, Alloc>>;
  control_block_allocloc control_block_alloc = alloc;

  auto cb_ptr = std::allocator_traits<control_block_allocloc>::allocate(
      control_block_alloc, 1);
  std::allocator_traits<control_block_allocloc>::construct(
      control_block_alloc, cb_ptr, 1, 0, false, alloc,
      std::forward<Args>(args)...);

  return SharedPtr<T>(cb_ptr);
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return AllocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}