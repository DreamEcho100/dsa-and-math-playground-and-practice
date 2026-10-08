#!/usr/bin/env python3
"""Free disk space by removing regenerable cache/checkpoint clutter.

Always removed, wherever found under the target directory:
    __pycache__/, *.pyc, .ipynb_checkpoints/,
    .pytest_cache/, .mypy_cache/, .ruff_cache/

.venv/ and .git/ are never walked: a normal run leaves .venv/ completely
alone (including the __pycache__/*.pyc inside it, which are part of the
installed packages' normal state, and walking thousands of venv files would be
slow too).

--deep additionally removes .venv/, but only when the target is the project
root (a venv only ever lives there). It's fully regenerable:
    python3 -m venv .venv
    .venv/bin/pip install -r requirements.txt
    .venv/bin/python -m ipykernel install --user --name=udemy-master-math-by-coding-in-python --display-name "Master Math by Coding in Python"
The last step re-registers the Jupyter kernel, which points at the venv's
Python and so stops working once the venv is deleted. (Or unregister it for
good: python3 -m jupyter kernelspec remove -f udemy-master-math-by-coding-in-python)

Uses only the standard library on purpose, so it runs with any python3 -
including before the project's own .venv exists.
"""

from __future__ import annotations

import argparse
import os
import shutil
import sys
from pathlib import Path

REMOVABLE_DIRS = {
    "__pycache__",
    ".ipynb_checkpoints",
    ".pytest_cache",
    ".mypy_cache",
    ".ruff_cache",
}
REMOVABLE_FILE_SUFFIXES = {".pyc"}
SKIPPED_DIRS = {".venv", ".git"}


def dir_size(path: Path) -> int:
    """Total size in bytes of every regular file under path, recursively."""
    return sum(
        entry.stat().st_size
        for entry in path.rglob("*")
        if entry.is_file() and not entry.is_symlink()
    )


def human_size(num_bytes: int) -> str:
    size = float(num_bytes)
    for unit in ("B", "KB", "MB", "GB"):
        if size < 1024 or unit == "GB":
            return f"{size:.1f}{unit}"
        size /= 1024
    return f"{size:.1f}GB"


def remove_dir(path: Path) -> int:
    freed = dir_size(path)
    shutil.rmtree(path)
    print(f"  removed {path}")
    return freed


def remove_file(path: Path) -> int:
    freed = path.stat().st_size
    path.unlink()
    return freed


def cleanup(target: Path) -> int:
    """Remove all regenerable clutter under target, returning bytes freed.

    Walks with os.walk so .venv/ and .git/ can be pruned instead of descended
    into; clutter directories are removed as found and not descended into.
    """
    freed = 0

    for current, dirnames, filenames in os.walk(target):
        current_path = Path(current)
        keep = []
        for name in dirnames:
            if name in SKIPPED_DIRS:
                continue
            if name in REMOVABLE_DIRS:
                freed += remove_dir(current_path / name)
            else:
                keep.append(name)
        dirnames[:] = keep

        for name in filenames:
            path = current_path / name
            if path.suffix in REMOVABLE_FILE_SUFFIXES and path.is_file():
                freed += remove_file(path)

    return freed


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Free disk space by removing regenerable cache/checkpoint clutter.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "examples:\n"
            "  scripts/cleanup.py                            whole project, keep .venv\n"
            "  scripts/cleanup.py sections/2-arithmetic       just one section (or lesson)\n"
            "  scripts/cleanup.py --deep                      whole project + remove .venv/\n"
        ),
    )
    parser.add_argument(
        "path",
        nargs="?",
        default=".",
        help="Limit cleanup to this directory, relative to the project root "
        "(e.g. a section or lesson folder). Default: the whole project.",
    )
    parser.add_argument(
        "--deep",
        action="store_true",
        help="Also remove .venv/ (project root only). Regenerate with: "
        "python3 -m venv .venv && .venv/bin/pip install -r requirements.txt",
    )
    args = parser.parse_args()

    project_root = Path(__file__).resolve().parent.parent
    target = (project_root / args.path).resolve()

    if not target.is_dir():
        parser.error(f"'{args.path}' is not a directory (relative to {project_root})")

    print(f"Cleaning: {target}")
    freed = cleanup(target)

    if args.deep:
        if target != project_root:
            print(
                f"warning: --deep only removes .venv at the project root, "
                f"ignoring it for '{args.path}'",
                file=sys.stderr,
            )
        else:
            venv = project_root / ".venv"
            if venv.is_dir():
                print(
                    "Removing .venv/ (regenerate with: python3 -m venv .venv "
                    "&& .venv/bin/pip install -r requirements.txt, then re-run "
                    "the ipykernel install from the README so the kernel "
                    "points at the new venv)"
                )
                freed += remove_dir(venv)

    print(f"Freed approximately: {human_size(freed)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
