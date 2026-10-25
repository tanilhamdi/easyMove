#!/usr/bin/env bash
# easyMove installer: builds the program, installs it, and creates the config file.
set -euo pipefail

cd "$(dirname "$0")"

PREFIX="${PREFIX:-$HOME/.local}"
BIN_DIR="$PREFIX/bin"
CFG_DIR="$HOME/.config/easyMove"   # must match the path hardcoded in menu.cpp
CFG_FILE="$CFG_DIR/easyMove.cfg"

if [ "${1:-}" = "--uninstall" ]; then
  rm -f "$BIN_DIR/easyMove"
  echo "Removed $BIN_DIR/easyMove"
  echo "Your config was kept: $CFG_FILE"
  echo "If you added $BIN_DIR to your PATH via install.sh, that line was left in your shell rc."
  exit 0
fi

if ! command -v g++ >/dev/null 2>&1; then
  echo "Error: g++ not found. Install a C++17 compiler first." >&2
  exit 1
fi

# fzf is an optional dependency (used for --fzf and when a file is not found)
if ! command -v fzf >/dev/null 2>&1; then
  echo "Warning: 'fzf' is not found on your system." >&2
  echo "         easyMove's interactive selection and fallback features require fzf." >&2
  echo "         Install it via your package manager (e.g. sudo apt install fzf / sudo pacman -S fzf / brew install fzf)." >&2
  echo >&2
fi

echo "Building..."
mkdir -p build
g++ -std=c++17 -O2 easyMove.cpp menu.cpp -o build/easyMove

echo "Installing to $BIN_DIR ..."
mkdir -p "$BIN_DIR"
install -m 755 build/easyMove "$BIN_DIR/easyMove"

# Create the config, but never touch an existing one
mkdir -p "$CFG_DIR"
if [ ! -e "$CFG_FILE" ]; then
  touch "$CFG_FILE"
  echo "Created config: $CFG_FILE"
else
  echo "Config already exists, left untouched: $CFG_FILE"
fi

# The last line printed depends on whether easyMove can already be found
FINAL="Done. Try: easyMove --help"

add_to_path() {
  case "${SHELL:-}" in
    */zsh|zsh)   RC="${ZDOTDIR:-$HOME}/.zshrc" ;;
    */bash|bash) RC="$HOME/.bashrc" ;;
    */fish|fish)
      echo "fish shell: run  fish_add_path $BIN_DIR"
      FINAL="Done. Run the fish_add_path command above, then try: easyMove --help"
      return ;;
    *)
      echo "$BIN_DIR is not in your PATH."
      FINAL="Done. Add $BIN_DIR to your PATH, then try: easyMove --help"
      return ;;
  esac

  LINE="export PATH=\"$BIN_DIR:\$PATH\""
  if grep -qsF "$LINE" "$RC"; then
    FINAL="Done. $BIN_DIR is already set up in $RC.
Open a new terminal (or run: source $RC), then try: easyMove --help"
    return
  fi

  read -r -p "$BIN_DIR is not in your PATH. Add it to $RC? (y/n) " ans || ans=n
  if [ "$ans" = "y" ] || [ "$ans" = "Y" ]; then
    printf "\n# added by easyMove install.sh\n%s\n" "$LINE" >> "$RC"
    echo "Added to $RC."
    FINAL="Done. Open a new terminal (or run: source $RC), then try: easyMove --help"
  else
    echo "Skipped. To do it yourself, add this line to $RC:"
    echo "  $LINE"
    FINAL="Done. After adding that line, open a new terminal, then try: easyMove --help"
  fi
}

case ":$PATH:" in
  *":$BIN_DIR:"*) ;;
  *) add_to_path ;;
esac

echo "$FINAL"
