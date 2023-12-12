#include "NativeFunctions.h"
#include "General.h"
#include <chrono>
#include <thread>

//Comando para pegar o valor de uma variável
void typeof(string params) {
    scopes.top().returned_string = getTypeOfVariable(params);
    scopes.top().current_return_type = "String";
}

//Comando de saída
void print(string params) {

    string r;
    if (params.find("\\n") != string::npos)
        resolveLineBreak(params, 1);
    else if (params.find("\\t") != string::npos)
        resolveTab(params, 1);
    else {
        if (isString(params))
            cout << params.substr(1, params.length() - 2);
        else if ((r = getVariableValue(params, true)) != "") {
            if (r.find("\\n") != string::npos)
                resolveLineBreak(r, 0);
            else if (r.find("\\t") != string::npos)
                resolveTab(r, 0);
            else
                cout << r;
        }
        else
            throwError(errors.INVALID_PARAMETER, params);
    }

}

//Comando de saída
void println(string params) {

    string r;
    if (params.find("\\n") != string::npos)
        resolveLineBreak(params, 1);
    else if (params.find("\\t") != string::npos)
        resolveTab(params, 1);
    else {
        if (isString(params))
            cout << params.substr(1, params.length() - 2) << endl;
        else if ((r = getVariableValue(params, true)) != "") {
            if (r.find("\\n") != string::npos)
                resolveLineBreak(r, 0);
            else if (r.find("\\t") != string::npos)
                resolveTab(r, 0);
            else
                cout << r << endl;
        }
        else
            throwError(errors.INVALID_PARAMETER, params);
    }

}

//Comando de saída formatado
void printf_(string params) {
    if (isString(params)) {
        string formated = formatInString(params);
        if (formated.find("\\n") != string::npos)
            resolveLineBreak(formated, 1);
        else if (params.find("\\t") != string::npos)
            resolveTab(params, 1);
        else
            cout << formated.substr(1, formated.length() - 2) << endl;
    }
    else
        throwError(errors.INVALID_PARAMETER, params);
}

//Lê uma linha como String
void readLine(string params) {
    cin >> scopes.top().returned_string;
    scopes.top().current_return_type = "String";
}

//Lê uma linha como Int
void readInt(string params) {
    string read_input;
    cin >> read_input;

    int read_int;
    try {
        read_int = stoi(read_input);
    }
    catch (exception& e) {
        throwError(errors.INCORRECT_TYPE, read_input + "\n\t" + e.what());
        throw - 1;
    }

    scopes.top().returned.returned_int = read_int;
    scopes.top().current_return_type = "Int";
}

//Lê uma linha como Double
void readDouble(string params) {
    string read_input;
    cin >> read_input;

    double read_double;
    try {
        read_double = stod(read_input);
    }
    catch (exception& e) {
        throwError(errors.INCORRECT_TYPE, read_input + "\n\t" + e.what());
        throw - 1;
    }

    scopes.top().returned.returned_double = read_double;
    scopes.top().current_return_type = "Double";
}

//Lê uma linha como Booleano
void readBoolean(string params) {
    string read_input;
    cin >> read_input;

    bool read_boolean = (read_input == "true" || read_input == "1");

    scopes.top().returned.returned_boolean = read_boolean;
    scopes.top().current_return_type = "Boolean";
}

//Lê uma linha como Char
void readChar(string params) {
    string read_input;
    cin >> read_input;

    scopes.top().returned.returned_char = read_input[0];
    scopes.top().current_return_type = "Char";
}

//Executa a função nativa
void executeNativeFunction(string content) {
    vector<string> command_parameters = splitFirst(content, '(');
    if (startsWith(command_parameters[1], ")"))
        functions[trim(command_parameters[0])]("");
    else
        functions[trim(command_parameters[0])](trim(command_parameters[1].substr(0, command_parameters[1].length() - 2)));
}

//Verifica se é uma função nativa
bool isNativeFunction(string content, bool ignore_error) {

    if (!read_without_execute) {
        if (isParenthesesOk(content)) {
            vector<string> command = splitFirst(content, '(');
            if (!command[0].empty()) {
                if (functions.count(trim(command[0])))
                    return true;
                else if (!ignore_error)
                    throwError(errors.UNDEFINED_FUNCTION, content);
            }
        }
        else {
            if (!ignore_error)
                throwError(errors.UNCLOSED_PARENTHESIS, content);
        }
    }

    return false;
}

