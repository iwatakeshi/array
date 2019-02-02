# array-cpp
A JavaScript + Python like array in C++.

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
  return 0;
}
```