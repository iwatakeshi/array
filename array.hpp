#ifndef ARRAY_H
#define ARRAY_H
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <stdexcept>
#include <algorithm>

using std::function;
using std::ostream;
using std::string;
using std::is_fundamental;

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
class Array {
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
  Array() {}

  Array(uint64_t size) {
    array_ = new T[size];
  }

  Array(Array const& other) {
    reserve(other.size_, false);
    for(auto i = 0; i < other.size_; i++) {
      array_[i] = other.array_[i];
    }

    offset_ = other.offset_;
    size_ = other.size_;
    length_ = other.length_;
  }

  ~Array() {
    delete [] array_;
    offset_ = 0;
    size_ = 0;
    length_ = 0;
  }

  /**
   * Return the value at the specified index.
   */
  T& operator [] (int64_t const& index) {
    if (index < 0 || index >= length()) {
      throw std::out_of_range("Index is out of bounds");
    }

    return array_[offset_ + index];
  }

  /**
   * Return an array its value(s) n times.
   */
  Array<T> operator * (uint64_t const & right) {
    Array<T> temp;
    for (auto i = 0; i < right; i++) {
     for (auto j = this->offset_; j < this->length_; j++) {
       temp.push(this->array_[j]);
     }
    }

    return temp;
  }

  Array<T>& operator = (Array<T> const& right) {
    if (size_ < right.size_) {
      reserve(right.size_, false);
    }

    size_ = right.size_;
    offset_ = 0;
    length_ = right.length_ + right.offset_;

    for (auto i = 0; i < size_; i++) {
      array_[i] = right.array_[right.offset_ + i];
    }

    return *this;
  }

  /**
   * Return a concantenated array.
   */
  Array<T> operator + (Array<T> const& right) {
    Array<T> temp;

    for(auto i = offset_; i < length_; i++) {
      temp.push(array_[i]);
    }
    

    for (auto i = right.offset_; i < right.length_; i++) {
      temp.push(right.array_[i]);
    }

    return temp;
  }

  /**
   * Assign a concantenated array.
   */
  Array<T>& operator += (Array<T> const& right) {
    for (auto i = right.offset_; i < right.length_; i++) {
      this->push(right.array_[i]);
    }

    return *this;
  }

  /**
   * Assign an array with its value(s) n times.
   */
  Array<T>& operator *= (uint64_t const& right) {
    T * temp = new T[(length_ - offset_) * right];
    auto index = 0;
    for (auto i = 0; i < right; i++) {
     for (auto j = offset_; j < length_; j++) {
       temp[index] = array_[j];
       index += 1;
     }
    }

    delete [] array_;
    array_ = temp;
    size_ = (length_ - offset_) * right;
    length_ = size_;
    offset_ = 0;

    return *this;
  }

  /**
   * Return an output stream representation of the array.
   */
  friend ostream& operator << (ostream& os, const Array<T>& array) {
    string seperator = ", ";
    string result = "";
    auto length = array.length_;
    for(auto i = array.offset_; i < length; i++) {
      if (i == length - 1) {
        seperator = "";
      }

      result += (array_utils::to_string(array.array_[i]) + seperator);
    }
    os  << "[" << result << "]";
    return os;
  }
  
  /**
   * Add an element to the front of the array.
   */
  void unshift(T const& value) {
    // Determine if the array is full
    auto size = 0;
    if (size_ == 0) {
      size = std::max((uint64_t) 1, (uint64_t) (size_ * 2));
    } else {
      size = is_full() ? 2 * size_ : length() + 1;
    }

    T * array = new T[size];
    
    array[0] = value;

    for(auto i = 1; i <= length(); i++) {
      array[i] = array_[offset_ + (i - 1)];
    }

    delete [] array_;
    array_ = array;

    length_ += 1;
    size_ = size;
  }
  
  /**
   * Remove an element from the front of the array.
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
   * Add an element to the back of the array.
   */
  void push(T const& value) {
    // Determine if the array is full
    if (size_ == 0) {
      reserve(std::max((uint64_t) 1, (uint64_t) size_ * 2), false);
    } else {
      reserve(is_full() ? 2 * size_ : length() + 1, true);
    }

    array_[length()] = value;
    length_ += 1;
  }
  
