#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H

#include <string>
#include <vector>
using namespace std;

string trim(string s);
string trimStart(string s);
string trimEnd(string s);
vector<string> split(string s, char separator);
vector<string> split(string s, string separator);
vector<string> splitFirst(string s, char separator);
vector<string> splitWhenParenthesesClosed(string s, char separator);
vector<string> splitLast(string s, char separator);

bool isString(string s);
bool startsWith(string s, string t);
bool endsWith(string s, string t);

#endif