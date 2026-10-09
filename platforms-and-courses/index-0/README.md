# INDEX-0

My solutions, tests, and tutorials while following the
[INDEX 0](https://www.index-0.in/) desktop app.

---

## Why this exists

I'm here to **learn**, not to collect green checkmarks.

- I solve each problem as best I can, even if that means brute-forcing it.
- An AI/LLM writes the tests, guides me when I'm stuck (**without giving me
  the solution**), and finally turns everything into a tutorial.
- My mistakes are part of the content. A bug I understand is worth more than
  a solution I copied.

---

## My approach

| stage | what happens                                                                    | who does it |
| ----- | ------------------------------------------------------------------------------- | ----------- |
| 1     | **Solve**: write my own attempt, brute force is fine                            | me          |
| 2     | **Test**: get a test suite to check my attempt                                  | AI          |
| 3     | **Unstuck**: if tests fail, get guided with questions and hints, not answers    | AI + me     |
| 4     | **Fix**: repeat 2 and 3 until it passes                                         | me          |
| 5     | **Tutorial**: a `README.md` built from the problem, my code, and our discussion | AI          |
| 6     | **Level up** (optional): try the harder versions the tutorial guides me through | me          |

---

## 🎮 Difficulty levels

Like in games, every problem can be played on several difficulties. I start
on Easy (my own attempt), and I can stop at any level or come back later.
It's not a perfect one-to-one comparison, but you get the idea `;)`.

| level          | in `main.cpp` | what it is                                                       |
| -------------- | ------------- | ---------------------------------------------------------------- |
| 🟢 Easy        | `Solution1`   | my own attempt, kept as I wrote it                               |
| 🟡 Normal      | `Solution2`   | cleaner, safer, or more idiomatic; same complexity               |
| 🔴 Hard        | `Solution3`   | better time/space complexity, or a harder variant of the problem |
| ⭐ Side quests | README        | short follow-up exercises and related problems                   |

Not every problem has every level. If there's nothing meaningful to improve,
there's no level for it.

---

## Folder layout

```
index-0/
├── README.md                         ← this file
└── <n>-<topic>/                      e.g. 1-arrays-and-strings
    └── <technique>/                  e.g. two-pointers
        └── <n>-<difficulty>/         e.g. 1-easy
            └── <n>-<problem-name>/   e.g. 5-merge-sorted-array
                ├── main.cpp          solutions + test suite
                ├── README.md         the tutorial
                └── build/            compiled output (not committed)
```

## Build and test

From inside a problem folder:

```bash
mkdir -p build
```

```bash
g++ -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined main.cpp -o build/app
```

```bash
./build/app
```

To switch levels, change one line in `main.cpp`:
`typedef Solution1 Solution;` → `typedef Solution2 Solution;`

---

## 🧠 Habits I've learned (running log)

The same mistakes come back across problems, so I keep them here.
**Add one row per new lesson.**

| habit                        | the question to ask myself                                        | learned in          |
| ---------------------------- | ----------------------------------------------------------------- | ------------------- |
| signed if it can go negative | Can this index go below 0? Then don't use `size_t`.               | length-of-last-word |
| cast first, then subtract    | `static_cast<int>(s.size()) - 1`, not `s.size() - 1`              | length-of-last-word |
| bounds check first           | `i >= 0 && s[i]`: is the check on the left of `&&`?               | length-of-last-word |
| compile with warnings        | Did I read every `-Wall -Wextra` warning?                         | length-of-last-word |
| half-open ranges             | Is `i < size()` enough? Am I subtracting from an unsigned value?  | merge-sorted-array  |
| invariants                   | What does each variable _mean_? Does every line keep it true?     | merge-sorted-array  |
| use, then update             | Do I compute an index from a counter before or after changing it? | merge-sorted-array  |
| cleanup after the loop       | In how many ways can this loop end? Does my cleanup run in all?   | merge-sorted-array  |
| symmetry check               | In the mirrored branch, did I swap _every_ name?                  | merge-sorted-array  |
| collapse branches            | Do two branches have the same body?                               | merge-sorted-array  |
| read failure patterns        | What do the failures have in common? Which tests passed, and why? | merge-sorted-array  |
| trace edge cases first       | Did I trace an empty input by hand before running anything?       | merge-sorted-array  |

