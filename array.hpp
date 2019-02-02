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
 /**
  * 	Initializes a new empty array.
  */
  Array() {}

  /**
   * 	Initializes a new array storing n copies of zeroes.
   */
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
   * Overloads [] to select elements from this array.
   */
  T& operator [] (int64_t const& index) const {
    if (index < 0 || index >= length()) {
      throw std::out_of_range("Index is out of bounds");
    }

    return array_[offset_ + index];
  }

  /**
   * Repeats the values in this array by n times.
   */
  Array<T> operator * (uint64_t const & right) const {
    Array<T> temp;
    for (auto i = 0; i < right; i++) {
      for(auto j = 0; j < this->length(); j++) {
        temp.push(this->operator[](j));
      }
    }

    return temp;
  }

  /**
   * Assigns this array with the array on the right hand side.
   */
  Array<T>& operator = (Array<T> const& right) const {
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
   * Concantenates two arrays.
   */
  Array<T> operator + (Array<T> const& right) const {
    Array<T> temp;

    for(auto i = 0; i < this->length(); i++) {
      temp.push(this->operator[](i));
    }
    

    for (auto i = 0; i < right.length(); i++) {
      temp.push(right.operator[](i));
    }

    return temp;
  }

  /**
   * Concantenates two arrays and assigns it to this array.
   */
  Array<T>& operator += (Array<T> const& right) {
    for (auto i = 0; i < right.length(); i++) {
      this->push(right.operator[](i));
    }

    return *this;
  }

  /**
   * Repeats the values in this array by n times and assigns it to this array.
   */
  Array<T>& operator *= (uint64_t const& right) const {
    T * temp = new T[length() * right];
    auto index = 0;
    for (auto i = 0; i < right; i++) {
     for (auto j = 0; j < length(); j++) {
       temp[index] = this->operator[](j);
       index += 1;
     }
    }

    delete [] array_;
    array_ = temp;
    size_ = (length()) * right;
    length_ = size_;
    offset_ = 0;

    return *this;
  }

  /**
   * Outputs the contents of the array to the given output stream.
   */
  friend ostream& operator << (ostream& os, const Array<T>& array) {
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
      size = is_full() ? 2 * size_ : length() + 1;
    }

    T * array = new T[size];
    
    array[0] = value;

    for(auto i = 1; i <= length(); i++) {
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

    array_[length()] = value;
    length_ += 1;
  }
  
  /**
   * Removes the value from the end of this array and returns it.
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
   * Iterates through each value in this array. 
   */
  void for_each(function<void (T)>const& lambda) const {
    for(auto i = 0; i < length(); i++) {
      lambda(this->operator[](i));
    }
  }

  /**
   * Iterates through each value in this array. 
   */
  void for_each(function<void (T, int64_t)>const& lambda) const {
    for(auto i = 0; i < length(); i++) {
      lambda(this->operator[](i), i);
    }
  }

  /**
   * Filters this array and returns a new array based on a condition.
   */
  Array<T> filter(function<bool (T)> const& lambda) const {
    Array<T> temp;
    for(auto i = 0; i < length(); i++) {
      if (lambda(this->operator[](i))) {
        temp.push(this->operator[](i));
      }
    }
    return temp;
  }

  /**
   * Filters this array and returns a new array based on a condition.
   */
  Array<T> filter(function<bool (T, int64_t)> const& lambda) const {
    Array<T> temp;
    for(auto i = 0; i < length(); i++) {
      if (lambda(this->operator[](i), i)) {
        temp.push(this->operator[](i));
      }
    }
    return temp;
  }

  /**
   *Maps each value in this array and returns a new array of type T.
   */
  Array<T> map(function<T (T)> const& lambda) const {
    Array<T> temp;
    for(auto i = 0; i < length(); i++) {
      temp.push(lambda(this->operator[](i)));
    }
    return temp;
  }

  /**
   * Maps each value in this array and returns a new array of type T.
   */
  Array<T> map(function<T (T, int64_t)> const& lambda) const {
    Array<T> temp;
    for(auto i = 0; i < length(); i++) {
      temp.push(lambda(this->operator[](i), i));
    }
    return temp;
  }

  /**
   * Maps each value in this array and returns a new array of type U.
   */
  template <typename U>
  Array<U> map(function<U (T)> const& lambda) const {
    Array<U> temp;
    for(auto i = 0; i < length(); i++) {
      temp.push(lambda(this->operator[](i)));
    }
    return temp;
  }

  /**
   *Maps each value in this array and returns a new array of type U.
   */
  template <typename U>
  Array<U> map(function<U (T, int64_t)> const& lambda) const {
    Array<U> temp;
    for(auto i = 0; i < length(); i++) {
      temp.push(lambda(this->operator[](i), i));
    }
    return temp;
  }

   /**
    * Reduces the values in this array into a single output value of type T.
    */
    T reduce(function<T (T, T)> const & lambda) const {
      T value = this->operator[](0);
      for(auto i = 1; i < length(); i++) {
        value = lambda(value, this->operator[](i));
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type T.
     */
    T reduce(function<T (T, T, int64_t)> const & lambda) const {
      T value = this->operator[](0);
      for(auto i = 1; i < length(); i++) {
        value = lambda(value, this->operator[](i), i);
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type T.
     */
    T reduce(function<T (T, T)> const & lambda, T initial) const {
      T value = initial;
      for(auto i = 0; i < length(); i++) {
        value = lambda(value, this->operator[](i));
      }
      return value;
    }

    /**
     * Reduces the values in this array into a single output value of type T.
     */
    T reduce(function<T (T, T, int64_t)> const & lambda, T initial) const {
      T value = initial;
      for(auto i = 0; i < length(); i++) {
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
      for(auto i = 0; i < length(); i++) {
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
      for(auto i = 0; i < length(); i++) {
        value = lambda(value, this->operator[](i), i);
      }
      return value;
    }
  
  /**
   * Reverses the values in this array and returns a new array.
   */
  Array<T> reverse() const {
    Array<T> temp;
    for(auto i = length() - 1; i >= 0; i--) {
      temp.push(this->operator[](i));
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
      result += (array_utils::to_string(x) + ((i == length() - 1) ? "" : seperator));
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
    return length() == 0;
  }
  
  /**
   * Determine whether the array is full.
   */
  bool is_full() const {
    return length() == size_;
  }
};


#endif
