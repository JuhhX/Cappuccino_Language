#ifndef VARIABLES_H
#define VARIABLES_H

#include <iostream>
#include <string>
#include <functional>

#include "General.h"
#include "BasicUtilities.h"
#include "StringUtilities.h"

using namespace std;

void variableExecute(string s, function<void(string)> f);
string getVariableValue(string s, bool ignore_throw);
bool variableExists(string s);
string getTypeOfVariable(string s);
string getTypeOfData(string s);
bool isVariableDeclaration(string content);
bool isVector(string params);
int getVectorSlots(string params);
void declareVariable(string content, bool update);
void declareInt(vector<string> values, string type_identifier);
void declareDouble(vector<string> values, string type_identifier);
void declareBoolean(vector<string> values, string type_identifier);
void declareChar(vector<string> values, string type_identifier);
void declareString(vector<string> values, string type_identifier, string content);
void updateVariable(string params);
bool isVariableUpdate(string params);

#endif // !VARIABLES_H
