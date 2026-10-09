/*
Merge Sorted Array

You're given two lists of integers, nums1 and nums2, each already sorted in
ascending order.

Return a single list containing all the numbers from both, still sorted in
ascending order.

  Input:  nums1 = [1, 2, 3], nums2 = [2, 5, 6]
  Output: [1, 2, 2, 3, 5, 6]

  Input:  nums1 = [], nums2 = [1]
  Output: [1]

  Input:  nums1 = [1], nums2 = []
  Output: [1]

Constraints
  0 <= nums1.length, nums2.length <= 200
  -10^9 <= nums1[i], nums2[i] <= 10^9
  nums1 and nums2 are each sorted in ascending order

Original source: https://leetcode.com/problems/merge-sorted-array/
See README.md for the walkthrough.
*/

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Solution1: your final version (README section 5)
// ---------------------------------------------------------------------------
class Solution1 {
public:
  std::vector<int> merge(const std::vector<int> &nums1,
                         const std::vector<int> &nums2) {
    std::vector<int> arr(nums1.size() + nums2.size());
    size_t i = 0;
    size_t j = 0;
    while (i < nums1.size() && j < nums2.size()) {
      int num1 = nums1[i];
      int num2 = nums2[j];

      if (num1 < num2) {
        arr[i + j] = num1;
        ++i;
      } else {
        arr[i + j] = num2;
        ++j;
      }
    }

    // Check if one of them is at the end and the other still has elements
    for (; i < nums1.size(); i++) {
      arr[i + j] = nums1[i];
    }
    for (; j < nums2.size(); j++) {
      arr[i + j] = nums2[j];
    }

    return arr;
  }
};

// ---------------------------------------------------------------------------
// Solution2: your turn (README section 6)
// Same algorithm, but with a dedicated write cursor `k`, and ties taken
// from nums1 (a "stable" merge).
// ---------------------------------------------------------------------------
class Solution2 {
public:
  std::vector<int> merge(const std::vector<int> &nums1,
                         const std::vector<int> &nums2) {
    std::vector<int> arr(nums1.size() + nums2.size());
    size_t i = 0; // next unread element of nums1
    size_t j = 0; // next unread element of nums2
    size_t k = 0; // next free slot in arr

    // TODO 1: while BOTH arrays still have unread elements,
    //         copy the smaller front element into arr[k], then advance k
    //         and the pointer you read from.
    //         On a tie, take from nums1 (README section 6b).

    // TODO 2: copy whatever is left in nums1.

    // TODO 3: copy whatever is left in nums2.

    return arr;
  }
};

// ---------------------------------------------------------------------------
// Solution3: your turn, harder (README section 9)
// LeetCode's real version: merge IN PLACE into nums1.
//
//   nums1 has size m + n: m real elements, then n empty slots.
//   nums2 has n elements.
//   When you're done, nums1 holds all m + n elements, sorted.
//
// Only tested when kTestInPlace (below) is true.
// ---------------------------------------------------------------------------
class Solution3 {
public:
  void merge(std::vector<int> &nums1, int m, const std::vector<int> &nums2,
             int n) {
    // TODO: read README section 9 before writing anything.
    //       Answer this first: "which end of nums1 is safe to write to?"

    // Silences "unused parameter" warnings. Delete this line when you start.
    (void)nums1, (void)m, (void)nums2, (void)n;
  }
};

// ===========================================================================
// Test settings: these are the only two lines you need to change
// ===========================================================================
typedef Solution1 Solution;          // Step 2: change to Solution2
constexpr bool kTestInPlace = false; // Step 3: change to true

// ===========================================================================
// Test harness. You don't need to change anything below, except adding your
// own test cases in main().
// ===========================================================================

constexpr int kRandomTests = 200;
constexpr int kMaxQuietFailures = 5;

