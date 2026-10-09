# strStr, part 2: from brute force to KMP

Your brute force (`Solution1`) is correct. This tutorial makes it faster.
Go slowly: each section relies on the one before it.

---

## 1. Where you are now

Your brute force works like this: for every starting position `i` in the
haystack, compare the needle character by character. On a mismatch, move the
start to `i + 1` and begin again from the first character of the needle.

| resource | cost         |
| -------- | ------------ |
| time     | **O(n · m)** |
| space    | O(1)         |

(`n` = haystack length, `m` = needle length.)

Most of the time this is fine. But your harness has a case that hurts:

```cpp
check(std::string(9999, 'a') + "b", std::string(5000, 'a') + "b", 4999);
```

At each of the first ~5000 start positions, brute force matches 5000 `a`'s and
then fails on the `b`. That's about **25 million** comparisons.
KMP solves the same case in a few **tens of thousands**.

---

## 2. What brute force wastes

Take `haystack = "sabvsad"`, `needle = "sad"`.

Start at `i = 0`:

```
index:     0  1  2  3  4  5  6
haystack:  s  a  b  v  s  a  d
needle:    s  a  d
           ✓  ✓  ✗        'd' != 'b'
```

Brute force now moves the start to `i = 1` and goes back to `needle[0]`:

```
index:     0  1  2  3  4  5  6
haystack:  s  a  b  v  s  a  d
needle:       s  a  d
              ✗           's' != 'a'
```

Look at what just happened. `haystack[1]` (the `a`) was **already read**
in the first attempt. Brute force reads it again. It keeps doing this: after
every failed attempt it **goes back** in the haystack.

With `"sad"` the waste is small. With 5000 `a`'s, every attempt re-reads up
to 5000 characters, and that's where the 25 million comes from.

> **KMP's promise: the haystack position never moves backwards.**
> Every haystack character is checked a small, fixed number of times.

---

## 3. The trick, in one picture

`"sad"` is too simple to show the trick, because it has no repeated parts.
Use this example instead:

```
haystack = "abababc"
needle   = "ababc"
```

Start matching:

```
index:     0  1  2  3  4  5  6
haystack:  a  b  a  b  a  b  c
needle:    a  b  a  b  c
           ✓  ✓  ✓  ✓  ✗      'c' != 'a'
```

We matched `"abab"`, then failed. Brute force would restart at `i = 1`.

But think about what we **know**: the last two characters we read are
`"ab"`, and `"ab"` is also how the needle **begins**. So slide the needle
forward and line its start up with that `"ab"`:

```
index:     0  1  2  3  4  5  6
haystack:  a  b  a  b  a  b  c
needle:          a  b  a  b  c
                 ✓  ✓            already known to match, no need to check
                       ?         continue checking HERE, at index 4
```

We don't go back. We stay at haystack index 4 and simply say:
"2 characters are already matched; compare `needle[2]` next."

**That is the whole idea of KMP.** On a mismatch, ask:

> "Of what I've matched so far, how much of its **end** is also the
> **beginning** of the needle?"

Keep that many characters as matched and continue, without going back.

The rest of this tutorial is just:

1. a table that answers that question for every possible situation, and
2. a loop that uses the table.

---

## 4. Two words you need: prefix and suffix

- A **prefix** is a piece taken from the **start** of a string.
- A **suffix** is a piece taken from the **end** of a string.

For `"abab"`:

| prefixes | suffixes |
| -------- | -------- |
| `a`      | `b`      |
| `ab`     | `ab`     |
| `aba`    | `bab`    |

We only count **proper** prefixes and suffixes. "Proper" means shorter
than the whole string. The whole string `"abab"` is trivially both its own
prefix and suffix, which tells us nothing.

Is any piece in **both** columns? Yes: `ab`. Its length is **2**.
That 2 is exactly the "keep 2 characters" from the picture in section 3.

---

## 5. The LPS table

**LPS** = **L**ongest proper **P**refix that is also a **S**uffix.

For each position `k` in the needle, look at the piece `needle[0..k]` and
write down the length of the longest proper prefix that is also a suffix.

For `needle = "ababc"`:

