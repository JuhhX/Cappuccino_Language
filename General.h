#ifndef GENERAL_H
#define GENERAL_H

#include <map>
#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <stack>

#include "Variables.h"
#include "NativeFunctions.h"

using namespace std;

static ifstream * file_reference;

static string filename;
static int current_line = 1;

static bool read_without_execute = false;

union FunctionsRetruns
{
	int returned_int;
	double returned_double;
	bool returned_boolean;
	char returned_char;
};

extern map<string, int> integer_variables;
extern map<string, double> double_variables;
extern map<string, bool> boolean_variables;
extern map<string, char> char_variables;
extern map<string, string> string_variables;

extern vector<string> variables_names;

struct FunctionsAssignment
{
	string visibility;
	string acessibility;
	string return_type;
	string name;
	string params;
	size_t start_in_line;

	FunctionsAssignment() = default;
};

class Scope {
	public:
		string readed_condition = "";
		string variable_wait_value;
		string scope_type;
		string step_for;

		int block_count = 0;
		size_t back_loop_in_position = 0;
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

		void push_to_scope_variables(string name) {
			scope_variables.push_back(name);
		}

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

extern stack<Scope> scopes;

static map<string, FunctionsAssignment> custom_functions;

void executeCustomFunction(string content);
bool isCustomFunction(string content, bool ignore_error);
void interpreterLine(string content);
void interpreterLineAndFindEnd(string content);
void interpreterLineInMethod(string content);
void throwError(string error_type, string line_content);
bool isParenthesesOk(string content);

string getFilename();
void setFilename(string f);
void readFile(string code_name);

#endif
