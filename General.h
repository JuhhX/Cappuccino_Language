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

extern ifstream * file_reference;

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

void throwError(string error_type, string line_content);

struct FunctionsAssignment
{
    string visibility;
    string acessibility;
    string return_type;
    string name;
    string params;
    size_t start_in_line;
};

class Object {
	public:
		string type_name;
		map<string, int> integer_variables;
		map<string, double> double_variables;
		map<string, bool> boolean_variables;
		map<string, char> char_variables;
		map<string, string> string_variables;

        map<string, FunctionsAssignment> custom_functions;

		vector<string> variables_names;

		vector<string> public_symbols;
		vector<string> private_symbols;

		string getVariableValue(string var_name) {
			if (existsVariable(var_name)) {
				string type = getTypeOfVariable(var_name);

				if (type == "Int") {
					return to_string(this->integer_variables[var_name]);
				}
				else if (type == "Double") {
					return to_string(this->double_variables[var_name]);
				}
				else if (type == "String") {
					return (this->string_variables[var_name]);
				}
				else if (type == "Boolean") {
					return to_string(this->boolean_variables[var_name]);
				}
				else if (type == "Char") {
					return to_string(this->char_variables[var_name]);
				}

			}
			else {
				throwError(errors.CLASS_DOESNT_CONTAINS_VARIABLE, var_name);
			}

            return "Undefined";
		}

