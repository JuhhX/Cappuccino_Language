#ifndef VARIABLES_H
#define VARIABLES_H

#include <iostream>
#include <string>
#include <functional>

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
bool isClassDeclaration(string params);
void registerClass(string params);
bool isObjectDeclaration(string content);
void declareObject(string content, bool ignore_errors);
bool isObjectVariableUpdate(string content);
void updateObjectVariable(string content);
bool isObjectMethod(string content);
void executeObjectMethod(string content);

#endif // !VARIABLES_H
