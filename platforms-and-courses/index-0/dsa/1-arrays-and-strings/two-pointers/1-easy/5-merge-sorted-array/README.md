# Merge Sorted Array

You're given two lists of integers, `nums1` and `nums2`, each already
sorted in ascending order.

Return a single list containing all the numbers from both, still sorted in
ascending order.

```
Input: nums1 = [1, 2, 3], nums2 = [2, 5, 6]
Output: [1, 2, 2, 3, 5, 6]
```

```
Input: nums1 = [], nums2 = [1]
Output: [1]
an empty list contributes nothing
```

```
Input: nums1 = [1], nums2 = []
Output: [1]
```

```
Constraints
0 <= nums1.length, nums2.length <= 200
-10^9 <= nums1[i], nums2[i] <= 10^9
nums1 and nums2 are each sorted in ascending order
```

<details>
<summary>💡 Hint</summary>

Both lists are already sorted. The smallest number overall must be at the
front of one of them. Which one?

Original source: [LeetCode](https://leetcode.com/problems/merge-sorted-array/)

</details>

---

## Files in this folder

| file       | what it is                                                          |
| ---------- | ------------------------------------------------------------------- |
| `main.cpp` | `Solution1` (yours), `Solution2` and `Solution3` (your turn), tests |
| this file  | the walkthrough                                                     |

---

## 1. What we're doing, in a picture

Think of two piles of cards, face up, each sorted with the smallest on top.
You build one sorted pile by always taking the **smaller of the two top
cards**.

```
nums1:  1  2  3          nums2:  2  5  6
        ^                        ^
        i                        j

arr:    _  _  _  _  _  _
        ^
        next free slot
```

Two things can get in the way:

- **One pile runs out first.** The other pile still has cards, and they all
  go on the end, in order, without any more comparing.
- **A pile can be empty from the start** (`[]`). Then there's nothing to
  compare at all.

---

## 2. The idea: two pointers, three phases

1. **Compare.** While **both** lists have unread elements, compare the two
   fronts, copy the smaller one, and advance that list's pointer.
2. **Leftovers of nums1.** Copy whatever is left in `nums1`.
3. **Leftovers of nums2.** Copy whatever is left in `nums2`.

**Why is the smaller front always the right next element?** Each list is
sorted, so its front is its smallest unread element. The smaller of the two
fronts is therefore the smallest unread element overall.

**The invariant.** An invariant is a sentence that is true at every step.
For your solution it is:

> `i + j` = how many elements have been written so far = the index of the
> next free slot in `arr`.

Say it out loud. Section 4 shows that one of your bugs came from breaking
this sentence for a single line.

---

## 3. Trace it (worked for you)

`nums1 = [1, 2, 3]`, `nums2 = [2, 5, 6]`, using `Solution1`.

| step | phase | i   | j   | `nums1[i]` | `nums2[j]` | take                   | write to `i + j` | arr after     |
| ---- | ----- | --- | --- | ---------- | ---------- | ---------------------- | ---------------- | ------------- |
| 1    | 1     | 0   | 0   | 1          | 2          | 1 from nums1           | 0                | `1 _ _ _ _ _` |
| 2    | 1     | 1   | 0   | 2          | 2          | tie → 2 from **nums2** | 1                | `1 2 _ _ _ _` |
| 3    | 1     | 1   | 1   | 2          | 5          | 2 from nums1           | 2                | `1 2 2 _ _ _` |
| 4    | 1     | 2   | 1   | 3          | 5          | 3 from nums1           | 3                | `1 2 2 3 _ _` |
| 5    | 1     | 3   | 1   | —          | —          | `i == 3` → **stop**    |                  |               |
| 6    | 2     | 3   | 1   | —          | —          | `i < 3` false, nothing |                  |               |
| 7    | 3     | 3   | 1   | —          | 5          | 5                      | 4                | `1 2 2 3 5 _` |
| 8    | 3     | 3   | 2   | —          | 6          | 6                      | 5                | `1 2 2 3 5 6` |

Answer: `[1, 2, 2, 3, 5, 6]`. ✓

Note step 2: on a tie, `num1 < num2` is false, so your code takes from
**nums2**. For `int`s that doesn't matter. Section 6 shows when it does.

### Your turn

Trace `nums1 = [4, 5, 6]`, `nums2 = [1, 2, 3]`. Which phase does most of the work?

<details>
<summary>Answer</summary>

| step | phase | i   | j   | take            | write to `i + j` |
| ---- | ----- | --- | --- | --------------- | ---------------- |
| 1    | 1     | 0   | 0   | 1 from nums2    | 0                |
| 2    | 1     | 0   | 1   | 2 from nums2    | 1                |
| 3    | 1     | 0   | 2   | 3 from nums2    | 2                |
| 4    | 1     | 0   | 3   | `j == 3` → stop |                  |
| 5    | 2     | 0   | 3   | 4               | 3                |
| 6    | 2     | 1   | 3   | 5               | 4                |
| 7    | 2     | 2   | 3   | 6               | 5                |

Phase 2 does half the work. Notice that `i + j` still lands on the right
slot even though `i` is 0: `j` remembers the three elements already written.

</details>

Now trace `nums1 = []`, `nums2 = [1]`. Does the `while` loop run at all?

<details>
<summary>Answer</summary>

No. `i < 0` is false right away. Phase 2 does nothing, and phase 3 writes
`arr[0 + 0] = 1`. Result: `[1]`. ✓

This is why phases 2 and 3 must live **after** the loop and not inside it:
when the loop never runs, the code inside it never runs either. That was
one of your first bugs (section 4).

</details>

---

## 4. Your journey: four versions, and what each mistake teaches

You went through four versions to get here. Every bug you hit is a
**classic**, one that experienced programmers still make. Understanding
_why_ you made each one matters more than the fix, because the "why" will
come back in the next problem.

The "why it probably happened" parts are my best guesses. You know your own
thinking better, so check whether they ring true.

### Version 1: `for` loop with `push_back`

```cpp
for (size_t i = 0, j = 0; i < nums1.size() - 1 && j < nums2.size() - 1;) {
  ...
  if (num1 == num2) { ++i; arr.push_back(num1); }
  else if (num1 < num2) { ++i; arr.push_back(num1); }
  else { ++j; arr.push_back(num2); }

  if (i < nums1.size() - 1 && j >= nums2.size() - 1) {
    for (size_t k = j; k < nums2.size() - 1; k++) arr.push_back(nums2[k]);
    break;
  } else if (...) { /* same, mirrored */ }
}
```

| bug | what was wrong                                         | symptom                                                                                                                                   |
| --- | ------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------- |
| A   | `i < size() - 1` stops one element early               | the last element of each list was never compared or copied                                                                                |
| B   | `size() - 1` on an empty vector                        | `0 - 1` wraps to 18446744073709551615 (section 4b of length-of-last-word). Result: a wrong `[]`, or reading `nums1[0]` of an empty vector |
| C   | the `num1 == num2` branch was identical to the `<` one | not a bug, just extra code                                                                                                                |
| D   | leftovers copied from the **wrong** list               | condition said "nums1 has elements left", body copied nums2                                                                               |
| E   | leftover code **inside** the loop                      | never ran if the loop never ran, or ended on its own condition                                                                            |

#### A and B: "last index" thinking

**Why it probably happened.** Two true facts got combined: "the last element
is at `size - 1`" and "loop while `i <` the end". Each is right on its own.
Together they stop one step too early. This is the most common source of
off-by-one errors.

**Change your mental model: half-open ranges.** Think of indexes as the
range `[0, size)`: the start is included and the end is excluded. Then
`i < size()` reads as **"i is a valid index"**. There's no `- 1` anywhere.

> **Habit:** if you write `size() - 1`, stop and ask two questions. Is
> `size()` guaranteed to be at least 1? Is it unsigned? If the answers are
> "no" and "yes", you have a wrap-around bug waiting.

#### C: enumerating cases and never collapsing them

**Why it probably happened.** When you design, listing `<`, `==`, `>`
separately is good thinking, because it makes you consider ties. The step
that was missing is the one _after_ listing.

**Change your mental model:** enumerate first, then **collapse**. If two
branches have the same body, merge their conditions. (In section 6 the tie
case matters again, but for a different reason: which list it should come
from.)

#### D: reasoning about "the other one"

**Why it probably happened.** With names like `i`/`j` and `nums1`/`nums2`,
it's easy to slip between "the list that finished" and "the list that has
leftovers". The condition was about one, and the body was about the other.

**Change your mental model:** read the condition out loud as a sentence and
**finish the sentence**. For example: "If nums1 still has elements… copy
the rest of **nums1**." The body should name the same list that the
condition says still has elements.

#### E: putting cleanup inside the loop

**Why it probably happened.** In length-of-last-word, your single loop did
two phases with a flag, and it worked. Here the same instinct backfired,
because this loop can end **before** it ever reaches your check. It can
also never start at all.

**Change your mental model:** for every loop, ask **"in how many ways can
this loop end?"** Code that must run no matter how the loop ends goes
**after** the loop.

> **Habit:** cleanup goes after the loop, not inside it.

#### Calling it "brute force"

It wasn't. You already had the optimal two-pointer idea. The real brute
force is "glue both lists together, then sort". That baseline matters for
two reasons:

- You can only say what you've _improved_ if you know the baseline.
- A slow but obviously correct version makes a perfect **referee**. The 200
  random tests in `main.cpp` use exactly that brute force to check your fast
  version.

### Version 2: pre-sized array, `arr[i + j]`

You fixed A to E: the `< size()` bounds, the leftovers after the loop, and
the leftovers from the right list. Two new bugs appeared:

```cpp
if (num1 < num2) {
  ++i;
  arr[i + j] = num1;   // F: increment BEFORE write
} else {
  ++j;
  arr[i + j] = num1;   // G: wrong variable
}
```

Your test output:

```
input: [1,2,3], [100]       expected: [1,2,3,100]   actual: [0,1,2,100]
input: [4,5,6], [1,2,3]     expected: [1,2,3,4,5,6] actual: [0,4,4,4,5,6]
```

#### F: increment before write

**What went wrong.** The first write went to index 1, so `arr[0]` kept its
initial `0`. Every write in the main loop landed one slot too far right.
The leftover loops then wrote to the _correct_ slots and overwrote some of
those misplaced values. In `[1,2,3], [100]`, the `3` at index 3 was
overwritten by `100`.

**Why it probably happened.** In version 1 you used `push_back`, where the
destination is always "the end". Swapping the order of `++i` and
`push_back` made no difference there. Once you switched to `arr[i + j]`,
the destination was **computed from** `i` and `j`, so the order suddenly
mattered. The old habit came along with you into the new code.

Interesting detail: your leftover loops, `for (; i < ...; i++) arr[i + j] = ...;`,
got the order right. A `for` loop always runs its increment _after_ the body.
The `while` loop made you write the order by hand.

**Change your mental model:**

- **Use, then update.** If an index is computed from a counter, use it
  first, then change the counter.
- **Think of the increment as a "commit".** `++i` means "this element is
  done". You only commit after the work is actually done.
- **Check the invariant on every line.** Between `++i` and the write,
  `i + j` no longer means "next free slot". The invariant was broken for
  exactly one line, and that was enough.

#### G: copy-paste slip

**Why it probably happened.** You wrote the `if` branch, copied it into the
`else`, and changed `++i` to `++j`, but not `num1` to `num2`.

**Change your mental model: the symmetry check.** After pasting a mirrored
branch, read it left to right, and for each name ask: "Which side does this
belong to?" **Every** side-1 name must become a side-2 name. Symmetric code
deserves a symmetric review.

#### What you did right: reading failure patterns

Your output had three patterns, and each one points at a bug:

| pattern                                         | meaning                                                |
| ----------------------------------------------- | ------------------------------------------------------ |
| every failing output starts with `0`            | slot 0 is never written (F)                            |
| `nums2`'s values never appear (`[0,4,4,4,5,6]`) | the nums2 branch doesn't write nums2's value (G)       |
| tests with an empty list **pass**               | the bugs live inside the `while` loop, which never ran |

> **Habit:** debug from the **pattern**, not a single failure. Also ask
> which tests passed, and why.

Also notice that the zeros from `std::vector<int> arr(size)` made bug F
_visible_. That's why the in-place tests fill empty slots with the strange
value `123456789` instead of `0`.

### Version 3: correct, with extra guards

```cpp
if (i < nums1.size()) {
  for (; i < nums1.size(); i++) { ... }
} else if (j < nums2.size()) {
  for (; j < nums2.size(); j++) { ... }
}
```

**Why it probably happened.** A defensive instinct: "make sure only one of
them runs."

**Change your mental model:** a `for` loop **is** already an `if`, because
it checks its condition before the first iteration. Ask: "What does this
loop do if its condition is false on entry?" It does nothing. So two plain
loops in a row are enough, and at most one of them does any work
(section 7, Q2).

### Version 4: `Solution1`

The two plain loops, plus `const std::vector<int>&` for inputs you don't
modify. Correct and clean. 🎉

### Summary: habits to carry into the next problem

| habit                  | the question to ask yourself                                            |
| ---------------------- | ----------------------------------------------------------------------- |
| half-open ranges       | Is `i < size()` enough? Am I subtracting from an unsigned value?        |
| invariants             | What does each variable _mean_? Does every line keep that true?         |
| use, then update       | Do I compute an index from a counter before or after changing it?       |
| phases                 | In how many ways can this loop end? Does my cleanup run in all of them? |
| symmetry check         | In the mirrored branch, did I swap _every_ name?                        |
| collapse branches      | Do two branches have the same body?                                     |
| trace edge cases first | Did I trace an empty input by hand before running anything?             |
| read failure patterns  | What do the failures have in common? Which tests passed, and why?       |

---

## 5. Review of your solution

This is `Solution1` in `main.cpp`.

**Verdict: correct.** It handles empty lists, uses `< size()` everywhere,
keeps the leftover phases after the loop, and runs in O(n + m) time.

Two small suggestions:

- **Write the invariant down.** A comment such as
  `// i + j == number of elements written == next free slot` turns a fact
  you keep in your head into one a reader can check.
- **Make the leftovers comment more precise.** "At most one of these two
  loops does anything" says exactly what is true. Can you explain why?
  (Section 7, Q2.)

---

## 6. Solution2: a write cursor and stable ties (your turn)

`arr[i + j]` works, but it relies on an invariant you must keep in your
head, and that's where bug F came from. A dedicated cursor `k` means "next
free slot": one variable with one meaning.

This is `Solution2` in `main.cpp`:

```cpp
std::vector<int> arr(nums1.size() + nums2.size());
size_t i = 0; // next unread element of nums1
size_t j = 0; // next unread element of nums2
size_t k = 0; // next free slot in arr

// TODO 1: while BOTH have unread elements: copy the smaller front into
//         arr[k], advance k and the pointer you read from. Ties: nums1.
// TODO 2: copy whatever is left in nums1.
// TODO 3: copy whatever is left in nums2.
```

### 6a. Does the order of "write" and "advance k" still matter?

Before writing any code, answer this: if you do `++k` first and then
`arr[k] = ...`, what happens? Can you do the write and the advance in
**one** expression, so that the wrong order becomes impossible?

<details>
<summary>Hint</summary>

Look up the difference between `k++` and `++k`. One of them gives you the
**old** value, then increments. Which value do you want as the index?

</details>

### 6b. Stable ties

Imagine merging cards sorted by number, where each card also has a colour:

```
nums1: (2, red)        nums2: (2, blue)
```

A **stable** merge keeps equal elements in their original order. Elements
from `nums1` came "first", so a stable merge outputs `(2, red)` before
`(2, blue)`. Merge sort depends on this property, and your merge is the
core step of merge sort (section 11e).

**Question:** your current `if (num1 < num2)` sends ties to the `else`
branch, which takes from nums2. Which comparison makes ties go to nums1?

<details>
<summary>Hint</summary>

You only need to change one character. Ask: "When `num1 == num2`, which
branch do I want to run?"

</details>

> **Honest note:** the tests can't detect this. Two equal `int`s are
> indistinguishable, so a merge with the ties the "wrong" way still passes.
> Follow-up 11a shows how to test stability for real.

### 6c. Is `k` the same as `i + j`?

<details>
<summary>Answer</summary>

Yes, always. Every write advances `k` **and** exactly one of `i`/`j`. So
`k` adds no new information. What it adds is a **name**, and names prevent
bugs.

</details>

### If a test fails

| failing tests                          | what's probably wrong                              |
| -------------------------------------- | -------------------------------------------------- |
| everything except `merge([], [])`      | the TODOs are still empty, so `arr` is all zeros   |
| outputs start with `0` / shifted right | `k` advanced before writing (bug F again)          |
| the values of one list are missing     | wrong variable in one branch (bug G again)         |
| `[1,2,3], [100]` or `[4,5,6], [1,2,3]` | a leftover loop is missing or reads the wrong list |
| sanitizer or assertion crash           | you read `nums1[i]` when `i == nums1.size()`       |

---

## 7. Questions people always ask

### Q1. Why `&&` and not `||` in the `while` condition?

<details>
<summary>Answer</summary>

Phase 1 compares `nums1[i]` with `nums2[j]`, so **both** must be valid. With
`||`, the loop keeps going after one list is finished and reads past its
end.

</details>

### Q2. Can both leftover loops do work?

<details>
<summary>Answer</summary>

No. The `while` loop only stops when `i == nums1.size()` **or**
`j == nums2.size()`. At least one list is finished, so its leftover loop
runs zero times. That's why you don't need the `if`/`else if` from
version 3.

</details>

### Q3. Can anything overflow?

<details>
<summary>Answer</summary>

No. Values are at most 10^9 in size, `int` goes up to about 2.147 × 10^9,
and you only **compare** values; you never add them.

A trap to know about: some people compare with `a - b < 0`. Here the
largest difference is 10^9 − (−10^9) = 2 × 10^9, which _barely_ fits. With
slightly wider constraints it would overflow. Use `<`; it can't overflow.

</details>

### Q4. Pre-sized `arr(n)` with indexes, or `reserve` plus `push_back`?

<details>
<summary>Answer</summary>

With `push_back`, the destination is always "the end", so bug F **cannot
happen**. `reserve(n + m)` avoids reallocations, and both approaches are
O(n + m).

Pre-sizing is necessary when you **can't** append. That's exactly the
situation in Solution3, where the output space already exists inside
`nums1`. So learn both.

</details>

### Q5. Doesn't the standard library already do this?

<details>
<summary>Hint</summary>

It does: `std::merge` in `<algorithm>`. It takes two input ranges and an
output iterator. Try writing it in one line. You'll need either a pre-sized
`arr` with `arr.begin()`, or an empty `arr` with `std::back_inserter(arr)`.
Is `std::merge` stable? Look it up.

</details>

---

## 8. What it costs

Work it out yourself first. How many times is each element touched? How
much memory do you allocate besides the output?

<details>
<summary>Answer</summary>

| version                   | time                        | extra space     |
| ------------------------- | --------------------------- | --------------- |
| brute force (glue + sort) | O((n + m) log(n + m))       | O(n + m) output |
| two pointers (yours)      | O(n + m): each element once | O(n + m) output |
| in place (Solution3)      | O(n + m)                    | O(1)            |

With at most 400 elements, all of them are instant. The difference matters
at scale, and inside merge sort, where this merge runs O(log n) levels
deep.

</details>

---

## 9. Solution3: LeetCode's real version, in place (your turn, harder)

The original LeetCode problem is different:

```
nums1 = [1, 2, 3, 0, 0, 0], m = 3      (size m + n; the last n slots are empty)
nums2 = [2, 5, 6],          n = 3

After merge: nums1 = [1, 2, 2, 3, 5, 6]
```

You must write the result **into `nums1`**, with no new array.

Don't start coding yet. Work through these questions in order.

### 9a. Try filling from the front

Use your current approach, writing into `nums1` from index 0. Trace
`nums1 = [1, 3, _, _]`, `nums2 = [2, 4]`. What happens to the `3` when you
write the `2`?

<details>
<summary>Answer</summary>

The `2` goes to `nums1[1]` and **destroys** the `3`, which you haven't read
yet. Filling from the front overwrites unread data.

</details>

### 9b. Where is the free space?

The empty slots are at the **back** of `nums1`. If you fill from the back,
which element belongs in the very last slot? Where can you find it?

<details>
<summary>Hint</summary>

The **largest** element goes last. It's the larger of the two **last real**
elements: `nums1[m - 1]` and `nums2[n - 1]`. Now everything mirrors your
Solution1: compare backs instead of fronts, take the larger instead of the
smaller, and walk left instead of right.

</details>

### 9c. Can the write cursor ever overwrite an unread element of nums1?

This question is the heart of the problem. Try to argue it yourself.

<details>
<summary>Answer</summary>

Let `i` be the last unread index of nums1, `j` the last unread index of
nums2, and `k` the next slot to fill. They start at `m - 1`, `n - 1`, and
`m + n - 1`. Each step decreases `k` and exactly one of `i`/`j`, so this
holds at every step:

```
k = i + j + 1
```

`j` never goes below `-1`, so `k >= i`. The two are equal only when
`j == -1`, which means nums2 is finished. So the write cursor can never
step on an unread element of nums1. ✓

</details>

### 9d. Which type for `i`, `j`, `k`?

Remember length-of-last-word, section 4c.

<details>
<summary>Hint</summary>

The indexes go **down**, and a loop ends when one of them goes **below 0**.
`size_t`

<details/>
