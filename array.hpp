#ifndef ARRAY_H
#define ARRAY_H
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using std::function;
using std::ostream;
using std::string;

namespace array_utils {
  /**
   * Credits
   * https://www.fluentcpp.com/2017/06/06/using-tostring-custom-types-cpp/
   */

  template<typename...>
  using try_to_instantiate = void;

  using disregard_this = void;

  template<template<typename...> class Expression, typename Attempt, typename... Ts>
  struct is_detected_impl : std::false_type{};

  template<template<typename...> class Expression, typename... Ts>
  struct is_detected_impl<Expression, try_to_instantiate<Expression<Ts...>>, Ts...> : std::true_type{};

  template<template<typename...> class Expression, typename... Ts>
  constexpr bool is_detected = is_detected_impl<Expression, disregard_this, Ts...>::value;

  // 1- detecting if std::to_string is valid on T
  template<typename T>
  using std_to_string_expression = decltype(std::to_string(std::declval<T>()));

  template<typename T>
  constexpr bool has_std_to_string = is_detected<std_to_string_expression, T>;

  // 2- detecting if to_string is valid on T
  template<typename T>
  using to_string_expression = decltype(to_string(std::declval<T>()));

  template<typename T>
  constexpr bool has_to_string = is_detected<to_string_expression, T>;

  // 3- detecting T can be sent to an ostringstream
  template<typename T>
  using ostringstream_expression = decltype(std::declval<std::ostringstream&>() << std::declval<T>());

  template<typename T>
  constexpr bool has_ostringstream = is_detected<ostringstream_expression, T>;

  // 1-  std::to_string is valid on T
  template<typename T, typename std::enable_if<has_std_to_string<T>, int>::type = 0>
  std::string to_string(T const& t) {
      return std::to_string(t);
  }

  // 2-  std::to_string is not valid on T, but to_string is
  template<typename T, typename std::enable_if<!has_std_to_string<T> && has_to_string<T>, int>::type = 0>
  std::string to_string(T const& t) {
      return to_string(t);
  }

  // 3-  neither std::string nor to_string work on T, let's stream it then
  template<typename T, typename std::enable_if<!has_std_to_string<T> && !has_to_string<T> && has_ostringstream<T>, int>::type = 0>
  std::string to_string(T const& t) {
      std::ostringstream oss;
      oss << t;
      return oss.str();
  }
}

template <class T>
class array {
private:
  // The pointer array
  T * array_ = nullptr;
  // The offset after an element is removed
  // from the front of the array.
  // This allows the array to have a O(1) when
  // removing an element from the front.
  uint64_t offset_ = 0;
  // The heap size.
  uint64_t size_ = 0;
  // The number of elements in the array.
  uint64_t length_ = 0;

public:
 /**
  * 	Initializes a new empty array.
  */
  array() {}

  /**
   * 	Initializes a new array storing n copies of zeroes.
   */
  array(uint64_t size) {
    array_ = new T[size];
    size_ = size;
    length_ = size;
  }

  /**
   * 	Initializes a new array storing n copies of the given value.
   */
  array(uint64_t size, T const& value): array(size) {
    for(auto i = 0; i < size; i++) {
      array_[i] = value;
    }
  }

  array(array const& other) {
    reserve(other.size_, false);
    for(auto i = 0; i < other.size_; i++) {
      array_[i] = other.array_[i];
    }

    offset_ = other.offset_;
    size_ = other.size_;
    length_ = other.length_;
  }

  /**
   * 	Initializes a new array with list initialization.
   */
  array(std::initializer_list<T> const& list) {
    reserve(list.size(), false);

    for(auto element : list) {
      this->push(element);
    }
  }

  ~array() {
    delete [] array_;
    offset_ = 0;
    size_ = 0;
    length_ = 0;
  }

  /**
   * Overloads [] to select elements from this array.
   */
  T& operator [] (int64_t const& index) {
    return array_[offset_ + index];
  }

  /**
   * Overloads [] to select elements from this array.
   */
  const T& operator [] (int64_t const& index) const {
    return array_[offset_ + index];
  }

  /**
   * Repeats the values in this array by n times.
   */
  array<T> operator * (uint64_t const & right) const {
    array<T> temp;
    for (auto i = 0; i < right; i++) {
      for(auto j = 0; j < (length_ - offset_); j++) {
        temp.push(this->operator[](j));
      }
    }

    return temp;
  }

  /**
   * Assigns this array with the array on the right hand side.
   */
  array<T>& operator = (array<T> const& right) {
    if (size_ < right.size_) {
      reserve(right.size_, false);
    }

    size_ = right.size_;
    offset_ = right.offset_;
    length_ = right.length_ + right.offset_;

    for (auto i = 0; i < size_; i++) {
      array_[i] = right.array_[i];
    }

    return *this;
  }

