/*
## Food for thought

- Which used to be proven can be faster? false or true cases?
- Optimizing space by passing read only things as a reference/pointer!
*/
#include <ios>
#include <iostream>
#include <string>

/**
 * @brief Solution1: naive approach, brute force, O(n + m) time, O(n + m) space
 * since we make two copies of each string
 */
class Solution1 {
private:
  std::string formatForBackspaceCompare(const std::string &str) {
    std::string strCopy = "";
    for (char letter : str) {
      if (letter == '#') {
        if (!strCopy.empty()) {
          strCopy.pop_back();
        }
        continue;
      }
      strCopy += letter;
    }
    return strCopy;
  }

public:
  bool backspaceCompare(const std::string &str1, const std::string &str2) {
    return formatForBackspaceCompare(str1) == formatForBackspaceCompare(str2);
  }
};

/**
 * @brief Solution2: two pointers from the tail, O(n + m) time, O(1) space
 * since we don't make any copies of the strings
 */

class Solution2 {
private:
  int nextValidTailIndex(const std::string &str, int i) {
    int pendingBackspacesAmount = 0;
    // Walk left while `i` is still inside the string (i >= 0),
    // stopping early if we land on a surviving character
    while (i >= 0) {
      if (str[i] == '#') {
        // Decrement the `i` position
        i--;
        // Incerement the accumulated skips
        pendingBackspacesAmount++;
      } else if (pendingBackspacesAmount > 0) {
        // Then, we will decrement the `i` position by one for each
        // skipped iteration, to get over what's deleted
        i--;
        pendingBackspacesAmount--;
      } else {
        // No accumulated skipped counters or the delete flag, servive!
        break;
      }
    }

    return i;
  }

public:
  bool backspaceCompare(const std::string &str1, const std::string &str2) {
    int i = static_cast<int>(str1.size()) - 1;
    int j = static_cast<int>(str2.size()) - 1;

    // We will make sure we exhausted all to be able to compare the actual final
    // form!
    // NOTE: Since we check from the end we have like a clear indicator if they
    // will _NOT_ match early
    while ((i >= 0 || j >= 0)) {
      i = nextValidTailIndex(str1, i);
      j = nextValidTailIndex(str2, j);

      // CASE 1: both found a surviving character
      //         -> compare them. differ?  ______
      if (i >= 0 && j >= 0) {
        // WHY? since we comparing from the tail of the strings
        // If both _(serviving)_ ends/tails _(that we found)_ don't match
        // Then there is no need to continue, they won't match anyway on
        // complete loop
        // like in "ab#cd#" and "ab##dc#"
        // already the first valid finding for both will be "c" and "d"
        // respectively
        // And they will servivie until the end so ne need to continue
        if (str1[i] != str2[j]) {
          return false;
        }
      }

      // CASE 2: exactly one found a character, the other hit -1
      //         -> what does that say about the two results?  ______
      if (
          // The second is the only empty
          (i >= 0 && j < 0) ||
          // The first is the only empty
          (j >= 0 && i < 0)) {
        // As in if on of the strings is already empty and the still on the has
        // a valid tail index Then nothing more needs to compar, the final form
        // will always not match
        return false;
      }

      // CASE 3: both hit -1
      //         -> both exhausted together. what should the loop do?
      // No need, the loop already exit on the next `while` condition check
      // NOTE: there is a chance any of them could end as `-2`!

      // then: step past the characters you just compared
      i--;
      j--;
    }

    return true;
  }
};

typedef Solution2 Solution;

// ---------------------------------------------------------------- test harness

static int testsRun = 0;
static int testsFailed = 0;

Solution solution;

void check(const std::string &s, const std::string &t, bool expected) {
  ++testsRun;
  const bool actual = solution.backspaceCompare(s, t);
  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "backspaceCompare(\"" << s
            << "\", \"" << t << "\")"
            << "  expected=" << std::boolalpha << expected
            << "  actual=" << std::boolalpha << actual << '\n';
}

int main() {
  std::cout << "--- backspaceCompare ---\n";

  // examples from the problem statement
  check("ab#c", "ad#c", true); // both -> "ac"
  check("ab##", "c#d#", true); // both -> ""
  check("a#c", "b", false);    // "c" vs "b"

  // // empty inputs
  check("", "", true);
  check("", "#", true);
  check("", "###", true);
  check("", "a", false);
  // return 0;

  // backspaces with nothing to delete
  check("###a", "a", true);
  check("#a", "a", true);
  check("a#", "", true);
  check("a#", "b#", true);
  check("ab#", "a", true);

  // deletions interleaved
  check("a##c", "#a#c", true); // both -> "c"
  check("abc###", "def###", true);
  check("ab#c#d", "ad", true); // "ad" vs "ad"?  trace it by hand first
  check("xywrrmp", "xywrrmu#p", true);
  check("bxj##tw", "bxo#j##tw", true);
  check("bxj##tw", "bxj###tw", false);

  // prefix traps: one std::string is a prefix of the other after
  //   processing
  check("abc", "ab", false);
  check("b", "a", false);
  check("a", "ab#", true);
  check("nzp#o#g", "b#nzp#o#g", true);

  //   // longer / stress
  check(std::string(200, 'a'), std::string(200, 'a'), true);
  check(std::string(200, '#'), "", true);
  check(std::string(100, 'a') + std::string(100, '#'), "", true);

  std::cout << "\n" << (testsRun - testsFailed) << "/" << testsRun << " passed";
  if (testsFailed > 0) {
    std::cout << "  (" << testsFailed << " FAILED)";
  }
  std::cout << '\n';

  return testsFailed == 0 ? 0 : 1;
}