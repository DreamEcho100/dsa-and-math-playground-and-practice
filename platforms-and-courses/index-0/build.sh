#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Build one problem (all its C/C++ files) into <problem>/build/app.

Usage:
  build.sh [path]         path = any file or folder inside a problem (default: .)
  build.sh --pick         choose a problem from a list, then build it
  build.sh --last         rebuild the last built problem (asks if there is none)
  build.sh --root [path]  print the problem root that [path] belongs to
  build.sh --list         list every problem under index-0
  build.sh -h             show this help

Finding the problem root (walking up from path, never above index-0):
  1. the nearest folder containing a '.problem' file, otherwise
  2. the nearest folder with a top-level .c/.cc/.cpp/.cxx defining 'int main('

Building:
  - compiles every source in the root and its subfolders,
    skipping build/ and folders starting with '_' or '.' (e.g. _scratch/)
  - adds -I for the root and for every folder containing headers
  - exactly one main() is allowed
  - writes build/compile_commands.json (for clangd)

The picker uses fzf if it's installed, otherwise a numbered menu
(type a number, or part of a name; Enter = the first entry = last built).

Environment overrides:
  CC (gcc)  CXX (g++)  CSTD (c17)  CXXSTD (c++17)  EXTRA_FLAGS ("")

After a successful build, index-0/.last-build points to the problem root.
EOF
}

root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

CC="${CC:-gcc}"
CXX="${CXX:-g++}"
CSTD="${CSTD:-c17}"
CXXSTD="${CXXSTD:-c++17}"

common=(-Wall -Wextra -g -O0 -fsanitize=address,undefined -fno-sanitize-recover=all)
read -r -a extra <<<"${EXTRA_FLAGS:-}"

main_re='(^|[^[:alnum:]_])int[[:space:]]+main[[:space:]]*\('
skip=(-type d \( -name build -o -name '.*' -o -name '_*' -o -name node_modules \) -prune)
is_source=(\( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \))
is_header=(\( -name '*.h' -o -name '*.hh' -o -name '*.hpp' -o -name '*.hxx' \))

die() {
  echo "build.sh: $*" >&2
  exit 1
}

defines_main() {
  grep -qE -- "$main_re" "$1"
}

