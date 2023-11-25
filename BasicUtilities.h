#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
using namespace std;

static struct Colors {
    const string BLACK = "\033[1;30m";
    const string RED = "\033[1;31m";
    const string GREEN = "\033[1;32m";
    const string YELLOW = "\033[1;33m";
    const string BLUE = "\033[1;34m";
    const string MAGENTA = "\033[1;35m";
    const string CYAN = "\033[1;36m";
    const string WHITE = "\033[1;37m";
    const string RESET = "\033[1;0m";
} colors;

static struct ErrorsType {
    const string SEMICOLON = "Error: \";\" expected";
    const string STATEMENT = "Error: Not a statement";
    const string UNCLOSED_STRING = "Error: illegal start of expression";
    const string UNCLOSED_PARENTHESIS = "Error: unclosed expression";
    const string VARIABLE_NOT_EXISTS = "Error: variable doesn't exist";
    const string INVALID_PARAMETER = "Error: the parameter don't match";
    const string UNDEFINED_FUNCTION = "Error: the function doesn't exist";
    const string INCORRECT_TYPE = "Error: the assigned type doesn't match";
    const string NAME_NOT_ALLOWED = "Error: This name is not allowed";
    const string MISSING_PARAMETERS = "Error: It's missing some parameter(s)";
} errors;

#endif // !UTILITIES

