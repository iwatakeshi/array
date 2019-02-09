#include <iostream>
#include "array.hpp"

using std::cout;
using std::endl;

int main() {
  Array<int> a;
  a.push(1);
  a.push(2);

  cout << "a: " << a << endl;
  
  a.for_each([](auto x, auto i) {
    cout << "a: " << i << " - " << x << endl;
  });

  Array<int> b;

  b.push(12);
  b.push(3);
  
  cout << "b: " << b << endl;

  auto c = a + b;

  cout << "c (a + b): " << c << endl;

  a += c;

  cout << "a (a + c): " << a << endl;


  cout << "b * 3: " << b * 3 << endl;

  a.unshift(100);

  cout << "a: " << a << endl;
  
  a.unshift(200);

  cout << "a: " << a << endl;

  a[0] = 1;

  cout << "a: " << a << endl;

  cout << a.reduce([](auto accumulator, auto current) -> int {
    return accumulator + current;
  }) << endl;

  cout << a.map([](auto value) {
    return value * 2;
  }) << endl;

  Array<int> d;

  d = { 1, 2, 3, 4 };

  cout << d << endl;


  cout << d.slice(1, 3) << endl;
  
  cout << d.slice(-1) << endl;

  return 0;
}