| k   | piece `needle[0..k]` | longest piece that is both start and end | `lps[k]` |
| --- | -------------------- | ---------------------------------------- | -------- |
| 0   | `a`                  | none (nothing shorter than 1 char)       | 0        |
| 1   | `ab`                 | none (`a` ≠ `b`)                         | 0        |
| 2   | `aba`                | `a`                                      | 1        |
| 3   | `abab`               | `ab`                                     | 2        |
| 4   | `ababc`              | none (ends in `c`, starts with `a`)      | 0        |

```
lps = [0, 0, 1, 2, 0]
```

**How to read it:** "if I have matched `j` characters and then fail, I can
keep `lps[j - 1]` of them."

In section 3 we had matched `j = 4` characters (`"abab"`) and failed.
`lps[3] = 2`, so we keep 2. That matches the picture.

**Why `j - 1`?** If `j` characters are matched, they are `needle[0]`
through `needle[j - 1]`. The table entry for that piece is at index `j - 1`.

### Practice

Fill in the table for `needle = "aabaa"` on paper first, then check.

<details>
<summary>Answer</summary>

| k   | piece   | longest piece that is both start and end | `lps[k]` |
| --- | ------- | ---------------------------------------- | -------- |
| 0   | `a`     | none                                     | 0        |
| 1   | `aa`    | `a`                                      | 1        |
| 2   | `aab`   | none (ends in `b`)                       | 0        |
| 3   | `aaba`  | `a`                                      | 1        |
| 4   | `aabaa` | `aa`                                     | 2        |

`lps = [0, 1, 0, 1, 2]`

</details>

And for `"sad"`? Every entry is 0, because no letter repeats. That's why it
couldn't demonstrate the trick: on a failure you always keep 0 characters.

---

## 6. The search loop

Assume for now that someone hands you the `lps` table. (Section 7 shows how
to build it.)

Two variables:

- `i` is the position in the haystack. **It never decreases.**
- `j` is how many needle characters are matched right now.
  This also means `needle[j]` is the next needle character to compare.

Each time around the loop, compare `haystack[i]` with `needle[j]`. Exactly
one of three things happens:

| case | situation              | what to do                                                |
| ---- | ---------------------- | --------------------------------------------------------- |
| 1    | they match             | `i++` and `j++`. If `j == m`, the whole needle matched.   |
| 2    | mismatch, and `j > 0`  | keep what you can: `j = lps[j - 1]`. **Don't touch `i`.** |
| 3    | mismatch, and `j == 0` | nothing to keep. `i++`.                                   |

Why don't we touch `i` in case 2? Because `haystack[i]` hasn't been
compared successfully yet. We only changed **which needle character** it
will be compared to. The next loop iteration compares it again, against the
new `needle[j]`. If that fails too, case 2 runs again with an even smaller
`j`, until something matches or `j` reaches 0.

### Your turn: fill in the TODOs

```cpp
#include <string>
#include <vector>

class Solution2 {
public:
  int strStr(const std::string &haystack, const std::string &needle) {
    const int n = static_cast<int>(haystack.size());
    const int m = static_cast<int>(needle.size());

    if (m == 0) {
      return 0;
    }

    const std::vector<int> lps = buildLps(needle);

    int i = 0; // position in haystack, only moves forward
    int j = 0; // how many needle characters are matched right now

    while (i < n) {
      if (haystack[i] == needle[j]) {
        // TODO case 1: advance both.
        //      If the whole needle is now matched, return where it STARTED.
      } else if (j > 0) {
        // TODO case 2: keep lps[j - 1] characters. Leave i alone.
      } else {
        // TODO case 3: nothing matched, move on in the haystack.
      }
    }

    return -1;
  }
};
```

One question before you write case 1: **where did the match start?**

<details>
<summary>Answer</summary>

Right after `i++`, `i` points **one past** the last matched character.
The needle is `m` characters long, so it started at `i - m`.

Check it with `haystack = "abc"`, `needle = "abc"`: after the third match,
`i = 3`, and `3 - 3 = 0`. ✓

</details>

