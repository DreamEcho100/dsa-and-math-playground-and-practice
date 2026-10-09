# Is Subsequence

You're given two strings, `s` and `t`.

Determine whether `s` is a subsequence of `t`.

A subsequence means every character of `s` appears in `t` in the same
relative order, though not necessarily right next to each other.

```
Input: s = "abc", t = "ahbgdc"
Output: true
a, b, c appear in that order within ahbgdc, with other letters in between
```

```
Input: s = "axc", t = "ahbgdc"
Output: false
there's no x anywhere in t
```

```
Input: s = "", t = "ahbgdc"
Output: true
an empty string is a subsequence of anything
```

```
Constraints
0 <= s.length <= 100
0 <= t.length <= 10^4
s and t consist of lowercase English letters and spaces
```

<details>
<summary>💡 Hint</summary>

You never need to look backward in `t` once you've matched a character.
What does that suggest about how many pointers you need, and how they should move?

</details>

## Files in this folder

| file       | what it is                                                                           |
| ---------- | ------------------------------------------------------------------------------------ |
| `main.cpp` | `Solution1` (original), `Solution2` (fix), `Solution3` (follow-up, your turn), tests |
| this file  | the walkthrough                                                                      |

---

## 1. What "subsequence" means, in a picture

`s = "abc"`, `t = "ahbgdc"`. Can you find `a`, then `b`, then `c` in `t`,
always moving **to the right**?

```
index:  0  1  2  3  4  5
t:      a  h  b  g  d  c
        ^     ^        ^
        a     b        c      -> yes, true
```

You may **skip** characters of `t`. You may **not** go back to the left,
and you may **not** use the same character of `t` twice.

`s = "axc"`: there is no `x` anywhere in `t`, so the answer is false.

`s = "ba"`, `t = "ab"`: both letters exist, but the `b` comes **after** the
`a` in `t`. Order matters, so the answer is false.

---

## 2. The idea: two fingers

Put one finger on `s` and one finger on `t`.

- The **`t` finger** moves right on **every** step. It reads each character of `t` once.
- The **`s` finger** moves right **only when** the two characters are equal.
  It points at "the next character of `s` I'm still looking for".

When the `t` finger runs off the end, check where the `s` finger is:

- If it got past the last character of `s`, everything was found: **true**.
- Otherwise something is missing: **false**.

That's the whole algorithm.

---

## 3. Trace it (worked for you)

