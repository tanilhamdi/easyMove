# easyMove

Move a file or folder into your current directory by name, without typing the path where it lives.

You save a few folders once (Downloads, Desktop, a USB mount...). After that, `easyMove report.pdf` finds `report.pdf` in any of them and moves it to wherever your terminal is. If the same name exists in several saved folders, you pick which one from a numbered list. If the name is not found at all, easyMove opens [fzf](https://github.com/junegunn/fzf) so you can search for it.

## Features

- Moves files **and folders** by exact name from your saved folders to the current directory
- Numbered menu when the same name exists in more than one saved folder
- fzf fallback: if the name is not found, fzf opens with your input as the starting query
- `--fzf` to browse everything in your saved folders interactively
- Asks before overwriting something with the same name in the current directory
- Manage saved folders from the command line (`--add`, `--delete`, `--list`)
- Small C++17 program, no libraries besides the standard library

## Requirements

- Linux (uses `$HOME` and `/`-style paths; macOS and Windows are untested)
- `g++` with C++17 support
- [`fzf`](https://github.com/junegunn/fzf) (optional, needed for `--fzf` and the not-found fallback)

Install fzf with your package manager, for example:

```sh
sudo apt install fzf      # Debian / Ubuntu
sudo pacman -S fzf        # Arch
brew install fzf          # macOS / Homebrew
```

## Install

```sh
git clone https://github.com/tanilhamdi/easyMove.git
cd easyMove
chmod +x install.sh
./install.sh
```

`install.sh` does the following:

1. Checks for `g++` and warns if `fzf` is missing (the install continues without it)
2. Builds the program into `build/easyMove`
3. Installs it to `~/.local/bin/easyMove`
4. Creates an empty config file at `~/.config/easyMove/easyMove.cfg` (an existing config is never touched)

If `~/.local/bin` is not in your `PATH`, the installer offers to add it to your `.zshrc` / `.bashrc` (it asks first). For fish it prints the `fish_add_path` command to run. After that, open a new terminal or run `source ~/.zshrc`, then try `easyMove --help`.

To use a different install location: `PREFIX=/some/path ./install.sh` (the binary goes to `/some/path/bin`).

### Build by hand

```sh
g++ -std=c++17 easyMove.cpp menu.cpp -o easyMove
```

If you build by hand, create the config folder and file yourself:

```sh
mkdir -p ~/.config/easyMove
touch ~/.config/easyMove/easyMove.cfg
```

### Uninstall

```sh
./install.sh --uninstall
```

This removes the binary and keeps your config. Delete `~/.config/easyMove/` yourself if you want it gone too. If the installer added `~/.local/bin` to your `PATH`, that line stays in your shell rc file.

## Usage

```
easyMove <filename>           Find <filename> in your saved folders and move it here
easyMove --add, -a [path]     Save a folder
easyMove --delete, -d <path>  Remove a saved folder (path exactly as shown by --list)
easyMove --list, -l           Show all saved folders
easyMove --fzf, -f            Browse your saved folders with fzf and move the selection here
easyMove --help, -h           Show this help
```

### Quick start

```sh
easyMove --add ~/Downloads
easyMove --add ~/Desktop
easyMove --list

cd ~/projects/myproject
easyMove report.pdf        # moves report.pdf here
```

Save folders with their **full path** (`~/Downloads` works because your shell expands it before easyMove sees it).

### When the name exists in several folders

```
$ easyMove target
[0]"/home/you/Downloads/target"
[1]"/home/you/Desktop/target"
Select the file you want to move.
->1
```

Type the number and press Enter. The chosen item is moved to the current directory.

### When the name is not found

If no saved folder contains that exact name, easyMove opens fzf with your input as the starting query, listing everything in your saved folders. Pick an entry with Enter, or press Esc to cancel.

```
$ easyMove repor
# fzf opens, pre-filled with "repor"
Successfully moved: "report.pdf"
```

You can also open fzf directly, without a query, with `easyMove --fzf`.

### When the name already exists in the current directory

```
"/home/you/projects/myproject/report.pdf" already exists. Overwrite? (y/n):
```

Only `y` or `Y` overwrites. Anything else cancels and leaves both files where they are.

## How it works

- Saved folders live in `~/.config/easyMove/easyMove.cfg`, one folder path per line. `--add` appends the path exactly as you typed it.
- Searching looks at the top level of each saved folder (it does not go into subfolders) and matches the **exact name**, case-sensitive. The fzf fallback lists the same top-level entries.
- Lines starting with `#` in the config are ignored. Note that `--delete` rewrites the file, which removes comment lines.
- You can also edit the config file by hand.
- The fzf fallback uses two temporary files in `~/.config/easyMove/` and removes them when it finishes.

## Known limitations

easyMove is stable for everyday use. These are the current rough edges, and they are planned for future updates:

- **Moving between different drives or partitions is not supported yet.** The move fails and the program exits with a filesystem error. If you had confirmed an overwrite, the existing item in the current directory is already removed at that point.
- If a saved folder no longer exists, `easyMove <filename>` stops with "invalid folder!". Fix it with `easyMove --delete <path>`.
- Names must match exactly (case-sensitive). Partial matching is only available through the fzf fallback.
- `--add` does not convert relative paths. Use full paths (or `~/...`), not `.`.
- If fzf is not installed, the fallback and `--fzf` print "Cancelled." instead of a clear "fzf not found" message.
- The config location is fixed at `~/.config/easyMove/easyMove.cfg`.
- `--delete` needs the path exactly as `--list` shows it.

## Roadmap

- Moving between drives (copy, then remove)
- Skip missing saved folders with a warning instead of stopping
- Store absolute paths in `--add`
- Clear error messages for move failures and missing fzf
- Tab autocomplete for file names (GNU readline)

## Contributing

Issues and pull requests are welcome. Bug reports with the exact command and output are especially useful.
