#ifndef GENERAL_H
#define GENERAL_H

#include <map>
#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <stack>
using namespace std;

static ifstream * file_reference;

static string filename;
static int current_line = 1;

static bool read_without_execute = false;

static union FunctionsRetruns
{
	int returned_int;
	double returned_double;
	bool returned_boolean;
	char returned_char;
};

static map<string, int> integer_variables;
static map<string, double> double_variables;
static map<string, bool> boolean_variables;
static map<string, char> char_variables;
static map<string, string> string_variables;

static vector<string> variables_names;

static struct FunctionsAssignment
{
	string visibility;
	string acessibility;
	string return_type;
	string name;
	string params;
	int start_in_line;
};

class Scope {
	public:
		string readed_condition = "";
		string variable_wait_value;
		string scope_type;
		string step_for;

		int block_count = 0;
		int back_loop_in_line = 0;

		bool execute_block = false;
		bool method_declaration = false;
		bool satisfied_condition = false;
		bool search_block_end = false;
		bool search_block_end_while = false;
		bool continue_from_end_while = false;
		bool search_block_end_for = false;
		bool search_block_end_method = false;
		bool verify_next_line = false;

		FunctionsRetruns returned;
		string returned_string;

		string current_return_type;

		vector<string> scope_variables;

		FunctionsAssignment * method_reference;

		~Scope() {

			for (string var : scope_variables) {
				if (integer_variables.count(var))
					integer_variables.erase(var);
				else if (double_variables.count(var))
					double_variables.erase(var);
				else if(boolean_variables.count(var))
					boolean_variables.erase(var);
				else if(char_variables.count(var))
					char_variables.erase(var);
				else if(string_variables.count(var))
					string_variables.erase(var);

				variables_names.pop_back();
			}

		}
		
};

static map<string, function<void(string)>> functions;

static stack<Scope> scopes;

static map<string, FunctionsAssignment> custom_functions;

void print(string params);
void println(string params);
bool isVariableDeclaration(string content);
void declareVariable(string content, bool update);
void executeNativeFunction(string content);
void executeCustomFunction(string content);
bool isParenthesesOk(string content);
bool isNativeFunction(string content, bool ignore_error);
bool isCustomFunction(string content, bool ignore_error);
void interpreterLine(string content);
void interpreterLineAndFindEnd(string content);
void throwError(string error_type, string line_content);

string resolveAssignment(string params);
string resolveEvaluation(string params);

string getFilename();
void setFilename(string f);
void initializeFunctions();
void readFile(string code_name);

#endif
