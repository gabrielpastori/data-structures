// Copyright (c) 2022

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include "./quake_heap.h"

using std::vector;
using std::map;
using std::make_pair;
using std::cout;
using std::endl;
using std::cin;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::stringstream;
using std::exception;

string ProcessCommand(
  QuakeHeap<int, string> *heap,
  map<string, QuakeHeap<int, string>::Locator*>* locators,
  string input_line
  ) {
  string output_line = "";
  try {
      if (input_line.substr(0, 13) == "set-n-levels:") {
        output_line = input_line;
      } else if (input_line == "list-heap") {
        vector<string> response = heap->ListHeap();
        output_line = "list-heap:";
        for (int i = 0; i < response.size(); i++) {
          output_line = output_line + "\n  " + response[i];
        }
      } else if (input_line.substr(0, 6) == "insert") {
        stringstream ss(input_line);
        string key, value;
        getline(ss, value, ':');
        getline(ss, value, ':');
        getline(ss, key, ':');
        QuakeHeap<int, string>::Locator* new_loc = heap->Insert(
          stoi(key), value);
        locators->insert(make_pair(value, new_loc));
        output_line = "insert(" + key + ", " + value + "): successful";
      } else if (input_line == "clear") {
        heap->Clear();
        output_line = "clear: successful";
      } else if (input_line == "get-min-key") {
        int min = heap->GetMinKey();
        output_line = "get-min-key: " + to_string(min);
      } else if (input_line.substr(0, 14) == "get-max-level:") {
        stringstream ss(input_line);
        string value;
        getline(ss, value, ':');
        getline(ss, value, ':');
        QuakeHeap<int, string>::Locator* loc = (*locators)[value];
        int max_level = heap->GetMaxLevel(loc);
        output_line = "get-max-level(" + value + "): " + to_string(max_level);
      }
  } catch (exception& e) {
    output_line += (input_line + ": Failure due to exception: \""
      + e.what() + "\"");
  }
  return output_line;
}

void RunTests(
  map<string, QuakeHeap<int, string>::Locator*>* locators
  ) {
  string line;
  cin >> line;
  stringstream ss(line);
  string n_levels;
  getline(ss, n_levels, ':');
  getline(ss, n_levels, ':');
  QuakeHeap<int, string>* heap = new QuakeHeap<int, string>(stoi(n_levels));
  cout << line << endl;
  while (cin >> line) {
    string output_line = ProcessCommand(heap, locators, line);
    cout << output_line << endl;
  }
}


int main() {
  map<string, QuakeHeap<int, string>::Locator*> locators;
  RunTests(&locators);

  return 0;
}
