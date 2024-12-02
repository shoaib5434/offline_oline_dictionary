#include "bits/stdc++.h"
#include "json.hpp"
#include "conio.h"

using namespace std;
using json = nlohmann::json;
#define PRESS_ANY_KEY                     \
  cout << "Press any key to continue..."; \
  _getch();
const string wordsFile = "words.txt";

struct TrieNode
{
  unordered_map<char, TrieNode *> children;
  bool isEndOfWord = false;
  string meaning = "";
};

class Trie
{
private:
  TrieNode *root;

  // helper function for collecting all suggestions
  void collectAllWords(TrieNode *node, string prefix, vector<string> &suggestions)
  {
    if (node->isEndOfWord)
    {
      suggestions.push_back(prefix);
    }
    for (auto child : node->children)
    {
      collectAllWords(child.second, prefix + child.first, suggestions);
    }
  }

public:
  Trie()
  {
    root = new TrieNode();
  }
  // insert word and meaning to trie
  void insert(const string&word, const string &meaning)
  {
    TrieNode *node = root;
    for (char c : word)
    {
      if (!node->children[c])
      {
        node->children[c] = new TrieNode();
      }
      node = node->children[c];
    }
    node->isEndOfWord = true;
    node->meaning = meaning;
  }
  // function to return suggestions given a prefix
  vector<string> getSuggestions(const string &prefix)
  {
    vector<string> suggestions;
    if (prefix == "")
      return suggestions;
    TrieNode *node = root;
    for (char c : prefix)
    {
      if (!node->children.count(c))
        return {}; // no suggestions found
      node = node->children[c];
    }
    collectAllWords(node, prefix, suggestions);
    return suggestions;
  }
  // function to return meaning of a word if store in trie
  string getMeaning(const string &word)
  {
    auto node = root;
    for (auto c : word)
    {
      if (!node->children.count(c))
        return "";
      node = node->children[c];
    }
    return node->isEndOfWord ? node->meaning : "";
  }

  void loadFromFile(const string &filename)
  {
    ifstream file(filename);
    string word, meaning;
    while (!file.eof())
    {
      string line;
      getline(file, line);
      int spacePosition = line.find(' ');
      word = line.substr(0, spacePosition);
      meaning = line.substr(spacePosition + 1);
      insert(word, meaning);
    }
    file.close();
  }
  // save a new ord to file and also insert to trie
  void saveWord(const string &word, const string &meaning)
  {
    bool wordExist = this->getMeaning(word) == "" ? false : true;
    if (!wordExist)
    {
      ofstream file(wordsFile, ios::app);
      file << word << ' ' << meaning << endl;
      insert(word, meaning);
    }
  }
};
// function to display suggestions
void displaySuggestions(const vector<string> &suggestions)
{
  if (suggestions.empty())
  {
    cout << "-- No suggestions found. " << endl;
  }
  else
  {
    for (const auto &suggestion : suggestions)
    {
      cout << "--> " << suggestion << endl;
    }
  }
}
// api call function to fetch meaning from dictionaryapi.dev
string fetchMeaning(const string &word, Trie &trie)
{
  string meaning = trie.getMeaning(word);
  if (meaning != "")
    return meaning;
  // used system curl
  string command = "curl -s https://api.dictionaryapi.dev/api/v2/entries/en/" + word;
  char buffer[128];
  string result = "";
  FILE *fp = _popen(command.c_str(), "r");
  if (fp)
  {
    // read data from buffer
    while (fgets(buffer, sizeof(buffer), fp))
    {
      result += buffer;
    }
    _pclose(fp);
  }
  return result;
}

int main()
{
  Trie trie;
  trie.loadFromFile(wordsFile);
  system("cls");
  cout << " *** Suggestions will appear hear " << endl
       << endl
       << "Enter Word (exit to exit the program): ";
  string prefix = "";
  char ch;
  vector<string> suggestions;
  while (true)
  {
    ch = _getch(); // get input on keypress

    if (ch == '\r')
    { // if user press enter
      system("cls");
      if (prefix == "exit")
      {
        cout << "See you soon! Goodbye.";
        exit(0);
      }
      string meaning = "";
      if (!suggestions.empty())
        prefix = suggestions[0];
      cout << endl
           << "Searching meaning of " << prefix << "...." << endl;
      try
      {
        string meaning = fetchMeaning(prefix, trie);
        system("cls");
        cout << "***   " << prefix << "  ***" << endl;
        json response;
        if (suggestions.empty())
          response = json::parse(meaning);
        if (suggestions.empty() && response.contains("title") && response["title"] == "No Definitions Found")
        {
          cout << "No meanings found." << endl;
        }
        else
        {
          if (suggestions.empty())
            meaning = response[0]["meanings"][0]["definitions"][0]["definition"];
          cout << "Meaning: " << meaning << endl;
          trie.saveWord(prefix, meaning);
        }
        PRESS_ANY_KEY
        prefix = "";
      }
      catch (const json::exception &e)
      {
        ;
        cout << "Unkown error occured. Please try again." << endl;
        PRESS_ANY_KEY
        prefix = "";
      }
    }
    else if (ch == '\b' && !prefix.empty())
      prefix.pop_back(); // if backspace is pressed
    else if (isalpha(ch))
      prefix += tolower(ch);
    system("cls");
    suggestions = trie.getSuggestions(prefix); // fetch suggestions
    displaySuggestions(suggestions);
    cout << "\nEnter Word (exit to exit the program): " << prefix;
  }

  return 0;
}