  /**
   * Assigns this array with the array on the right hand side.
   */
  array<T>& operator = (std::initializer_list<T> const& list) {
    T * temp = new T[list.size()];
    auto index = 0;
    for(auto element : list) {
      temp[index++] = element;
    }
    
    delete [] array_;
    array_ = temp;
    size_ = list.size();
    length_ = size_;
    offset_ = 0;

    return *this;
  }

  /**
   * Concantenates two arrays.
   */
  array<T> operator + (array<T> const& right) const {
    array<T> temp;

    for(auto i = 0; i < (length_ - offset_); i++) {
      temp.push(this->operator[](i));
    }
    

    for (auto i = 0; i < (right.length_ - right.offset_); i++) {
      temp.push(right.operator[](i));
    }

    return temp;
  }

  /**
   * Concantenates two arrays and assigns it to this array.
   */
  array<T>& operator += (array<T> const& right) {
    for (auto i = 0; i < (right.length_ - right.offset_); i++) {
      this->push(right.operator[](i));
    }

    return *this;
  }

  /**
   * Repeats the values in this array by n times and assigns it to this array.
   */
  array<T>& operator *= (uint64_t const& right) const {
    T * temp = new T[(length_ - offset_) * right];
    auto index = 0;
    for (auto i = 0; i < right; i++) {
     for (auto j = 0; j < (length_ - offset_); j++) {
       temp[index] = this->operator[](j);
       index += 1;
     }
    }

    delete [] array_;
    array_ = temp;
    size_ = ((length_ - offset_)) * right;
    length_ = size_;
    offset_ = 0;

    return *this;
  }

  /**
   * Outputs the contents of the array to the given output stream.
   */
  friend ostream& operator << (ostream& os, const array<T>& array) {
    os  << "[ " << array.join(", ") << " ]";
    return os;
  }
  
  /**
   * Adds a new value to the beginning of this array.
   */
  void unshift(T const& value) {
    // Determine if the array is full
    auto size = 0;
    if (size_ == 0) {
      size = std::max((uint64_t) 1, (uint64_t) (size_ * 2));
    } else {
      size = is_full() ? 2 * size_ : (length_ - offset_) + 1;
    }

    T * array = new T[size];
    
    array[0] = value;

    for(auto i = 1; i <= (length_ - offset_); i++) {
      array[i] = this->operator[]((i - 1));
    }

    delete [] array_;
    array_ = array;

    length_ += 1;
    size_ = size;
  }
  
  /**
   * Removes the value from the beginning of this array and returns it.
   */
  T& shift() {
    if (is_empty()) {
      throw std::out_of_range("Array is empty"); 
    }

    T& element = array_[offset_];
    offset_ += 1;

    return element;
  }
  
  /**
   * Adds a new value to the end of this array.
   */
  void push(T const& value) {
    // Determine if the array is full
    if (size_ == 0) {
      reserve(std::max((uint64_t) 1, (uint64_t) size_ * 2), false);
    } else if (is_full()) {
      reserve(2 * size_, true);
    }

    array_[(length_ - offset_)] = value;
    length_ += 1;
  }
  
  /**
   * Removes the value from the end of this array and returns it.
   */
  T& pop() {
    if (is_empty()) {
      throw std::out_of_range("Array is empty");
    }

    T& element = array_[(length_ - offset_) - 1];
    length_ -= 1;

    return element;
  }

  /**
   * Returns the element at the given index with bound checking.
   */
  T& at(int64_t index) {
    if (index < 0 || index >= (length_ - offset_)) {
      throw std::out_of_range("Index is out of bounds");
    }

    return this->operator[](index);
  }

  /**
   * Sets the value at the given index with bound checking.
   */
  void at(int64_t index, T const& value) {
    if (index < 0 || index >= (length_ - offset_)) {
      throw std::out_of_range("Index is out of bounds");
    }

    this->operator[](index) = value;
  }

  /**
   * Iterates through each value in this array. 
   */
  void for_each(function<void (T)>const& lambda) const {
    for(auto i = 0; i < (length_ - offset_); i++) {
      lambda(this->operator[](i));
    }
  }

  /**
   * Iterates through each value in this array. 
   */
  void for_each(function<void (T, int64_t)>const& lambda) const {
    for(auto i = 0; i < (length_ - offset_); i++) {
      lambda(this->operator[](i), i);
    }
  }

  /**
   * Filters this array and returns a new array based on a condition.
   */
  array<T> filter(function<bool (T)> const& lambda) const {
    array<T> temp;
    for(auto i = 0; i < (length_ - offset_); i++) {
      if (lambda(this->operator[](i))) {
        temp.push(this->operator[](i));
      }
    }
    return temp;
  }

  /**
   * Filters this array and returns a new array based on a condition.
   */
  array<T> filter(function<bool (T, int64_t)> const& lambda) const {
    array<T> temp;
    for(auto i = 0; i < (length_ - offset_); i++) {
      if (lambda(this->operator[](i), i)) {
        temp.push(this->operator[](i));
      }
    }
    return temp;
  }