//Converte um tipo para Inteiro
void toInt(string params) {
    string type;
    bool isVariable = false;

    if (variableExists(params)) {
        type = getTypeOfVariable(params);
        isVariable = true;
    }
    else 
        type = getTypeOfData(params);

    if (type == "Double") 
        scopes.top().returned.returned_int = (int)stod((isVariable) ? getVariableValue(params, false) : params);
    else if(type == "Int")
        scopes.top().returned.returned_int = stoi((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Boolean") {
        if (isVariable) {
            scopes.top().returned.returned_int = stoi((isVariable) ? getVariableValue(params, false) : params);
        }
        else {
            if (params == "true")
                scopes.top().returned.returned_int = 1;
            else if(params == "false")
                scopes.top().returned.returned_int = 0;
            else {
                //ERRO DE CONVERSÃO
            }

        }
    }
    else if (type == "String") {
        try {
            scopes.top().returned.returned_int = stoi((isVariable) ? getVariableValue(params, false) : params);
        }
        catch (int error) {
            //ERRO DE CONVERSÃO
            throw error;
        }
    }

    scopes.top().current_return_type = "Int";
}

//Converte um tipo para Double
void toDouble(string params) {
    string type;
    bool isVariable = false;

    if (variableExists(params)) {
        type = getTypeOfVariable(params);
        isVariable = true;
    }
    else
        type = getTypeOfData(params);

    if (type == "Double")
        scopes.top().returned.returned_double = stod((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Int")
        scopes.top().returned.returned_double = (double)stoi((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Boolean") {
        if (isVariable) {
            scopes.top().returned.returned_double = (double)stoi((isVariable) ? getVariableValue(params, false) : params);
        }
        else {
            if (params == "true")
                scopes.top().returned.returned_double = 1.0;
            else if (params == "false")
                scopes.top().returned.returned_double = 0.0;
            else {
                //ERRO DE CONVERSÃO
            }

        }
    }
    else if (type == "String") {
        try {
            scopes.top().returned.returned_double = stod((isVariable) ? getVariableValue(params, false) : params);
        }
        catch (int error) {
            //ERRO DE CONVERSÃO
            throw error;
        }
    }

    scopes.top().current_return_type = "Double";
}

//Converte um tipo para Booleano
void toBoolean(string params) {
    string type;
    bool isVariable = false;

    if (variableExists(params)) {
        type = getTypeOfVariable(params);
        isVariable = true;
    }
    else
        type = getTypeOfData(params);

    if (type == "Double")
        scopes.top().returned.returned_boolean = stoi((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Int")
        scopes.top().returned.returned_boolean = stoi((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Boolean") {
        if (isVariable) {
            scopes.top().returned.returned_boolean = stoi((isVariable) ? getVariableValue(params, false) : params);
        }
        else {
            if (params == "true")
                scopes.top().returned.returned_boolean = 1;
            else if (params == "false")
                scopes.top().returned.returned_boolean = 0;
            else {
                //ERRO DE CONVERSÃO
            }

        }
    }
    else if (type == "String") {
        try {
            if(params == "\"true\"")
               scopes.top().returned.returned_boolean = 1;
            else if(params == "\"false\"")
               scopes.top().returned.returned_boolean = 0;
        }
        catch (int error) {
            //ERRO DE CONVERSÃO
            throw error;
        }
    }

    scopes.top().current_return_type = "Boolean";
}

//Converte um tipo para String
void toString(string params) {
    string type;
    bool isVariable = false;

    if (variableExists(params)) {
        type = getTypeOfVariable(params);
        isVariable = true;
    }
    else
        type = getTypeOfData(params);

    if (type == "Double")
        scopes.top().returned_string = ((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Int")
        scopes.top().returned_string = ((isVariable) ? getVariableValue(params, false) : params);
    else if (type == "Boolean") {
        if (isVariable) {
            scopes.top().returned_string = ((isVariable) ? getVariableValue(params, false) : params);
        }
        else {
            if (params == "true")
                scopes.top().returned_string = "true";
            else if (params == "false")
                scopes.top().returned_string = "false";
            else {
                //ERRO DE CONVERSÃO
            }

        }
    }
    else if (type == "String") {
        try {
            scopes.top().returned_string = ((isVariable) ? getVariableValue(params, false) : params);
        }
        catch (int error) {
            //ERRO DE CONVERSÃO
            throw error;
        }
    }

    scopes.top().current_return_type = "String";
}

//
void sleep(string params) {
    this_thread::sleep_for(chrono::seconds(stoi(params)));
}

//
void clear(string params) {
    system("cls");
}

//Inicializa as funções
void initializeFunctions() {
    functions["print"] = print;
    functions["println"] = println;
    functions["printf"] = printf_;
    functions["readLine"] = readLine;
    functions["readInt"] = readInt;
    functions["readDouble"] = readDouble;
    functions["readBoolean"] = readBoolean;
    functions["readChar"] = readChar;
    functions["typeof"] = typeof;
    functions["toInt"] = toInt;
    functions["toDouble"] = toDouble;
    functions["toBoolean"] = toBoolean;
    functions["toString"] = toString;
    functions["sleep"] = sleep;
    functions["clear"] = clear;
}