  /**
   * Remove an element from the back of the array.
   */
  T& pop() {
    if (is_empty()) {
      throw std::out_of_range("Array is empty");
    }

    T& element = array_[length() - 1];
    length_ -= 1;

    return element;
  }

  /**
   * Iterate through each value in the array. 
   */
  void for_each(function<void (T)>const& lambda) {
    for(uint64_t i = 0; i < length(); i++) {
      lambda(array_[offset_ + i]);
    }
  }

  /**
   * Iterate through each value in the array. 
   */
  void for_each(function<void (T, uint64_t)>const& lambda) {
    for(uint64_t i = 0; i < length(); i++) {
      lambda(array_[offset_ + i], i);
    }
  }

  /**
   * Filter the array based on a condition.
   */
  Array<T> filter(function<bool (T)> const& lambda) {
    Array<T> temp;
    for(uint64_t i = 0; i < length(); i++) {
      if (lambda(array_[offset_ + i])) {
        temp.push(array_[offset_ + i]);
      }
    }
    return temp;
  }

  /**
   * Filter the array based on a condition.
   */
  Array<T> filter(function<bool (T, uint64_t)> const& lambda) {
    Array<T> temp;
    for(uint64_t i = 0; i < length(); i++) {
      if (lambda(array_[offset_ + i], i)) {
        temp.push(array_[offset_ + i]);
      }
    }
    return temp;
  }

  /**
   * Map the values in the array and return a new array.
   */
  Array<T> map(function<T (T)> const& lambda) {
    Array<T> temp;
    for(uint64_t i = 0; i < length(); i++) {
      temp.push(lambda(array_[offset_ + i]));
    }
    return temp;
  }

  /**
   * Map the values in the array and return a new array.
   */
  Array<T> map(function<T (T, uint64_t)> const& lambda) {
    Array<T> temp;
    for(uint64_t i = 0; i < length(); i++) {
      temp.push(lambda(array_[offset_ + i], i));
    }
    return temp;
  }

  /**
   * Map the values in the array and return a new array.
   */
  template <typename U>
  Array<U> map(function<U (T)> const& lambda) {
    Array<U> temp;
    for(uint64_t i = 0; i < length(); i++) {
      temp.push(lambda(array_[offset_ + i]));
    }
    return temp;
  }

  /**
   * Map the values in the array and return a new array.
   */
  template <typename U>
  Array<U> map(function<U (T, uint64_t)> const& lambda) {
    Array<U> temp;
    for(uint64_t i = 0; i < length(); i++) {
      temp.push(lambda(array_[offset_ + i], i));
    }
    return temp;
  }
  
  /**
   * Reverse the elements in the array.
   */
  Array<T> reverse() {
    Array<T> temp;
    for(auto i = length() - 1; i >= 0; i--) {
      temp.push(this->operator[](i));
    }
    return temp;
  } 
  
  /**
   * Concantenates the elements as a single string with a default seperator of ','.
   */
  string join() {
    return this->join(",");
  }
  
  /**
   * Concantenates the elements as a single string.
   */
  string join(const string& seperator) {
    string result = "";
    this->for_each([&] (T x) {
      result += (((string) x) + seperator);
    });
    return result;
  }
  
  /**
   * Allocate space on the heap.
   */
  void reserve(uint64_t size) {
    reserve(size, false);
  }
  
  /**
   * Allocate space on the heap.
   */
  void reserve(uint64_t size, bool copy) {
    T * array = new T[size];

    if (copy) {
      for(auto i = 0; i < size_; i++) {
        array[i] = array_[offset_ + i];
      }
    }

    delete [] array_;

    size_ = size;
    array_ = array;
  }
  
  /**
   * Return the number of elements in the array.
   */
  int64_t length() {
    return length_ - offset_;
  }
  
  /**
   * Determine whether the array is empty.
   */
  bool is_empty() {
    return length() == 0;
  }
  
  /**
   * Determine whether the array is full.
   */
  bool is_full() {
    return length() == size_;
  }
};


#endif