has_top_level_main() {
  local f
  for f in "$1"/*.c "$1"/*.cc "$1"/*.cpp "$1"/*.cxx; do
    if [[ -f "$f" ]] && defines_main "$f"; then
      return 0
    fi
  done
  return 1
}

# Prints the problem root for a file or folder.
# Returns 2 if the path doesn't exist, 1 if no problem root was found.
find_root() {
  local start p
  start="$(realpath -e -- "$1" 2>/dev/null)" || return 2
  [[ -d "$start" ]] || start="$(dirname -- "$start")"
  case "$start/" in "$root"/*) ;; *) return 1 ;; esac

  p="$start"
  while :; do
    if [[ -e "$p/.problem" ]]; then
      echo "$p"
      return 0
    fi
    [[ "$p" == "$root" ]] && break
    p="$(dirname -- "$p")"
  done

  p="$start"
  while :; do
    if has_top_level_main "$p"; then
      echo "$p"
      return 0
    fi
    [[ "$p" == "$root" ]] && break
    p="$(dirname -- "$p")"
  done
  return 1
}

relative() {
  if [[ "$1" == "$root" ]]; then echo "."; else echo "${1#"$root"/}"; fi
}

list_problems() {
  {
    find "$root" "${skip[@]}" -o -type f -name .problem -printf '%h\n'
    find "$root" "${skip[@]}" -o -type f "${is_source[@]}" -print0 |
      xargs -0 -r grep -lE -- "$main_re" |
      while IFS= read -r f; do dirname -- "$f"; done
  } | sort -u |
    while IFS= read -r d; do find_root "$d" || true; done |
    sort -uV |
    while IFS= read -r d; do relative "$d"; done
}

# Prints the last built problem (relative), or nothing.
last_built() {
  local p
  if p="$(realpath -e -- "$root/.last-build" 2>/dev/null)"; then
    relative "$p"
  fi
}

# Shows a list in the terminal and prints the chosen problem (relative).
# The menu goes to stderr, so only the choice ends up on stdout.
pick_problem() {
  local items=() rest=() matches=() last item reply tag i
  [[ -t 0 ]] || die "--pick needs an interactive terminal"

  mapfile -t items < <(list_problems)
  ((${#items[@]} > 0)) || die "no problems found under $root"

  last="$(last_built)"
  if [[ -n "$last" ]]; then
    for item in "${items[@]}"; do
      [[ "$item" == "$last" ]] || rest+=("$item")
    done
    if ((${#rest[@]} < ${#items[@]})); then
      items=("$last" "${rest[@]}")
    fi
  fi

  if command -v fzf >/dev/null 2>&1; then
    reply="$(printf '%s\n' "${items[@]}" | fzf --prompt='problem> ' --height=40% --reverse)" ||
      die "no problem picked"
    echo "$reply"
    return 0
  fi

  echo "Which problem?" >&2
  for i in "${!items[@]}"; do
    tag=""
    [[ "${items[i]}" == "$last" ]] && tag="   (last built)"
    printf '%3d) %s%s\n' "$((i + 1))" "${items[i]}" "$tag" >&2
  done

  while :; do
    read -r -p "Number or part of a name [1], q to cancel: " reply || die "no problem picked"
    reply="${reply:-1}"
    [[ "$reply" == q ]] && die "no problem picked"

    if [[ "$reply" =~ ^[0-9]+$ ]]; then
      if ((reply >= 1 && reply <= ${#items[@]})); then
        echo "${items[reply - 1]}"
        return 0
      fi
      echo "  pick a number between 1 and ${#items[@]}" >&2
      continue
    fi

    matches=()
    for item in "${items[@]}"; do
      [[ "$item" == *"$reply"* ]] && matches+=("$item")
    done
    if ((${#matches[@]} == 1)); then
      echo "${matches[0]}"
      return 0
    fi
    if ((${#matches[@]} == 0)); then
      echo "  nothing matches '$reply'" >&2
    else
      echo "  '$reply' matches ${#matches[@]} problems, be more specific:" >&2
      printf '       %s\n' "${matches[@]}" >&2
    fi
  done
}

json_str() {
  local s="${1//\\/\\\\}"
  s="${s//\"/\\\"}"
  printf '"%s"' "$s"
}

json_entry() {
  local dir="$1" file="$2" args="" a
  shift 2
  for a in "$@"; do
    args+="${args:+, }$(json_str "$a")"
  done
  printf '  {"directory": %s, "file": %s, "arguments": [%s]}' \
    "$(json_str "$dir")" "$(json_str "$file")" "$args"
}

target="."
pick=0
use_last=0
run=0
app_args=()
while (($#)); do
  case "$1" in
    --list) list_problems; exit 0 ;;
    --root) find_root "${2:-.}" || exit $?; exit 0 ;;
    --pick) pick=1 ;;
    --last) use_last=1 ;;
    --run) run=1 ;;
    --) shift; app_args=("$@"); break ;;
    -h | --help) usage; exit 0 ;;
    -*) die "unknown option '$1' (try -h)" ;;
    *) target="$1" ;;
  esac
  shift
done


if ((use_last)); then
  last="$(last_built)"
  if [[ -n "$last" ]]; then
    target="$root/$last"
  elif [[ -t 0 ]]; then
    echo "build.sh: nothing has been built yet (or the last built folder was renamed/moved)." >&2
    pick=1
  else
    die "nothing has been built yet (or the last built folder was renamed/moved). Build or pick a problem first."
  fi
fi

if ((pick)); then
  choice="$(pick_problem)" || exit 1
  echo "picked: $choice" >&2
  target="$root/$choice"
fi

rc=0
problem="$(find_root "$target")" || rc=$?
if ((rc == 2)); then
  if [[ "$target" == *.last-build* ]]; then
    die "nothing has been built yet (or the last built folder was renamed/moved). Build or pick a problem first."
  fi
  die "'$target' does not exist"
elif ((rc != 0)); then
  die "no problem found at or above '$target'. A problem folder needs a top-level source file with 'int main(' or a '.problem' file. (Is the active editor a file inside a problem?)"
fi

mapfile -d '' sources < <(
  find "$problem" "${skip[@]}" -o -type f "${is_source[@]}" -print0 | sort -zV
)

mains=()
for src in "${sources[@]}"; do
  if defines_main "$src"; then
    mains+=("${src#"$problem"/}")
  fi
done
if ((${#mains[@]} > 1)); then
  printf 'build.sh: more than one main() in %s:\n' "$(relative "$problem")" >&2
  printf '  %s\n' "${mains[@]}" >&2
  die "keep one; move experiments into a folder starting with '_' (e.g. _scratch/)"
fi

includes=(-I"$problem")
mapfile -t header_dirs < <(
  find "$problem" "${skip[@]}" -o -type f "${is_header[@]}" -printf '%h\n' | sort -u
)
for d in "${header_dirs[@]}"; do
  [[ "$d" == "$problem" ]] || includes+=(-I"$d")
done

out="$problem/build"
obj="$out/obj"
rm -rf "$obj"
mkdir -p "$obj"

linker="$CC"
objects=()
entries=()
failed=0

for src in "${sources[@]}"; do
  rel="${src#"$problem"/}"
  o="$obj/${rel//\//__}.o"
  case "$src" in
    *.c)
      cmd=("$CC" -std="$CSTD" "${common[@]}" "${extra[@]}" "${includes[@]}" -c "$src" -o "$o")
      ;;
    *)
      cmd=("$CXX" -std="$CXXSTD" "${common[@]}" -D_GLIBCXX_ASSERTIONS "${extra[@]}" "${includes[@]}" -c "$src" -o "$o")
      linker="$CXX"
      ;;
  esac
  entries+=("$(json_entry "$problem" "$src" "${cmd[@]}")")
  "${cmd[@]}" || failed=1
  objects+=("$o")
done

{
  echo "["
  printf '%s' "${entries[0]}"
  printf ',\n%s' "${entries[@]:1}"
  printf '\n]\n'
} >"$out/compile_commands.json"

((failed == 0)) || die "compilation failed"

"$linker" "${common[@]}" "${extra[@]}" "${objects[@]}" -o "$out/app"

ln -sfn "$problem" "$root/.last-build"

echo "built: $(relative "$problem") -> build/app (${#sources[@]} source(s), ${#includes[@]} include dir(s), linked with $linker)"

if ((run)); then
  echo "--- running $(relative "$problem")/build/app ---" >&2
  cd "$problem"
  exec "$out/app" "${app_args[@]}"
fi