// LeetCode fills the empty tail of nums1 with 0. We use a strange value
// instead: if Solution3 ever treats an empty slot as real data (or never
// fills it), 123456789 shows up in the output and the test fails loudly.
constexpr int kEmptySlot = 123456789;

int testsRun = 0;
int testsFailed = 0;
int inPlaceRun = 0;
int inPlaceFailed = 0;
int quietFailuresPrinted = 0;

std::string toString(const std::vector<int> &v, size_t limit = 12) {
  std::string out = "[";
  for (size_t k = 0; k < v.size() && k < limit; ++k) {
    if (k > 0) {
      out += ",";
    }
    out += std::to_string(v[k]);
  }
  if (v.size() > limit) {
    out += ",... (" + std::to_string(v.size()) + " total)";
  }
  return out + "]";
}

// Describes the FIRST place two vectors differ. Much easier to read than
// two 40-element lists side by side. Only called when they do differ.
std::string firstDifference(const std::vector<int> &expected,
                            const std::vector<int> &actual) {
  const size_t common = std::min(expected.size(), actual.size());
  for (size_t k = 0; k < common; ++k) {
    if (expected[k] != actual[k]) {
      return "index " + std::to_string(k) + ": expected " +
             std::to_string(expected[k]) + ", actual " +
             std::to_string(actual[k]);
    }
  }
  return "size: expected " + std::to_string(expected.size()) + ", actual " +
         std::to_string(actual.size());
}

// quiet = true is used by the random tests: passes are not printed, and at
// most kMaxQuietFailures failures are printed.
void printResult(const std::string &call, bool ok,
                 const std::vector<int> &expected,
                 const std::vector<int> &actual, bool quiet) {
  if (quiet) {
    if (ok || quietFailuresPrinted >= kMaxQuietFailures) {
      return;
    }
    ++quietFailuresPrinted;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << call << '\n';
  if (!ok) {
    std::cout << "          expected = " << toString(expected, 50) << '\n'
              << "          actual   = " << toString(actual, 50) << '\n'
              << "          first difference at "
              << firstDifference(expected, actual) << '\n';
  }
}

void checkInPlace(const std::vector<int> &nums1, const std::vector<int> &nums2,
                  const std::vector<int> &expected, bool quiet) {
  ++inPlaceRun;

  // Build LeetCode-style input: nums1's real elements, then n empty slots.
  std::vector<int> buffer = nums1;
  buffer.resize(nums1.size() + nums2.size(), kEmptySlot);

  Solution3 solution;
  solution.merge(buffer, static_cast<int>(nums1.size()), nums2,
                 static_cast<int>(nums2.size()));

  const bool ok = (buffer == expected);
  if (!ok) {
    ++inPlaceFailed;
  }
  printResult("in-place merge(" + toString(nums1) + ", " + toString(nums2) +
                  ")",
              ok, expected, buffer, quiet);
}

void check(const std::vector<int> &nums1, const std::vector<int> &nums2,
           const std::vector<int> &expected, bool quiet = false) {
  ++testsRun;

  Solution solution;
  const std::vector<int> actual = solution.merge(nums1, nums2);

  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }
  printResult("merge(" + toString(nums1) + ", " + toString(nums2) + ")", ok,
              expected, actual, quiet);

  if (kTestInPlace) {
    checkInPlace(nums1, nums2, expected, quiet);
  }
}

// The real brute force: glue both lists together, then sort.
// It's obviously correct, so it acts as the referee for the random tests.
std::vector<int> bruteForce(const std::vector<int> &nums1,
                            const std::vector<int> &nums2) {
  std::vector<int> all = nums1;
  all.insert(all.end(), nums2.begin(), nums2.end());
  std::sort(all.begin(), all.end());
  return all;
}

std::vector<int> randomSorted(std::mt19937 &rng, int maxLength, int low,
                              int high) {
  std::uniform_int_distribution<int> lengthDist(0, maxLength);
  std::uniform_int_distribution<int> valueDist(low, high);
  std::vector<int> v(lengthDist(rng));
  for (int &x : v) {
    x = valueDist(rng);
  }
  std::sort(v.begin(), v.end());
  return v;
}