`s = "abc"`, `t = "ahbgdc"`. `matched` = position of the `s` finger
(also: how many characters of `s` we've found so far).

| step | k (t finger) | `t[k]` | looking for `s[matched]` | equal? | matched after |
| ---- | ------------ | ------ | ------------------------ | ------ | ------------- |
| 1    | 0            | `a`    | `a`                      | ✓      | 1             |
| 2    | 1            | `h`    | `b`                      | ✗      | 1             |
| 3    | 2            | `b`    | `b`                      | ✓      | 2             |
| 4    | 3            | `g`    | `c`                      | ✗      | 2             |
| 5    | 4            | `d`    | `c`                      | ✗      | 2             |
| 6    | 5            | `c`    | `c`                      | ✓      | 3             |

`matched == 3 == s.size()`, so the result is **true**.

Notice: `k` moves on **every** row. `matched` moves only on a ✓.

### Your turn

Trace `s = "aec"`, `t = "abcde"` on paper, then check.

<details>
<summary>Answer</summary>

| step | k   | `t[k]` | looking for | equal? | matched after |
| ---- | --- | ------ | ----------- | ------ | ------------- |
| 1    | 0   | `a`    | `a`         | ✓      | 1             |
| 2    | 1   | `b`    | `e`         | ✗      | 1             |
| 3    | 2   | `c`    | `e`         | ✗      | 1             |
| 4    | 3   | `d`    | `e`         | ✗      | 1             |
| 5    | 4   | `e`    | `e`         | ✓      | 2             |

`t` ran out with `matched == 2`, but `s.size() == 3`. Result: **false**.

The `c` in `t` went by at step 3 while we were still looking for `e`.
We can't go back for it. Order matters.

</details>

---

## 4. Your first version, and the hidden bug

This is `Solution1` in `main.cpp`:

```cpp
int currCharCheckIndex = 0;
for (char c : comparedTo) {
  if (c == sequence[currCharCheckIndex]) {
    currCharCheckIndex++;
  }
}
return currCharCheckIndex == sequence.size();
```

It's the right idea (section 2), and it gives the right answer for every
input the problem allows. But look at what happens **after** all of `s`
has been found.

`s = "ab"`, `t = "abx"`:

| `c` | index before | reads           | equal? | index after |
| --- | ------------ | --------------- | ------ | ----------- |
| `a` | 0            | `sequence[0]`   | ✓      | 1           |
| `b` | 1            | `sequence[1]`   | ✓      | 2           |
| `x` | 2            | `sequence[2]` ⚠ | ✗      | 2           |

`"ab"` has characters at positions 0 and 1 only. On the last row the code
reads position **2**, which is one past the end.

### Why it didn't crash

`std::string` secretly keeps a `'\0'` (the "null character", value 0)
right after the last character. Since C++11, reading `s[s.size()]` is
**allowed** and always gives `'\0'`.

So `sequence[2]` returned `'\0'`. `'x'` is not `'\0'`, so nothing matched,
the index stayed at 2, and the answer was correct.

The same thing happens when `s` is empty: every step reads `sequence[0]`,
which is that hidden `'\0'`.

### Why it's still a bug

The code only works because `t` never contains a `'\0'`. The problem
promises that (letters and spaces only), but `std::string` does not:
a string **can** contain `'\0'` characters in the middle.

```cpp
isSubsequence("a", std::string("a\0\0", 3)); // correct answer: true
```

| `c`    | index before | reads                         | what happens                              |
| ------ | ------------ | ----------------------------- | ----------------------------------------- |
| `a`    | 0            | `sequence[0]` = `a`           | match, index → 1                          |
| `'\0'` | 1            | `sequence[1]` = hidden `'\0'` | "match"! index → 2 (fake match)           |
| `'\0'` | 2            | `sequence[2]`                 | **two** past the end: undefined behaviour |

Undefined behaviour means the program may do anything: return garbage,
crash, or appear to work. Here it almost certainly returns `false`
(the index is already 2, and `2 != 1`), which is wrong.

That's the last test in `main.cpp`. You'll watch it fail in section 8.

> **Lesson (same as in strStr): check that an index is valid _before_ you
> use it, not after.**

---

## 5. The fix

This is `Solution2` in `main.cpp`:

```cpp
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
```

What changed: the loop now also stops when `matchedIndex` reaches
`sequence.size()`. That gives you two wins:

1. **`sequence[matchedIndex]` is only read when it's a real character.**
   The hidden `'\0'` is never touched, and neither is anything past it.
2. **It stops early.** Once all of `s` is found, there's no reason to read
   the rest of `t`.

And `s = ""` still works: `0 < 0` is false, so the loop body never runs,
and `0 == 0` returns true.

---

## 6. Three questions people always ask

### Q1. Can this loop run forever?

<details>
<summary>Answer</summary>

No. Look at where `i++` is: in the `for` header, **not** inside the `if`.
It runs on every iteration, match or no match.

`i` goes up by 1 every time, and `comparedTo.size()` never changes, so
after at most `|t|` iterations `i < comparedTo.size()` is false and the
loop ends.

This version **would** loop forever:

```cpp
size_t i = 0;
while (i < comparedTo.size() && matched < sequence.size()) {
  if (comparedTo[i] == sequence[matched]) {
    matched++;
    i++; // BUG: i only moves on a match
  }
}
```

On the first mismatch nothing changes, so the same comparison repeats forever.

A good habit for any loop with pointers: **"Does something move forward on
every single iteration?"** If yes, the loop ends.

(In KMP, `i` sometimes deliberately stays still. That's why the strStr
tutorial needed a separate explanation of why it still finishes.)

</details>

### Q2. When `t[i]` matches, is it always safe to take it right away? What if a later copy would have been better?

<details>
<summary>Answer</summary>

Taking the **earliest** match is never worse. Example: `s = "ab"`, `t = "aba"`.

- Take the `a` at position 0: you can still find `b` at position 1. ✓
- Take the `a` at position 2 instead: nothing is left after it, so `b` can't be found. ✗

The earlier you match a character, the **more** of `t` is left for the rest
of `s`. Matching early can only leave you more options, never fewer.

That's also why the hint says you never need to look backward: anything
to the left is already "used up" or was skipped because it wasn't needed
at that moment.

(This style of algorithm, "always take the best-looking choice right now",
is called **greedy**.)

</details>

### Q3. Why `size_t` here, when strStr used `int`?

<details>
<summary>Answer</summary>

Ask two questions about the variable:

1. **Can it ever go negative, or do I subtract from it?**
2. **What do I compare it against?**

Here, `i` and `matchedIndex` start at 0 and only do `++`. They never go
negative and are never subtracted from. And they're compared against
`.size()`, which is `size_t`. So `size_t` matches perfectly.

In strStr, the code did `lps[j - 1]`. With `size_t`, `0 - 1` wraps around to
a gigantic number instead of `-1`. That's why `int` was the safer choice there.

Your original code mixed the two: `int currCharCheckIndex` compared with
`sequence.size()`. Compile with `-Wall -Wextra` and the compiler warns
about it (`comparison of integer expressions of different signedness`).
**Never compare an `int` directly with a `size_t`.**

</details>

---

## 7. What it costs

| resource | cost                            |
| -------- | ------------------------------- |
| time     | **O(\|t\|)**, one pass over `t` |
| space    | O(1), just two counters         |

For **one** question ("is this `s` inside this `t`?") you can't do better.
When the answer is false, you have to look at all of `t` to be sure the
missing character isn't hiding somewhere.

---

## 8. Test Solution1 and Solution2

All commands below are run from inside this folder **here** and then make sure there's a `build` folder.

```bash
mkdir -p build
```

### How one line picks the solution

`main.cpp` chooses what to test with:

```cpp
typedef Solution1 Solution;
```

Solution1 and Solution2 are asked `isSubsequence(s, t)`. Solution3
(section 9) works differently: it's built from `t` first, then asked
`isSubsequence(s)`. The small `answer<>` function in the harness handles
both shapes, so changing this **one line** is enough to switch between all
three. You don't need to understand `answer<>` in detail to use it.

The suite has **33 tests**.

### Step 1: Solution1, see the bug

Leave the `typedef` as `Solution1`. Compile:

See the main `platforms-and-courses/index-0/1-arrays-and-strings/two-pointers/1-easy/1-backspace-string-compare/README.md` for running and debugging instructions

Expected:

```text
32/33 passed  (1 FAILED)
```

The one failure should be the **last** test, the embedded-`'\0'` one from
section 4. That failure is the point of this step: it's the bug, on purpose.

Any **other** failure here means that test's expected value is wrong,
because Solution1 is correct for every input the problem allows.

Expected warnings:

- `comparison of integer expressions of different signedness` in Solution1 (Q3).
- `unused variable` warnings in `Solution3` (`c`, `prev`, `list`). They go
  away when you fill in its TODOs in section 9.

The sanitizers probably **won't** report the bad read: `"a"` is so short
that it's stored inside the string object itself, so reading `sequence[2]`
stays inside memory the program owns. If you want to see the bad read get
caught, build with the standard library's own bounds checks:

See the main `platforms-and-courses/index-0/1-arrays-and-strings/two-pointers/1-easy/1-backspace-string-compare/README.md` for running and debugging instructions

The program should stop on the last test with an assertion message
mentioning `__pos <= size()`. That's `operator[]` catching the read two
past the end. (This flag is for g++'s standard library.)

### Step 2: Solution2, the fix

Change one line:

```cpp
typedef Solution2 Solution;
```

Compile and run with the same two commands as step 1. Expected:

```text
33/33 passed
```

The embedded-`'\0'` test now passes, because Solution2 never reads
`sequence[matchedIndex]` once `matchedIndex == sequence.size()`.

---

## 9. Follow-up: what if there are a billion `s` strings?

LeetCode's follow-up:

> Suppose there are lots of incoming `s` (s1, s2, …, sk with k ≥ 10⁹),
> and you check each one against the **same** `t`. How would you change your code?

With `Solution2`, every query walks through `t` again: up to 10,000 steps,
times a billion queries. The `t` finger keeps re-reading the same string.

Same cure as KMP: **`t` never changes, so do some work on it once, save
the result in a table, and answer every query from the table.**

### The table

For every character, write down the positions where it appears in `t`,
in increasing order. For `t = "abcabc"`:

```
index:  0  1  2  3  4  5
t:      a  b  c  a  b  c

positions['a'] = [0, 3]
positions['b'] = [1, 4]
positions['c'] = [2, 5]
```

### Answering a query

Keep `prev`, the position in `t` of the character you matched last.
Start with `prev = -1`, meaning "before the start of `t`".

For each character of `s`, ask: **"In this character's list, what's the
first position bigger than `prev`?"**

- If there is one, that's your match (earliest possible, see Q2). Set `prev` to it.
- If there isn't, the answer is **false**.

If you get through all of `s`, the answer is **true**.

This is the same greedy algorithm as before. Instead of walking over every
character of `t` one by one, you **jump** straight to the next useful one.

### Finding "first position bigger than `prev`" fast

The list is sorted, so you don't scan it. You use **binary search**: look
at the middle, throw away the half that can't contain the answer, and repeat.
A list of 10,000 needs only about 14 looks.

C++ already has this: `std::upper_bound(list.begin(), list.end(), prev)`
returns the **first element strictly greater than `prev`**, or `list.end()`
if there is none.

```
list = [0, 3]
upper_bound(..., -1) -> points at 0
upper_bound(...,  2) -> points at 3
upper_bound(...,  3) -> list.end()   (nothing bigger than 3)
```

### Trace (worked for you): `t = "abcabc"`

| s     | char | list     | first > prev | prev after   |
| ----- | ---- | -------- | ------------ | ------------ |
| `cab` | `c`  | `[2, 5]` | 2            | 2            |
|       | `a`  | `[0, 3]` | 3            | 3            |
|       | `b`  | `[1, 4]` | 4            | 4 → **true** |
| `cba` | `c`  | `[2, 5]` | 2            | 2            |
|       | `b`  | `[1, 4]` | 4            | 4            |
|       | `a`  | `[0, 3]` | none         | → **false**  |

Both traces are tests in `main.cpp`, together with `"bb"` (true) and
`"bbb"` (false) against the same `t`.

### Your turn: fill in the TODOs

This is `Solution3` in `main.cpp`:

```cpp
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
  std::vector<std::vector<int>> positions; // positions[c] = where c appears in t
};
```

Answer these before writing code:

**1. Why 256 lists and not 26?**

<details>
<summary>Answer</summary>

The usual trick `c - 'a'` only works for lowercase letters. The problem
also allows **spaces**: `' ' - 'a'` is `-65`, which is out of bounds.

Instead, use the character's own number as the index. A byte has 256
possible values, so 256 lists cover every character: letters, spaces,
even `'\0'`.

</details>

**2. Why the `unsigned char` cast?**

<details>
<summary>Answer</summary>

On many compilers, plain `char` is **signed**, so some characters have
negative values. A negative index is out of bounds. Casting to
`unsigned char` turns every character into a number from 0 to 255.

</details>

**3. Why `upper_bound` ("strictly greater") and not `lower_bound` ("greater or equal")?**

<details>
<summary>Answer</summary>

Try `s = "aa"`, `t = "a"`. The list for `a` is `[0]`.

- First `a`: first position > -1 is 0. `prev = 0`.
- Second `a`:
  - `upper_bound`: first position **> 0**? None, so **false**. ✓
  - `lower_bound`: first position **>= 0** is 0 again, so **true**. ✗

`lower_bound` would use the same character of `t` twice, which isn't allowed.
The test `check("aa", "a", false)` catches this.

</details>

**4. Why is `prev` an `int`?**

<details>
<summary>Answer</summary>

It starts at `-1`. Q3's rule says: if it can be negative, use a signed
type. `size_t prev = -1` would silently become a gigantic number, and
`upper_bound` would never find anything, so every query would return false.

</details>

**5. When is `Solution3` worse than `Solution2`?**

<details>
<summary>Answer</summary>

When there's only **one** query. Building the table already reads all of `t`,
which is everything `Solution2` does, and then the query runs on top of that.
The table only pays off when it's reused for many queries.

(The test harness builds a fresh `Solution3` for every test, because every
test uses a different `t`. That's fine for checking correctness, but it's
not how you'd use it for speed.)

</details>

<details>
<summary>Full solution (only after you've tried)</summary>

```cpp
  explicit Solution3(const std::string &t) : positions(256) {
    for (int k = 0; k < static_cast<int>(t.size()); k++) {
      const unsigned char c = static_cast<unsigned char>(t[k]);
      positions[c].push_back(k);
    }
  }

  bool isSubsequence(const std::string &s) const {
    int prev = -1;

    for (char ch : s) {
      const unsigned char c = static_cast<unsigned char>(ch);
      const std::vector<int> &list = positions[c];

      auto it = std::upper_bound(list.begin(), list.end(), prev);
      if (it == list.end()) {
        return false;
      }
      prev = *it;
    }

    return true;
  }
```

`push_back` adds `k` to the end of the list. Because `k` only goes up,
every list is automatically sorted, which is exactly what `upper_bound` needs.

</details>

### Cost

| approach                          | build table | per query                | extra memory |
| --------------------------------- | ----------- | ------------------------ | ------------ |
| Solution2 (two fingers)           | none        | O(\|t\|)                 | O(1)         |
| Solution3 (lists + binary search) | O(\|t\|)    | **O(\|s\| · log \|t\|)** | O(\|t\|)     |

With |t| = 10,000 and |s| = 100: about 10,000 steps per query for
`Solution2`, about 100 × 14 = 1,400 for `Solution3`. Over a billion queries,
that's the difference that matters.

Same trade as KMP: spend memory once, save time on every lookup.

---

## 10. Test Solution3

### Step 3: Solution3

Fill in the two TODOs, then change one line:

```cpp
typedef Solution3 Solution;
```

Compile and run with the same two commands as step 1. Expected:

```text
33/33 passed
```

If something goes wrong, the symptom points at the bug:

| what you see                                              | what's probably wrong                                                  |
| --------------------------------------------------------- | ---------------------------------------------------------------------- |
| every test that expects `false` fails                     | the TODOs are still empty, so it always returns `true`                 |
| `("aa", "a")` fails                                       | `lower_bound` instead of `upper_bound`                                 |
| `("cba", "abcabc")` fails                                 | `prev` isn't updated after a match                                     |
| every test that expects `true` with a non-empty `s` fails | `prev` is `size_t`, or the table was never filled                      |
| crash or sanitizer report                                 | `*it` read when `it == list.end()`, or a plain `char` used as an index |

### Step 4 (optional): referee mode

Once Solution3 passes on its own, make every test also run Solution2 and
print a warning if the two ever disagree. Replace `check()` with:

```cpp
void check(const std::string &s, const std::string &t, bool expected) {
  ++testsRun;

  const bool actual = answer<Solution>(s, t);
  const bool refResult = answer<Solution2>(s, t);

  const bool ok = (actual == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "isSubsequence(\""
            << preview(s) << "\", \"" << preview(t) << "\")"
            << "  expected=" << std::boolalpha << expected
            << "  actual=" << actual << '\n';

  if (refResult != actual) {
    std::cout << "          MISMATCH: Solution2 said " << std::boolalpha
              << refResult << '\n';
  }
}
```

The referee is **Solution2**, not Solution1: Solution1 has the bug from
section 4. Keep the `expected` values too: two solutions agreeing doesn't
prove both are right.

The `typedef` still works in referee mode. Try `typedef Solution1 Solution;`
again: the embedded-`'\0'` test now prints both a FAIL and a MISMATCH line.

---

## Checklist

- [ ] I can explain the two-finger idea without looking.
- [ ] I traced `"aec"` / `"abcde"` and got false.
- [ ] I can explain why the original read `sequence[2]` on `"ab"` / `"abx"`, and why that didn't crash.
- [ ] Step 1 printed `32/33 passed  (1 FAILED)`, and the failure was the embedded-`'\0'` test.
- [ ] Step 2 printed `33/33 passed` with `typedef Solution2 Solution;`.
- [ ] I filled in the `Solution3` TODOs without opening the solution.
- [ ] Step 3 printed `33/33 passed` with `typedef Solution3 Solution;`.
- [ ] (Optional) referee mode shows no MISMATCH lines for Solution3.

Paste your output, or tell me the first step where you got stuck.
