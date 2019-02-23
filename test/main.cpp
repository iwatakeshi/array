#include <array.hpp>
#include <mocha.hpp>
#include <ostream>
#include <string>

using namespace mocha;

namespace mocha::helpers {
  // Extend the helpers for comparisons
  template <typename T>
  bool strict_equal(array<T> a, array<T> b) {
    return a == b;
  }

  template <typename T>
  bool equal(array<T> a, array<T> b) {
    return a == b;
  }
}

int main() {
  describe("constructor", [&] {
    describe("array()", [&] {
      array<int> a;
      it("should have a capacity of 0", [&] {
        return expect(a.capacity()).to->equal(0)->result();
      });

      it("should have a length of 0", [&] {
        return expect(a.length()).to->equal(0)->result();
      });
    });

    describe("array(n)", [&] {
      array<int> a(10);
      it("should have a capacity of 10", [&] {
        return expect(a.capacity()).to->equal(10)->result();
      });

      it("should have a length of 10", [&] {
        return expect(a.length()).to->equal(10)->result();
      });
    });

    describe("array(n, value)", [&] {
      array<int> a(2, 100);
      it("should have a capacity of 2", [&] {
        return expect(a.capacity()).to->equal(2)->result();
      });

      it("should have a length of 2", [&] {
        return expect(a.length()).to->equal(2)->result();
      });

      it("should have a value of 100 at index 0", [&] {
        return expect(a[0]).to->equal(100)->result();
      });

      it("should have a value of 100 at index 1", [&] {
        return expect(a[1]).to->equal(100)->result();
      });
    });

    describe("array(list)", [&] {
      array<int> a = { 1 };
      it("should have a capacity of 1", [&] {
        return expect(a.capacity()).to->equal(1)->result();
      });

      it("should have a length of 1", [&] {
        return expect(a.length()).to->equal(1)->result();
      });

      it("should have a value of 1 at index 0", [&] {
        return expect(a[0]).to->equal(1)->result();
      });
    });
  });

  describe("method", [&] {
    describe("unshift(value)", [&] {
      array<int> a;
      it("should add a new value to the beginning of the array.", [&] {
        a.unshift(1);
        return expect(a[0]).to->equal(1)->result();
      });
    });

    describe("shift()", [&] {
      array<int> a = { 1 };
      it("should remove the value from the beginning of the array and return it.", [&] {
        a.shift();
        return expect(a.length()).to->equal(0)->result();
      });
    });

    describe("push(value)", [&] {
      array<int> a;
      it("should remove the value from the beginning of the array and return it.", [&] {
        a.push(10);
        return expect(a.length()).to->equal(1)->result() + 
               expect(a[0]).to->equal(10)->result();
      });
    });

    describe("pop", [&] {
      array<int> a = { 1, 2, 3 };
      it("should remove the value from the beginning of the array and return it.", [&] {
        result_t result;
        int count = 3;
        while (count > 0) {
          result += expect(a.pop()).to->equal(count--)->result();
        }

        result += expect(a.length()).to->equal(count)->result();

        return result;
      });
    });

    describe("at(value)", [&] {
      array<int> a = { 1, 2, 3 };
      it("should return the element at the given index with bound checking.", [&] {
        return expect(a.at(2)).to->equal(3)->result() +
               expect(a.at(1)).to->equal(2)->result() +
               expect(a.at(0)).to->equal(1)->result();
      });
    });

    describe("at(value)", [&] {
      array<int> a = { 1, 2, 3 };
      it("should return the element at the given index with bound checking.", [&] {
        auto test = [&](int index, int expected) {
          a.at(index, expected);
          return expect(a.at(index)).to->equal(expected)->result();
        };
        return test(0, 2) + test(1, 4) + test(2, 6);
      });
    });

    describe("for_each((value, index?) -> void)", [&] {
      it("should iterate through each value in the array.", [&] {
        array<int> a = { 1, 2, 3 };
        std::vector<int> b = { 1, 2, 3 };
        result_t result;
        a.for_each([&] (auto value, int index) {
          result += expect(value).to->equal(b[index])->result();
        });
        
        return result;
      });
    });

    describe("filter((value, index?) -> bool): array<T>", [&] {
      it("should filter the array and return a new array based on a condition.", [&] {
        array<int> a = { 1, 2, 3 };
        array<int> expected = { 1, 3 };
        return expect(a.filter([&] (int value) { return value != 2; }))
        .to->equal(expected)->result();
      });
    });

    describe("map((value, index?) -> T): array<T>", [&] {
      it ("should map each value in the array and return a new array of type T", [&] {
        array<int> a = { 1, 2, 3 };
        array<int> expected = { 2, 4, 6 };
        return expect(a.map([&](int value) { return value * 2; }))
        .to->equal(expected)->result();
      });
    });

    describe("map<U>((value, index?) -> T): array<U>", [&] {
      it ("should map each value in the array and return a new array of type T", [&] {
        array<int> a = { 1, 2, 3 };
        array<double> expected = { 2.0, 4.0, 6.0 };
        return expect(a.map([&](int value) { return value * 2; }))
        .to->equal(expected)->result();
      });
    });

  });

  std::cout << mocha::output() << std::endl;

  return 0;
}