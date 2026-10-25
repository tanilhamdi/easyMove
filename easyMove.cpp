#include "menu.hpp"
#include <iostream>
#include <string>
#include <vector>

static void printHelp() {
  std::cout
      << R"(easyMove (alpha) - move a file from your saved folders to the current directory
  
  Usage:
    easyMove <filename>          Find <filename> in your saved folders and move it here
    easyMove --add, -a [path]    Save a folder (asks for the path if you do not give one)
    easyMove --delete, -d <path> Remove a saved folder (path exactly as shown by --list)
    easyMove --list, -l          Show all saved folders
    easyMove --fzf, f            Uses fzf for finding the files you want to move easyly
    easyMove --help, -h          Show this help

  
  If the same filename exists in several saved folders, you will get a numbered
  list and can pick which one to move.
  
  Config file:
    ~/.config/easyMove/easyMove.cfg
    One folder path per line. Created automatically on the first --add.
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

  if (arg == "--add" || arg == "-a") {
    mainMenu.addPath(argc > 2 ? argv[2] : "");
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