  /**
   *Maps each value in this array and returns a new array of type T.
   */
  array<T> map(function<T (T)> const& lambda) const {
    array<T> temp;
    for(auto i = 0; i < (length_ - offset_); i++) {
      temp.push(lambda(this->operator[](i)));
    }
    return temp;
  }

  /**
   * Maps each value in this array and returns a new array of type T.
   */
  array<T> map(function<T (T, int64_t)> const& lambda) const {
    array<T> temp;
    for(auto i = 0; i < (length_ - offset_); i++) {
      temp.push(lambda(this->operator[](i), i));
    }
    return temp;
  }

  /**
   * Maps each value in this array and returns a new array of type U.
   */
  template <typename U>
  array<U> map(function<U (T)> const& lambda) const {
    array<U> temp;
    for(auto i = 0; i < (length_ - offset_); i++) {
      temp.push(lambda(this->operator[](i)));
    }
    return temp;
  }

  /**
   *Maps each value in this array and returns a new array of type U.
   */
  template <typename U>
  array<U> map(function<U (T, int64_t)> const& lambda) const {
    array<U> temp;
    for(auto i = 0; i < (length_ - offset_); i++) {
      temp.push(lambda(this->operator[](i), i));
    }
    return temp;
  }

   /**
    * Reduces the values in this array into a single output value of type T.
    */
    T reduce(function<T (T, T)> const & lambda) const {
      T value = this->operator[](0);
      for(auto i = 1; i < (length_ - offset_); i++) {
        value = lambda(value, this->operator[](i));
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type T.
     */
    T reduce(function<T (T, T, int64_t)> const & lambda) const {
      T value = this->operator[](0);
      for(auto i = 1; i < (length_ - offset_); i++) {
        value = lambda(value, this->operator[](i), i);
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type T.
     */
    T reduce(function<T (T, T)> const & lambda, T initial) const {
      T value = initial;
      for(auto i = 0; i < (length_ - offset_); i++) {
        value = lambda(value, this->operator[](i));
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type T.
     */
    T reduce(function<T (T, T, int64_t)> const & lambda, T initial) const {
      T value = initial;
      for(auto i = 0; i < (length_ - offset_); i++) {
        value = lambda(value, this->operator[](i), i);
      }
      return value;
    }


    /**
     * Reduces the values in this array into a single output value of type U.
     */
    template <typename U>
    U reduce(function<U (U, T)> const & lambda, U initial) const {
      U value = initial;
      for(auto i = 0; i < (length_ - offset_); i++) {
        value = lambda(value, this->operator[](i));
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type U.
     */
    template <typename U>
    U reduce(function<U (U, T, int64_t)> const & lambda, U initial) const {
      U value = initial;
      for(auto i = 0; i < (length_ - offset_); i++) {
        value = lambda(value, this->operator[](i), i);
      }
      return value;
    }
  
  /**
   * Reverses the values in this array and returns a new array.
   */
  array<T> reverse() const {
    array<T> temp(length_ - offset_);
    auto index = 0;
    for(auto i = (length_ - offset_) - 1; i >= 0; i--) {
      temp[index] = this->operator[](i);
      index += 1;
    }
    return temp;
  }

  /**
   * Returns a new array with the values from the specified index.
   */
  array<T> slice(int64_t index) const {
    return slice(index, (length_ - offset_));
  }

  /**
   * Returns a new array with the values from the specified range.
   */
  array<T> slice(int64_t begin, int64_t end) const {
    int64_t len = length_ - offset_;
    begin = (begin >= 0) ? begin : std::max(int64_t(0), len + begin);
    end = (end <= len) ? end : std::min(end, len);

    if (end < 0) {
      end = len + end;
    }

    auto size = end - begin;

    array<T> temp(size);
    for (auto i = 0; i < size; i++) {
      temp[i] = this->operator[](begin + i);
    }
    return temp;
  } 
  
  /**
   * Returns a string of this array using ',' as the default seperator.
   */
  string join() const {
    return this->join(",");
  }
  
  /**
   * Returns a string of this array using a provided seperator.
   */
  string join(const string& seperator) const {
    string result = "";
    this->for_each([&] (T x, auto i) {
      result += (array_utils::to_string(x) + ((i == (length_ - offset_) - 1) ? "" : seperator));
    });
    return result;
  }
  
  /**
   * Allocates space on the heap.
   */
  void reserve(uint64_t size) {
    reserve(size, false);
  }
  
  /**
   * Allocates space on the heap.
   */
  void reserve(uint64_t size, bool copy) {
    T * array = new T[size];

    if (copy) {
      for(auto i = 0; i < size_; i++) {
        array[i] = this->operator[](i);
      }
    }

    delete [] array_;

    size_ = size;
    array_ = array;
  }
  
  /**
   * Return the number of elements in the array.
   */
  int64_t length() const {
    return length_ - offset_;
  }
  
  /**
   * Determine whether the array is empty.
   */
  bool is_empty() const {
    return (length_ - offset_) == 0;
  }
  
  /**
   * Determine whether the array is full.
   */
  bool is_full() const {
    return (length_ - offset_) == size_;
  }
};


#endif
