# `Array<T>`
A JavaScript + Python like array in C++.


## API

| Constructor  | Performance  | Description  |
|---|---|---|
| `Array()`         | *O(1)* | Initializes a new empty array.  |
| `Array(n)`        | *O(1)* | Initializes a new array of size `n`.  |
| `Array(n, value)` | *O(n)* | Initializes a new array of size `n` with the given value.  |
| `Array(list)`     | *O(n)* | Initializes a new array with list initialization. |

| Method  |  Performance  |  Description  |
|---|---|---|
| `unshift(value): void`                                          | *O(n)* | Adds a new value to the beginning of this array.  |
| `shift(): T`                                                    | *O(1)* | Removes the value from the beginning of this array and returns it.  |
| `push(value): void`                                             | *O(1)* | Adds a new value to the end of this array.  |
| `pop(value): T`                                                 | *O(1)* | Removes the value from the end of this array and returns it.  |
| `at(index): T`                                                  | *O(1)* | Returns the element at the given index with bound checking. |
| `at(index, value): T`                                           | *O(1)* | Sets the value at the given index with bound checking. |
| `for_each((value, index?) -> void): void`                       | *O(n)* | Iterates through each value in this array. |
| `filter((value, index?) -> bool): Array<T>`                     | *O(n)* | Filters this array and returns a new array based on a condition. |
| `map((value, index?) -> T): Array<T>`                           | *O(n)* | Maps each value in this array and returns a new array of type `T`. |
| `map<U>((value, index?) -> T): Array<U>`                        | *O(n)* | Maps each value in this array and returns a new array of type `U`. |
| `reduce((accumulator, current, index?) -> T): T`                | *O(n)* | Reduces the values in this array into a single output value of type `T`. |
| `reduce((accumulator, current, index?) -> T, initial): T`       | *O(n)* | Reduces the values in this array into a single output value of type `T`. |
| `reduce<U>((accumulator, current, index?) -> U, initital): U`   | *O(n)* | Reduces the values in this array into a single output value of type `U`. |
| `reverse(): Array<T>`                                           | *O(n)* | Reverses the values in this array and returns a new array. |
| `slice(index): Array<T>`                                        | *O(n)* | Returns a new array with the values from the specified index. |
| `slice(begin, end): Array<T>`                                   | *O(n)* | Returns a new array with the values from the specified range. |
| `join(): string`                                                | *O(n)* | Returns a string of this array using `,` as the default seperator. |
| `join(seperator): string`                                       | *O(n)* | Returns a string of this array using a provided seperator. |
| `length(): int`                                                 | *O(1)* | Returns the number of elements in this array. |
| `is_empty(): int`                                               | *O(1)* | Returns **true** if this array is empty. |

|  Operator  |  Performance  |  Description  |
|---|---|---|
| `a[index]`     | *O(1)*   | Overloads **[]** to select elements from this array. |
| `a + b`        | *O(n)*   | Concantenates two arrays. |
| `a += b`       | *O(n)*   | Concantenates two arrays and assigns it to `a`. |
| `a = { 1 } `   | *O(n)*   | Assigns the values in a list to `a`. |
| `a * n`        | *O(n^2)* | Repeats the values in this array by `n` times. |
| `a *= n`       | *O(n^2)* | Repeats the values in this array by `n` times and assigns it to `a`. |
| `ostream << a` | *O(n)*   | Outputs the contents of the array to the given output stream. |

## Example

```cpp
#include <iostream>
#include "array.hpp"

using std::cout;
using std::endl;

int main() {
  Array<int> a;
  
  // Insert elements at the end
  a.push(1);
  a.push(2);

  // Print out the elements
  cout << "a: " << a << endl;
  
  // Iterate through the array
  a.for_each([](auto x, auto i) {
    cout << "a: " << i << " - " << x << endl;
  });


  Array<int> b;

  b.push(12);
  b.push(3);
  
  cout << "b: " << b << endl;

  // Concantenate two arrays
  // Note: It does not mutate 'a' or 'b'
  auto c = a + b;

  cout << "c (a + b): " << c << endl;

  // Concantenate and assign
  // Note: It does mutate 'a'
  a += c;

  cout << "a (a + c): " << a << endl;

  // Repeat the elements 'n' times
  cout << "b * 3: " << b * 3 << endl;

  // Insert the element to the front of the list
  a.unshift(100);

  cout << "a: " << a << endl;

  // Initialize with list initialization
  Array<int> d = { 1, 2, 3, 4 };

  // Get a sub list within the range
  auto e = d.slice(1, 3)
  
  cout << "e: " << e << endl;

  // Get a sub list using a negative index

  auto f = d.slice(-1);

  cout << "f: " << f << endl;

  return 0;
}
```