<details>
<summary>Full solution (only after you've tried)</summary>

```cpp
      if (haystack[i] == needle[j]) {
        i++;
        j++;
        if (j == m) {
          return i - m;
        }
      } else if (j > 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
```

</details>

### Trace 1 (worked for you): `haystack = "abababc"`, `needle = "ababc"`, `lps = [0,0,1,2,0]`

| step | i   | j   | compare      | case | do this                 | i, j after |
| ---- | --- | --- | ------------ | ---- | ----------------------- | ---------- |
| 1    | 0   | 0   | `a` vs `a` ✓ | 1    | advance both            | 1, 1       |
| 2    | 1   | 1   | `b` vs `b` ✓ | 1    | advance both            | 2, 2       |
| 3    | 2   | 2   | `a` vs `a` ✓ | 1    | advance both            | 3, 3       |
| 4    | 3   | 3   | `b` vs `b` ✓ | 1    | advance both            | 4, 4       |
| 5    | 4   | 4   | `a` vs `c` ✗ | 2    | `j = lps[3] = 2`        | **4**, 2   |
| 6    | 4   | 2   | `a` vs `a` ✓ | 1    | advance both            | 5, 3       |
| 7    | 5   | 3   | `b` vs `b` ✓ | 1    | advance both            | 6, 4       |
| 8    | 6   | 4   | `c` vs `c` ✓ | 1    | advance both → `j == 5` | 7, 5       |

Return `i - m = 7 - 5 = 2`. Check: `haystack[2..6]` is `"ababc"`. ✓

Look at step 5: `i` stayed at 4. The haystack never went backwards.

### Trace 2 (your turn): `haystack = "sabvsad"`, `needle = "sad"`, `lps = [0,0,0]`

Do it on paper, then open the answer.

<details>
<summary>Answer</summary>

| step | i   | j   | compare      | case | do this                 | i, j after |
| ---- | --- | --- | ------------ | ---- | ----------------------- | ---------- |
| 1    | 0   | 0   | `s` vs `s` ✓ | 1    | advance both            | 1, 1       |
| 2    | 1   | 1   | `a` vs `a` ✓ | 1    | advance both            | 2, 2       |
| 3    | 2   | 2   | `b` vs `d` ✗ | 2    | `j = lps[1] = 0`        | 2, 0       |
| 4    | 2   | 0   | `b` vs `s` ✗ | 3    | `i++`                   | 3, 0       |
| 5    | 3   | 0   | `v` vs `s` ✗ | 3    | `i++`                   | 4, 0       |
| 6    | 4   | 0   | `s` vs `s` ✓ | 1    | advance both            | 5, 1       |
| 7    | 5   | 1   | `a` vs `a` ✓ | 1    | advance both            | 6, 2       |
| 8    | 6   | 2   | `d` vs `d` ✓ | 1    | advance both → `j == 3` | 7, 3       |

Return `i - m = 7 - 3 = 4`. Check: `haystack[4..6]` is `"sad"`. ✓

Notice that brute force went back to `i = 1` after step 3. KMP never does.

</details>

---

## 7. Building the LPS table in code

By hand, you find each entry by eye. In code, there's a neat shortcut:
**building the table is the same matching game, played by the needle
against itself.**

- `i` walks the needle from position 1. (`lps[0]` is always 0.)
- `len` is how many characters of the "start of the needle" currently match
  the characters ending at `i`.

The three cases are the same as in the search:

```cpp
#include <string>
#include <vector>

static std::vector<int> buildLps(const std::string &needle) {
  const int m = static_cast<int>(needle.size());
  std::vector<int> lps(m, 0);

  int len = 0; // length of the current prefix-that-is-also-a-suffix
  int i = 1;   // lps[0] is always 0, so start at 1

  while (i < m) {
    if (needle[i] == needle[len]) {
      // case 1: the match grows by one
      len++;
      lps[i] = len;
      i++;
    } else if (len > 0) {
      // case 2: fall back to a shorter match, and DON'T move i
      len = lps[len - 1];
    } else {
      // case 3: nothing matches here
      lps[i] = 0;
      i++;
    }
  }

  return lps;
}
```

Trace for `"ababc"`:

| i   | len | compare `needle[i]` vs `needle[len]` | case | result              |
| --- | --- | ------------------------------------ | ---- | ------------------- |
| 1   | 0   | `b` vs `a` ✗                         | 3    | `lps[1] = 0`, i → 2 |
| 2   | 0   | `a` vs `a` ✓                         | 1    | `lps[2] = 1`, i → 3 |
| 3   | 1   | `b` vs `b` ✓                         | 1    | `lps[3] = 2`, i → 4 |
| 4   | 2   | `c` vs `a` ✗                         | 2    | `len = lps[1] = 0`  |
| 4   | 0   | `c` vs `a` ✗                         | 3    | `lps[4] = 0`, i → 5 |

Result: `[0, 0, 1, 2, 0]`, the same as the hand-made table. ✓

> It's normal if this function feels like magic at first. If the hand-made
> tables make sense to you, you understand KMP. The code is just a fast way
> to produce them. Run it on `"aabaa"` and compare it with your practice answer.

---

## 8. Why this is fast

- In the search loop, `i` only moves forward, so cases 1 and 3 together
  happen at most `n` times.
- Case 2 makes `j` smaller. But `j` only grows in case 1, one step at a time.
  You can't go down more steps than you went up. So case 2 also happens at
  most `n` times in total.
- Total: at most about `2n` loop iterations. Building the table is the same
  argument on the needle: about `2m`.

| resource | brute force | KMP          |
| -------- | ----------- | ------------ |
| time     | O(n · m)    | **O(n + m)** |
| space    | O(1)        | O(m)         |

You spend a little memory (the table) to save a lot of time. That trade,
memory for speed, comes up constantly.

---

## 9. Why `int` here and not `size_t`?

`size_t` is unsigned: it can't hold negative numbers. If you compute `0 - 1`
with `size_t`, it doesn't become `-1`. It **wraps around** to an enormous
number, about 18 quintillion.

KMP does subtractions: `lps[j - 1]`, `lps[len - 1]`, `i - m`. If a bug ever let
`j == 0` reach `lps[j - 1]`:

- with `size_t`, you'd index position 18 quintillion. That's undefined
  behaviour, and it might not crash right away, so the bug can hide.
- with `int`, you get `lps[-1]`. That's still a bug, but `-1` is an obviously
  wrong value that you'll spot in a debugger, and sanitizers flag it.

**Rule of thumb:** if a variable can go negative or you subtract from it, use
`int`, and convert `.size()` once at the top:
`const int n = static_cast<int>(haystack.size());`.
If it only ever counts up, `size_t` is fine. Just don't compare an `int`
directly with a `size_t`.

---

## 10. Test it: let the slow version check the fast one

`Solution1` is slow but proven correct. Use it as a **referee**: run both
solutions on every test and complain if they disagree.

Replace `check()` with:

```cpp
void check(const std::string &haystack, const std::string &needle,
           int expected) {
  ++testsRun;

  Solution1 slow;
  Solution2 fast;
  const int slowResult = slow.strStr(haystack, needle);
  const int fastResult = fast.strStr(haystack, needle);

  const bool ok = (fastResult == expected);
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "strStr(\""
            << preview(haystack) << "\", \"" << preview(needle) << "\")"
            << "  expected=" << expected << "  actual=" << fastResult << '\n';

  if (slowResult != fastResult) {
    std::cout << "          MISMATCH: brute force said " << slowResult << '\n';
  }
}
```

Add these cases. They exercise case 2 (the fallback), which brute force never
needed:

```cpp
  // KMP fallback: needles with repeated parts
  check("sabvsad", "sad", 4);         // trace 2
  check("abababc", "ababc", 2);       // trace 1: one fallback
  check("aabaxaabaa", "aabaa", 5);    // fallback chain: j goes 4 -> 1 -> 0
  check("ababababca", "ababca", 4);
  check("aaaaab", "aaab", 2);
  check("abababab", "abab", 0);
  check("mississippi", "issipi", -1); // near miss
```

Remember to add `#include <vector>` at the top of the file.

See the main `platforms-and-courses/index-0/1-arrays-and-strings/two-pointers/1-easy/1-backspace-string-compare/README.md` for running and debugging instructions

---

## Checklist

- [ ] I can fill in an LPS table by hand (`"ababc"`, `"aabaa"`).
- [ ] I can explain in one sentence why `i` never goes backwards.
- [ ] I filled in the three TODOs without opening the solution.
- [ ] I traced `"sabvsad"` / `"sad"` and got `4`.
- [ ] All tests pass and there are no MISMATCH lines.

Show me your output, or the first step where you got stuck.
