#include "menu.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

menu::menu(string option) {
  this->option = option;
  char *homeDir = getenv("HOME");
  if (homeDir != nullptr) {
    this->cfgPath = string(homeDir) + "/.config/easyMove/easyMove.cfg";
  }
}

vector<string> menu::readcfg() {
  vector<string> lines;
  ifstream file(this->cfgPath);

  if (!file.is_open()) {
    cerr << "[ERROR] Cfg file couldn't be opened! Path: " << this->cfgPath
         << "\n";
    return lines;
  }

  string line;
  while (getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    lines.push_back(line);
  }

  // Debug to see if it can read properly
  //  for (size_t i = 0; i < lines.size(); i++) {
  //    cout << "[DEBUG]:" << lines[i] << "\n";
  //  }

  return lines;
}

void menu::deletePath(string path) {
  vector<string> cfgContent = this->readcfg();
  vector<string> upadtedLines;
  string newcfgContent;
  for (int i = 0; i < cfgContent.size(); i++) {
    if (cfgContent[i] == path) {
      continue;
    } else {
      upadtedLines.push_back(cfgContent[i]);
    }
  }

  for (int i = 0; i < upadtedLines.size(); i++) {
    newcfgContent += upadtedLines[i];
    newcfgContent += "\n";
  }

  ofstream cfgFile(this->cfgPath);
  if (cfgFile.is_open()) {
    cfgFile << newcfgContent;
    cout << "Path deleted successfully!" << "\n";
  } else {
    cout << "ERROR: Could not open config file!" << "\n";
  }

  // cout << "[DEBUG] newcfgContetn: " << newcfgContent << "\n";
}

void menu::addPath(string path) {

  ofstream cfgFile(this->cfgPath, ios::app);
  if (cfgFile.is_open()) {
    cfgFile << path << "\n";
    cout << "Path added successfully!";
  } else {
    cout << "ERROR: Could not open config file!" << "\n";
  }
}

vector<fs::path> menu::listDirectory() {
  vector<string> paths = readcfg();
  vector<fs::path> files;
  for (int i = 0; i < paths.size(); i++) {
    if (!fs::exists(paths[i]) || !fs::is_directory(paths[i])) {
      std::cout << "invalid folder!\n";
      std::exit(1);
    }

    fs::path path = paths[i];
    fs::directory_iterator it(path);
    fs::directory_iterator end;

    while (it != end) {
      // cout << "[DEBUG] " << it->path() << "\n";
      files.push_back(it->path());
      ++it;
    }

    //    for (int i = 0; i < files.size(); i++) {
    //      cout << "[DEBUG] " << files[i] << "\n";
    //    }
  }

  return files;
}

void menu::fzf(string input) {
  vector<fs::path> ls = listDirectory();
  fs::path cfgDir = fs::path(this->cfgPath).parent_path();
  string tempInput = cfgDir / "fzf_input.txt";
  string tempOutput = cfgDir / "fzf_output.txt";

  ofstream inFile(tempInput);
  for (int i = 0; i < ls.size(); i++) {
    inFile << ls[i].string() << "\n";
  }
  inFile.close();

  string cmd = "fzf";
  if (!input.empty()) {
    cmd += " -q \"" + input + "\"";
  }

  cmd += " < " + tempInput + " > " + tempOutput;
  //  cout << "[DEBUG]" << cmd << "\n";

  int result = std::system(cmd.c_str());
  if (result != 0) {
    fs::remove(tempInput);
    fs::remove(tempOutput);
    cout << "Cancelled.\n";
    return;
  }

  ifstream outFile(tempOutput);
  string selectedPathStr;
  getline(outFile, selectedPathStr);
  outFile.close();

  fs::remove(tempInput);
  fs::remove(tempOutput);

  if (selectedPathStr.empty()) {
    return;
  }

  fs::path selectedFile(selectedPathStr);
  string pwd = fs::current_path();
  fs::path dst = pwd / selectedFile.filename();

  error_code ec;
  if (fs::exists(dst, ec)) {
    cout << dst << " already exists. Overwrite? (y/n): ";
    string answer;
    getline(cin, answer);

    if (answer != "y" && answer != "Y") {
      cout << "Cancelled.\n";
      return;
    }
    fs::remove_all(dst, ec);
  }

  fs::rename(selectedFile, dst);
  cout << "Successfully moved: " << dst.filename() << "\n";

  //  for (int i = 0; i < ls.size(); i++) {
  //    cout << "[DEBUG]" << ls[i].filename() << "\n";
  //  }
}

void menu::move(string targetFile) {

  string pwd = fs::current_path();

  vector<fs::path> files = listDirectory();
  int countTarget = 0;
  vector<fs::path> targetFiles;
  for (int i = 0; i < files.size(); i++) {
    if (files[i].filename() == targetFile) {
      targetFiles.push_back(files[i]);
      countTarget++;
    }
  }

  if (countTarget == 1) {

    fs::path dst = pwd / targetFiles[0].filename();
    error_code ec;
    if (fs::exists(dst, ec)) {
      cout << dst << " already exists. Overwrite? (y/n): ";
      string answer;
      getline(cin, answer);

      if (answer != "y" && answer != "Y") {
        cout << "Cancelled.\n";
        return;
      }

      fs::remove_all(dst, ec);
    }
    fs::rename(targetFiles[0], pwd / targetFiles[0].filename());
  } else if (countTarget == 0) {
    // cout << "No Such a File In Your Paths! \n";
    fzf(targetFile);
    return;
  } else {
    for (int i = 0; i < targetFiles.size(); i++) {
      cout << "[" << i << "]" << targetFiles[i] << "\n";
    }

    string userInput;
    cout << "Select the file you want to move." << "\n";
    cout << "->";
    if (!getline(cin, userInput)) {
      cerr << "\nNo input.\n";
      return;
    }
    int fileNumber;
    if (!userInput.empty() &&
        all_of(userInput.begin(), userInput.end(), ::isdigit)) {
      fileNumber = stoi(userInput);
    } else {
      cerr << "Error: input must be an integer.\n";
      return;
    }

    if (targetFiles.size() - 1 < fileNumber) {
      cerr << "Invalid Option!";
      std::exit(1);
    }

    fs::path dst = pwd / targetFiles[fileNumber].filename();
    error_code ec;
    if (fs::exists(dst, ec)) {
      cout << dst << " already exists." << "\n" << "Overwrite? (y/n):";
      string answer;
      getline(cin, answer);

      if (answer != "y" && answer != "Y") {
        cout << "Cancelled.\n";
        return;
      }

      fs::remove_all(dst, ec);
    }
    fs::rename(targetFiles[fileNumber],
               pwd / targetFiles[fileNumber].filename());
  }

  // cout << "[DEBUG] Count Target:" << countTarget << "\n";
}
