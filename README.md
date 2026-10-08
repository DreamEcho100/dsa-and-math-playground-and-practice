# Master Math by Coding in Python

Notes and notebooks from the Udemy course
[Math with Python](https://www.udemy.com/course/math-with-python/) by Mike X Cohen,
followed along inside Neovim (jupytext + molten-nvim + quarto-nvim, see the
`dev-setup` dotfiles repo for the editor setup).

## Structure

The course grows as I work through it, so the exact contents change; this
describes the conventions, not a snapshot:

- `sections/<number>-<section-name>/` holds one folder per course section.
  Lessons live inside as `<number>-<lesson-name>/` folders with the
  notebook(s) to run for that lesson.
- `requirements.txt` (repo root) is the single list of packages for every
  lesson, and `.venv/` (gitignored, created locally, see Setup) is the single
  environment that serves them all.
- `scripts/` holds helper scripts (`cleanup.py`, see Cleanup).
- `playground/` is scratch space, and `_ignore/` (gitignored) holds the
  course's original files.

## Setup

Do this **once**, from the repo root, before running any notebook. One virtual
environment serves every lesson. `requirements.txt` is only a list of
packages; nothing is installed until you run the commands below.

**Prerequisite** (once per machine, Ubuntu/Debian): `python3 -m venv` needs
the venv package.

```sh
sudo apt install python3-venv
```

**1. Create the environment and install the packages**

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
```

**2. Register a Jupyter kernel that uses it**, so Neovim (molten-nvim) runs
notebooks with this venv instead of the system Python:

```sh
.venv/bin/python -m ipykernel install --user --name=udemy-master-math-by-coding-in-python --display-name "Master Math by Coding in Python"
```

If a kernel called `udemy-master-math-by-coding-in-python` already exists (for example one pointing
at the system Python), this replaces it.

**3. Check it worked**

```sh
.venv/bin/python -c "import numpy, matplotlib, scipy, sympy; print('ok')"
python3 -m jupyter kernelspec list     # should list udemy-master-math-by-coding-in-python
grep '"/' ~/.local/share/jupyter/kernels/udemy-master-math-by-coding-in-python/kernel.json   # should show this repo's .venv/bin/python
```

In Neovim, attach it with `:MoltenInit udemy-master-math-by-coding-in-python` (restart a running
kernel with `:MoltenRestart` after any change here). Pyright picks up the
repo's `.venv` on its own; if "import could not be resolved" warnings stay,
run `:LspRestart`.

### Removing it (kernel and environment)

The kernel and the venv are separate things; remove both when you're done
with the course or want to start over:

```sh
python3 -m jupyter kernelspec list                                      # find the exact name
python3 -m jupyter kernelspec remove -f udemy-master-math-by-coding-in-python     # unregister the kernel
rm -rf .venv                                                            # delete the environment (or: scripts/cleanup.py --deep)
```

Removing only the venv leaves a registered kernel whose Python no longer
exists (`:MoltenInit` then fails to start it); removing only the kernel leaves
the venv unused. If an older kernel with a different name (for example
`math-with-python`) is still registered, remove it the same way:
`python3 -m jupyter kernelspec remove -f math-with-python`. Recreate everything
any time with Setup steps 1-2.

### Adding a package later

Add its name to `requirements.txt`, then install from the file with the
venv's own pip (so it can never fall back to the system Python):

```sh
echo "<package>" >> requirements.txt
.venv/bin/pip install -r requirements.txt
```

Don't use `pip freeze > requirements.txt`: it overwrites the short list above
with every package in whatever Python is active (hundreds of lines), and the
`>` empties the file before `pip` runs, so even a failed command wipes it.

### Troubleshooting

| You see                                                  | Why                                                                      | Fix                                                                          |
| -------------------------------------------------------- | ------------------------------------------------------------------------ | ---------------------------------------------------------------------------- |
| `ModuleNotFoundError: No module named 'sympy'` in a cell | The kernel's Python doesn't have the package (usually the system Python) | Do Setup steps 1-2, then `:MoltenRestart`                                    |
| `source: no such file or directory: .venv/bin/activate`  | The venv hasn't been created yet                                         | `python3 -m venv .venv` (step 1)                                             |
| `error: externally-managed-environment`                  | `pip` ran against the system Python, which Ubuntu/Debian block           | Create the venv and use `.venv/bin/pip`. Don't use `--break-system-packages` |
| `python3 -m venv` fails about `ensurepip`                | `python3-venv` isn't installed                                           | `sudo apt install python3-venv`                                              |
| `requirements.txt` suddenly has 100+ lines               | `pip freeze >` was run outside the venv                                  | Restore the short list (numpy, matplotlib, scipy, sympy, ipykernel)          |
| Pyright "import could not be resolved"                   | LSP started before the venv existed                                      | `:LspRestart`                                                                |

You can leave an activated shell with `deactivate`; with the `.venv/bin/...`
commands above you never need to activate it.

## Running a lesson in Neovim

```vim
:e sections/1-introductions-and-installations/1-create-a-beautiful-harmonograph/<notebook>.ipynb
:MoltenInit udemy-master-math-by-coding-in-python
```

If `:MoltenInit` can't find that kernel or a cell fails with
`ModuleNotFoundError`, Setup hasn't been done (or the kernel still points at
another Python): see Setup and its Troubleshooting table. Do it once; after
that, each session is just open the notebook and `:MoltenInit`.

Then, on a code cell:

| Key                         | Action                                                         |
| --------------------------- | -------------------------------------------------------------- |
| `<leader>jr`                | Run/create the cell under the cursor                           |
| `<leader>jv` (visual)       | Run the selected lines                                         |
| `]b` / `[b`                 | Jump to the next/previous code block, run or not               |
| `]j` / `[j`                 | Jump between cells you've already run (Molten's own concept)   |
| `<leader>jx`                | Interrupt the running cell (the kernel and its variables stay) |
| `<leader>jc` / `<leader>jC` | Clear this cell's output / every output in the file            |
| `<leader>jn`                | New notebook (type just a name; created next to where you are) |

See `docs/neovim-tutorials-from-0-to-hero/21-python-math-jupyter.md` in the
`dev-setup` repo for the full walkthrough and troubleshooting.

## Cleanup

`scripts/cleanup.py` frees disk space by removing clutter that's always
safe to delete because it's fully regenerable — nothing here is source
content:

| Removed                       | Regenerated by                                                                                                                                                |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `__pycache__/`                | Python, automatically, next time a module is imported                                                                                                         |
| `*.pyc`                       | Python, automatically                                                                                                                                         |
| `.ipynb_checkpoints/`         | Jupyter/jupytext, automatically on next save                                                                                                                  |
| `.pytest_cache/`              | pytest, automatically, if/when you run tests                                                                                                                  |
| `.mypy_cache/`                | mypy, automatically, if/when you run it                                                                                                                       |
| `.ruff_cache/`                | ruff, automatically, if/when you run it                                                                                                                       |
| `.venv/` (only with `--deep`) | `python3 -m venv .venv && .venv/bin/pip install -r requirements.txt`, then re-run the `ipykernel install` from Setup (the kernel points at the venv's Python) |

A normal run never looks inside `.venv/` (or `.git/`), so it can't touch the
installed packages; only `--deep` removes the venv, and then as a whole.
`--deep` does **not** unregister the Jupyter kernel; to remove that too, see
"Removing it (kernel and environment)" under Setup.

It's a plain Python 3 standard-library script (no dependencies), so it runs
with any `python3` — including before the project's own `.venv` exists yet.

**Usage:**

```sh
# Whole project, keeps .venv/
python3 scripts/cleanup.py
# or, since it's executable:
scripts/cleanup.py

# Only one section or lesson directory (path is relative to the project root)
scripts/cleanup.py sections/2-arithmetic
scripts/cleanup.py sections/1-introductions-and-installations/1-create-a-beautiful-harmonograph

# Whole project, plus remove .venv/ too (only meaningful without a path arg —
# a venv only ever lives at the project root)
scripts/cleanup.py --deep

# Full option/argument reference
scripts/cleanup.py --help
```

It reports approximately how much disk space was freed, and prints each
removed directory as it goes. It never touches your actual notebooks, code,
or `requirements.txt` — only the cache/checkpoint categories listed above
(and `.venv/`, only when you explicitly pass `--deep`).
