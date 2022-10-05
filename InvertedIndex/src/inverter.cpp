#include <map>
#include <queue>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <inverter.h>

using namespace std;


string build_inverted_index(string filename) {
  map<string, set<int>> invertedIndex;
  fstream texts;
  int count = 0;
  string output = "";
  texts.open(filename, ios::in);
  if(texts.is_open()) {
    string line;
    while(getline(texts, line)) {
      fstream stringfile;
      stringfile.open(line);
      string word;
      char ch;
      while(stringfile >> noskipws >> ch) {
        if(isalpha((int)ch)) {
          word.push_back(ch);
        } else {
          if(word.empty()) {
            continue;
          } else if(invertedIndex.find(word) == invertedIndex.end()) {
            set<int> docs;
            docs.insert(count);
            invertedIndex[word] = docs;
          } else {
            invertedIndex[word].insert(count);
          }
          word = "";
          continue;
        }
      }
      count++;
    }
  }
  for(map<string, set<int>>::iterator it = invertedIndex.begin(); it != invertedIndex.end(); it++) {
    output.append(it->first + ": ");
    set<int> newSet = it->second;
    int setCount = 0;
    for(set<int>::iterator itr = newSet.begin(); itr != newSet.end(); itr++){
      output.append(std::to_string(*itr));
      if(setCount == (int)(newSet.size() - 1)) {
        break;
      } 
      output.append(" ");
      setCount++;
    }
    output.append("\n");
  }
  return output;

}
