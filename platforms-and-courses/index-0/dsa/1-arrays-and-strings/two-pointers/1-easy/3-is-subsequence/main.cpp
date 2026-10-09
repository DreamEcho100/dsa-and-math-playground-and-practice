#include <algorithm>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

class Solution1 {
public:
  bool isSubsequence(const std::string &sequence,
                     const std::string &comparedTo) {
    int currCharCheckIndex = 0;
    for (char c : comparedTo) {
      if (c == sequence[currCharCheckIndex]) {
        currCharCheckIndex++;
      }
    }

    return currCharCheckIndex == sequence.size();
  }
};

// Solution2: the fix (README section 5).
// Never reads past the end of `sequence`, and stops early.
class Solution2 {
public:
  bool isSubsequence(const std::string &sequence,
                     const std::string &comparedTo) {
    size_t matchedIndex = 0;
    for (size_t i = 0; i < comparedTo.size() && matchedIndex < sequence.size();
         i++) {
      const char c = comparedTo[i];
      if (c == sequence[matchedIndex]) {
        matchedIndex++;
      }
    }

    return matchedIndex == sequence.size();
  }
};

// Solution3: the follow-up, many queries against the same t (README section 9).
// Your turn: fill in the two TODOs.
class Solution3 {
public:
  // build the table once for t
  explicit Solution3(const std::string &t) : positions(256) {
    for (int k = 0; k < static_cast<int>(t.size()); k++) {
      const unsigned char c = static_cast<unsigned char>(t[k]);
      // TODO: add position k to the list for character c
    }
  }

  // answer one query, using only the table
  bool isSubsequence(const std::string &s) const {
    int prev = -1; // position in t of the last matched character

    for (char ch : s) {
      const unsigned char c = static_cast<unsigned char>(ch);
      const std::vector<int> &list = positions[c];
      // TODO: find the first position in list that is > prev
      //       none?     -> return false
      //       found it? -> prev = that position
    }

    return true;
  }

private:
  std::vector<std::vector<int>>
      positions; // positions[c] = where c appears in t
};

// step 1: Solution1 -> 32/33, only the embedded-'\0' test fails (on purpose)
// step 2: Solution2 -> 33/33
// step 3: Solution3 -> 33/33, once its TODOs are filled in
typedef Solution1 Solution;

// ---------------------------------------------------------------- test harness

static int testsRun = 0;
static int testsFailed = 0;

// Solution1 and Solution2 are asked isSubsequence(s, t) directly.
// Solution3 is built from t first, then asked isSubsequence(s).
// `if constexpr` picks the right way at compile time, so changing the one
// typedef line above is enough to switch between all three.
template <typename Impl>
bool answer(const std::string &s, const std::string &t) {
  if constexpr (std::is_same<Impl, Solution3>::value) {
    const Solution3 impl(t);
    return impl.isSubsequence(s);
  } else {
    Impl impl;
    return impl.isSubsequence(s, t);
  }
}

// keep long stress-test strings readable in the output
std::string preview(const std::string &str) {
  const size_t maxLen = 20;
  if (str.size() <= maxLen) {
    return str;
  }
  return str.substr(0, maxLen) + "...(len=" + std::to_string(str.size()) + ")";
}

void check(const std::string &s, const std::string &t, bool expected) {
  ++testsRun;

  const bool actual = answer<Solution>(s, t);
  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "isSubsequence(\""
            << preview(s) << "\", \"" << preview(t) << "\")"
            << "  expected=" << std::boolalpha << expected
            << "  actual=" << actual << '\n';
}

int main() {
  std::cout << "--- isSubsequence ---\n";

  // examples from the problem statement
  check("abc", "ahbgdc", true);
  check("axc", "ahbgdc", false);

  // empty strings
  check("", "", true);
  check("a", "", false);
  check("", "a", true);

  // single characters
  check("a", "a", true);
  check("a", "b", false);

  // same characters
  check("aaaaaa", "aaaaaa", true);

  // order matters
  check("ab", "ab", true);
  check("ba", "ab", false);
  check("aec", "abcde", false);
  check("ace", "abcde", true);

  // s longer than t
  check("abc", "ab", false);
  check("aaa", "aa", false);

  // each character of t can be used only once
  check("aa", "a", false); // also catches lower_bound in Solution3
  check("aab", "ab", false);
  check("aab", "aaab", true);

  // matches at the edges of t
  check("a", "ahbgdc", true);
  check("c", "ahbgdc", true);
  check("ahbgdc", "ahbgdc", true);

  // only the last character of s is missing
  check("abd", "abc", false);

  // spaces are legal characters (see constraints)
  check(" ", "a b", true);
  check("a b", "ab", false);
  check("a b", "a  b", true);

  // follow-up: jumping through the position lists (README section 9)
  check("cab", "abcabc", true);  // c@2, a@3, b@4
  check("cba", "abcabc", false); // c@2, b@4, then no a after 4
  check("bb", "abcabc", true);   // b@1, b@4
  check("bbb", "abcabc", false); // only two b's
  check("abcabc", "abcabc", true);

  // stress (constraints: |s| <= 100, |t| <= 10^4)
  check(std::string(100, 'a'), std::string(10000, 'a'), true);
  check(std::string(100, 'a'), std::string(99, 'a'), false);
  check(std::string(99, 'a') + "b", std::string(10000, 'a'), false);

  // outside the constraints, but legal std::string:
  // exposes Solution1's reliance on sequence[size()] == '\0'
  check("a", std::string("a\0\0", 3), true);

  std::cout << "\n" << (testsRun - testsFailed) << "/" << testsRun << " passed";
  if (testsFailed > 0) {
    std::cout << "  (" << testsFailed << " FAILED)";
  }
  std::cout << '\n';

  return testsFailed == 0 ? 0 : 1;
}
