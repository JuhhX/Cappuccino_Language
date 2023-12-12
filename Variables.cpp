#include "Variables.h"
#include "General.h"

map<string, int> integer_variables;
map<string, double> double_variables;
map<string, bool> boolean_variables;
map<string, char> char_variables;
map<string, string> string_variables;
map<string, Object> classes_variables;

vector<string> variables_names;

map<string, size_t> class_model;

stack<Scope> scopes;
ifstream* file_reference;

//Se encontrar uma variavel com este nome, executa a função f para ela;
void variableExecute(string s, function<void(string)> f) {

    string returned = "";

    if (integer_variables.count(s))
        returned = to_string(integer_variables[s]);
    else if (double_variables.count(s))
        returned = to_string(double_variables[s]);
    else if (string_variables.count(s))
        returned = string_variables[s];
    else if (boolean_variables.count(s))
        returned = to_string(boolean_variables[s]);
    else if (char_variables.count(s))
        returned = to_string(char_variables[s]);
    else {
        throwError(errors.VARIABLE_NOT_EXISTS, s);
        throw - 1;
    }

    if (returned != "")
        f(returned);
}

//Apenas retorna o valor da variavel
string getVariableValue(string s, bool ignore_throw) {
    string returned = "";

    if (integer_variables.count(s))
        returned = to_string(integer_variables[s]);
    else if (double_variables.count(s))
        returned = to_string(double_variables[s]);
    else if (string_variables.count(s))
        returned = string_variables[s];
    else if (boolean_variables.count(s))
        returned = to_string(boolean_variables[s]);
    else if (char_variables.count(s))
        returned = (char(char_variables[s]));
    else if (s.find(".") != string::npos) {
        vector<string> object_name = splitFirst(s, '.');
        
        if (classes_variables.count(trim(object_name[0]))) {
            if (classes_variables[object_name[0]].existsVariable(trim(object_name[1]))) {
                if (classes_variables[object_name[0]].isPublic(trim(object_name[1])) || scopes.top().isClassMethod) {
                    returned = classes_variables[object_name[0]].getVariableValue(trim(object_name[1]));
                }
                else {
                    throwError(errors.PROPERTY_INACCESSIBLE, s);
                }
            }
            //ISSO TEM Q ESTAR EM OUTRO METODO
            else if (isObjectMethod(s)) {
                executeObjectMethod(s);
                if(scopes.top().current_return_type == "String")
                    returned = scopes.top().returned_string;
                else if(scopes.top().current_return_type == "Int")
                    returned = to_string(scopes.top().returned.returned_int);
                else if(scopes.top().current_return_type == "Double")
                    returned = to_string(scopes.top().returned.returned_double);
                else if(scopes.top().current_return_type == "Boolean")
                    returned = to_string(scopes.top().returned.returned_boolean);
                else if(scopes.top().current_return_type == "Char")
                    returned = to_string(scopes.top().returned.returned_char);
            }
        }
        else if(!ignore_throw) {
            throwError(errors.VARIABLE_NOT_EXISTS, s);
            throw - 1;
        }

    }
    else if (!ignore_throw) {
        throwError(errors.VARIABLE_NOT_EXISTS, s);
        throw - 1;
    }

    return returned;
}

//Verifica se a variavel existe
bool variableExists(string s) {
    if (s.find(";") != string::npos)
        s = s.substr(0, s.length() - 1);

    if (s.find(".") != string::npos)
        s = trim(splitFirst(s, '.')[0]);

    for (int x = 0; x < variables_names.size(); x++) {
        if (variables_names[x] == s)
            return true;
    }
    return false;
}

//Obtem o tipo da variavel
string getTypeOfVariable(string s) {
    string returned = "Null";

    if (integer_variables.count(s))
        returned = "Int";
    else if (double_variables.count(s))
        returned = "Double";
    else if (string_variables.count(s))
        returned = "String";
    else if (boolean_variables.count(s))
        returned = "Boolean";
    else if (char_variables.count(s))
        returned = "Char";

    return returned;
}

