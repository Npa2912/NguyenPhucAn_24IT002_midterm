#!/bin/sh
# Portable functional regression checks, run with: make test
set -u
BASE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BIN="$BASE/myls"
TMP="${TMPDIR:-/tmp}/myls-test-$$"
PASS=0
FAIL=0
mkdir -p "$TMP/dir/sub" || exit 1
trap 'rm -rf "$TMP"' EXIT HUP INT TERM
printf 'a\n' > "$TMP/dir/a.txt"
printf 'bb\n' > "$TMP/dir/b.txt"
printf 'hidden\n' > "$TMP/dir/.hidden"
printf 'sub\n' > "$TMP/dir/sub/nested.txt"
# Distinct modification times prevent unstable same-second ties in -t comparisons.
touch -t 202001010101 "$TMP/dir/a.txt"
touch -t 202101010101 "$TMP/dir/b.txt"
touch -t 202201010101 "$TMP/dir/sub"
touch -t 202301010101 "$TMP/dir/.hidden"
ln -s a.txt "$TMP/dir/link" 2>/dev/null || :
check() {
  label=$1; shift
  if "$@"; then echo "PASS $label"; PASS=$((PASS+1));
  else echo "FAIL $label"; FAIL=$((FAIL+1)); fi
}
compare() {
  "$BIN" "$@" > "$TMP/actual" 2> "$TMP/error1" || return 1
  ls -1 "$@" > "$TMP/expected" 2> "$TMP/error2" || return 1
  diff -u "$TMP/expected" "$TMP/actual" >/dev/null
}
check basic compare "$TMP/dir"
check hidden compare -a "$TMP/dir"
check almost_all compare -A "$TMP/dir"
check reverse compare -r "$TMP/dir"
check size_sort compare -S "$TMP/dir"
check time_sort compare -t "$TMP/dir"
check classify compare -F "$TMP/dir"
check directory_only compare -d "$TMP/dir"
check recursive compare -R "$TMP/dir"
check inode compare -i "$TMP/dir"
check multiple_operands compare "$TMP/dir/a.txt" "$TMP/dir"
check missing_path sh -c '"$1" /path/that/does/not/exist >/dev/null 2>&1; [ "$?" -ne 0 ]' sh "$BIN"
check unknown_flag sh -c '"$1" -Z >/dev/null 2>&1; [ "$?" -ne 0 ]' sh "$BIN"
printf '\nResults: %s passed, %s failed\n' "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