        //Declara a variável inteira
        void declareInt(vector<string> values, string accessibility, string type_identifier) {
            //Valor atribuido
            if (values.size() == 2) {
                //Atualização de vetores
                if (isVector(values[0])) {
                    int slots = getVectorSlots(trim(values[0]));
                    values[0] = splitFirst(values[0], '[')[0];
                    this->integer_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = stoi(resolveAssignment(values[1]));
                }
                else
                    this->integer_variables[values[0]] = stoi(resolveAssignment(values[1]));
            }
            //Apenas declaração
            else {
                if (isVector(type_identifier)) {
                    int slots = getVectorSlots(trim(type_identifier));

                    for (int x = 0; x < slots; x++) {
                        string indice = to_string(x);
                        this->declareVariable("Int " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", accessibility, false);
                    }
                }
                else
                    this->integer_variables[values[0]] = 0;
            }
        }

        //Declara a variavel double
        void declareDouble(vector<string> values, string accessibility, string type_identifier) {
            if (values.size() == 2) {
                
                //Atualização de vetores
                if (isVector(values[0])) {
                    int slots = getVectorSlots(trim(values[0]));
                    values[0] = splitFirst(values[0], '[')[0];
                    this->double_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = stod(resolveAssignment(values[1]));
                }
                else
                    this->double_variables[values[0]] = stod(resolveAssignment(values[1]));
                
            }
            else {
                if (isVector(type_identifier)) {
                    int slots = getVectorSlots(trim(type_identifier));

                    for (int x = 0; x < slots; x++) {
                        this->declareVariable("Double " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", accessibility, false);
                    }
                }
                else {
                    this->double_variables[values[0]] = 0.0;
                }
            }
        }

        //Declara a variavel String
        void declareString(vector<string> values, string type_identifier, string accessibility, string content) {
            if (values.size() == 2)
               
                //Atualização de vetores
                if (isVector(values[0])) {
                    int slots = getVectorSlots(trim(values[0]));
                    values[0] = splitFirst(values[0], '[')[0];

                    string string_value = resolveAssignmentString(values[1]);

                    this->string_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = (string_value.substr(1, string_value.length() - 3));
                }
                else if (variableExists(values[1]) || values[1].find("+") != string::npos)
                    this->string_variables[values[0]] = resolveAssignmentString(values[1].substr(0, values[1].length() - 1));
                else if (values[1].find("\"") == 0 && values[1].rfind("\"") == values[1].length() - 2)
                    //Desconsidera as aspas e o ;
                    this->string_variables[values[0]] = resolveAssignmentString(values[1].substr(1, values[1].length() - 3));
                else {
                    throwError(errors.UNCLOSED_STRING, content);
                    throw - 1;
                }
                
            else {
                if (isVector(type_identifier)) {
                    int slots = getVectorSlots(trim(type_identifier));

                    for (int x = 0; x < slots; x++) {
                        this->declareVariable("String " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", accessibility, false);
                    }
                }
                else {
                    string_variables[values[0]] = "";
                }
            }
        }

        //Declara a variavel booleana
        void declareBoolean(vector<string> values, string accessibility, string type_identifier) {
            if (values.size() == 2) {
                //Atualização de vetores
                if (isVector(values[0])) {
                    int slots = getVectorSlots(trim(values[0]));
                    values[0] = splitFirst(values[0], '[')[0];
                    this->boolean_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = stoi(resolveEvaluation(values[1]));
                }
                else
                    this->boolean_variables[values[0]] = false;
                
            }
            else {
                if (isVector(type_identifier)) {
                    int slots = getVectorSlots(trim(type_identifier));

                    for (int x = 0; x < slots; x++) {
                        this->declareVariable("Boolean " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", accessibility, false);
                    }
                }
                else {

                    this->boolean_variables[values[0]] = false;
                }
            }
        }

        //Declara a variavel char
        void declareChar(vector<string> values, string accessibility, string type_identifier) {
            if (values.size() == 2) {
                
                //Atualização de vetores
                if (isVector(values[0])) {
                    int slots = getVectorSlots(trim(values[0]));
                    values[0] = splitFirst(values[0], '[')[0];
                    this->char_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = resolveAssignment(values[1])[0];
                }
                else
                    this->char_variables[values[0]] = values[1][0];
                
            }
            else {
                if (isVector(type_identifier)) {
                    int slots = getVectorSlots(trim(type_identifier));

                    for (int x = 0; x < slots; x++) {
                        this->declareVariable("Char " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "]" + " = 0;", accessibility, false);
                    }
                }
                else {
                    this->char_variables[values[0]] = 0;
                }
            }
        }

        //Coloca a variavel em seu devido map
        void declareVariable(string content, string accessibility, bool update) {

            vector<string> p = splitFirst(content, ' ');

            if (p.size() == 2) {

                vector<string> values;
                if (p[1].find("=") != string::npos) {
                    values = splitFirst(p[1], '=');
                    values[0] = trim(values[0]);
                    values[1] = trim(values[1]);
                }
                else
                    values.push_back(trim(p[1].substr(0, p[1].length() - 1)));


                try {
                    if (startsWith(p[0], "Int"))
                        this->declareInt(values, accessibility, p[0]);
                    else if (startsWith(p[0], "Double"))
                        this->declareDouble(values, p[0], accessibility);
                    else if (startsWith(p[0], "String"))
                        this->declareString(values, p[0], accessibility, content);
                    else if (startsWith(p[0], "Boolean"))
                        this->declareBoolean(values, p[0], accessibility);
                    else if (startsWith(p[0], "Char"))
                        this->declareChar(values, p[0], accessibility);

                    if (!update) {
                        this->variables_names.push_back(values[0]);

                        if (accessibility == "public") 
                            public_symbols.push_back(values[0]);
                        else if (accessibility == "private")
                            private_symbols.push_back(values[0]);
                        
                    }
                }
                catch (exception& e) {
                    throwError(e.what(), content);
                    throw - 1;
                }

            }
            else
                throwError(errors.STATEMENT, content);

        }

        //
        void declareMethod(string params) {
            vector<string> method_pices = splitFirst(params, '(');
            vector<string> keywords = split(method_pices[0], ' ');
            FunctionsAssignment new_function;

            new_function.visibility = trim(keywords[0]);
            new_function.return_type = trim(keywords[1]);
            new_function.name = trim(keywords[2]);
            new_function.start_in_line = (int)(file_reference->tellg()) - (params.length() + 2);
            new_function.params = split(method_pices[1], ')')[0];

            if (new_function.name == "main")
                throwError(errors.NAME_NOT_ALLOWED, new_function.name);

            if (new_function.visibility == "public") 
                public_symbols.push_back(new_function.name);
            else if (new_function.visibility == "private") 
                private_symbols.push_back(new_function.name);

            this->custom_functions[new_function.name] = new_function;
        }

        bool isPublic(string name) {
            for (string s : private_symbols) {
                if (s == name) 
                    return false;
            }

            for (string s : public_symbols) {
                if (s == name)
                    return true;
            }
            
            return false;
        }

        string getTypeOfVariable(string s) {
            string returned = "Null";

            if (this->integer_variables.count(s))
                returned = "Int";
            else if (this->double_variables.count(s))
                returned = "Double";
            else if (this->string_variables.count(s))
                returned = "String";
            else if (this->boolean_variables.count(s))
                returned = "Boolean";
            else if (this->char_variables.count(s))
                returned = "Char";

            return returned;
        }
	
	
		bool existsVariable(string var_name) {
			for (string var : variables_names) {
				if (var_name == var)
					return true;
			}
			return false;
		}

};

extern map<string, int> integer_variables;
extern map<string, double> double_variables;
extern map<string, bool> boolean_variables;
extern map<string, char> char_variables;
extern map<string, string> string_variables;
extern map<string, size_t> class_model;
extern map<string, Object> classes_variables;

extern vector<string> variables_names;

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
		bool search_block_end_class = false;
		bool verify_next_line = false;
		bool isClassMethod = false;

		FunctionsRetruns returned;
		string returned_string;

		string current_return_type;

		vector<string> scope_variables;

        Object* class_reference;
		FunctionsAssignment * method_reference;

		void pop_last_variable() {
			string var = scope_variables[scope_variables.size() - 1];

			if (integer_variables.count(var))
				integer_variables.erase(var);
			else if (double_variables.count(var))
				double_variables.erase(var);
			else if (boolean_variables.count(var))
				boolean_variables.erase(var);
			else if (char_variables.count(var))
				char_variables.erase(var);
			else if (string_variables.count(var))
				string_variables.erase(var);

			scope_variables.pop_back();
			variables_names.pop_back();
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
void executeCustomFunction(string content, Object * c);
bool isCustomFunction(string content, bool ignore_error);
void interpreterLine(string content);
void interpreterLineAndFindEnd(string content);
void interpreterLineInMethod(string content);
void interpreterLineInClass(string content);
void interpreterLineInClass(string content, Object * c);
bool isParenthesesOk(string content);

string getFilename();
void setFilename(string f);
void readFile(string code_name);

#endif
