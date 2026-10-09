
/*
Find the Index of the First Occurrence in a String

You're given two strings, haystack and needle.

Find the index of the first occurrence of `needle` inside `haystack`, and return
that index, or -1 if `needle` does not appear anywhere.

If needle is empty, return 0, since an empty string is considered to occur at
position 0.

Input: haystack = "sadbutsad", needle = "sad"
Output: 0
"sad" first appears starting at index 0
Input: haystack = "leetcode", needle = "leeto"
Output: -1
"leeto" never appears as a contiguous substring of "leetcode"
Input: haystack = "hello", needle = "ll"
Output: 2
Constraints
1 <= length of haystack <= 10^4
0 <= length of needle <= 10^4
haystack and needle consist of lowercase English characters only
*/

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

/**
 * @brief Solution1: brute force, O(n * m) with worst case scenario of O(n^2)
 * time, O(1) extra space because we don't make any copies of the strings
 */
class Solution1 {
public:
  int strStr(const std::string haystack, const std::string needle) {
    size_t haystackSize = haystack.size();
    size_t needleSize = needle.size();

    if (haystackSize == 0) {
      if (needleSize == 0) {
        return 0;
      }
      return -1;
    }

    if (needleSize == 0) {
      return 0;
    }

    for (size_t i = 0; i < haystackSize; i++) {
      if (haystack[i] == needle[0]) {
        if (i + needleSize - 1 > haystackSize - 1) {
          return -1;
        }

        size_t j = 0;
        // for (; j < needleSize; j++) {
        //   if (needle[j] != haystack[i + j]) {
        //     break;
        //   }
        // }
        while (j < needleSize && needle[j] == haystack[i + j]) {
          j++;
        }

        if (j == needleSize) {
          return i;
        }
      }
    }

    return -1;
  }
};

/**
 * @brief Solution2: KMP, O(n + m) time, O(m) extra space.
 * Your turn (README section 6): fill in the three TODOs.
 *
 * WARNING: while the TODOs are empty, nothing moves `i`, so the loop never
 * ends. Don't switch the typedef to Solution2 until you've filled them in.
 */
class Solution2 {

public:
  // README section 7: lps[k] = length of the longest proper prefix of
  // needle[0..k] that is also a suffix of needle[0..k]
  static std::vector<int> buildLps(const std::string &needle) {}

  int strStr(const std::string &haystack, const std::string &needle) {

    return -1;
  }
};

// ---------------------------------------------------------------- test harness

// step 1: test Solution1 first, to make sure the tests themselves are right
// step 2: once Solution2 is filled in, change this line to Solution2
typedef Solution1 Solution;

// ---------------------------------------------------------------- test harness

static int testsRun = 0;
static int testsFailed = 0;

Solution solution;

// keep long stress-test strings readable in the output
std::string preview(const std::string &str) {
  const size_t maxLen = 20;
  if (str.size() <= maxLen) {
    return str;
  }
  return str.substr(0, maxLen) + "...(len=" + std::to_string(str.size()) + ")";
}

std::string toString(const std::vector<int> &values) {
  std::string out = "[";
  for (size_t k = 0; k < values.size(); k++) {
    if (k > 0) {
      out += ",";
    }
    out += std::to_string(values[k]);
  }
  return out + "]";
}

// checks buildLps on its own, so a wrong table is reported directly
void checkLps(Solution2 solution, const std::string &needle,
              const std::vector<int> &expected) {
  ++testsRun;
  const std::vector<int> actual = solution.buildLps(needle);
  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "buildLps(\"" << needle
            << "\")  expected=" << toString(expected)
            << "  actual=" << toString(actual) << '\n';
}
// Just to avoid lint errors
void checkLps(Solution1 solution, const std::string &needle,
              const std::vector<int> &expected) {
  return;
}

void check(const std::string &haystack, const std::string &needle,
           int expected) {
  ++testsRun;
  const int actual = solution.strStr(haystack, needle);
  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "strStr(\""
            << preview(haystack) << "\", \"" << preview(needle) << "\")"
            << "  expected=" << expected << "  actual=" << actual << '\n';
}

int main() {
  std::cout << "--- buildLps ---\n";

  // check if `solution` is an instance of Solution2
  if constexpr (std::is_same<Solution, Solution2>::value) {
    checkLps(solution, "sad", {0, 0, 0}); // no repeats: every entry is 0
    checkLps(solution, "ababc", {0, 0, 1, 2, 0}); // README section 5
    checkLps(solution, "aabaa", {0, 1, 0, 1, 2}); // README section 5 practice
    checkLps(solution, "aaaa", {0, 1, 2, 3});     // every prefix overlaps
    checkLps(solution, "abacabab",
             {0, 0, 1, 0, 1, 2, 3, 2}); // needs a fallback inside
  }

  std::cout << "\n--- strStr ---\n";

  // examples from the problem statement
  check("sadbutsad", "sad", 0);
  check("leetcode", "leeto", -1);
  check("hello", "ll", 2);

  // empty needle -> 0
  check("a", "", 0);
  check("abc", "", 0);

  // single characters
  check("a", "a", 0);
  check("a", "b", -1);
  check("abc", "a", 0);
  check("abc", "c", 2); // last index (old off-by-one bug)
  check("ab", "b", 1);

  // needle longer than / equal to haystack
  check("a", "aa", -1);
  check("abc", "abcd", -1);
  check("abc", "abc", 0);

  // match at the very end
  check("hello", "lo", 3);
  check("sadbutsad", "tsad", 5);
  check("mississippi", "pi", 9);

  // first occurrence wins
  check("abab", "ab", 0);
  check("xxabab", "ab", 2);
  check("aaa", "a", 0);

  // partial-match traps: first char (or more) matches, then fails
  check("sadbutsad", "sax", -1); // old j == needleSize - 1 bug
  check("abcabd", "abd", 3);
  check("mississippi", "issip", 4);
  check("aaaab", "aab", 2);
  check("aaa", "aaaa", -1);

  // not enough room left after a first-char match
  check("abcda", "ab", 0);
  check("xyzab", "abc", -1);

  // KMP fallback: needles with repeated parts (README section 10)
  check("sabvsad", "sad", 4);      // trace 2
  check("abababc", "ababc", 2);    // trace 1: one fallback
  check("aabaxaabaa", "aabaa", 5); // fallback chain: j goes 4 -> 1 -> 0
  check("ababababca", "ababca", 4);
  check("aaaaab", "aaab", 2);
  check("abababab", "abab", 0);
  check("mississippi", "issipi", -1); // near miss

  // longer / stress (constraints: up to 10^4)
  check(std::string(10000, 'a'), std::string(10000, 'a'), 0);
  check(std::string(10000, 'a'), "b", -1);
  check(std::string(9999, 'a') + "b", std::string(5000, 'a') + "b", 4999);

  std::cout << "\n" << (testsRun - testsFailed) << "/" << testsRun << " passed";
  if (testsFailed > 0) {
    std::cout << "  (" << testsFailed << " FAILED)";
  }
  std::cout << '\n';

  return testsFailed == 0 ? 0 : 1;
}
