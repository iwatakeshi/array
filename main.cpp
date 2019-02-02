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
  return 0;
}