//Obtem o tipo do dado
string getTypeOfData(string s) {
    string returned = "Null";
    s = split(s, ";")[0];

    if (startsWith(s, "\"") && endsWith(s, "\""))
        returned = "String";
    else if (s == "true" || s == "false")
        returned = "Boolean";
    else if (startsWith(s, "\'") && endsWith(s, "\'"))
        returned = "Char";
    else {
        bool digit = true;
        int point_count = 0;

        for (char c : s) {
            if (c == '.') {
                point_count++;
                continue;
            }

            if (!isdigit(c)) {
                digit = false;
                break;
            }
        }

        if (point_count == 1 && digit)
            returned = "Double";
        else if (digit)
            returned = "Int";
    }

    return returned;
}

//Verifica se uma variável está sendo declarada
bool isVariableDeclaration(string content) {
    return content.find("Int") == 0 || content.find("Double") == 0 || content.find("String") == 0 || content.find("Boolean") == 0 || content.find("Char") == 0;
}

//Verifica se a variavel é um vetor
bool isVector(string params) {
    int brackets_open_count = 0;
    int brackets_close_count = 0;

    for (char c : params) {
        if (c == '[')
            brackets_open_count++;
        else if (c == ']')
            brackets_close_count++;
    }

    return brackets_open_count == 1 && brackets_close_count == 1;
}

//Obtem quantos slots tem o vetor durante a declaração
int getVectorSlots(string params) {
    size_t inits = params.find("[");
    size_t ends = params.rfind("]");
    params = params.substr(inits + 1, (ends - inits) - 1);

    return stoi(resolveAssignment(params));
}

