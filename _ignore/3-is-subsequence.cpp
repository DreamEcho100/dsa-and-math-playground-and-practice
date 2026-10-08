#include <iostream>
#include <string>

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

typedef Solution2 Solution;

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
void check(const std::string &s, const std::string &t, bool expected) {
  ++testsRun;

  Solution1 slow;
  Solution2 fast;
  const bool slowResult = slow.isSubsequence(s, t);
  const bool fastResult = fast.isSubsequence(s, t);

  const bool ok = (fastResult == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "isSubsequence(\""
            << preview(s) << "\", \"" << preview(t) << "\")"
            << "  expected=" << std::boolalpha << expected
            << "  actual=" << fastResult << '\n';

  if (slowResult != fastResult) {
    std::cout << "          MISMATCH: two-pointer said " << std::boolalpha
              << slowResult << '\n';
  }
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
  check("aa", "a", false);
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

  // stress (constraints: |s| <= 100, |t| <= 10^4)
  check(std::string(100, 'a'), std::string(10000, 'a'), true);
  check(std::string(100, 'a'), std::string(99, 'a'), false);
  check(std::string(99, 'a') + "b", std::string(10000, 'a'), false);

  // outside the constraints, but legal std::string:
  // exposes reliance on sequence[size()] == '\0'
  check("a", std::string("a\0\0", 3), true);

  return 0;
}