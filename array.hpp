#ifndef ARRAY_H
#define ARRAY_H
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iterator>
#include <to_string.hpp>

using std::function;
using std::ostream;
using std::string;

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
  // The heap capacity.
  uint64_t capacity_ = 0;
  // The number of elements in the array.
  uint64_t length_ = 0;

public:

  class iterator: public std::iterator<
    std::random_access_iterator_tag,   // iterator_category
    T,                      // value_type
    T,                      // difference_type
    const T*,               // pointer
    T&                       // reference
    > {
    /**
     * Credits
     * https://stackoverflow.com/a/31886483/1251031
     */
    public:
      typedef iterator self_type;
      typedef T value_type;
      typedef T& reference;
      typedef T* pointer;
      typedef int difference_type;
      iterator() : ptr_(nullptr) {}
      iterator(T* right) : ptr_(right) {}
      iterator(const iterator &it) : ptr_(it.ptr_) {}
      inline iterator& operator+=(difference_type right) { ptr_ += right; return *this; }
      inline iterator& operator-=(difference_type right) { ptr_ -= right; return *this; }
      inline reference operator*() const { return *ptr_; }
      inline pointer operator->() const { return ptr_; }
      inline reference operator[](difference_type right) const { return ptr_[right]; }

      inline iterator& operator++() { ++ptr_; return *this; }
      inline iterator& operator--() { --ptr_; return *this; }
      inline iterator operator++(int) { iterator tmp(*this); ++ptr_; return tmp; }
      inline iterator operator--(int) { iterator tmp(*this); --ptr_; return tmp; }
      inline difference_type operator+(const iterator& right) { return ptr_ + right.ptr; }
      inline difference_type operator-(const iterator& right) const { return ptr_ - right.ptr_; }
      inline iterator operator+(difference_type right) const { return iterator(ptr_ + right); }
      inline iterator operator-(difference_type right) const { return iterator(ptr_ - right); }
      friend inline iterator operator+(difference_type left, const iterator& right) { return iterator(left + right.ptr_); }
      friend inline iterator operator-(difference_type left, const iterator& right) { return iterator(left - right.ptr_); }

      inline bool operator==(const iterator& right) const { return ptr_ == right.ptr_;}
      inline bool operator!=(const iterator& right) const { return ptr_ != right.ptr_;}
      inline bool operator>(const iterator& right) const { return ptr_ > right.ptr_;}
      inline bool operator<(const iterator& right) const { return ptr_ < right.ptr_;}
      inline bool operator>=(const iterator& right) const { return ptr_ >= right.ptr_;}
      inline bool operator<=(const iterator& right) const { return ptr_ <= right.ptr_;}
    private:
      pointer ptr_;
  };

 /**
  * 	Initializes a new empty array.
  */
  array() {}

  /**
   * 	Initializes a new array storing n copies of zeroes.
   */
  array(uint64_t capacity) {
    reserve(capacity);
    length_ = capacity;
  }

  /**
   * 	Initializes a new array storing n copies of the given value.
   */
  array(uint64_t capacity, T const& value): array(capacity) {
    for(auto i = 0; i < capacity; i++) {
      array_[i] = value;
    }
  }

  array(array const& other) {
    reserve(other.capacity_, false);
    for(auto i = 0; i < other.capacity_; i++) {
      array_[i] = other.array_[i];
    }

    offset_ = other.offset_;
    capacity_ = other.capacity_;
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
    capacity_ = 0;
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
    if (capacity_ < right.capacity_) {
      reserve(right.capacity_, false);
    }

    capacity_ = right.capacity_;
    offset_ = right.offset_;
    length_ = right.length_ + right.offset_;

    for (auto i = 0; i < capacity_; i++) {
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
    capacity_ = list.size();
    length_ = capacity_;
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
    capacity_ = ((length_ - offset_)) * right;
    length_ = capacity_;
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
    auto capacity = 0;
    if (capacity_ == 0) {
      capacity = std::max((uint64_t) 1, (uint64_t) (capacity_ * 2));
    } else {
      capacity = is_full() ? 2 * capacity_ : (length_ - offset_) + 1;
    }

    T * array = new T[capacity];
    
    array[0] = value;

    for(auto i = 1; i <= (length_ - offset_); i++) {
      array[i] = this->operator[]((i - 1));
    }

    delete [] array_;
    array_ = array;

    length_ += 1;
    capacity_ = capacity;
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
    if (capacity_ == 0) {
      reserve(std::max((uint64_t) 1, (uint64_t) capacity_ * 2), false);
    } else if (is_full()) {
      reserve(2 * capacity_, true);
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

    auto capacity = end - begin;

    array<T> temp(capacity);
    for (auto i = 0; i < capacity; i++) {
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
      result += (utils::to_string(x) + ((i == (length_ - offset_) - 1) ? "" : seperator));
    });
    return result;
  }
  
  /**
   * Allocates space on the heap.
   */
  void reserve(uint64_t capacity) {
    reserve(capacity, false);
  }
  
  /**
   * Allocates space on the heap.
   */
  void reserve(uint64_t capacity, bool copy) {
    T * array = new T[capacity];

    if (copy) {
      for(auto i = offset_; i < capacity_; i++) {
        array[i] = this->operator[](i);
      }
    }

    delete [] array_;

    length_ = length_ - offset_;
    offset_ = 0;
    capacity_ = capacity;
    array_ = array;
  }
  
  /**
   * Return the number of elements in the array.
   */
  int64_t length() const {
    return length_ - offset_;
  }

  /**
   * Returns the heap capacity of the array.
   */
  int64_t capacity() const {
    return capacity_;
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
    return (length_ - offset_) == capacity_;
  }

  /**
   * Returns an iterator pointing to the first element in the array.
   */
  iterator begin() {
    return iterator(array_ + offset_);
  }

  /**
   * Returns an iterator referring to the past-the-end element in the array container.
   */
  iterator end() {
    return iterator(array_ + (length_ - offset_));
  }
};

#endif