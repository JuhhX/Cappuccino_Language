#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H

#include <string>
#include <vector>
#include <iostream>

#include "Evaluations.h"
#include "Variables.h"

using namespace std;

string trim(string s);
string trimStart(string s);
string trimEnd(string s);
vector<string> split(string s, char separator);
vector<string> split(string s, string separator);
vector<string> splitFirst(string s, char separator);
vector<string> splitWhenParenthesesClosed(string s, char separator);
vector<string> splitLast(string s, char separator);

string resolveFormatExpression(string params);
string formatInString(string s);
void resolveTab(string t, int retreat);
void resolveLineBreak(string t, int retreat);

bool isString(string s);
bool startsWith(string s, string t);
bool endsWith(string s, string t);

#endif