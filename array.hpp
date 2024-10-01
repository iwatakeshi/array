#ifndef STD_LIB_ARRAY_H
#define STD_LIB_ARRAY_H

#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>

namespace stdlib {

/**
 * @brief A dynamic array class template.
 * 
 * This class provides a dynamic array implementation with various utility functions
 * such as push, pop, insert, and more. It supports iterators and can be used with
 * standard algorithms.
 * 
 * @tparam T The type of elements stored in the array.
 */
template <typename T>
class array {
  private:
  /// @brief Pointer to the dynamically allocated array.
  std::unique_ptr<T[]> array_;
  /// @brief The offset of the first element in the array.
  std::size_t offset_ = 0;
  /// @brief The capacity of the array.
  std::size_t capacity_ = 0;
  /// @brief The length of the array.
  std::size_t length_ = 0;

  static constexpr bool is_string = std::is_same_v<std::remove_cvref_t<T>, const char*> || std::is_same_v<std::remove_cvref_t<T>, char*> || std::is_same_v<std::remove_cvref_t<T>, std::string>;

  public:
  /// @brief The type of elements stored in the array.
  using value_type = T;
  /// @brief The type of size of the array.
  using size_type = std::size_t;
  /// @brief The type of difference between iterators.
  using difference_type = std::ptrdiff_t;
  /// @brief The type of reference to elements.
  using reference = T&;
  /// @brief The type of const reference to elements.
  using const_reference = const T&;
  /// @brief The type of pointer to elements.
  using pointer = T*;
  /// @brief The type of const pointer to elements.
  using const_pointer = const T*;
  /// @brief The type of iterator.
  using iterator = T*;
  /// @brief The type of const iterator.
  using const_iterator = const T*;

  /// @brief Default constructor.
  array() = default;

  /**
   * @brief Constructs an array with a specified capacity.
   * 
   * @param capacity The initial capacity of the array.
   */
  explicit array(size_type capacity) :
      array_(std::make_unique<T[]>(capacity)), capacity_(capacity), length_(capacity) { }

  /**
   * @brief Constructs an array with a specified capacity and fills it with a value.
   * 
   * @param capacity The initial capacity of the array.
   * @param value The value to fill the array with.
   */
  array(size_type capacity, const T& value) :
      array(capacity) {
    std::fill_n(array_.get(), capacity, value);
  }

  /**
   * @brief Copy constructor.
   * 
   * @param other The array to copy from.
   */
  array(const array& other) :
      array_(std::make_unique<T[]>(other.capacity_)),
      offset_(other.offset_), capacity_(other.capacity_), length_(other.length_) {
    std::copy_n(other.array_.get(), capacity_, array_.get());
  }

  /**
   * @brief Move constructor.
   * 
   * @param other The array to move from.
   */
  array(array&&) noexcept = default;
  /**
   * @brief Move assignment operator.
   * 
   * @param other The array to move from.
   * @return Reference to this array.
   */
  array& operator=(array&&) noexcept = default;
  /**
   * @brief Constructs an array from an initializer list.
   * 
   * @param list The initializer list to construct the array from.
   */
  array(std::initializer_list<T> list) :
      array_(std::make_unique<T[]>(list.size())),
      capacity_(list.size()), length_(list.size()) {
    std::copy(list.begin(), list.end(), begin());
  }

