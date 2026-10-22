# easyMove

> **Alpha (v0.1.0-alpha).** It works, but expect rough edges. See [Known issues](#known-issues).

Move a file or folder into your current directory by name, without typing the path where it lives.

You save a few folders once (Downloads, Desktop, a USB mount...). After that, `easyMove report.pdf` finds `report.pdf` in any of them and moves it to wherever your terminal is. If the same name exists in several saved folders, you pick which one from a numbered list.

## Features

- Moves files **and folders** by exact name from your saved folders to the current directory
- Numbered menu when the same name exists in more than one saved folder
- Asks before overwriting something with the same name in the current directory
- Manage saved folders from the command line (`--add`, `--delete`, `--list`)
- Small C++17 program, no dependencies besides the standard library

## Requirements

- Linux (uses `$HOME` and `/`-style paths; macOS and Windows are untested)
- `g++` with C++17 support

## Install

```sh
git clone https://github.com/tanilhamdi/easyMove.git
cd easyMove
chmod +x install.sh
./install.sh
```

`install.sh` does three things:

1. Builds the program into `build/easyMove`
2. Installs it to `~/.local/bin/easyMove`
3. Creates an empty config file at `~/.config/easyMove/easyMove.cfg` (an existing config is never touched)

If `~/.local/bin` is not in your `PATH`, the installer offers to add it to your `.zshrc` / `.bashrc` (it asks first). After that, open a new terminal or run `source ~/.zshrc`, then try `easyMove --help`.

To use a different install location: `PREFIX=/some/path ./install.sh` (the binary goes to `/some/path/bin`).

### Build by hand

```sh
g++ -std=c++17 easyMove.cpp menu.cpp -o easyMove
```

If you build by hand, create the config folder yourself: `mkdir -p ~/.config/easyMove`.

### Uninstall

```sh
./install.sh --uninstall
```

This removes the binary and keeps your config. Delete `~/.config/easyMove/` yourself if you want it gone too.

## Usage

```
easyMove <filename>           Find <filename> in your saved folders and move it here
easyMove --add, -a [path]     Save a folder (asks for the path if you do not give one)
easyMove --delete, -d <path>  Remove a saved folder (path exactly as shown by --list)
easyMove --list, -l           Show all saved folders
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

### When the name exists in several folders

```
$ easyMove target
[0]"/home/you/Downloads/target"
[1]"/home/you/Desktop/target"
Select the file you want to move.
-> 1
Moved: "/home/you/Desktop/target" -> "/home/you/projects/myproject"
```

### When the name already exists in the current directory

```
"/home/you/projects/myproject/report.pdf" already exists. Overwrite? (y/n):
```

Only `y` or `Y` overwrites. Anything else cancels and leaves both files where they are.

## How it works

- Saved folders live in `~/.config/easyMove/easyMove.cfg`, one folder path per line. `--add` stores the full absolute path, so `easyMove --add .` works from any directory.
- Searching looks at the top level of each saved folder (it does not go into subfolders) and matches the **exact name**, case-sensitive.
- Lines starting with `#` in the config are ignored. Note that `--delete` rewrites the file, which removes comment lines.
- You can also edit the config file by hand.

## Known issues

- Moving between different drives or partitions is not supported yet. You get an error and nothing is moved or deleted.
- If a saved folder no longer exists, `easyMove <filename>` stops with "invalid folder!". Fix it with `easyMove --delete <path>`.
- Names must match exactly (case-sensitive, no partial matching yet).
- The config location is fixed at `~/.config/easyMove/easyMove.cfg`.
- `--delete` needs the path exactly as `--list` shows it.

## Roadmap

- Tab autocomplete for file names (GNU readline)
- Partial and fuzzy name matching
- Moving between drives (copy, then remove)
- Skip missing saved folders with a warning instead of stopping

## Contributing

Issues and pull requests are welcome. This is an early alpha, so bug reports with the exact command and output are especially useful.
