#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include <iostream>
#include <string>
#include <functional>

#include "General.h"
#include "BasicUtilities.h"
#include "StringUtilities.h"

using namespace std;

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

#endif