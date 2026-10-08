
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
  std::cout << "--- strStr ---\n";

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