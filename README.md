# `array<T>`
A JavaScript + Python like array in C++.

[![CMake](https://github.com/iwatakeshi/array/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/iwatakeshi/array/actions/workflows/cmake-single-platform.yml)

## Build Instructions

This project uses CMake as its build system. A Makefile is provided for convenience to simplify the build process.

### Prerequisites

- C++ compiler with C++23 support
- CMake (version 3.10 or higher)
- Make (optional, for using the provided Makefile)

### Dependencies

- `fmt` library for formatting strings
- `Catch2` framework for unit testing

### Building the Project

You can build the project using either CMake directly or the provided Makefile.

#### Using the Makefile

1. To build the project:
   ```
   make build
   ```

2. To run tests:
   ```
   make test
   ```

3. To clean the build directory:
   ```
   make clean
   ```

4. For help on available commands:
   ```
   make help
   ```

#### Using CMake Directly

1. Create a build directory:
   ```
   mkdir build && cd build
   ```

2. Configure the project:
   ```
   cmake ..
   ```

3. Build the project:
   ```
   cmake --build .
   ```

4. Run tests:
   ```
   ctest
   ```

## API

| Constructor  | Performance  | Description  |
|---|---|---|
| `array()`         | *O(1)* | Initializes a new empty array.  |
| `array(n)`        | *O(1)* | Initializes a new array of size `n`.  |
| `array(n, value)` | *O(n)* | Initializes a new array of size `n` with the given value.  |
| `array(list)`     | *O(n)* | Initializes a new array with list initialization. |

| Method  |  Performance  |  Description  |
|---|---|---|
| `unshift(value): void`                                          | *O(n)* | Adds a new value to the beginning of this array. |
| `shift(): T`                                                    | *O(1)* | Removes the value from the beginning of this array and returns it. |
| `push(value): void`                                             | *Amortized O(1)* | Adds a new value to the end of this array. |
| `pop(value): T`                                                 | *O(1)* | Removes the value from the end of this array and returns it. |
| `at(index): T`                                                  | *O(1)* | Returns the element at the given index with bound checking. |
| `at(index, value)`                                              | *O(1)* | Sets the value at the given index with bound checking. |
| `for_each((value, index?) -> void): void`                       | *O(n)* | Iterates through each value in this array. |
| `filter((value, index?) -> bool): array<T>`                     | *O(n)* | Filters this array and returns a new array based on a condition. |
| `map((value, index?) -> T): array<T>`                           | *O(n)* | Maps each value in this array and returns a new array of type `T`. |
| `map<U>((value, index?) -> T): array<U>`                        | *O(n)* | Maps each value in this array and returns a new array of type `U`. |
| `reduce((accumulator, current, index?) -> T): T`                | *O(n)* | Reduces the values in this array into a single output value of type `T`. |
| `reduce((accumulator, current, index?) -> T, initial): T`       | *O(n)* | Reduces the values in this array into a single output value of type `T`. |
| `reduce<U>((accumulator, current, index?) -> U, initital): U`   | *O(n)* | Reduces the values in this array into a single output value of type `U`. |
| `reverse(): array<T>`                                           | *O(n)* | Reverses the values in this array and returns a new array. |
| `slice(index): array<T>`                                        | *O(n)* | Returns a new array with the values from the specified index. |
| `slice(begin, end): array<T>`                                   | *O(n)* | Returns a new array with the values from the specified range. |
| `join(): string`                                                | *O(n)* | Returns a string of this array using `,` as the default separator. |
| `join(separator): string`                                       | *O(n)* | Returns a string of this array using a provided separator. |
| `size(): size_type`                                             | *O(1)* | Returns the number of elements in this array. |
| `capacity(): size_type`                                         | *O(1)* | Returns the capacity of the array. |
| `empty(): bool`                                                 | *O(1)* | Returns **true** if this array is empty. |
| `begin(): iterator`                                             | *O(1)* | Returns an iterator pointing to the first element in the array. |
| `end(): iterator`                                               | *O(1)* | Returns an iterator referring to the past-the-end element in the array container. |

|  Operator  |  Performance  |  Description  |
|---|---|---|
| `a[index]`     | *O(1)*   | Overloads **[]** to select elements from this array. |
| `a + b`        | *O(n + m)* | Concatenates two arrays of sizes n and m. |
| `a += b`       | *O(n + m)* | Concatenates two arrays of sizes n and m and assigns it to `a`. |
| `a = { ... }`  | *O(n)*   | Assigns the values in a list to `a`. |
| `a * n`        | *O(n * m)* | Repeats the values in this array of size n, m times. |
| `a *= n`       | *O(n * m)* | Repeats the values in this array of size n, m times and assigns it to `a`. |
| `ostream << a` | *O(n)*   | Outputs the contents of the array to the given output stream. |

## Example

```cpp
#include <iostream>
#include <array.hpp>
using namespace stdlib;
using std::cout;
using std::endl;

int main() {
  array<int> a;
  
  // Insert elements at the end
  a.push(1);
  a.push(2);

  // Print out the elements
  cout << "a: " << a << endl;
  
  // Iterate through the array
  a.for_each([](auto x, auto i) {
    cout << "a: " << i << " - " << x << endl;
  });

  array<int> b;

  b.push(12);
  b.push(3);
  
  cout << "b: " << b << endl;

  // Concatenate two arrays
  // Note: It does not mutate 'a' or 'b'
  auto c = a + b;

  cout << "c (a + b): " << c << endl;

  // Concatenate and assign
  // Note: It does mutate 'a'
  a += c;

  cout << "a (a + c): " << a << endl;

  // Repeat the elements 'n' times
  cout << "b * 3: " << b * 3 << endl;

  // Insert the element to the front of the list
  a.unshift(100);

  cout << "a: " << a << endl;

  // Initialize with list initialization
  array<int> d = { 1, 2, 3, 4 };

  // Get a sub list within the range
  auto e = d.slice(1, 3);
  
  cout << "e: " << e << endl;

  // Get a sub list using a negative index
  auto f = d.slice(-1);

  cout << "f: " << f << endl;

  // Use a range-based for loop to iterate through the array
  for (auto& n : f) {
    cout << n << endl;
  }

  return 0;
}
```
