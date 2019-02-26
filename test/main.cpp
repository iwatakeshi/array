#include <array.hpp>
#include <mocha.hpp>
#include <ostream>
#include <string>
#include <sstream>

using namespace mocha;

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

    describe("push(value): T", [&] {
      array<int> a;
      it("should remove the value from the beginning of the array and return it.", [&] {
        a.push(10);
        return expect(a.length()).to->equal(1)->result() + 
               expect(a[0]).to->equal(10)->result();
      });
    });

    describe("pop()", [&] {
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

    describe("at(value): T", [&] {
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
      it ("should map each value in the array and return a new array of type T.", [&] {
        array<int> a = { 1, 2, 3 };
        array<int> expected = { 2, 4, 6 };
        return expect(a.map([&](int value) { return value * 2; }))
        .to->equal(expected)->result();
      });
    });

    describe("map<U>((value, index?) -> T): array<U>", [&] {
      it ("should map each value in the array and return a new array of type T.", [&] {
        array<int> a = { 1, 2, 3 };
        array<double> expected = { 2.0, 4.0, 6.0 };
        return expect(a.map([&](int value) { return value * 2; }))
        .to->equal<array<double>>(expected, [&] (auto a, auto b) {
          return a == b;
        })->result();
      });
    });

    describe("reduce((accumulator, current, index?) -> T): T", [&] {
      it ("should reduce the values in the array into a single output value of type T.", [&] {
        array<int> a = { 1, 2, 3 };
        return expect(a.reduce([&](auto acc, auto cur) {
          return acc + cur;
        })).to->equal(6)->result();
      });
    });

    describe("reduce<U>((accumulator, current, index?) -> U, initital): U", [&] {
      it("should reduce the values in the array into a single output value of type T.", [&] {
        array<int> a = { 1, 2, 3 };
        return expect(a.reduce<double>([&](auto acc, auto cur) {
          return acc + cur;
        }, 1.5)).to->equal(7.5)->result();
      });
    });

    describe("reverse(): array<T>", [&] {
      it("should reverses the values in the array and returns a new array.", [&] {
        array<int> a = { 1, 2, 3 };
        array<int> expected = { 3, 2, 1 };
        return expect(a.reverse()).to->equal(expected)->result();
      });
    });

    describe("slice(index): array<T>", [&] {
      it("should return a new array with the values from the specified index.", [&] {
        array<int> a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        array<int> b = { 5, 6, 7, 8, 9 };
        array<int> c = { 9 };
        array<int> d = { 7, 8, 9 };
        return expect(a.slice(5)).to->equal(b)->result() + 
               expect(a.slice(-1)).to->equal(c)->result() +
               expect(a.slice(-3)).to->equal(d)->result();
      });
    });

    describe("slice(begin, end): array<T>", [&] {
      it("should return a new array with the values from the specified range.", [&] {
        array<int> a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        array<int> b = { 5, 6, 7 };
        return expect(a.slice(5, 8)).to->equal(b)->result();
      });
    });

    describe("join(): string", [&] {
      it("should return a string of the array using ',' as the default seperator.", [&] {
        array<int> a = { 1, 2, 3 };
        return expect(a.join()).to->equal("1,2,3")->result();
      });
    });

    describe("join(seperator): string", [&] {
      it("should return a string of the array using a provided seperator.", [&] {
        array<int> a = { 1, 2, 3 };
        return expect(a.join(" + ")).to->equal("1 + 2 + 3")->result();
      });
    });

    describe("length(): int", [&] {
      it("should return the number of elements in the array.", [&] {
        array<int> a = { 1, 2, 3 };
        return expect(a.length()).to->equal(3)->result();
      });
    });

    describe("capacity(): int", [&] {
      it("should return the capacity of the array.", [&] {
        array<int> a(10);
        return expect(a.capacity()).to->equal(10)->result();
      });
    });

    describe("is_empty(): int", [&] {
      it("should true if the array is empty.", [&] {
        array<int> a;
        array<int> b(10);
        return expect(a.is_empty()).to->equal(true)->result() + 
               expect(b.is_empty()).to->equal(false)->result();
      });
    });

    describe("begin(): iterator", [&] {
      it("should return an iterator pointing to the first element in the array.", [&] {
        array<int> a = { 1, 2, 3 };
        auto b = a.begin();
        return expect(*b).to->equal(1)->result();
      });
    });

    describe("end(): iterator", [&] {
      it("should return an iterator referring to the past-the-end element in the array container.", [&] {
        array<int> a = { 1, 2, 3 };
        auto b = a.end();
        return expect(*(b - 1)).to->equal(3)->result();
      });
    });
  });

  describe("operator", [&] {
    describe("a[index]", [&] {
      it("should overload [] to select elements from the array.", [&] {
        array<int> a = { 1, 2, 3 };
        return expect(a[0]).to->equal(1)->result() +
               expect(a[1]).to->equal(2)->result() +
               expect(a[2]).to->equal(3)->result();
      });
    });

    describe("a + b", [&] {
      it("should concantenate two arrays.", [&] {
        array<int> a { 1, 2, 3 };
        array<int> a_copy { 1, 2, 3 };
        array<int> b { 4, 5, 6 };
        array<int> c { 1, 2, 3, 4, 5, 6};
        return expect(a + b).to->equal(c)->result() +
               expect(a).to->equal(a_copy)->result();
      });
    });

    describe("a += b", [&] {
      it("should concantenates two arrays and assigns it to a", [&] {
        array<int> a { 1, 2, 3 };
        array<int> a_copy { 1, 2, 3 };
        array<int> b { 4, 5, 6 };
        array<int> c { 1, 2, 3, 4, 5, 6};
        a += b;
        return expect(a).to->equal(c)->result() +
               expect(a).to->never->equal(a_copy)->result();
      });
    });

    describe("a = { 1 }", [&] {
      it("should assigns the values in a list to a.", [&] {
        array<int> a = { 1 };
        return expect(a[0]).to->equal(1)->result();
      });
    });

    describe("a * n", [&] {
      it("should repeat the values in the array by n times.", [&] {
        array<int> a = { 1 };
        array<int> b = a * 3;
        array<int> c = { 1, 1, 1 };
        return expect(a).to->never->equal(c)->result() +
               expect(b).to->equal(c)->result();
      });
    });
    
    describe("a *= n", [&] {
      it("should repeat the values in the array by n times and assigns it to a.", [&] {
        array<int> a = { 1 };
        array<int> a_copy = a;
        array<int> c = { 1, 1, 1 };
        a *= 3;
        return expect(a).to->equal(c)->result() +
               expect(a).to->never->equal(a_copy)->result();
      });
    });

    describe("ostream << a", [&] {
      it("should output the contents of the array to the given output stream.", [&] {
        array<int> a = { 1, 2, 3 };
        std::stringstream s1;
        std::stringstream s2;
        s1 << a;
        s2 << "[ 1, 2, 3 ]";
        return expect(s1.str()).to->equal(s2.str())->result();
      });
    });

  });

  mocha::print_summary();

  return 0;
}