#include "array.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace stdlib;

TEST_CASE("Array construction", "[array]") {
  SECTION("Default constructor") {
    array<int> arr;
    REQUIRE(arr.size() == 0);
    REQUIRE(arr.capacity() == 0);
  }

  SECTION("Constructor with capacity") {
    array<int> arr(10);
    REQUIRE(arr.size() == 10);
    REQUIRE(arr.capacity() == 10);
  }

  SECTION("Constructor with capacity and value") {
    array<int> arr(10, 5);
    REQUIRE(arr.size() == 10);
    REQUIRE(arr.capacity() == 10);
    for (int i = 0; i < 10; ++i) {
      REQUIRE(arr[i] == 5);
    }
  }

  SECTION("Copy constructor") {
    array<int> arr1(10, 5);
    array<int> arr2(arr1);
    REQUIRE(arr2.size() == 10);
    REQUIRE(arr2.capacity() == 10);
    for (int i = 0; i < 10; ++i) {
      REQUIRE(arr2[i] == 5);
    }
  }

  SECTION("Move constructor") {
    array<int> arr1(10, 5);
    array<int> arr2(std::move(arr1));
    REQUIRE(arr2.size() == 10);
    REQUIRE(arr2.capacity() == 10);
    for (int i = 0; i < 10; ++i) {
      REQUIRE(arr2[i] == 5);
    }
  }

  SECTION("Initializer list constructor") {
    array<int> arr = { 1, 2, 3, 4, 5 };
    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);
    for (int i = 0; i < 5; ++i) {
      REQUIRE(arr[i] == i + 1);
    }
  }

  SECTION("Range constructor") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    array<int> arr(vec);
    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);
    for (int i = 0; i < 5; ++i) {
      REQUIRE(arr[i] == i + 1);
    }
  }

  SECTION("Iterator constructor") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    array<int> arr(vec.begin(), vec.end());
    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);
    for (int i = 0; i < 5; ++i) {
      REQUIRE(arr[i] == i + 1);
    }
  }
}

TEST_CASE("Array element access", "[array]") {
  array<int> arr = { 1, 2, 3, 4, 5 };

  SECTION("operator[]") {
    REQUIRE(arr[0] == 1);
    REQUIRE(arr[4] == 5);
  }

  SECTION("at()") {
    REQUIRE(arr.at(0) == 1);
    REQUIRE(arr.at(4) == 5);
    REQUIRE_THROWS_AS(arr.at(5), std::out_of_range);
  }
}

TEST_CASE("Array modification", "[array]") {
  array<int> arr;

  SECTION("push() and pop()") {
    arr.push(1);
    arr.push(2);
    arr.push(3);
    REQUIRE(arr.size() == 3);
    REQUIRE(arr.pop() == 3);
    REQUIRE(arr.pop() == 2);
    REQUIRE(arr.pop() == 1);
    REQUIRE(arr.size() == 0);
    REQUIRE_THROWS_AS(arr.pop(), std::out_of_range);
  }

  SECTION("unshift() and shift()") {
    arr.unshift(1);
    arr.unshift(2);
    arr.unshift(3);
    REQUIRE(arr.size() == 3);

    auto val1 = arr.shift();
    REQUIRE(val1 == 3);
    REQUIRE(arr.size() == 2);
    REQUIRE(arr[0] == 2);
    REQUIRE(arr[1] == 1);

    auto val2 = arr.shift();
    REQUIRE(val2 == 2);
    REQUIRE(arr.size() == 1);
    REQUIRE(arr[0] == 1);

    auto val3 = arr.shift();
    REQUIRE(val3 == 1);
    REQUIRE(arr.size() == 0);

    REQUIRE_THROWS_AS(arr.shift(), std::out_of_range);
  }

  SECTION("insert()") {
    arr.push(1);
    arr.push(2);
    arr.push(4);
    arr.insert(arr.begin() + 2, 3);
    REQUIRE(arr.size() == 4);
    REQUIRE(arr[2] == 3);
  }
}

TEST_CASE("Array utility functions", "[array]") {
  array<int> arr = { 1, 2, 3, 4, 5 };

  SECTION("size() and capacity()") {
    REQUIRE(arr.size() == 5);
    REQUIRE(arr.capacity() == 5);
  }

  SECTION("empty() and full()") {
    REQUIRE(!arr.empty());
    REQUIRE(arr.full());
  }

  SECTION("reserve()") {
    arr.reserve(10);
    REQUIRE(arr.capacity() == 10);
  }

  SECTION("swap()") {
    array<int> arr2 = { 6, 7, 8, 9, 10 };
    arr.swap(arr2);
    REQUIRE(arr[0] == 6);
    REQUIRE(arr2[0] == 1);
  }

  SECTION("operator== and operator<=>") {
    array<int> arr2 = { 1, 2, 3, 4, 5 };
    REQUIRE(arr == arr2);
    array<int> arr3 = { 1, 2, 3, 4, 6 };
    REQUIRE((arr <=> arr3) == std::strong_ordering::less);
  }

  SECTION("operator* and operator+=") {
    array<int> arr2 = arr * 2;
    REQUIRE(arr2.size() == 10);
    for (int i = 0; i < 5; ++i) {
      REQUIRE(arr2[i] == arr[i]);
      REQUIRE(arr2[i + 5] == arr[i]);
    }
    arr += arr2;
    REQUIRE(arr.size() == 15);
  }

  SECTION("reverse()") {
    array<int> arr2 = arr.reverse();
    REQUIRE(arr2[0] == 5);
    REQUIRE(arr2[4] == 1);
  }

  SECTION("slice()") {
    array<int> arr2 = arr.slice(1, 4);
    REQUIRE(arr2.size() == 3);
    REQUIRE(arr2[0] == 2);
    REQUIRE(arr2[2] == 4);
  }

  SECTION("join()") {
    REQUIRE(arr.join(", ") == "1, 2, 3, 4, 5");
  }
}