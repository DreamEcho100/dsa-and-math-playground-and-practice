You're given two strings that may contain the character '#', which means "backspace": it deletes the character before it (if any).

Simulate typing both strings, and decide whether the final results are equal.

```
Input: s = "ab#c", t = "ad#c"
Output: true
both type out to "ac"
```

```
Input: s = "ab##", t = "c#d#"
Output: true
both type out to an empty string
```

```
Input: s = "a#c", t = "b"
Output: false
s types out to "c", t types out to "b"
```

```
Constraints
0 <= s.length, t.length <= 200
s and t consist only of lowercase English letters and the character '#'
```

<details>
<summary>💡 Hints</summary>
the stack approach works and is already efficient - can you get the same result without building an extra data structure?

---

<think>They're explicitly asking to be taught this time, so I should shift from pure Socratic questioning toward actually explaining the reasoning behind the two-pointer approach, while still leaving room for them to assemble the final code themselves with solid scaffolding.

The core insight is that scanning right-to-left, whenever you hit a character, you already know every '#' that could delete it since those are all to its right, so you can resolve its fate immediately. I'm sketching a helper that walks backward through a string tracking pending deletions to find the next surviving character, then a comparison loop that uses this helper from both ends to check whether two strings match after applying backspaces.

I'm thinking the best teaching approach is to fully explain the helper's logic, then hand them the main loop with guiding questions about the edge cases—like why the outer condition needs "or" not "and", and why both pointers decrement together—so they derive it themselves before I verify with a trace.

</think>
Right — but let's be precise about what you'd actually be winning, because this trips people up in interviews.

## What the optimization buys you

Your current solution:

| resource | cost         |
| -------- | ------------ |
| time     | O(n + m)     |
| space    | **O(n + m)** |

The two-pointer version:

| resource | cost     |
| -------- | -------- |
| time     | O(n + m) |
| space    | **O(1)** |

**Time doesn't improve.** Two sequential loops is `O(n) + O(m)`, and merging them into one interleaved loop is still `O(n + m)`. You already knew that — you said it yourself when you dropped the constant.

What changes is **space**. You stop materialising the two result strings. That's the prize.

(Secondary bonus: you can bail out early. `"axxxxxxxx..."` vs `"bxxxxxxxx..."` differ at the last character, so you return `false` after one comparison instead of processing 400 characters. Doesn't change the worst case, but it's real.)

## Why right-to-left — the whole idea in one picture

You already named it: _a character's fate is determined by what lies to its right._

Make that concrete. You're standing on `b` in `"ab#c"`:

```
a  b  #  c
   ^
```

**Scanning left-to-right**, you arrive at `b` having seen only `a`. Does `b` survive? You have no idea — you'd have to read ahead to find out. That's why the naive solution needs a buffer: it _provisionally_ appends `b`, then retracts it later. The buffer exists to hold decisions you haven't finished making.

**Scanning right-to-left**, you arrive at `b` having already seen `c` and `#`. Every character that could possibly delete `b` has already passed under your eye. You can decide _right now_, permanently, and never revisit it.

No revisiting means no buffer. That's the whole trick.

## Turning "I owe deletions" into a variable

Walking leftward, you need to remember how many pending backspaces you're carrying. Call it `skips`. At each position exactly one of three things is true:

| you see                | meaning                                        | do                           |
| ---------------------- | ---------------------------------------------- | ---------------------------- |
| `'#'`                  | another backspace                              | `skips++`, keep walking left |
| a letter, `skips > 0`  | this letter is consumed by a pending backspace | `skips--`, keep walking left |
| a letter, `skips == 0` | **this letter survives**                       | stop                         |

Notice the symmetry: `#` is a deposit, a letter with debt outstanding is a withdrawal. You're running a tiny counter instead of a tiny stack — and _that_ downgrade from O(n) structure to O(1) counter is where the space saving literally comes from.

```cpp
// Walk left from i until we land on a character that survives.
// Returns that index, or -1 if everything from i leftward is erased.
int nextValidIndex(const string &str, int i) {
  int skips = 0;
  while (i >= 0) {
    if (str[i] == '#') {
      skips++;
      i--;
    } else if (skips > 0) {
      skips--;
      i--;
    } else {
      break; // str[i] survives
    }
  }
  return i;
}
```

**Why `int` and not `size_t`?** The `-1` sentinel. `size_t` is unsigned — `size_t(0) - 1` is a colossal positive number, your `i >= 0` is _always_ true, and you walk off the front of the string. You met this exact bug earlier with `fmax`. Same lesson, new costume: **unsigned types cannot represent "one before the beginning."**

## Your turn: the driving loop

The helper hands you _the next surviving character_. Now compare the two streams.

```cpp
bool backspaceCompareFast(const string &str1, const string &str2) {
  int i = static_cast<int>(str1.size()) - 1;
  int j = static_cast<int>(str2.size()) - 1;

  while (i >= 0 || j >= 0) {
    i = nextValidIndex(str1, i);
    j = nextValidIndex(str2, j);

    // CASE 1: both found a surviving character
    //         -> compare them. differ?  ______
    //
    // CASE 2: exactly one found a character, the other hit -1
    //         -> what does that say about the two results?  ______
    //
    // CASE 3: both hit -1
    //         -> both exhausted together. what should the loop do?

    // then: step past the characters you just compared
    i--;
    j--;
  }

  return true;
}
```

Four questions to answer before you write a line:

1. **Case 2** — one string still has surviving characters and the other is spent. Is that ever `true`? Think about `"abc"` vs `"ab"`.
2. **Why `||` and not `&&`** in the loop condition? Hand-trace `"abc"` vs `"ab"` with `&&` and watch what it wrongly concludes.
3. **Why `i--; j--;` at the bottom?** Delete those two lines mentally and trace `"ab"` vs `"ab"`. What does `nextValidIndex` return next iteration? What kind of loop do you now have?
4. **Case 3** — if both are `-1`, do you need an explicit branch, or does the loop condition already handle it?

## Verify like this

Trace `"ab##"` vs `"c#d#"` on paper — fill every cell before compiling:

| iter | i   | j   | after `nextValidIndex` (i, j) | chars | verdict |
| ---- | --- | --- | ----------------------------- | ----- | ------- |
| 1    | 3   | 3   | ?                             | ?     | ?       |
| 2    | ?   | ?   | ?                             | ?     | ?       |

Then the payoff you built the harness for — **keep both implementations** and point `check` at the new one:

```cpp
const bool actual = backspaceCompareFast(s, t);
```

Same 23 tests, one line changed. If the fast version is genuinely equivalent, it goes green instantly. If it doesn't, the failing case names itself.

Even better, assert they agree — a poor man's differential test:

```cpp
const bool slow = backspaceCompare(s, t);
const bool fast = backspaceCompareFast(s, t);
if (slow != fast) {
  cout << "  MISMATCH between implementations!\n";
}
```

This is the standard move when optimizing: keep the obviously-correct version as the oracle, and make the clever version prove itself against it.

Write the three cases, run the suite, and show me what you get.

</details>

Original source: [LeetCode](https://leetcode.com/problems/backspace-string-compare/)