  /**
   * @brief Constructs an array from a range.
   * 
   * @tparam Range The type of the range.
   * @param range The range to construct the array from.
   */
  template <typename Range>
    array(Range&& range, 
          std::enable_if_t<std::ranges::range<Range>, int> = 0)
        : array_(std::make_unique<T[]>(std::ranges::distance(range))), 
          length_(std::ranges::distance(range)), 
          capacity_(std::ranges::distance(range)) {
        std::ranges::copy(range, array_.get());
    }
  /**
   * @brief Constructs an array from a pair of iterators.
   * 
   * @tparam InputIt The type of the iterators.
   * @param first The first iterator.
   * @param last The last iterator.
   */
  template <typename InputIt>
    array(InputIt first, InputIt last,
          std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0)
        : array_(nullptr), length_(0), capacity_(0), offset_(0) {
        size_type n = std::distance(first, last);
        reserve(n);
        for (auto it = first; it != last; ++it) {
            push(*it);
        }
    }
  /**
   * @brief Copy assignment operator.
   * 
   * @param other The array to copy from.
   * @return Reference to this array.
   */
  array& operator=(const array& other) {
    if (this != &other) {
      array temp(other);
      swap(temp);
    }
    return *this;
  }
  /**
   * @brief Assignment operator from an initializer list.
   * 
   * @param list The initializer list to assign from.
   * @return Reference to this array.
   */
  array& operator=(std::initializer_list<T> list) {
    array temp(list);
    swap(temp);
    return *this;
  }
  /**
   * @brief Swaps the contents of this array with another array.
   * 
   * @param other The array to swap with.
   */
  void swap(array& other) noexcept {
    using std::swap;
    swap(array_, other.array_);
    swap(offset_, other.offset_);
    swap(capacity_, other.capacity_);
    swap(length_, other.length_);
  }
  /**
   * @brief Swaps the contents of two arrays.
   * 
   * @param lhs The first array.
   * @param rhs The second array.
   */
  friend void swap(array& lhs, array& rhs) noexcept {
    lhs.swap(rhs);
  }
  /**
   * @brief Accesses an element by index.
   * 
   * @param index The index of the element.
   * @return Reference to the element.
   */
  reference operator[](difference_type index) { return array_[offset_ + index]; }
  /**
   * @brief Accesses an element by index (const version).
   * 
   * @param index The index of the element.
   * @return Constant reference to the element.
   */
  const_reference operator[](difference_type index) const { return array_[offset_ + index]; }
  /**
   * @brief Multiplies the array by a scalar.
   * 
   * @param n The scalar to multiply by.
   * @return A new array with the elements repeated n times.
   */
  array operator*(size_type n) const {
    array result;
    result.reserve(size() * n);
    for (size_type i = 0; i < n; ++i) {
      result.insert(result.end(), begin(), end());
    }
    return result;
  }
  /**
   * @brief Multiplies the array by a scalar and assigns the result to this array.
   * 
   * @param n The scalar to multiply by.
   * @return Reference to this array.
   */
  array& operator*=(size_type n) {
    *this = *this * n;
    return *this;
  }
  /**
   * @brief Concatenates this array with another array.
   * 
   * @param other The array to concatenate with.
   * @return A new array with the elements of both arrays.
   */
  array operator+(const array& other) const {
    array result;
    result.reserve(size() + other.size());
    result.insert(result.end(), begin(), end());
    result.insert(result.end(), other.begin(), other.end());
    return result;
  }
  /**
   * @brief Concatenates another array to this array.
   * 
   * @param other The array to concatenate.
   * @return Reference to this array.
   */
  array& operator+=(const array& other) {
    insert(end(), other.begin(), other.end());
    return *this;
  }
  /**
   * @brief Compares this array with another array for equality.
   * 
   * @param other The array to compare with.
   * @return True if the arrays are equal, false otherwise.
   */
  bool operator==(const array& other) const {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  /**
   * @brief Compares this array with another array.
   * 
   * @param other The array to compare with.
   * @return The result of the comparison.
   */
  auto operator<=>(const array& other) const {
    return std::lexicographical_compare_three_way(begin(), end(), other.begin(), other.end());
  }
  /**
   * @brief Outputs the array to a stream.
   * 
   * @param os The output stream.
   * @param arr The array to output.
   * @return The output stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const array& arr) {
    os << "[ " << arr.join(", ") << " ]";
    return os;
  }
  /**
   * @brief Adds an element to the beginning of the array.
   * 
   * @param value The value to add.
   */
  void unshift(const T& value) {
    if (offset_ > 0) {
      array_[--offset_] = value;
      ++length_;
    } else {
      size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
      reserve(new_capacity);
      std::shift_right(begin(), end(), 1);
      array_[0] = value;
      ++length_;
    }
  }

  /**
   * @brief Removes and returns the first element of the array.
   * 
   * @return The first element of the array.
   * @throws std::out_of_range If the array is empty.
   */
  T shift() {
    if (empty()) {
      throw std::out_of_range("Array is empty");
    }
    T value = std::move(array_[offset_]);
    ++offset_;
    --length_;
    return value;
  }
  /**
   * @brief Adds an element to the end of the array.
   * 
   * @param value The value to add.
   */
  void push(const T& value) {
    if (length_ == capacity_) {
      size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
      reserve(new_capacity);
    }
    array_[length_++] = value;
  }
  /**
   * @brief Removes and returns the last element of the array.
   * 
   * @return The last element of the array.
   * @throws std::out_of_range If the array is empty.
   */
  T pop() {
    if (empty()) {
      throw std::out_of_range("Array is empty");
    }
    return std::move(array_[--length_]);
  }
  /**
   * @brief Accesses an element by index with bounds checking.
   * 
   * @param index The index of the element.
   * @return Reference to the element.
   * @throws std::out_of_range If the index is out of bounds.
   */
  reference at(difference_type index) {
    if (index < 0 || static_cast<size_type>(index) >= size()) {
      throw std::out_of_range("Index out of bounds");
    }
    return (*this)[index];
  }
  /**
   * @brief Accesses an element by index with bounds checking (const version).
   * 
   * @param index The index of the element.
   * @return Constant reference to the element.
   * @throws std::out_of_range If the index is out of bounds.
   */
  const_reference at(difference_type index) const {
    if (index < 0 || static_cast<size_type>(index) >= size()) {
      throw std::out_of_range("Index out of bounds");
    }
    return (*this)[index];
  }
  /**
   * @brief Applies a function to each element of the array.
   * 
   * @param f The function to apply.
   */
  void for_each(const std::function<void(T)>& f) const {
    std::for_each(begin(), end(), f);
  }
  /**
   * @brief Applies a function to each element of the array, passing the index as well.
   * 
   * @param f The function to apply.
   */
  void for_each(const std::function<void(T, size_type)>& f) const {
    for (size_type i = 0; i < size(); ++i) {
      f((*this)[i], i);
    }
  }
  /**
   * @brief Filters the array using a predicate.
   * 
   * @tparam Pred The type of the predicate.
   * @param pred The predicate to use for filtering.
   * @return A new array with the elements that satisfy the predicate.
   */
  template <std::invocable<T> Pred>
  array filter(Pred pred) const {
    return array(std::ranges::filter_view(std::ranges::subrange(begin(), end()), pred));
  }
  /**
   * @brief Filters the array using a predicate, passing the index as well.
   * 
   * @tparam Pred The type of the predicate.
   * @param pred The predicate to use for filtering.
   * @return A new array with the elements that satisfy the predicate.
   */
  template <std::invocable<T, size_type> Pred>
  array filter(Pred pred) const {
    array result;
    for (size_type i = 0; i < size(); ++i) {
      if (pred((*this)[i], i)) {
        result.push((*this)[i]);
      }
    }
    return result;
  }
  /**
   * @brief Maps the array using a function.
   * 
   * @tparam F The type of the function.
   * @param f The function to use for mapping.
   * @return A new array with the mapped elements.
   */
  template <std::invocable<T> F>
  auto map(F f) const -> array<std::invoke_result_t<F, T>> {
    return array<std::invoke_result_t<F, T>>(std::ranges::transform_view(std::ranges::subrange(begin(), end()), f));
  }
  /**
   * @brief Maps the array using a function, passing the index as well.
   * 
   * @tparam F The type of the function.
   * @param f The function to use for mapping.
   * @return A new array with the mapped elements.
   */
  template <std::invocable<T, size_type> F>
  auto map(F f) const -> array<std::invoke_result_t<F, T, size_type>> {
    array<std::invoke_result_t<F, T, size_type>> result;
    result.reserve(size());
    for (size_type i = 0; i < size(); ++i) {
      result.push(f((*this)[i], i));
    }
    return result;
  }
  /**
   * @brief Reduces the array to a single value using a function.
   * 
   * @tparam U The type of the result.
   * @param f The function to use for reducing.
   * @param initial The initial value.
   * @return The reduced value.
   */
  template <typename U>
  U reduce(const std::function<U(U, T)>& f, U initial) const {
    return std::accumulate(begin(), end(), initial, f);
  }
  /**
   * @brief Reduces the array to a single value using a function.
   * 
   * @tparam Func The type of the function.
   * @param func The function to use for reducing.
   * @param initial The initial value.
   * @return The reduced value.
   */
  template <typename Func>
  T reduce(Func func, T initial) const {
    T accumulator = initial;
    for (uint64_t i = 0; i < length_; ++i) {
      accumulator = func(accumulator, array_[i]);
    }
    return accumulator;
  }
  /**
   * @brief Reverses the array.
   * 
   * @return A new array with the elements in reverse order.
   */
  array reverse() const {
    array result(*this);
    std::ranges::reverse(result);
    return result;
  }
  /**
   * @brief Slices the array.
   * 
   * @param start The start index.
   * @param end The end index (default is -1, which means the end of the array).
   * @return A new array with the sliced elements.
   */
  array slice(difference_type start, difference_type end = -1) const {
    if (end < 0) end += size();
    start = std::clamp<difference_type>(start, 0, size());
    end = std::clamp<difference_type>(end, start, size());
    return array(begin() + start, begin() + end);
  }
  /**
   * @brief Joins the elements of the array into a string.
   * 
   * @param sep The separator to use (default is ",").
   * @return A string with the joined elements.
   */
  std::string join(std::string_view sep = ",") const {
    return std::accumulate(begin(), end(), std::string {},
      [&sep](const std::string& a, const T& b) {
        return a.empty() ? to_string(b) : a + std::string(sep) + to_string(b);
      });
  }
  /**
   * @brief Reserves memory for the array.
   * 
   * @param new_capacity The new capacity of the array.
   */
  void reserve(size_type new_capacity) {
    if (new_capacity > capacity_) {
      auto new_array = std::make_unique<T[]>(new_capacity);
      std::copy_n(array_.get() + offset_, size(), new_array.get());
      array_ = std::move(new_array);
      capacity_ = new_capacity;
      length_ -= offset_;
      offset_ = 0;
    }
  }

  /**
   * @brief Inserts elements into the array at a specified position.
   * 
   * This function inserts a range of elements into the array at the specified position.
   * If the current capacity is insufficient to accommodate the new elements, the array
   * is reallocated with a larger capacity.
   * 
   * @tparam InputIt The type of the input iterator.
   * @param pos The position to insert at. This must be a valid iterator within the array.
   * @param first The first iterator of the range to insert.
   * @param last The last iterator of the range to insert.
   * 
   * @pre `pos` must be a valid iterator within the array.
   * @pre The range `[first, last)` must be a valid range.
   * 
   * @post The elements in the range `[first, last)` are inserted at the position `pos`.
   *       The size of the array is increased by the number of elements inserted.
   * 
   * @throws std::bad_alloc if memory allocation fails.
   * 
   * @note If the array is reallocated, all iterators, pointers, and references to elements
   *       in the array are invalidated.
   */
  template <typename InputIt>
  void insert(const_iterator pos, InputIt first, InputIt last) {
    size_type n = std::distance(first, last);
    assert(pos >= begin() && pos <= end()); // Ensure pos is within valid range
    auto pos_index = pos - begin();
    if (size() + n > capacity_) {
      reserve((size() + n) * 2);
      pos = begin() + pos_index; // Update pos after reserve
    }
    pos_index = pos - begin();
    assert(pos_index + n <= capacity_); // Ensure we don't move elements out of bounds
    std::move_backward(begin() + pos_index, end(), end() + n);
    std::copy(first, last, begin() + pos_index);
    length_ += n;
  }

  /**
   * @brief Inserts an element into the array at a specified position.
   * 
   * This function inserts an element into the array at the specified position.
   * If the current capacity is insufficient to accommodate the new element, the array
   * is reallocated with a larger capacity.
   * 
   * @param pos The position to insert at. This must be a valid iterator within the array.
   * @param value The value to insert.
   * 
   * @pre `pos` must be a valid iterator within the array.
   * 
   * @post The element `value` is inserted at the position `pos`.
   *       The size of the array is increased by one.
   * 
   * @throws std::bad_alloc if memory allocation fails.
   * 
   * @note If the array is reallocated, all iterators, pointers, and references to elements
   *       in the array are invalidated.
   */
  void insert(const_iterator pos, const T& value) {
    assert(pos >= begin() && pos <= end()); // Ensure pos is within valid range
    auto pos_index = pos - begin();
    if (size() + 1 > capacity_) {
        reserve((size() + 1) * 2);
        pos = begin() + pos_index; // Update pos after reserve
    }
    pos_index = pos - begin();
    // Shift elements to the right
    std::move_backward(begin() + pos_index, end(), end() + 1);
    // Insert the new element
    array_[pos_index] = value;
    ++length_;
}

  /// @return The the size of the array.
  [[nodiscard]] size_type size() const noexcept { return length_ - offset_; }
  /// @return The capacity of the array.
  [[nodiscard]] size_type capacity() const noexcept { return capacity_; }
  /// @brief Determines if the array is empty. 
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }
  /// @brief Determines if the array is full.
  [[nodiscard]] bool full() const noexcept { return size() == capacity_; }
  /// @return An iterator to the beginning of the array.
  iterator begin() noexcept { return array_.get() + offset_; }
  /// @return A const iterator to the beginning of the array.
  const_iterator begin() const noexcept { return array_.get() + offset_; }
  /// @return An iterator to the end of the array.
  iterator end() noexcept { return array_.get() + length_; }
  /// @return A const iterator to the end of the array.
  const_iterator end() const noexcept { return array_.get() + length_; }

  private:
  template <typename U>
  static std::string to_string(const U& value) {
        if constexpr (is_string) {
            return fmt::format("\"{}\"", value);
        } else {
            return fmt::format("{}", value);
        }
    }
};

} // namespace stdlib

#endif // STD_LIB_ARRAY_H