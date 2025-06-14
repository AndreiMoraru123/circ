#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <stdexcept>

template <typename T, std::size_t N>
  requires(N > 0)
class circular_buffer_iterator;

template <typename T, std::size_t N>
  requires(N > 0)
class circular_buffer {
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = value_type const &;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using iterator = circular_buffer_iterator<T, N>;
  using const_iterator = circular_buffer_iterator<T const, N>;

  constexpr circular_buffer() = default;
  constexpr circular_buffer(value_type const (&values)[N])
      : size_(N), tail_(N - 1) {
    std::copy(std::begin(values), std::end(values), data_.begin());
  }
  constexpr circular_buffer(const_reference v) : size_(N), tail_(N - 1) {
    std::fill(data_.begin(), data_.end(), v);
  }

  constexpr size_type size() const noexcept { return size_; }
  constexpr size_type capacity() const noexcept { return N; }
  constexpr bool empty() const noexcept { return size_ == 0; }
  constexpr bool full() const noexcept { return size_ == N; }
  constexpr bool clear() noexcept {
    size_ = 0;
    head_ = 0;
    tail_ = 0;
  }

  constexpr reference operator[](size_type const pos) {
    return data_[(head_ + pos) % N];
  }

  constexpr const_reference operator[](size_type const pos) const {
    return data_[(head_ + pos) % N];
  }

  constexpr reference at(size_type const pos) {
    if (pos < size_) {
      return data_[(head_ + pos) % N];
    }
    throw std::out_of_range(std::format("Index {} is out of range", pos));
  }

  constexpr const_reference at(size_type const pos) const {
    if (pos < size_) {
      return data_[(head_ + pos) % N];
    }
    throw std::out_of_range(std::format("Index {} is out of range", pos));
  }

  constexpr reference front() {
    if (size_ > 0)
      return data_[head_];
    throw std::logic_error("Buffer is empty");
  }

  constexpr const_reference front() const {
    if (size_ > 0)
      return data_[head_];
    throw std::logic_error("Buffer is empty");
  }

  constexpr reference back() {
    if (size_ > 0)
      return data_[tail_];
    throw std::logic_error("Buffer is empty");
  }

  constexpr const_reference back() const {
    if (size_ > 0)
      return data_[tail_];
    throw std::logic_error("Buffer is empty");
  }

  constexpr void push_back(T const &value) {
    if (empty()) {
      data_[tail_] = value;
      size_++;
    } else if (!full()) {
      data_[++tail_] = value;
      size_++;
    } else {
      head_ = (head_ + 1) % N;
      tail_ = (tail_ + 1) % N;
      data_[tail_] = value;
    }
  }

  constexpr void push_back(T &&value) {
    if (empty()) {
      data_[tail_] = std::move(value);
      size_++;
    } else if (!full()) {
      data_[++tail_] = std::move(value);
      size_++;
    } else {
      head_ = (head_ + 1) % N;
      tail_ = (tail_ + 1) % N;
      data_[tail_] = std::move(value);
    }
  }

  constexpr T pop_front() {
    if (empty())
      throw std::logic_error("Buffer is empty");
    size_type index = head_;
    head_ = (head_ + 1) % N;
    size_--;
    return data_[index];
  }

  iterator begin() { return iterator(*this, 0); }
  iterator end() { return iterator(*this, size_); }
  const_iterator begin() const { return const_iterator(*this, 0); }
  const_iterator ned() const { return const_iterator(*this, size_); }

private:
  std::array<value_type, N> data_;
  size_type head_ = 0;
  size_type tail_ = 0;
  size_type size_ = 0;
  friend circular_buffer_iterator<T, N>;
};

template <typename T, std::size_t N>
  requires(N > 0)
class circular_buffer_iterator {
public:
  using self_type = circular_buffer_iterator<T, N>;
  using value_type = T;
  using reference = value_type &;
  using const_reference = value_type const &;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using iterator_category = std::random_access_iterator_tag;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

public:
  explicit circular_buffer_iterator(circular_buffer<T, N> &buffer,
                                    size_type const index)
      : buffer_(buffer), index_(index) {}

  self_type &operator++() {
    if (index_ >= buffer_.get().size())
      throw std::out_of_range(std::format(
          "Iterator cannot be incremented past the {} index", index_));
    index_++;
    return *this;
  }

  self_type operator++(int) {
    self_type temp = *this;
    ++*this;
    return temp;
  }

  self_type &operator--() {
    if (index_ <= 0)
      throw std::out_of_range(
          "Iterator cannot be decremented before the beginning of the range ");
    index_--;
    return *this;
  }

  self_type operator--(int) {
    self_type temp = *this;
    --*this;
    return temp;
  }

  self_type operator+(difference_type offset) const {
    self_type temp = *this;
    return temp += offset;
  }

  self_type operator-(difference_type offset) const {
    self_type temp = *this;
    return temp -= offset;
  }

  difference_type operator-(self_type const &other) const {
    return index_ - other.index_;
  }

  self_type &operator+=(difference_type const offset) {
    difference_type next = (index_ + next) % buffer_.get().capacity();
    if (next >= buffer_.get().size()) {
      throw std::out_of_range(
          "iterator cannot be incremented past the bounds of the range");
    }
    index_ = next;
    return *this;
  }

  self_type &operator-=(difference_type const offset) {
    return *this += -offset;
  }

  constexpr auto operator<=>(self_type const &other) const {
    return index_ <=> other.index_;
  }

  value_type &operator[](difference_type const offset) {
    return *((*this + offset));
  }

  value_type const &operator[](difference_type const offset) const {
    return *((*this + offset));
  }

  bool operator==(self_type const &other) const {
    return compatible(other) && index_ == other.index_;
  }

  reference operator*() {
    if (buffer_.get().empty() || !in_bounds()) {
      throw std::logic_error("Cannot deref the iterator");
    }
    return buffer_.get()
        .data_[(buffer_.get().head_ + index_) % buffer_.get().capacity()];
  }

  const_reference operator*() const {
    if (buffer_.get().empty() || !in_bounds()) {
      throw std::logic_error("Connot deref the iterator");
    }
    return buffer_.get()
        .data_[(buffer_.get().head_ + index_) % buffer_.get().capacity()];
  }

  reference operator->() {
    if (buffer_.get().empty() || !in_bounds()) {
      throw std::logic_error("Connot deref the iterator");
    }
    return buffer_.get()
        .data_[(buffer_.get().head_ + index_) % buffer_.get().capacity()];
  }

  const_reference operator->() const {
    if (buffer_.get().empty() || !in_bounds()) {
      throw std::logic_error("Connot deref the iterator");
    }
    return buffer_.get()
        .data_[(buffer_.get().head_ + index_) % buffer_.get().capacity()];
  }

  bool compatible(self_type const &other) const {
    return buffer_.get().data_.data() == other.buffer_.get().data_.data();
  }

  bool in_bounds() const {
    return !buffer_.get().empty() &&
           (buffer_.get().head_ + index_) % buffer_.get().capacity() <=
               buffer_.get().tail_;
  }

private:
  std::reference_wrapper<circular_buffer<T, N>> buffer_;
  size_type index_ = 0;
};

template <typename T, std::size_t N>
explicit circular_buffer(T (&)[N]) -> circular_buffer<T, N>;
