#include <array.hpp>
#include <mocha.hpp>
#include <ostream>
#include <string>

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
        return expect(a.pop()).to->equal(3)->result() +
               expect(a.pop()).to->equal(2)->result() +
               expect(a.pop()).to->equal(1)->result() +
               expect(a.length()).to->equal(0)->result();
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
      array<int> a = { 1, 2, 3 };
      std::vector<int> b = { 1, 2, 3 };
      std::vector<test_result> results(b.size());
      test_result result;
      a.for_each([&] (auto value, int index) {
        results.push_back(expect(value).to->equal(b[index])->result());
      });

    });

  });

  std::cout << mocha::output() << std::endl;

  return 0;
}