# Backspace String Compare

You're given two strings. `#` means backspace: delete the character
immediately before it, **if there is one**. Do both strings produce the
same final text?

```text
s = "ab#c" → "ac"
t = "ad#c" → "ac"
answer: true
```

```text
s = "ab##" → ""
t = "c#d#" → ""
answer: true
```

```text
s = "a#c" → "c"
t = "b"   → "b"
answer: false
```

Constraints: each input has at most 200 characters, consisting of lowercase
English letters and `#`.

Source: [LeetCode — Backspace String Compare](https://leetcode.com/problems/backspace-string-compare/).

`main.cpp` contains two solutions and a shared test suite. Start with
`Solution1`, then change one `typedef` to test `Solution2`.

---

## 1. First, make the rules concrete

Type `"a##b"` from left to right:

| Character typed | Text afterward | Why                        |
| --------------- | -------------- | -------------------------- |
| `a`             | `"a"`          | Add `a`.                   |
| `#`             | `""`           | Delete `a`.                |
| `#`             | `""`           | Nothing remains to delete. |
| `b`             | `"b"`          | Add `b`.                   |

The result is `"b"`. A backspace does **not** delete a character that
will be typed later.

Try `"ab#c#d"` yourself before opening the answer.

<details>
<summary>Answer</summary>

`a` → `"a"` → `b` → `"ab"` → `#` → `"a"` → `c` → `"ac"` →
`#` → `"a"` → `d` → `"ad"`.

</details>

---

## 2. Solution1: actually build the typed text

For each input string, keep a result string:

- Letter: add it to the end.
- `#`: remove the last letter **if the result isn't empty**.

Then compare the two results. That's `Solution1::typedText()` in `main.cpp`.

This is a good first solution. It's easy to reason about and gives us a
reference against which to test the more subtle solution.

If the input lengths are `n` and `m`:

| Resource    | Cost     | Why                                              |
| ----------- | -------- | ------------------------------------------------ |
| Time        | O(n + m) | Visit every input character at most once.        |
| Extra space | O(n + m) | The two result strings may hold all the letters. |

**Question:** Why check `result.empty()` before `result.pop_back()`?

<details>
<summary>Answer</summary>

For `"###"`, every backspace finds an empty result. Calling `pop_back()`
on an empty string is invalid. Ignore a backspace when there's nothing
to delete.

</details>

---

## 3. What are we trying to improve?

`Solution1` is already linear-time. We're **not** trying to turn O(n + m)
time into something faster.

Instead, can we compare the results **without storing either result**?
That would reduce extra space from O(n + m) to O(1).

The useful observation is:

> A character is deleted by a `#` somewhere **to its right**.

So if we walk **right to left**, by the time we reach a letter, we've
already encountered every backspace to its right that could delete it.

---

## 4. Find one surviving character from the right

`Solution2::nextValidIndex(text, i)` starts at index `i` and walks left.
It keeps `skips`: the number of letters that backspaces still need to
delete.

| What we see               | Action                                                          |
| ------------------------- | --------------------------------------------------------------- |
| `#`                       | Add one pending deletion (`++skips`); move left.                |
| Letter, with `skips > 0`  | This letter is deleted; use one pending deletion and move left. |
| Letter, with `skips == 0` | This letter survives; return its index.                         |

If we walk past index 0, return `-1`: there's no surviving letter left.

### Trace: `"ab#c"`

```text
index:  0  1  2  3
text:   a  b  #  c
```

Starting at index 3:

- `c` survives, so return **3**.
- The caller compares `c`, then resumes at index **2**.
- Index 2 is `#`: `skips = 1`, move left.
- Index 1 is `b`: that backspace deletes it, `skips = 0`, move left.
- Index 0 is `a`: it survives, so return **0**.

We found the surviving text **backwards**: first `c`, then `a`.
We didn't build `"ac"`.

### Your turn: trace `"a##b"`

Starting at its last index, which surviving character do we find? What
does the next call return?

<details>
<summary>Answer</summary>

The first call returns the index of `b`: **3**.

The next call starts at index 2. It sees two `#` characters, so
`skips` becomes 2. The `a` uses up one deletion. We run past the
beginning and return **-1**. There is no other surviving character.

</details>

---

## 5. Why `int` for the indices?

Walking left from index 0 gives **-1**, meaning "we're done."
`int` can represent -1.

`size_t` is unsigned. With a `size_t` index, subtracting one from zero
wraps around to a huge positive number. A condition like `i >= 0` would
then _always_ be true; it couldn't tell us when we've passed the start.

Notice how `main.cpp` computes the starting index:

```cpp
int i = static_cast<int>(s.size()) - 1;
```

The conversion happens **before** subtraction. If `s` is empty, this
gives `-1` directly. The problem's maximum length is 200, so converting
its length to `int` is safe here.

---

## 6. Compare the two streams of surviving characters

`Solution2::backspaceCompare()` keeps one index per string: `i` and `j`.

Each round:

1. Find the next surviving character in each string.
2. If **both** strings are exhausted, return `true`.
3. If **only one** is exhausted, return `false`.
4. Otherwise compare the characters. If they differ, return `false`.
5. Move both indices left of the characters just compared.

For example, `"ab#c"` and `"ad#c"` each yield `c`, then `a`, then
nothing. Their surviving characters agree, so the answer is `true`.

### Why does the outer loop use `||`, not `&&`?

```cpp
while (i >= 0 || j >= 0)
```

<details>
<summary>Answer</summary>

Consider `"ab"` versus `"b"`. The typed results are `"ab"` and `"b"`,
so the answer must be `false`.

| round | i, j before | surviving chars | result                | i, j after |
| ----- | ----------- | --------------- | --------------------- | ---------- |
| 1     | 1, 0        | `b` vs `b`      | equal, move both left | 0, -1      |

Now `s` still has `a` at index 0, but `t` is exhausted.

- With `&&`: `0 >= 0 && -1 >= 0` is false, so the loop stops and the
  function returns `true`. **Wrong.**
- With `||`: `0 >= 0 || -1 >= 0` is true, so the loop runs again.
  `s` yields `a`, `t` yields -1, only one is exhausted, so it returns
  `false`. **Correct.**

`||` keeps going while **either** side might still have characters.

The suite catches this with `check("", "a", false)`: with `&&`, the loop
never runs at all and the function returns `true`.

</details>

### Why move both indices left after comparing?

<details>
<summary>Answer</summary>

Those surviving characters have already been compared. If we don't
decrement, the next round finds them again and repeats forever.

</details>

### Can `nextValidIndex()` run forever?

<details>
<summary>Answer</summary>

No. Whenever it continues rather than returning a surviving letter,
it decrements `i`. It eventually reaches `-1`.

In the outer loop, a successful comparison also decrements both
indices. No character is processed indefinitely.

</details>

---

## 7. Cost of Solution2

| Resource    | Cost     | Why                                                          |
| ----------- | -------- | ------------------------------------------------------------ |
| Time        | O(n + m) | Each index only travels left; no character is revisited.     |
| Extra space | O(1)     | Two indices and a pending-deletions counter per helper call. |

Both solutions have the **same worst-case time**. The gain is memory:
`Solution2` does not construct copies of the final texts.

It can also stop early on a mismatch, though that doesn't change its
worst-case time complexity.

---

## 8. Test both solutions with the same suite

See the main `platforms-and-courses/index-0/1-arrays-and-strings/two-pointers/1-easy/1-backspace-string-compare/README.md` for running and debugging instructions

The suite has **35 tests**. The last line should be:

```text
35/35 passed
```

This step checks the **tests**, not the clever solution. `Solution1` is
simple enough to trust, so if a test fails here, that test's expected
value is probably wrong.

### Step 2: test Solution2

Change one line in `main.cpp`:

```cpp
typedef Solution2 Solution;
```

Compile and run again with the same two commands. Same 35 tests, one
line changed. You should again see `35/35 passed`.

If you rewrite `Solution2` yourself and a test fails, the failing test
points at the bug:

| what you see                                 | what's probably wrong                                          |
| -------------------------------------------- | -------------------------------------------------------------- |
| `("", "a")` or `("a", "")` says `true`       | the loop uses `&&`, or "one side exhausted" returns `true`     |
| the program never finishes                   | `--i; --j;` is missing after a successful comparison           |
| crash or sanitizer report                    | `text[i]` is read without checking `i >= 0` first              |
| `("ab#c", "ac")` or `("a##c", "#a#c")` fails | the `skips` logic is wrong: a letter with `skips > 0` survived |

If a test fails, the harness prints `FAIL`, the expected result, and the
actual result. It also returns a nonzero exit code, so a failure isn't
hidden by a successful process exit.

### Step 3 (optional): referee mode

Once both versions pass on their own, make every test run **both**
implementations and print a warning if they disagree. Replace `check()`
in `main.cpp` with:

```cpp
void check(const std::string &s, const std::string &t, bool expected) {
  ++testsRun;

  Solution1 referee;
  Solution2 candidate;
  const bool refResult = referee.backspaceCompare(s, t);
  const bool actual = candidate.backspaceCompare(s, t);

  const bool ok = actual == expected;
  if (!ok) {
    ++testsFailed;
  }

  std::cout << (ok ? "  PASS  " : "  FAIL  ") << "backspaceCompare(\""
            << preview(s) << "\", \"" << preview(t) << "\")"
            << "  expected=" << std::boolalpha << expected
            << "  actual=" << actual << '\n';

  if (refResult != actual) {
    std::cout << "          MISMATCH: Solution1 said " << std::boolalpha
              << refResult << '\n';
  }
}
```

In referee mode, `typedef ... Solution;` and `Solution solution;` aren't
used anymore, so delete them.

Keep the `expected` values too: **two implementations agreeing doesn't
prove they're both correct.** They could share the same mistake.

---

## Checklist

- [ ] I can type `"ab#c"` by hand and get `"ac"`.
- [ ] I can explain why right-to-left lets me decide whether a letter survives.
- [ ] I traced `"a##b"` and found `b`, then no more characters.
- [ ] I can explain what `skips` counts.
- [ ] I can explain, using `"ab"` vs `"b"`, why one exhausted string means `false`.
- [ ] Step 1 printed `35/35 passed` with `typedef Solution1 Solution;`.
- [ ] Step 2 printed `35/35 passed` with `typedef Solution2 Solution;`.

Paste your output, or tell me the first step where you got stuck.