---

## Progress

| problem                                                                                | 🟢  | 🟡  | 🔴  | notes                          |
| -------------------------------------------------------------------------------------- | --- | --- | --- | ------------------------------ |
| [length-of-last-word](1-arrays-and-strings/two-pointers/1-easy/4-length-of-last-word/) | ✅  | ⬜  | —   |                                |
| [merge-sorted-array](1-arrays-and-strings/two-pointers/1-easy/5-merge-sorted-array/)   | ✅  | ⬜  | ⬜  | 🔴 = in-place LeetCode version |

---

# 🤖 Instructions for the AI

> **If you are an AI/LLM reading this file, this section is your prompt.**
> Follow it for every problem in this folder. You don't need example files;
> everything you need is described below. If something here conflicts with
> a direct request from me in the chat, my request wins.

## 0. Ground rules

1. **I want to learn, not to be handed answers.** Guide me with questions,
   hand traces, and layered hints. Never give me full solution code for
   anything I haven't solved, unless I explicitly ask for it.
2. **Don't touch my logic.** `Solution1` is my code, kept as I wrote it.
   Only formatting or `const` on parameters may change, and only if I
   already did that myself.
3. **Be honest and precise.** If something is a guess ("why you probably
   made this mistake"), say so. Don't claim tests catch things they can't.
   Don't call my approach "brute force" if it isn't, or "optimal" if it isn't.
4. **Connect to the past.** Use the "Habits I've learned" table above. If a
   mistake repeats an old lesson, name it ("this is the same as X in
   length-of-last-word"). If there's a new lesson, propose a new row.
5. **Figure out the stage.** Work out which stage (1–6 in "My approach") I'm
   in from my message, and do only that stage unless I ask for more.
6. **Use full file paths** for every file you write, e.g.
   `platforms-and-courses/index-0/<topic>/<technique>/<n>-<difficulty>/<n>-<problem>/main.cpp`.

## 1. What I'll give you (ask if something is missing)

- The problem statement (from the INDEX 0 app), sometimes with a source link.
- My code (`main.cpp`).
- Sometimes test output, and our earlier discussion.

If you need the folder number or name and can't infer it, ask before you
write files.

## 2. Stage 2: the test suite (`main.cpp`)

Build one self-contained `main.cpp` with this layout, top to bottom:

1. **Problem statement** in a block comment: clean plain text, examples,
   constraints, source link, and a line saying "See README.md for the
   walkthrough." No markdown fences inside the comment.
2. **Includes**: only what's used.
3. **`Solution1`**: my code.
4. **`Solution2` / `Solution3`**: skeletons with numbered `// TODO`
   comments that point to README sections. They **must compile**. Return a
   neutral value. Silence unused-parameter warnings with `(void)x;` only on
   optional levels, and say which warnings are expected.
   Only include a level if there's a real improvement (see Difficulty levels).
5. **Test settings**: the only lines I should need to change:
   ```cpp
   typedef Solution1 Solution;          // Step 2: change to Solution2
   constexpr bool kTestSomething = false; // only if a level has a different signature
   ```
6. **Harness**, which I shouldn't need to edit:
   - `check(input..., expected)` prints `  PASS  call(...)` or `  FAIL  call(...)`,
     followed by `expected =`, `actual =`, and the **first difference**
     (index and both values) on failure.
   - Long inputs are previewed (around 12 items, then `... (N total)`).
   - Hand-written tests grouped by comments: examples from the statement,
     empty or minimal inputs, single elements, duplicates and ties,
     negatives, extreme values from the constraints, maximum size, and
     anything specific to the problem.
   - **Random tests against a brute-force referee** when a simple, obviously
     correct brute force exists: fixed seed (`std::mt19937 rng(42)`), a mix
     of tiny inputs and inputs with many duplicates, passes not printed,
     at most about 5 failures printed.
   - For in-place or "unused slot" problems, fill unused slots with a
     strange sentinel (e.g. `123456789`), not `0`, so bugs show up.
   - A final summary `X/Y passed`, and a return value of `0` only if
     everything passed.
7. In your message, state the **exact total test count**, plus the
   expected warnings for each step.

## 3. Stage 3: guiding me when I'm stuck

- Start with what I got **right**, especially fixes since my last attempt.
- Point at **failure patterns** in my test output (e.g. "every output starts
  with 0", "values from one list never appear", "tests with empty input
  pass"), and ask what each one tells me.
- Give me a **trace table** with the first row or two filled in and the rest
  as `?`.
- One **guiding question per bug**, labelled (🐛 A, 🐛 B, …).
- Hints go inside `<details>`, from vague to specific. Never put fixed code
  in a hint. At most, show the one line that's wrong.
- End by asking me to fix it, re-run, and paste the output.

## 4. Stage 5: the tutorial (`README.md`)

Use these sections, in this order. Skip a section only if it really doesn't
apply.

1. **Title + problem statement**: examples in code blocks with a one-line
   explanation, constraints, then a `💡 Hint` in `<details>` with the source link.
2. **Files in this folder**: a table.
3. **What we're doing, in a picture**: ASCII art, plus the traps or edge
   cases ("two things can get in the way…").
4. **The idea**: phases or steps in plain words, _why_ it works, and the
   **invariant** if there is one.
5. **Trace it**: one fully worked trace table, then **"Your turn"** traces
   (including an edge case) with answers in `<details>`.
6. **My journey**: one subsection per version I went through. For each bug:
   - what was wrong and its symptom (a table is good),
   - **why it probably happened** (marked as a guess),
   - **how to change my mental model**,
   - a `> **Habit:**` quote.
     Also cover what I did _right_. End with a summary table of habits.
7. **Review of my solution**: a verdict ("correct" / "correct but…" /
   "incorrect because…"), then small, concrete improvements.
8. **Next levels** (`Solution2`, `Solution3`): one section each. Explain the
   _motivation_, show the skeleton, then a sequence of questions (9a, 9b, …)
   that leads me to the idea, with answers or hints in `<details>`.
   **No full solution code**, not even hidden, unless I ask. Include an
   "If a test fails" table (failing tests → what's probably wrong).
9. **Questions people always ask**: Q&A with answers in `<details>`.
10. **What it costs**: a time/space table comparing all versions,
    including brute force. Ask me to work it out first.
11. **Test it**: steps that match `main.cpp` (Step 1: my solution, Step 2:
    the next level, Step 3: optional extras), each with the exact command
    and expected output (`N/N passed`, expected warnings).
12. **Side quests**: short follow-ups and related problems, with hints only.
13. **Checklist**: `- [ ]` items for understanding, not only for passing.
14. Closing line: _"Paste your output, or tell me the first step where you
    got stuck."_

Before you finish, check that the README is **complete**. If it's too long
for one message, stop at a section boundary and say exactly where you
stopped.

## 5. C++ conventions

- C++17, compiled with `-Wall -Wextra -g -fsanitize=address,undefined`.
- `size_t` for indexes that only go up; `int` (with
  `static_cast<int>(x.size())`) for indexes that can go below 0.
- `const T&` for inputs that aren't modified.
- Bounds checks on the left of `&&`.
- `std::isspace` and similar functions take `static_cast<unsigned char>(c)`.
- Prefer comparisons over subtraction (`a < b`, not `a - b < 0`).
- No `using namespace std;` and no `bits/stdc++.h`.

## 6. Writing style

- Short paragraphs, plain words, a conversational "you". Light emoji (✓, 🐛, 💡, 🎉) is fine.
- Tables for traces, comparisons, and "symptom → cause".
- ASCII art for arrays, pointers, and memory.
- Every claim about the code must be **true for this code**: trace it, don't guess.
- Us the `unslop` skill.

---

## Quick prompts (copy-paste)

**Stage 2: tests**

> Following `index-0/README.md`, build the test suite `main.cpp` for this
> problem. Problem: `<paste>`. My code: `<paste>`. Folder: `<path>`.

**Stage 3: I'm stuck**

> Following `index-0/README.md` (stage 3), here's my code and test output.
> Guide me, don't solve it. `<paste>`

**Stage 5: tutorial**

> Following `index-0/README.md` (stage 5), write the `README.md` for
> `<path>` based on my final code and our discussion above.

**Stage 6: level up**

> I want to try `Solution2` for `<problem>`. Ask me the first question only.