int main() {
  std::cout << "Hand-written tests\n";

  // examples from the problem statement
  check({1, 2, 3}, {2, 5, 6}, {1, 2, 2, 3, 5, 6});
  check({}, {1}, {1});
  check({1}, {}, {1});

  // both empty
  check({}, {}, {});

  // one array entirely before the other
  check({4, 5, 6}, {1, 2, 3}, {1, 2, 3, 4, 5, 6});
  check({1, 2, 3}, {4, 5, 6}, {1, 2, 3, 4, 5, 6});

  // perfect interleaving
  check({1, 3, 5, 7, 9}, {2, 4, 6, 8, 10}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

  // one array sits inside the other / one element vs many
  check({1, 5}, {2, 3, 4}, {1, 2, 3, 4, 5});
  check({3}, {1, 2, 4, 5}, {1, 2, 3, 4, 5});
  check({100}, {1, 2, 3}, {1, 2, 3, 100});
  check({1, 2, 3}, {100}, {1, 2, 3, 100});

  // duplicates and ties
  check({1, 1, 1}, {1, 1, 1}, {1, 1, 1, 1, 1, 1});
  check({2, 2, 2, 2}, {1, 1, 1, 1}, {1, 1, 1, 1, 2, 2, 2, 2});
  check({5, 5, 5, 5, 5}, {5, 5, 5}, {5, 5, 5, 5, 5, 5, 5, 5});
  check({0}, {0}, {0, 0});
  check({1, 1, 2, 3, 4, 5, 6, 9}, {1, 1, 2, 2, 7, 8, 8, 8},
        {1, 1, 1, 1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 8, 8, 9});

  // negatives
  check({-5, -3, -1}, {-4, -2, 0}, {-5, -4, -3, -2, -1, 0});
  check({-1}, {-1}, {-1, -1});
  check({-3, -2, -1}, {-3, -2, -1}, {-3, -3, -2, -2, -1, -1});

  // extreme values from the constraints
  check({-1000000000}, {1000000000}, {-1000000000, 1000000000});
  check({1000000000, 1000000000}, {-1000000000},
        {-1000000000, 1000000000, 1000000000});

  // maximum size: 200 + 200
  std::vector<int> evens;
  std::vector<int> odds;
  std::vector<int> zeroTo399;
  for (int k = 0; k < 200; ++k) {
    evens.push_back(2 * k);
    odds.push_back(2 * k + 1);
  }
  for (int k = 0; k < 400; ++k) {
    zeroTo399.push_back(k);
  }
  check(evens, odds, zeroTo399);

  std::cout << "\nRandom tests vs brute force (only failures are printed)\n";
  std::mt19937 rng(42); // fixed seed: a failure shows up again on every run
  for (int t = 0; t < kRandomTests; ++t) {
    const bool tiny = (t % 4 == 0);     // lots of empty and 1-element lists
    const bool manyTies = (t % 3 == 0); // small value range: many duplicates
    const int maxLength = tiny ? 3 : 200;
    const int low = manyTies ? -5 : -1000000000;
    const int high = manyTies ? 5 : 1000000000;

    const std::vector<int> a = randomSorted(rng, maxLength, low, high);
    const std::vector<int> b = randomSorted(rng, maxLength, low, high);
    check(a, b, bruteForce(a, b), true);
  }

  std::cout << '\n'
            << (testsRun - testsFailed) << '/' << testsRun << " passed\n";
  if (kTestInPlace) {
    std::cout << "Solution3 (in place): " << (inPlaceRun - inPlaceFailed) << '/'
              << inPlaceRun << " passed\n";
  }

  return (testsFailed == 0 && inPlaceFailed == 0) ? 0 : 1;
}
