#include <iostream>
#include <string>

// your original solution: correct
class Solution1 {
public:
  int lengthOfLastWord(const std::string &s) {
    bool wasInWord = false; // TODO (section 4a): unused, delete this line
    int lastWordCount = 0;

    for (int i = s.size() - 1; i >= 0; --i) {
      if (s[i] == ' ') {
        if (lastWordCount > 0) {
          return lastWordCount;
        }
        continue;
      }

      lastWordCount++;
    }

    return lastWordCount;
  }
};

// your turn (section 5): two phases, two loops
class Solution2 {
public:
  int lengthOfLastWord(const std::string &s) {
    int i = static_cast<int>(s.size()) - 1; // start at the last character

    // phase 1: skip the spaces at the end
    // TODO: while i is still inside the string AND s[i] is a space, step left

    // phase 2: count the letters of the last word
    int length = 0;
    // TODO: while i is still inside the string AND s[i] is not a space,
    //       count it and step left

    return length;
  }
};

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

void check(const std::string &s, int expected) {
  ++testsRun;
  const int actual = solution.lengthOfLastWord(s);
  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "lengthOfLastWord(\""
            << preview(s) << "\")"
            << "  expected=" << expected << "  actual=" << actual << '\n';
}

int main() {
  std::cout << "--- lengthOfLastWord ---\n";

  // examples from the problem statement
  check("Hello World", 5);
  check(" fly me to the moon ", 4);
  check("luffy is still joyboy", 6);

  // one word, no spaces: counting must stop at the start of the string
  check("a", 1);
  check("abc", 3);
  check("luffy", 5);

  // trailing spaces must be skipped
  check("a ", 1);
  check("a   ", 1);
  check("abc   ", 3);

  // leading spaces
  check(" a", 1);
  check("   abc", 3);

  // both
  check("   a   ", 1);

  // several spaces between words
  check("a    bc", 2);
  check("ab    c", 1);

  // last word shorter / longer than the others
  check("abcdef g", 1);
  check("a bcdef", 5);

  // stress (constraints: up to 10^4)
  check(std::string(10000, 'a'), 10000);  // one huge word
  check(std::string(9999, ' ') + "a", 1); // word at the very end
  check("a" + std::string(9999, ' '), 1); // walk over 9999 spaces
  check(std::string(5000, 'a') + " " + std::string(4999, 'b'), 4999);

  // outside the constraints: no word at all
  check("   ", 0);
  check("", 0); // exercises int i = s.size() - 1 on an empty string

  std::cout << "\n" << (testsRun - testsFailed) << "/" << testsRun << " passed";
  if (testsFailed > 0) {
    std::cout << "  (" << testsFailed << " FAILED)";
  }
  std::cout << '\n';

  return testsFailed == 0 ? 0 : 1;
}
