# C++ CLI Dictinoary tool
## About the tool
The simple tool provides an interface to enter the words and search for the meaning.
It gives real-time suggestions from offline stored words. If a word is not stored offline, it will be searched online and then stored offline in words.txt file for future use.
 ## Code
 The code is easy to understand. It uses Trie to store all the offline stored words and then let you search meanings offline for the words that are already saved.
 The code uses system CURL to call dictionary api to search for a word that is not on the trie.