//Declara a variável inteira
void declareInt(vector<string> values, string type_identifier) {
    //Valor atribuido
    if (values.size() == 2) {
        if (isNativeFunction(values[1], true)) {
            executeNativeFunction(values[1]);
            if (scopes.top().current_return_type == "Int")
                integer_variables[values[0]] = scopes.top().returned.returned_int;
            else
                throwError(errors.INCORRECT_TYPE, values[1]);
        }
        else if (isCustomFunction(values[1], true)) {
            string line;

            if (values[1][values[1].length() - 2] != ')') {
                integer_variables[values[0]] = stoi(resolveAssignment(values[1]));
            }
            else {
                scopes.top().back_loop_in_position = (int)(file_reference->tellg());
                executeCustomFunction(values[1]);

                while (getline(*file_reference, line) && scopes.top().search_block_end_method) {
                    if (line.empty() || startsWith(line, "//")) continue;
                    if (!line.empty()) line = trim(line);

                    interpreterLineInMethod(line);
                }

                if (scopes.top().current_return_type == "Int")
                    integer_variables[values[0]] = scopes.top().returned.returned_int;
                else
                    throwError(errors.INCORRECT_TYPE, values[1]);

                file_reference->clear();
                file_reference->seekg(scopes.top().back_loop_in_position);
                current_line = scopes.top().back_loop_in_line;
            }
        }
        else if (values[1].find(".") != string::npos) {
            vector<string> object_name = splitFirst(values[1], '.');

            if (classes_variables.count(trim(object_name[0]))) {
                if (classes_variables[object_name[0]].existsVariable(trim(object_name[1]))) {
                    if (classes_variables[object_name[0]].isPublic(trim(object_name[1])) || scopes.top().isClassMethod) {
                        integer_variables[values[0]] = stoi(classes_variables[object_name[0]].getVariableValue(trim(object_name[1])));
                    }
                    else {
                        throwError(errors.PROPERTY_INACCESSIBLE, values[1]);
                    }
                }
                //ISSO TEM Q ESTAR EM OUTRO METODO
                else if (isObjectMethod(values[1])) {
                    executeObjectMethod(values[1]);
                    if (scopes.top().current_return_type == "String")
                        integer_variables[values[0]] = stoi(scopes.top().returned_string);
                    else if (scopes.top().current_return_type == "Int")
                        integer_variables[values[0]] = (scopes.top().returned.returned_int);
                    else if (scopes.top().current_return_type == "Double")
                        integer_variables[values[0]] = (int)(scopes.top().returned.returned_double);
                    else if (scopes.top().current_return_type == "Boolean")
                        integer_variables[values[0]] = (scopes.top().returned.returned_boolean);
                    else if (scopes.top().current_return_type == "Char")
                        integer_variables[values[0]] = (scopes.top().returned.returned_char);
                }
            }
            else {
                throwError(errors.VARIABLE_NOT_EXISTS, values[1]);
                throw - 1;
            }

        }
        else {
            //Atualização de vetores
            if (isVector(values[0])) {
                int slots = getVectorSlots(trim(values[0]));
                values[0] = splitFirst(values[0], '[')[0];
                integer_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = stoi(resolveAssignment(values[1]));
            }
            else
                integer_variables[values[0]] = stoi(resolveAssignment(values[1]));
        }
    }
    //Apenas declaração
    else {
        if (isVector(type_identifier)) {
            int slots = getVectorSlots(trim(type_identifier));

            for (int x = 0; x < slots; x++) {
                string indice = to_string(x);
                declareVariable("Int " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", false);
            }
        }
        else
            integer_variables[values[0]] = 0;
    }
}

//Declara a variavel double
void declareDouble(vector<string> values, string type_identifier) {
    if (values.size() == 2) {
        if (isNativeFunction(values[1], true)) {
            executeNativeFunction(values[1]);
            if (scopes.top().current_return_type == "Double")
                double_variables[values[0]] = scopes.top().returned.returned_double;
            else if (scopes.top().current_return_type == "Int")
                double_variables[values[0]] = scopes.top().returned.returned_int;
            else
                throwError(errors.INCORRECT_TYPE, values[1]);
        }
        else if (isCustomFunction(values[1], true)) {
            string line;

            if (values[1][values[1].length() - 2] != ')') {
                double_variables[values[0]] = stod(resolveAssignment(values[1]));
            }
            else {
                scopes.top().back_loop_in_position = (int)(file_reference->tellg());
                executeCustomFunction(values[1]);

                while (getline(*file_reference, line) && scopes.top().search_block_end_method) {
                    if (line.empty() || startsWith(line, "//")) continue;
                    if (!line.empty()) line = trim(line);

                    interpreterLineInMethod(line);
                }

                if (scopes.top().current_return_type == "Double")
                    double_variables[values[0]] = scopes.top().returned.returned_double;
                else
                    throwError(errors.INCORRECT_TYPE, values[1]);

                file_reference->clear();
                file_reference->seekg(scopes.top().back_loop_in_position);
                current_line = scopes.top().back_loop_in_line;
            }
        }
        else if (values[1].find(".") != string::npos) {
            vector<string> object_name = splitFirst(values[1], '.');

            if (classes_variables.count(trim(object_name[0]))) {
                if (classes_variables[object_name[0]].existsVariable(trim(object_name[1]))) {
                    if (classes_variables[object_name[0]].isPublic(trim(object_name[1])) || scopes.top().isClassMethod) {
                        double_variables[values[0]] = stoi(classes_variables[object_name[0]].getVariableValue(trim(object_name[1])));
                    }
                    else {
                        throwError(errors.PROPERTY_INACCESSIBLE, values[1]);
                    }
                }
                //ISSO TEM Q ESTAR EM OUTRO METODO
                else if (isObjectMethod(values[1])) {
                    executeObjectMethod(values[1]);
                    if (scopes.top().current_return_type == "String")
                        double_variables[values[0]] = stod(scopes.top().returned_string);
                    else if (scopes.top().current_return_type == "Int")
                        double_variables[values[0]] = (scopes.top().returned.returned_int);
                    else if (scopes.top().current_return_type == "Double")
                        double_variables[values[0]] = (scopes.top().returned.returned_double);
                    else if (scopes.top().current_return_type == "Boolean")
                        double_variables[values[0]] = (scopes.top().returned.returned_boolean);
                    else if (scopes.top().current_return_type == "Char")
                        double_variables[values[0]] = (scopes.top().returned.returned_char);
                }
            }
            else {
                throwError(errors.VARIABLE_NOT_EXISTS, values[1]);
                throw - 1;
            }

        }
        else {
            //Atualização de vetores
            if (isVector(values[0])) {
                int slots = getVectorSlots(trim(values[0]));
                values[0] = splitFirst(values[0], '[')[0];
                double_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = stod(resolveAssignment(values[1]));
            }
            else
                double_variables[values[0]] = stod(resolveAssignment(values[1]));
        }
    }
    else {
        if (isVector(type_identifier)) {
            int slots = getVectorSlots(trim(type_identifier));

            for (int x = 0; x < slots; x++) {
                declareVariable("Double " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", false);
            }
        }
        else {
            double_variables[values[0]] = 0.0;
        }
    }
}

//Declara a variavel String
void declareString(vector<string> values, string type_identifier, string content) {
    if (values.size() == 2)
        if (isNativeFunction(values[1], true)) {
            executeNativeFunction(values[1]);
            if (scopes.top().current_return_type == "String")
                string_variables[values[0]] = scopes.top().returned_string;
            else
                throwError(errors.INCORRECT_TYPE, values[1]);
        }
        else if (isCustomFunction(values[1], true)) {
            string line;
            scopes.top().back_loop_in_position = (int)(file_reference->tellg());
            executeCustomFunction(values[1]);

            while (getline(*file_reference, line) && scopes.top().search_block_end_method) {
                if (line.empty() || startsWith(line, "//")) continue;
                if (!line.empty()) line = trim(line);

                interpreterLineInMethod(line);
            }

            if (scopes.top().current_return_type == "String")
                string_variables[values[0]] = scopes.top().returned_string;
            else
                throwError(errors.INCORRECT_TYPE, values[1]);

            file_reference->clear();
            file_reference->seekg(scopes.top().back_loop_in_position);
            current_line = scopes.top().back_loop_in_line;
        }
        else if (values[1].find(".") != string::npos) {
            vector<string> object_name = splitFirst(values[1], '.');

            if (classes_variables.count(trim(object_name[0]))) {
                if (classes_variables[object_name[0]].existsVariable(trim(object_name[1]))) {
                    if (classes_variables[object_name[0]].isPublic(trim(object_name[1])) || scopes.top().isClassMethod) {
                        string_variables[values[0]] = stoi(classes_variables[object_name[0]].getVariableValue(trim(object_name[1])));
                    }
                    else {
                        throwError(errors.PROPERTY_INACCESSIBLE, values[1]);
                    }
                }
                //ISSO TEM Q ESTAR EM OUTRO METODO
                else if (isObjectMethod(values[1])) {
                    executeObjectMethod(values[1]);
                    if (scopes.top().current_return_type == "String")
                        string_variables[values[0]] = scopes.top().returned_string;
                    else if (scopes.top().current_return_type == "Int")
                        string_variables[values[0]] = to_string(scopes.top().returned.returned_int);
                    else if (scopes.top().current_return_type == "Double")
                        string_variables[values[0]] = to_string(scopes.top().returned.returned_double);
                    else if (scopes.top().current_return_type == "Boolean")
                        string_variables[values[0]] = to_string(scopes.top().returned.returned_boolean);
                    else if (scopes.top().current_return_type == "Char")
                        string_variables[values[0]] = to_string(scopes.top().returned.returned_char);
                }
            }
            else {
                throwError(errors.VARIABLE_NOT_EXISTS, values[1]);
                throw - 1;
            }

        }
        else {
            //Atualização de vetores
            if (isVector(values[0])) {
                int slots = getVectorSlots(trim(values[0]));
                values[0] = splitFirst(values[0], '[')[0];

                string string_value = resolveAssignmentString(values[1]);

                string_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = (string_value.substr(1, string_value.length() - 3));
            }
            else if (variableExists(values[1]) || values[1].find("+") != string::npos)
                string_variables[values[0]] = resolveAssignmentString(values[1].substr(0, values[1].length() - 1));
            else if (values[1].find("\"") == 0 && values[1].rfind("\"") == values[1].length() - 2)
                //Desconsidera as aspas e o ;
                string_variables[values[0]] = resolveAssignmentString(values[1].substr(1, values[1].length() - 3));
            else {
                throwError(errors.UNCLOSED_STRING, content);
                throw - 1;
            }
        }
    else {
        if (isVector(type_identifier)) {
            int slots = getVectorSlots(trim(type_identifier));

            for (int x = 0; x < slots; x++) {
                declareVariable("String " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", false);
            }
        }
        else {
            string_variables[values[0]] = "";
        }
    }
}

//Declara a variavel booleana
void declareBoolean(vector<string> values, string type_identifier) {
    if (values.size() == 2) {
        if (values[1] == "true;")
            boolean_variables[values[0]] = true;
        else if (isNativeFunction(values[1], true)) {
            executeNativeFunction(values[1]);
            if (scopes.top().current_return_type == "Boolean")
                boolean_variables[values[0]] = scopes.top().returned.returned_boolean;
            else
                throwError(errors.INCORRECT_TYPE, values[1]);
        }
        else if (values[1].find(".") != string::npos) {
            vector<string> object_name = splitFirst(values[1], '.');

            if (classes_variables.count(trim(object_name[0]))) {
                if (classes_variables[object_name[0]].existsVariable(trim(object_name[1]))) {
                    if (classes_variables[object_name[0]].isPublic(trim(object_name[1])) || scopes.top().isClassMethod) {
                        boolean_variables[values[0]] = stoi(classes_variables[object_name[0]].getVariableValue(trim(object_name[1])));
                    }
                    else {
                        throwError(errors.PROPERTY_INACCESSIBLE, values[1]);
                    }
                }
                //ISSO TEM Q ESTAR EM OUTRO METODO
                else if (isObjectMethod(values[1])) {
                    executeObjectMethod(values[1]);
                    if (scopes.top().current_return_type == "String")
                        boolean_variables[values[0]] = stoi(scopes.top().returned_string);
                    else if (scopes.top().current_return_type == "Int")
                        boolean_variables[values[0]] = (scopes.top().returned.returned_int);
                    else if (scopes.top().current_return_type == "Double")
                        boolean_variables[values[0]] = (scopes.top().returned.returned_double);
                    else if (scopes.top().current_return_type == "Boolean")
                        boolean_variables[values[0]] = (scopes.top().returned.returned_boolean);
                    else if (scopes.top().current_return_type == "Char")
                        boolean_variables[values[0]] = (scopes.top().returned.returned_char);
                }
            }
            else {
                throwError(errors.VARIABLE_NOT_EXISTS, values[1]);
                throw - 1;
            }

        }
        else if (values[1] != "true;" || values[1] != "false;")
            boolean_variables[values[0]] = stoi(resolveEvaluation(values[1]));
        else {
            //Atualização de vetores
            if (isVector(values[0])) {
                int slots = getVectorSlots(trim(values[0]));
                values[0] = splitFirst(values[0], '[')[0];
                boolean_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = stoi(resolveEvaluation(values[1]));
            }
            else
                boolean_variables[values[0]] = false;
        }
    }
    else {
        if (isVector(type_identifier)) {
            int slots = getVectorSlots(trim(type_identifier));

            for (int x = 0; x < slots; x++) {
                declareVariable("Boolean " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "];", false);
            }
        }
        else {

            boolean_variables[values[0]] = false;
        }
    }
}

//Declara a variavel char
void declareChar(vector<string> values, string type_identifier) {
    if (values.size() == 2) {
        if (isNativeFunction(values[1], true)) {
            executeNativeFunction(values[1]);
            if (scopes.top().current_return_type == "Char")
                char_variables[values[0]] = scopes.top().returned.returned_char;
            else
                throwError(errors.INCORRECT_TYPE, values[1]);
        }
        else {
            //Atualização de vetores
            if (isVector(values[0])) {
                int slots = getVectorSlots(trim(values[0]));
                values[0] = splitFirst(values[0], '[')[0];
                char_variables[trim(values[0]) + "[" + to_string(slots) + "]"] = resolveAssignment(values[1])[0];
            }
            else
                char_variables[values[0]] = values[1][0];
        }
    }
    else {
        if (isVector(type_identifier)) {
            int slots = getVectorSlots(trim(type_identifier));

            for (int x = 0; x < slots; x++) {
                declareVariable("Char " + trim(values[0]) + "[" + resolveEvaluation(to_string(x)) + "]" + " = 0;", false);
            }
        }
        else {
            char_variables[values[0]] = 0;
        }
    }
}

//Coloca a variavel em seu devido map
void declareVariable(string content, bool update) {

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
                declareInt(values, p[0]);
            else if (startsWith(p[0], "Double"))
                declareDouble(values, p[0]);
            else if (startsWith(p[0], "String"))
                declareString(values, p[0], content);
            else if (startsWith(p[0], "Boolean"))
                declareBoolean(values, p[0]);
            else if (startsWith(p[0], "Char"))
                declareChar(values, p[0]);

            if (!update) {
                scopes.top().scope_variables.push_back(values[0]);
                variables_names.push_back(values[0]);
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

//Verifica se é uma atualização no valor de uma variável
bool isVariableUpdate(string params) {

    vector<string> var_value;

    if (params.find("+=") != string::npos)
        var_value = splitFirst(params, '+');
    else if (params.find("-=") != string::npos)
        var_value = splitFirst(params, '-');
    else if (params.find("*=") != string::npos)
        var_value = splitFirst(params, '*');
    else if (params.find("/=") != string::npos)
        var_value = splitFirst(params, '/');
    else if (params.find("%=") != string::npos)
        var_value = splitFirst(params, '%');
    else if (params.find("=") != string::npos)
        var_value = split(params, '=');
    else {
        string update_final = params.substr(params.length() - 3, 3);
        return update_final == "++;" || update_final == "--;";
    }

    if (var_value.size() == 2) {
        if (variableExists(trim(var_value[0])))
            return true;
        else if (isVector(var_value[0])) {
            int slots = getVectorSlots(trim(var_value[0]));
            var_value[0] = splitFirst(var_value[0], '[')[0];
            string declaration = var_value[0] + "[" + to_string(slots) + "]";
            if (variableExists(declaration)) 
                return true;
            
        }
        else if (startsWith(var_value[0], "this.")) {
            return true;
        }
        else {
            if (!isNativeFunction(params, true))
                throwError(errors.VARIABLE_NOT_EXISTS, params);
        }
    }
    else
        if (!isNativeFunction(params, true))
            throwError(errors.STATEMENT, params);

    return false;
}

//Atualiza a variável
void updateVariable(string params) {

    if (params.find("=") != string::npos) {
        string check_vector = splitFirst(params, '=')[0];
        if (isVector(trim(check_vector))) {
            int slots = getVectorSlots(trim(check_vector));
            string name = trim(splitFirst(check_vector, '[')[0]);
            string assignment = trim(splitLast(params, ']')[1]);
            params = name + "[" + to_string(slots)+"]" + assignment;
        }
    }

    if (params.find("+=") != string::npos) {
        string var_name = splitFirst(params, '+')[0];
        string update_value = splitFirst(params, '=')[1];

        string type = getTypeOfVariable(trim(var_name));

        declareVariable(type + " " + var_name + "=" + var_name + "+" + update_value, true);
    }
    else if (params.find("-=") != string::npos) {
        string var_name = splitFirst(params, '-')[0];
        string update_value = splitFirst(params, '=')[1];

        string type = getTypeOfVariable(trim(var_name));

        declareVariable(type + " " + var_name + "=" + var_name + "-" + update_value, true);
    }
    else if (params.find("*=") != string::npos) {
        string var_name = splitFirst(params, '*')[0];
        string update_value = splitFirst(params, '=')[1];

        string type = getTypeOfVariable(trim(var_name));

        declareVariable(type + " " + var_name + "=" + var_name + "*" + update_value, true);
    }
    else if (params.find("/=") != string::npos) {
        string var_name = splitFirst(params, '/')[0];
        string update_value = splitFirst(params, '=')[1];

        string type = getTypeOfVariable(trim(var_name));

        declareVariable(type + " " + var_name + "=" + var_name + "/" + update_value, true);
    }
    else if (params.find("%=") != string::npos) {
        string var_name = splitFirst(params, '%')[0];
        string update_value = splitFirst(params, '=')[1];

        string type = getTypeOfVariable(trim(var_name));

        declareVariable(type + " " + var_name + "=" + var_name + "%" + update_value, true);
    }
    else if (params.find("=") != string::npos) {
        string var_name = split(params, '=')[0];

        if (startsWith(var_name, "this.")) {
            var_name = splitFirst(var_name, '.')[1];
            string value = split(params, '=')[1];
            string type = scopes.top().class_reference->getTypeOfVariable(trim(var_name));
            scopes.top().class_reference->declareVariable(type + " " + var_name + " = " + value, "public", true);
        }
        else {
            string type = getTypeOfVariable(trim(var_name));
            declareVariable(type + " " + params, true);
        }
    }
    else {
        string var_name = params.substr(0, params.length() - 3);
        string update_final = params.substr(params.length() - 3, 3);

        if (update_final == "++;") {
            string type = getTypeOfVariable(var_name);
            declareVariable(type + " " + var_name + " = " + resolveAssignment(var_name + "+1"), true);
        }
        else if (update_final == "--;") {
            string type = getTypeOfVariable(var_name);
            declareVariable(type + " " + var_name + " = " + resolveAssignment(var_name + "-1"), true);
        }
    }

}

//
bool isClassDeclaration(string params) {
    return startsWith(params, "class");
}

//
void registerClass(string params) {
    //Escopo na mesma linha
    vector<string> class_pieces = splitFirst(params, ' ');

    if (endsWith(trim(class_pieces[1]), "{")) {
        //Armazena nome e onde a classe começa
        string class_name = class_pieces[1].substr(0, class_pieces[1].length() - 1);
        class_model[trim(class_name)] = file_reference->tellg();

        //Ignora a classe
        Scope new_scope;
        scopes.push(new_scope);

        scopes.top().execute_block = false;
        scopes.top().search_block_end_class = true;
        scopes.top().block_count++;
    }

}

//
bool isObjectDeclaration(string content) {
    vector<string> declaration_pieces = splitFirst(content, ' ');
    if (declaration_pieces.size() == 2) {
        return (class_model.count(trim(declaration_pieces[0])));
    }
    return false;
}

//
void declareObject(string content, bool ignore_errors) {
    //Sem o new
    vector<string> var_pieces = splitFirst(content, ' ');
    string name = var_pieces[1];
    name = name.substr(0, name.length() - 1);
    string type = var_pieces[0];

    Object new_class;
    new_class.type_name = type;

    if (!variableExists(name)) {
        string line;
        scopes.top().scope_variables.push_back(name);

        scopes.top().back_loop_in_position = (int)(file_reference->tellg());

        Scope new_scope;
        scopes.push(new_scope);

        file_reference->clear();
        file_reference->seekg(class_model[trim(type)]);

        scopes.top().search_block_end_class = true;
        scopes.top().execute_block = true;
        scopes.top().block_count++;

        while (getline(*file_reference, line) && scopes.top().search_block_end_class) {
            if (line.empty() || startsWith(line, "//")) continue;
            if (!line.empty()) line = trim(line);

            interpreterLineInClass(line, &new_class);
        }

        file_reference->clear();
        file_reference->seekg(scopes.top().back_loop_in_position);
        current_line = scopes.top().back_loop_in_line;

        vector<string> p = splitFirst(content, ' ');

        //Definido porque o escopo precisa ter controle dessa variavel
        if (p.size() == 2) {

            vector<string> values;
            if (p[1].find("=") != string::npos) {
                values = splitFirst(p[1], '=');
                values[0] = trim(values[0]);
                values[1] = trim(values[1]);
            }
            else
                values.push_back(trim(p[1].substr(0, p[1].length() - 1)));
            variables_names.push_back(values[0]);
        }

    }
    else {
        //Disparar erro
    }
    classes_variables[name] = new_class;
}

// 
bool isObjectVariableUpdate(string content) {
    vector<string> variable_parts = splitFirst(content, '.');

    if (variable_parts.size() == 2) {
        if (variable_parts[1].find("=") != string::npos && classes_variables.count(variable_parts[0]))
            return true;
    }

    return false;
}

void updateObjectVariable(string content) {
    vector<string> variable_parts = splitFirst(content, '.');

    if (variable_parts.size() == 2) {
        if (variable_parts[1].find("=") != string::npos && classes_variables.count(trim(variable_parts[0]))) {
            string declaration = classes_variables[trim(variable_parts[0])].getTypeOfVariable(trim(splitFirst(variable_parts[1], '=')[0]));
            string property_name = trim(splitFirst(variable_parts[1], '=')[0]);

            if (classes_variables[trim(variable_parts[0])].isPublic(property_name)) {
                classes_variables[trim(variable_parts[0])].declareVariable(declaration + " " + variable_parts[1], "public", true);
            }
            else {
                throwError(errors.PROPERTY_INACCESSIBLE, content);
            }

        }
    }
}

//
bool isObjectMethod(string content) {
    vector<string> variable_parts = splitFirst(content, '.');

    if (variable_parts.size() == 2) {
        if (isParenthesesOk(content) && classes_variables.count(variable_parts[0]))
            return true;
    }

    return false;
}

//
void executeObjectMethod(string content) {
    vector<string> methods_parts = splitFirst(content, '.');

    if (methods_parts.size() == 2) {
        if (isParenthesesOk(content) && classes_variables.count(methods_parts[0])) {
            string method_name = trim(splitFirst(methods_parts[1], '(')[0]);

            if (classes_variables[trim(methods_parts[0])].isPublic(method_name)) {
                string line;
                scopes.top().back_loop_in_position = (int)(file_reference->tellg());
                scopes.top().isClassMethod = true;
                executeCustomFunction(content, &classes_variables[trim(methods_parts[0])]);

                while (getline(*file_reference, line) && scopes.top().search_block_end_method) {
                    if (line.empty() || startsWith(line, "//")) continue;
                    if (!line.empty()) line = trim(line);

                    interpreterLineInMethod(line);
                }

                file_reference->clear();
                file_reference->seekg(scopes.top().back_loop_in_position);
                current_line = scopes.top().back_loop_in_line;
                scopes.top().isClassMethod = false;

            }
            else {
                throwError(errors.PROPERTY_INACCESSIBLE, content);
            }
        }

    }
}
