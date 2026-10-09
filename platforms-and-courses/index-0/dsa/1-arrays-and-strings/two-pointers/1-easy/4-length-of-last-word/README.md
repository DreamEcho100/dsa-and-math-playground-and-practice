# Length of Last Word

You're given a string made up of words separated by one or more spaces,
and possibly with leading or trailing spaces too. A "word" is a maximal
run of non-space characters.

Return the length of the last word in the string.

The string always contains at least one non-space character, so there
is always a last word to find.

```
Input: s = "Hello World"
Output: 5
the last word is "World", which has 5 characters
```

```
Input: s = " fly me to the moon "
Output: 4
trailing spaces are ignored; the last word is "moon"
```

```
Input: s = "luffy is still joyboy"
Output: 6
the last word is "joyboy"
```

```
Constraints
1 <= s.length <= 10^4
s consists of English letters and spaces only
there is at least one word in s
```

<details>
<summary>💡 Hint</summary>

You only care about the last word. Do you really need to process the whole
string from the front?

Original source: [LeetCode](https://leetcode.com/problems/length-of-last-word/)

</details>

---

## Files in this folder

| file       | what it is                                                  |
| ---------- | ----------------------------------------------------------- |
| `main.cpp` | `Solution1` (your original), `Solution2` (your turn), tests |
| this file  | the walkthrough                                             |

---

## 1. What we're looking for, in a picture

`s = " fly me to the moon "` (note the space at both ends):

```
index:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
s:      _  f  l  y  _  m  e  _  t  o  _  t  h  e  _  m  o  o  n  _
                                                    ^^^^^^^^^^
                                                    last word: 4
```

(`_` = space.)

Two things can get in the way:

- **Spaces after the last word** (index 19). They don't count.
- **The last word might be the only word**, with nothing before it
  (`"luffy"`). Then there's no space to tell you where the word starts.
  The word simply begins at index 0.

---

## 2. The idea: read from the end

The last word is at the **end**, so start there and walk left.
The walk has two phases:

1. **Skip spaces.** While you're on a space, step left.
   Now you're standing on the last letter of the last word.
2. **Count letters.** While you're on a letter, count it and step left.
   Stop at a space, or when you run off the start of the string.

The count is the answer.

You never look at the words before the last one. In
`"luffy is still joyboy"` you read 6 letters and 1 space, then you're done.

---

## 3. Trace it (worked for you)

`s = " fly me to the moon "`, size 20. `i` starts at the last index, 19.

| step | i   | `s[i]` | phase       | do this          | length after |
| ---- | --- | ------ | ----------- | ---------------- | ------------ |
| 1    | 19  | `_`    | skip spaces | step left        | 0            |
| 2    | 18  | `n`    | count       | count, step left | 1            |
| 3    | 17  | `o`    | count       | count, step left | 2            |
| 4    | 16  | `o`    | count       | count, step left | 3            |
| 5    | 15  | `m`    | count       | count, step left | 4            |
| 6    | 14  | `_`    | count       | space → **stop** | 4            |

Answer: **4**. Indexes 0 to 13 were never read.

### Your turn

Trace `s = "luffy"` (one word, no spaces at all). What makes the counting
phase stop?

<details>
<summary>Answer</summary>

| step | i   | `s[i]` | phase | do this                  | length after |
| ---- | --- | ------ | ----- | ------------------------ | ------------ |
| 1    | 4   | `y`    | count | count, step left         | 1            |
| 2    | 3   | `f`    | count | count, step left         | 2            |
| 3    | 2   | `f`    | count | count, step left         | 3            |
| 4    | 1   | `u`    | count | count, step left         | 4            |
| 5    | 0   | `l`    | count | count, step left         | 5            |
| 6    | -1  | —      | count | **off the start → stop** | 5            |

Answer: **5**.

There was no space to stop at. The counting stopped because `i` became
`-1`, meaning "left of the first character". **Your loop must check for
that**, or it will read `s[-1]`.

</details>

---

## 4. Review of your solution

This is `Solution1` in `main.cpp`:

```cpp
class Solution1 {
public:
  int lengthOfLastWord(const std::string &s) {
    bool wasInWord = false;
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
```

**Verdict: correct.** It reads from the end, it ignores trailing spaces
(a space with `lastWordCount == 0` is skipped), it stops at the first space
after the word, and the final `return` handles a word that starts at
index 0 (the `"luffy"` trace). Nice work.

It does both phases from section 2 in **one** loop. The `if (lastWordCount > 0)`
is what tells them apart: count 0 means "still skipping spaces", count above 0
means "inside the word".

Three things to improve.

### 4a. `wasInWord` is never used

You declared it, maybe planning to use it, then solved the problem without it.
Compile with warnings and the compiler tells you:

```
warning: unused variable 'wasInWord' [-Wunused-variable]
```

Delete it. Unused variables make a reader wonder what they're for.

> **Habit:** always compile with `-Wall -Wextra`. The compiler is a free reviewer.

### 4b. `int i = s.size() - 1` does the subtraction in the wrong type

`s.size()` is `size_t`, which is **unsigned** (it can't be negative).
In `s.size() - 1`, the subtraction happens **first**, while the value is
still `size_t`. Only **then** is the result converted to `int`.

That's fine when `s` has characters. But with an empty string:

```
s.size()      = 0          (size_t)
s.size() - 1  = 18446744073709551615   (0 - 1 wraps around!)
convert to int -> -1       (on common compilers; guaranteed only since C++20)
```

It ends up as `-1` and the loop doesn't run, so it happens to work. But it
only works by going through a gigantic number and back.

Convert **first**, then subtract, and the gigantic number never exists:

```cpp
int i = static_cast<int>(s.size()) - 1; // 0 - 1 = -1, as an int
```

The constraints say `s` has at least 1 character, so this can't bite you
here. But the habit protects you in every problem where the input might be empty.

### 4c. Here `int` is the RIGHT choice, not `size_t`

In is-subsequence we used `size_t` because the index only went **up**.
Here `i` goes **down**, and the loop only ends when `i` goes **below zero**
(the `"luffy"` trace ended at `i = -1`).

A `size_t` can never be below zero. Try it:

```cpp
for (size_t i = s.size() - 1; i >= 0; --i) { // BUG
```

- `i >= 0` is **always** true for an unsigned type, so the loop never stops on its own.
- When `i` is 0 and you do `--i`, it wraps to 18446744073709551615.
- The next `s[i]` reads far outside the string: undefined behaviour.

Same rule as before: **if a variable can go negative, use a signed type.**
Your instinct to use `int` was right. Section 4b is just about _how_ you
initialise it.

---

## 5. A cleaner version: two phases, two loops

Your single loop works, but the reader has to figure out that
`lastWordCount > 0` secretly means "which phase am I in". Writing the two
phases as two loops makes the code say exactly what section 2 says.

### Your turn: fill in the TODOs

This is `Solution2` in `main.cpp`:

```cpp
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
```

One question before you write the conditions: **in what order should the
two checks go?**

<details>
<summary>Answer</summary>

Bounds check **first**: `i >= 0 && s[i] == ' '`.

`&&` stops as soon as the left side is false. This is called
**short-circuiting**. When `i` is `-1`, `i >= 0` is false, so `s[i]` is
never evaluated and `s[-1]` is never read.

Written the other way round, `s[i] == ' ' && i >= 0` reads `s[-1]` first
and checks afterwards, when it's too late.

This is the same lesson as is-subsequence: **check that an index is valid
before you use it.**

</details>

<details>
<summary>Full solution (only after you've tried)</summary>

```cpp
class Solution2 {
public:
  int lengthOfLastWord(const std::string &s) {
    int i = static_cast<int>(s.size()) - 1;

    while (i >= 0 && s[i] == ' ') {
      i--;
    }

    int length = 0;
    while (i >= 0 && s[i] != ' ') {
      length++;
      i--;
    }

    return length;
  }
};
```

</details>

---

## 6. Questions people always ask

### Q1. Can either loop run forever?

<details>
<summary>Answer</summary>

No. Both loops do `i--` on **every** iteration, and both stop when
`i < 0`. `i` starts at most at 9999, so each loop runs at most 10,000 times.
Together they still read each character at most once, because phase 2
starts where phase 1 stopped.

Same habit as before: _does something move on every iteration?_ Yes, `i`.

</details>

### Q2. What does Solution2 return if the string is only spaces, like `"   "`?

<details>
<summary>Answer</summary>

Phase 1 walks all the way to `i = -1`. Phase 2 doesn't run at all, so the
result is `0`.

The constraints promise this can't happen, but it's good to know your code
gives a sensible answer anyway instead of crashing.

</details>

### Q3. The hint asks: "do you need to process the whole string from the front?" What would the front-to-back version look like?

<details>
<summary>Answer</summary>

Walk left to right and keep a counter. On a letter: if the previous
character was a space, a **new** word starts, so reset the counter to 0
first; then add 1. On a space, do nothing.

```cpp
int length = 0;
for (size_t k = 0; k < s.size(); k++) {
  if (s[k] != ' ') {
    if (k > 0 && s[k - 1] == ' ') {
      length = 0; // a new word starts here
    }
    length++;
  }
}
return length;
```

When the loop ends, `length` belongs to the last word that was seen.

It's correct, but it **always** reads all of `s`. For
`"luffy is still joyboy"` it counts `luffy`, throws it away, counts `is`,
throws it away, and so on. The backward version reads only what it needs.
That's what the hint is pointing at.

(`size_t` is fine here because `k` only goes up. `k - 1` is safe because
of the `k > 0 &&` check in front of it.)

</details>

---

## 7. What it costs

| version               | time                                    | space |
| --------------------- | --------------------------------------- | ----- |
| front to back         | always all of `s`: O(n)                 | O(1)  |
| back to front (yours) | trailing spaces + last word: O(n) worst | O(1)  |

Both are O(n) in the worst case. For example, `"a"` followed by 9,999
spaces makes the backward version walk through all of them. But the backward
version usually reads far less. Big-O is the same; the real work isn't.

---

## 8. Test it

All commands below are run from inside this folder **here** and then make sure there's a `build` folder.

```bash
mkdir -p build
```

Testing happens in three steps. Do them in order.

### Step 1: test your original (checks the tests)

`main.cpp` starts with:

See the main `platforms-and-courses/index-0/1-arrays-and-strings/two-pointers/1-easy/1-backspace-string-compare/README.md` for running and debugging instructions

Expected: **`22/22 passed`**, plus two warnings:

- `unused variable 'wasInWord'`. Delete that line (section 4a) and recompile.
- `unused variable 'i'` in `Solution2`. It goes away once you fill in the TODOs.

This step checks the **tests**, not your code. Solution1 is already known
to be correct, so if a test fails here, that test's expected value is wrong.

### Step 2: test your new version

Fill in the `Solution2` TODOs, then change one line in `main.cpp`:

```cpp
typedef Solution2 Solution;
```

Recompile and run with the same two commands. Same 22 tests, one line
changed. If a test fails, its name tells you which situation your loops
miss. For example:

| failing test         | what's probably wrong                                |
| -------------------- | ---------------------------------------------------- |
| `"luffy"`, `"a"`     | phase 2 doesn't stop at the start (`i >= 0` missing) |
| `"a   "`, `"abc   "` | phase 1 doesn't skip trailing spaces                 |
| `"Hello World"`      | phase 2 doesn't stop at a space                      |
| crash / sanitizer    | `s[i]` is checked before `i >= 0`                    |

### Step 3 (optional): referee mode

Once both pass on their own, make every test run **both** solutions and
print a warning if they ever disagree. Replace `check()` in `main.cpp` with:

```cpp
void check(const std::string &s, int expected) {
  ++testsRun;

  Solution1 referee;
  Solution2 candidate;
  const int refResult = referee.lengthOfLastWord(s);
  const int actual = candidate.lengthOfLastWord(s);

  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "lengthOfLastWord(\""
            << preview(s) << "\")"
            << "  expected=" << expected << "  actual=" << actual << '\n';

  if (refResult != actual) {
    std::cout << "          MISMATCH: Solution1 said " << refResult << '\n';
  }
}
```

In referee mode, `typedef ... Solution;` and `Solution solution;` aren't
used anymore, so delete them.

---

## 9. Follow-ups (short exercises)

### 9a. Return the last word itself, not its length

After phase 2, where does the word start? Use `s.substr(start, length)`.

<details>
<summary>Answer</summary>

Phase 2 stops with `i` one position **left** of the word, either on a
space or at `-1`. So the word starts at `i + 1`:

```cpp
return s.substr(i + 1, length);
```

Check with `"luffy"`: `i` ends at `-1`, so `substr(0, 5)` = `"luffy"`. ✓

</details>

### 9b. Words can be separated by tabs and newlines too

Replace `s[i] == ' '` with `std::isspace(...)` from `<cctype>`.
There's a trap: what type must you pass to it?

<details>
<summary>Answer</summary>

```cpp
std::isspace(static_cast<unsigned char>(s[i]))
```

`std::isspace` needs a value from 0 to 255 (or `EOF`). Plain `char` can be
negative on many compilers, which is undefined behaviour. This is the same
`unsigned char` cast as in is-subsequence's `Solution3`.

</details>

### 9c. Use the standard library instead of loops

`std::string` has `find_last_not_of(' ')` (the last non-space) and
`find_last_of(' ', pos)` (the last space at or before `pos`). Both return
`std::string::npos` if nothing is found. Write the solution with them.

<details>
<summary>Answer</summary>

```cpp
const size_t end = s.find_last_not_of(' ');    // last letter of last word
const size_t space = s.find_last_of(' ', end); // space before it, or npos
return static_cast<int>(space == std::string::npos ? end + 1 : end - space);
```

`" fly me to the moon "`: `end = 18`, `space = 14`, so `18 - 14 = 4`. ✓

`"luffy"`: `end = 4`, `space = npos`, so `4 + 1 = 5`. ✓

The `npos` check matters: without it you'd compute `end - npos` and rely on
unsigned wrap-around to give the right number. It does work, but nobody
reading it would know why.

**Careful:** this version assumes there is at least one word, as the
constraints promise. For `"   "`, `end` is `npos`, and the result is
garbage instead of 0. The loop version (Q2) handles that case; this one
would need an extra `if (end == std::string::npos) return 0;`.

</details>

---

## Checklist

- [ ] I can explain the two phases (skip spaces, count letters) without looking.
- [ ] I traced `"luffy"` and know what stops the counting.
- [ ] Step 1 printed `22/22 passed` with `typedef Solution1 Solution;`.
- [ ] I deleted `wasInWord` and the warning is gone.
- [ ] I can explain why `static_cast<int>(s.size()) - 1` is better than `s.size() - 1`.
- [ ] I can explain why `size_t i` with `i >= 0` never stops.
- [ ] I filled in the `Solution2` TODOs without opening the solution.
- [ ] Step 2 printed `22/22 passed` with `typedef Solution2 Solution;`.

Paste your output, or tell me the first step where you got stuck.
