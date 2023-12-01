#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include <iostream>
#include <string>
#include <functional>
#include <map>

#include "BasicUtilities.h"
#include "StringUtilities.h"

using namespace std;

static map<string, function<void(string)>> functions;

void print(string params);
void println(string params);
void printf_(string params);
void readLine(string params);
void readInt(string params);
void readDouble(string params);
void readBoolean(string params);
void readChar(string params);
void executeNativeFunction(string content);
bool isNativeFunction(string content, bool ignore_error);
void initializeFunctions();

#endif