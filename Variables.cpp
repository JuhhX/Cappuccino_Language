#include "Variables.h"
#include "General.h"

map<string, int> integer_variables;
map<string, double> double_variables;
map<string, bool> boolean_variables;
map<string, char> char_variables;
map<string, string> string_variables;

vector<string> variables_names;

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

//Verifica se uma string está sendo declarada
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
        string type = getTypeOfVariable(trim(var_name));
        declareVariable(type + " " + params, true);
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
