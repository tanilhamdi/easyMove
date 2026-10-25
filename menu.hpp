#pragma once
#include <filesystem>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

class menu {
private:
  string option;
  string cfgPath;

public:
  menu(string option);
  vector<string> readcfg();
  void addPath(string path);
  void fzf(string input);
  void deletePath(string path);
  void move(string targetFile);
  vector<fs::path> listDirectory();
};
