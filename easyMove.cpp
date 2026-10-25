#include "menu.hpp"
#include <iostream>
#include <string>
#include <vector>

static void printHelp() {
  std::cout
      << R"(easyMove - move a file from your saved folders to the current directory

Usage:
  easyMove <filename>           Find <filename> in your saved folders and move it here
  easyMove --latest, -n         Move the most recently modified file from your saved folders
  easyMove --fzf, -f            Pick a file from your saved folders with fzf
  easyMove --add, -a <path>     Save a folder
  easyMove --delete, -d <path>  Remove a saved folder (path exactly as shown by --list)
  easyMove --list, -l           Show all saved folders
  easyMove --help, -h           Show this help

If the same filename exists in several saved folders, you will get a numbered
list and can pick which one to move. If the name is not found, fzf opens with
your input as the starting query.

--latest ignores folders, hidden files and unfinished downloads
(.part, .crdownload).

Config file:
  ~/.config/easyMove/easyMove.cfg
  One folder path per line.
)";
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printHelp();
    return 1;
  }

  std::string arg = argv[1];
  menu mainMenu("-");

  if (arg == "--help" || arg == "-h") {
    printHelp();
    return 0;
  }

  if (arg == "--list" || arg == "-l") {
    std::vector<std::string> paths = mainMenu.readcfg();
    if (paths.empty()) {
      std::cout
          << "No saved folders yet. Add one with: easyMove --add <folder>\n";
    }
    for (size_t i = 0; i < paths.size(); i++) {
      std::cout << paths[i] << "\n";
    }
    return 0;
  }

  if (arg == "--fzf" || arg == "-f") {
    mainMenu.fzf("");
    return 0;
  }

  if (arg == "--latest" || arg == "-n") {
    mainMenu.latest();
    return 0;
  }

  if (arg == "--add" || arg == "-a") {
    if (argc < 3) {
      std::cerr << "Error: --add needs a path. Example: easyMove --add "
                   "~/Downloads\n";
      return 1;
    }
    mainMenu.addPath(argv[2]);
    return 0;
  }

  if (arg == "--delete" || arg == "-d") {
    if (argc < 3) {
      std::cerr
          << "Error: --delete needs a path. Use --list to see saved paths.\n";
      return 1;
    }
    mainMenu.deletePath(argv[2]);
    return 0;
  }

  if (arg[0] == '-') {
    std::cerr << "Unknown option: " << arg << "\n\n";
    printHelp();
    return 1;
  }

  mainMenu.move(arg);
  return 0;
}
