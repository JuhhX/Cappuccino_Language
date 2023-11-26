#include <iostream>
#include "GeneralFunctions.h"
#include "StringUtilities.h"
#include "BasicUtilities.h"

using namespace std;

//Se encontrar uma variavel com este nome, executa a função f para ela;
void variableExecute(string s, function<void (string)> f) {

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

     if(returned != "")
        f(returned);
}

//Apenas retorna o valor da variavel
string getVariableValue(string s, bool ignore_throw){
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
    else if(!ignore_throw) {
        throwError(errors.VARIABLE_NOT_EXISTS, s);
        throw - 1;
    }

    return returned;
}

//Verifica se a variavel existe
bool variableExists(string s) {
    if(s.find(";") != string::npos)
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

//Resolve uma expressão dentro de uma string
string resolveFormatExpression(string params) {
    if (variableExists(params))
        return (getVariableValue(params, true));
    else
        return resolveEvaluation(params);
}

//Formata a string com variaveis dentro
string formatInString(string s) {
    
    string result = "";
    int current_pos = 0;
    int expression_init = 0;
    bool is_reading = false;

    while (current_pos < s.length()) {
        
        if (s[current_pos] == '{'){
            expression_init = current_pos + 1;
            is_reading = true;
        }
        else if (s[current_pos] == '}') {
            result += resolveFormatExpression(s.substr(expression_init, current_pos - expression_init));
            is_reading = false;
        }
        else if(!is_reading)
            result += s[current_pos];

        current_pos++;
    }
    
    
    return result;
}

//Faz a tabulação de linhas em strings
void resolveTab(string t, int retreat) {
    vector<string> splited = split(t, "\\t");
    int count = 0;

    for (string s : splited) {
        if (count == 0)  cout << s.substr(retreat, s.length()) << "\t";
        else if (count == splited.size() - 1) cout << s.substr(0, s.length() - retreat);
        else cout << s << "\t";
        count++;
    }
}

//Faz a quebra de linhas nas strings
void resolveLineBreak(string t, int retreat) {
    vector<string> splited = split(t, "\\n");
    int count = 0;

    for (string s : splited) {
        if (s.find("\\t") != string::npos) {
            resolveTab(s, 0);
            if (count == 0)  cout  << endl;
            else cout << endl;
        }
        else {
            if (count == 0)  cout << s.substr(retreat, s.length()) << endl;
            else if (count == splited.size() - 1) cout << s.substr(0, s.length() - retreat);
            else cout << s << endl;
        }
        count++;
    }
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

//Dispara erros
void throwError(string error_type, string line_content) {
    cout << endl << colors.RED << (filename + ":" + to_string(current_line) + ": ") << error_type << colors.RESET << endl;
    cout << "\t" << line_content << endl;
    read_without_execute = true;
}

//Verifica se uma string está sendo declarada
bool isVariableDeclaration(string content) {

    if (content.find("Int") == 0 || content.find("Double") == 0 || content.find("String") == 0 || content.find("Boolean") == 0 || content.find("Char") == 0)
        return true;

    return false;
}

//Verifica se o caracter é uma expressão matemática
bool isOperation(string params){
    if (params == to_string('+') || params == to_string('-') || params == to_string('*') || params == to_string('/') || params == to_string('%'))
        return true;
    return false;
}

//Expressão de multiplicação
void multiply_expressions(stack<double>* numbers) {
    double n1 = numbers->top();
    numbers->pop();
    double n2 = numbers->top();
    numbers->pop();
    numbers->push(n1 * n2);
}

//Expressão de divisão
void divide_expressions(stack<double>* numbers) {
    double n2 = numbers->top();
    numbers->pop();
    double n1 = numbers->top();
    numbers->pop();
    numbers->push(n1 / n2);
}

//Expressão de divisão
void module_expressions(stack<double>* numbers) {
    int n2 = (int) numbers->top();
    numbers->pop();
    int n1 = (int) numbers->top();
    numbers->pop();
    numbers->push(n1 % n2);
}

//Expressão de soma
void sum_expressions(stack<double>* numbers) {
    double n1 = numbers->top();
    numbers->pop();
    double n2 = numbers->top();
    numbers->pop();
    numbers->push(n1 + n2);
}

//Expressão de subtração
void subtraction_expressions(stack<double>* numbers) {
    double n1 = numbers->top();
    numbers->pop();
    double n2 = numbers->top();
    numbers->pop();
    numbers->push(n1 - n2);
}

//Verifica se é um operador booleano
bool isBooleanOperation(char params) {

    return params == '>' || params == '<' || params == '=' || params == '!' || params == '&' || params == '|';

}

//Resolve uma atribuição de Inteiros e Doubles
string resolveAssignment(string params) {
    stack<double> numbers, aux_numbers;
    stack<char> operations, aux_operations;
    string temp;
    bool is_reading_parenthesis = false;
    int parenthesis_count = 0;
    
    //Separa os operadores dos numeros
    for (char c : params) {
        if (c == ' ' || c == ';' || isBooleanOperation(c)) continue;

        if (!is_reading_parenthesis) {
            if (isOperation(to_string(c))) {
                if (variableExists(temp))
                    numbers.push(stod(getVariableValue(temp, false)));
                else
                    numbers.push(stod(temp.empty() ? "0" : temp));
                operations.push(c);
                temp = "";
            }
            else {
                if (c == '(') {
                    parenthesis_count++;
                    is_reading_parenthesis = true;
                }
                else
                    temp += c;
            }
        }
        else {
            if (c == ')') {
                parenthesis_count--;
                if (parenthesis_count == 0) {
                    temp = resolveAssignment(temp);
                    is_reading_parenthesis = false;
                }
            }
            else if (c == '(')
                parenthesis_count++;
            else
                temp += c;
        }
    }

    if (!temp.empty())
        if (variableExists(temp))
            numbers.push(stod(getVariableValue(temp, false)));
        else
            numbers.push(stod(temp));

    if (numbers.size() == 1 && operations.empty())
        return to_string(numbers.top());

    //Resolve multiplicação e divisão
    while (!operations.empty()) {
        char op = operations.top();
        operations.pop();

        if (op == '*')
            multiply_expressions(&numbers);
        else if (op == '/')
            divide_expressions(&numbers);
        else if(op == '%')
            module_expressions(&numbers);
        else {
            if (!numbers.empty()) {
                double n1 = numbers.top();
                numbers.pop();
                aux_numbers.push(n1);

                if (!numbers.empty()) {
                    double n2 = numbers.top();
                    numbers.pop();

                    if (!operations.empty() && (operations.top() == '*' || operations.top() == '/'))
                        numbers.push(n2);
                    else
                        aux_numbers.push(n2);
                }
            }
            aux_operations.push(op);
        }

    }

    if (numbers.size() == 1 && aux_operations.size() == 0)
        return to_string(numbers.top());
    else if(numbers.size() == 1)
        aux_numbers.push(numbers.top());

    numbers = aux_numbers;
    operations = aux_operations;

    //Resolve adição e subtração
    while (!operations.empty()) {
        char op = operations.top();
        operations.pop();

        if (op == '+') 
            sum_expressions(&numbers);
        else if (op == '-') 
            subtraction_expressions(&numbers);
    }

    return to_string(numbers.top());
}

//Resolve uma atribuição de String
string resolveAssignmentString(string params) {

    if (variableExists(params))
        return getVariableValue(params, true);
    else if (params.find("+") != string::npos) {
        string result = "";
        vector<string> operands = split(params, '+');

        for (string s : operands) {
            s = trim(s);

            if (startsWith(s, "\"") && endsWith(s, "\""))
                result += s.substr(1, s.length() - 2);
            else if(variableExists(s)) 
                result += getVariableValue(s, true);
            else {
                throwError(errors.UNCLOSED_STRING, params);
                throw - 1;
            }
        }

        return result;

    }
    return formatInString(params);
}

//Resolve a expressão NOT
void not_expression(stack<string> *ops, stack<string> * aux_ops, stack<Operands> * opr, stack<Operands> *aux_opr) {
    while (!ops -> empty()) {
        string op = ops -> top();
        ops -> pop();

        Operands n1 = opr->top();
        opr->pop();

        if (op == to_string('!')) {
            Operands op_struct;
            op_struct.double_value = !n1.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops -> push(op);
            aux_opr->push(n1);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve expressões de MAIOR, MAIOR IGUAL, MENOR e MENOR IGUAL
void more_or_less_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        if (op == to_string('<')) {
            op_struct.double_value = n1.double_value < n2.double_value;
            opr->push(op_struct);
        }
        else if (op == "<=") {
            op_struct.double_value = n1.double_value <= n2.double_value;
            opr->push(op_struct);
        }
        else if (op == to_string('>')) {
            op_struct.double_value = n1.double_value > n2.double_value;
            opr->push(op_struct);
        }
        else if (op == ">=") {
            op_struct.double_value = n1.double_value >= n2.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve expressões de IGUAL ou DIFERENTE
void equals_or_different_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        string n1_value = (n1.is_string) ? n1.string_value : to_string(n1.double_value);
        string n2_value = (n2.is_string) ? n2.string_value : to_string(n2.double_value);
        if (op == "==") {
            op_struct.double_value = n1_value == n2_value;
            opr->push(op_struct);
        }
        else if (op == "!=") {
            op_struct.double_value = n1_value != n2_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve a expressões AND
void and_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        if (op == "&&") {
            op_struct.double_value = n1.double_value && n2.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve a expressões OR
void or_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        if (op == "||") {
            op_struct.double_value = n1.double_value || n2.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve uma expressão booleana
string resolveEvaluation(string params) {

    stack<Operands> operands, aux_operands;
    stack<string> operations, aux_operations;
    string temp;
    bool is_reading_parenthesis = false;
    int parenthesis_count = 0;

    for (int x = 0; x < params.length(); x++) {

        if (params[x] == ' ' || params[x] == ';')
            continue;

        if (!is_reading_parenthesis) {
            if (isBooleanOperation(params[x])) {
                if (isBooleanOperation(params[x + 1])) {
                    operations.push(params.substr(x, 2));
                    x++;
                }
                else
                    operations.push(to_string(params[x]));

                if (temp != "") {
                    Operands op_struct;
                    if (temp == "true") temp = "1";
                    else if (temp == "false") temp = "0";
                    else if (variableExists(temp)) {
                        if (getTypeOfVariable(temp) != "String") {
                            op_struct.double_value = stod(getVariableValue(temp, false));
                        }
                        else {
                            op_struct.is_string = true;
                            op_struct.string_value = "\"" + getVariableValue(temp, false) + "\"";
                        }
                        operands.push(op_struct);
                    }
                    else {
                        if (startsWith(temp, "\"") && endsWith(temp, "\"")) {
                            op_struct.is_string = true;
                            op_struct.string_value = resolveAssignmentString(temp);
                        }
                        else
                            op_struct.double_value = stod(resolveAssignment(temp));
                        operands.push(op_struct);
                    }
                }
                temp = "";
            }
            else {
                if (params[x] == '(') {
                    parenthesis_count++;
                    is_reading_parenthesis = true;
                }
                else
                    temp += params[x];
            }
        }
        else {
            if (params[x] == ')') {
                parenthesis_count--;
                if (parenthesis_count == 0) {
                    temp = resolveEvaluation(temp);
                    is_reading_parenthesis = false;
                }
            }
            else if (params[x] == '(')
                parenthesis_count++;
            else
                temp += params[x];
        }

    }

    if (temp != "") {
        Operands op_struct;
        if (temp == "true") temp = "1";
        else if (temp == "false") temp = "0";
        else if (variableExists(temp)) {
            if (getTypeOfVariable(temp) != "String") {
                op_struct.double_value = stod(getVariableValue(temp, false));
            }
            else {
                op_struct.is_string = true;
                op_struct.string_value = "\"" + getVariableValue(temp, false) + "\"";
            }
            operands.push(op_struct);
        }
        else {
            //Arrumar o resolveAssignment
            if (startsWith(temp, "\"") && endsWith(temp, "\"")) {
                op_struct.is_string = true;
                op_struct.string_value = resolveAssignmentString(temp);
            }
            else
                op_struct.double_value = stod(resolveAssignment(temp));
            operands.push(op_struct);
        }
    }

    not_expression(&operations, &aux_operations, &operands, &aux_operands);

    more_or_less_expression(&operations, &aux_operations, &operands, &aux_operands);
    
    equals_or_different_expression(&operations, &aux_operations, &operands, &aux_operands);

    and_expression(&operations, &aux_operations, &operands, &aux_operands);

    or_expression(&operations, &aux_operations, &operands, &aux_operands);

    if (operands.top().is_string) {
        string formated = operands.top().string_value;
        operands.top().string_value = formated.substr(1, formated.length() - 2);
    }

    return (operands.top().is_string) ? operands.top().string_value : to_string((int)operands.top().double_value);
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
            values.push_back(trim(p[1].substr(0, p[1].length() - 1 )));
        

        try {
            if (p[0] == "Int") {
                if (values.size() == 2) {
                    if (isNativeFunction(values[1], true)) {
                        executeNativeFunction(values[1]);
                        if (scopes.top().current_return_type == "Int")
                            integer_variables[values[0]] = scopes.top().returned.returned_int;
                        else
                            throwError(errors.INCORRECT_TYPE, values[1]);
                    }
                    else if (isCustomFunction(values[1], true)) {
                        scopes.top().variable_wait_value = content;
                        executeCustomFunction(values[1]);
                    }
                    else
                        integer_variables[values[0]] = stoi(resolveAssignment(values[1]));
                }
                else
                    integer_variables[values[0]] = 0;
            }
            else if (p[0] == "Double") {
                if (values.size() == 2) {
                    if (isNativeFunction(values[1], true)) {
                        executeNativeFunction(values[1]);
                        if (scopes.top().current_return_type == "Double")
                            double_variables[values[0]] = scopes.top().returned.returned_double;
                        else if(scopes.top().current_return_type == "Int")
                            double_variables[values[0]] = scopes.top().returned.returned_int;
                        else
                            throwError(errors.INCORRECT_TYPE, values[1]);
                    }
                    else if (isCustomFunction(values[1], true)) {
                        scopes.top().variable_wait_value = content;
                        executeCustomFunction(values[1]);
                    }
                    else
                        double_variables[values[0]] = stod(resolveAssignment(values[1]));
                }
                else
                    double_variables[values[0]] = 0.0;
            }
            else if (p[0] == "String") {
                if (values.size() == 2)
                    if (isNativeFunction(values[1], true)) {
                        executeNativeFunction(values[1]);
                        if (scopes.top().current_return_type == "String")
                            string_variables[values[0]] = scopes.top().returned_string;
                        else
                            throwError(errors.INCORRECT_TYPE, values[1]);
                    }
                    else if (isCustomFunction(values[1], true)) {
                        scopes.top().variable_wait_value = content;
                        executeCustomFunction(values[1]);
                    }
                    else {
                        if (variableExists(values[1]) || values[1].find("+") != string::npos) 
                            string_variables[values[0]] = resolveAssignmentString(values[1].substr(0, values[1].length() - 1));
                        else if (values[1].find("\"") == 0 && values[1].rfind("\"") == values[1].length() - 2) 
                            //Desconsidera as aspas e o ;
                            string_variables[values[0]] = resolveAssignmentString(values[1].substr(1, values[1].length() - 3));
                        else {
                            throwError(errors.UNCLOSED_STRING, content);
                            throw -1;
                        }
                    }
                else
                    string_variables[values[0]] = "";
            }
            else if (p[0] == "Boolean") {
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
                    else
                        boolean_variables[values[0]] = false;
                }
                else
                    boolean_variables[values[0]] = false;
            }
            else if (p[0] == "Char") {
                if (values.size() == 2) {
                    if (isNativeFunction(values[1], true)) {
                        executeNativeFunction(values[1]);
                        if (scopes.top().current_return_type == "Char")
                            char_variables[values[0]] = scopes.top().returned.returned_char;
                        else
                            throwError(errors.INCORRECT_TYPE, values[1]);
                    }
                    else
                        char_variables[values[0]] = values[1][0];
                }
                else
                    char_variables[values[0]] = 0;
            }

            if (!update) {
                scopes.top().scope_variables.push_back(values[0]);
                variables_names.push_back(values[0]);
            }
        }
        catch (exception& e) {
            throwError(e.what(), content);
            throw -1;
        }

    }
    else
        throwError(errors.STATEMENT, content);

}

//Executa a função nativa
void executeNativeFunction(string content) {
    vector<string> command_parameters = splitFirst(content, '(');
    if (startsWith(command_parameters[1], ")"))
        functions[trim(command_parameters[0])]("");
    else
        functions[trim(command_parameters[0])](trim(command_parameters[1].substr(0, command_parameters[1].length() - 2)));
}

//Executa métodos definidos no código
void executeCustomFunction(string content) {
    vector<string> command_parameters = splitFirst(content, '(');
    
    //Sem parametros
    if (startsWith(command_parameters[1], ")") && custom_functions[trim(command_parameters[0])].params == "") {
        
        scopes.top().back_loop_in_position = (int)(file_reference->tellg());
        scopes.top().back_loop_in_line= current_line;

        Scope new_scope;
        scopes.push(new_scope);

        file_reference->clear();
        file_reference->seekg(custom_functions[trim(command_parameters[0])].start_in_line);
        current_line = custom_functions[trim(command_parameters[0])].start_in_line;

        scopes.top().method_reference = &custom_functions[trim(command_parameters[0])];
        scopes.top().search_block_end_method = true;
        scopes.top().execute_block = true;
        scopes.top().block_count++;
    }
    //Com parametros
    else {

        scopes.top().back_loop_in_position = (int)(file_reference->tellg());
        scopes.top().back_loop_in_line= current_line;
        command_parameters[1] = command_parameters[1].substr(0, command_parameters[1].length() - 2);

        vector<string> informed_params = split(command_parameters[1], ',');
        vector<string> method_params = split(custom_functions[trim(command_parameters[0])].params, ',');

        if (informed_params.size() == method_params.size()) {
            Scope new_scope;
            scopes.push(new_scope);

            for (int x = 0; x < method_params.size(); x++) {
                declareVariable(trim(method_params[x]) + "=" + trim(informed_params[x]) + ";", false);
            }

            file_reference->clear();
            file_reference->seekg(custom_functions[trim(command_parameters[0])].start_in_line);
            current_line = custom_functions[trim(command_parameters[0])].start_in_line;

            scopes.top().method_reference = &custom_functions[trim(command_parameters[0])];
            scopes.top().search_block_end_method = true;
            scopes.top().execute_block = true;
            scopes.top().block_count++;
        }
        else {
            throwError(errors.MISSING_PARAMETERS, custom_functions[trim(command_parameters[0])].params);
        }
    }
}

//Verifica se os parenteses foram fechados corretamente
bool isParenthesesOk(string content) {
    int parenthesis_count = 0;
    int current_position = 0;
    bool has_parenthesis = false;

    while (current_position < content.length()) {
        if (content[current_position] == '(') {
            parenthesis_count++;
            has_parenthesis = true;
        }
        else if (content[current_position] == ')')
            parenthesis_count--;

        current_position++;
    }

    return (parenthesis_count == 0 && has_parenthesis) ? 1 : 0;
}

//Verifica se é uma função nativa
bool isNativeFunction(string content, bool ignore_error) {

    if (!read_without_execute) {
        if (isParenthesesOk(content)) {
            vector<string> command = splitFirst(content, '(');
            if (functions.count(trim(command[0])))
                return true;
            else if(!ignore_error)
                throwError(errors.UNDEFINED_FUNCTION , content);
        }
        else {
            if(!ignore_error)
                throwError(errors.UNCLOSED_PARENTHESIS, content);
        }
    }

    return false;
}

//Verifica se é um método definido no código
bool isCustomFunction(string content, bool ignore_error) {
    if (!read_without_execute) {
        if (isParenthesesOk(content)) {
            vector<string> command = splitFirst(content, '(');
            if (custom_functions.count(trim(command[0])))
                return true;
            else {
                if(!ignore_error)
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

//Verifica se é um bloco de condição
bool isConditionBlock(string params) {
    params = trim(params);
    if (isParenthesesOk(params)){
        if (startsWith(params, "else"))
            return true;
        return startsWith(params, "if");
    }
    return startsWith(params, "else");
}

//Verifica se está iniciando um escopo
bool initsBlock(string params) {
    int count = 0;
    int open_brace_count = 0;
    

    for (char c : params) {
        if (c == '{')
            open_brace_count++;
    }

    return open_brace_count == 1;
}

//Verifica a variante do condition block
void executeConditionBlock(string params){

    if (startsWith(trim(params), "else if")) {

        vector<string> condition = splitFirst(params, '(');
        condition[1] = trim(condition[1]);

        vector<string> in_line;
        scopes.top().readed_condition = "else if";

        bool satisfied = scopes.top().satisfied_condition;

        if (scopes.top().readed_condition == "if" || scopes.top().readed_condition == "else if") {
        
            //Divide o final da condição do inicio do comando
            if (!endsWith(condition[1], ")"))
                in_line = splitWhenParenthesesClosed(condition[1], ')');

            if (in_line.size() == 2) {
                string in_line_command = trim(in_line[1]);

                string resolved_condition = resolveEvaluation(in_line[0]);
                if (resolved_condition == "1" && in_line_command != "{" && !satisfied) {
                    interpreterLine(in_line_command);
                    scopes.top().scope_type = "condition_in_line";
                    scopes.top().satisfied_condition = true;
                }
                else if (initsBlock(condition[1])) {
                    //Se resolved for false ele está caindo aqui e considerado inicio de bloco por causa do printf
                    Scope new_scope;
                    scopes.push(new_scope);

                    scopes.top().scope_type = "condition";

                    scopes.top().execute_block = resolved_condition == "1" && !satisfied;
                    scopes.top().search_block_end = true;
                    scopes.top().block_count++;

                    if (!satisfied && scopes.top().execute_block)
                        scopes.top().satisfied_condition = scopes.top().execute_block;

                }
            }
            else {
                Scope new_scope;
                scopes.push(new_scope);

                scopes.top().scope_type = "condition";

                scopes.top().execute_block = resolveEvaluation(condition[1].substr(0, condition[1].length() - 1)) == "1" && !satisfied;
                scopes.top().verify_next_line = true;

                if (!satisfied && scopes.top().execute_block)
                    scopes.top().satisfied_condition = scopes.top().execute_block;
            }
        }
        else {
            //DISPARAR O ERRO DO ELSE IF SEM IF
        }
    }
    else if (startsWith(trim(params), "else")) {

        if (scopes.top().readed_condition == "if" || scopes.top().readed_condition == "else if") {
            if (endsWith(params, ";")) {
                if (!scopes.top().satisfied_condition) {
                    vector<string> in_line = splitFirst(params, ' ');
                    interpreterLine(in_line[1]);
                }
            }
            else if (initsBlock(params)) {
                Scope new_scope;
                new_scope.satisfied_condition = scopes.top().satisfied_condition;
                scopes.push(new_scope);

                scopes.top().scope_type = "condition";

                scopes.top().search_block_end = true;
                if (!scopes.top().satisfied_condition)
                    scopes.top().execute_block = true;
                else
                    scopes.top().execute_block = false;
                scopes.top().block_count++;
            }
            else {

                Scope new_scope;
                new_scope.satisfied_condition = scopes.top().satisfied_condition;
                scopes.push(new_scope);

                scopes.top().scope_type = "condition";

                if (!scopes.top().satisfied_condition) {
                    scopes.top().verify_next_line = true;
                    scopes.top().execute_block = true;
                }
                else {
                    scopes.top().verify_next_line = true;
                    scopes.top().execute_block = false;
                }
            }
        }
        else {
            //DISPARAR ERRO DO ELSE SEM O IF
        }

    }
    else {
        vector<string> condition = splitFirst(params, '(');
        condition[1] = trim(condition[1]);

        vector<string> in_line;
        scopes.top().readed_condition = "if";
    
        //Divide o final da condição do inicio do comando
        if (!endsWith(condition[1], ")"))
            in_line = splitWhenParenthesesClosed(condition[1], ')');

        if (in_line.size() == 2) {
            string in_line_command = trim(in_line[1]);
            string resolved_condition = resolveEvaluation(in_line[0]);
            if (resolved_condition == "1" && in_line_command != "{") {
                /*interpreterLine(in_line_command);
                scopes.top().scope_type = "condition_in_line";
                scopes.top().execute_block = true;
                scopes.top().satisfied_condition = true;*/

                if (isCustomFunction(in_line_command, true)) {
                    scopes.top().back_loop_in_position = (int)(file_reference->tellg());
                    scopes.top().back_loop_in_line = current_line;

                    interpreterLine(in_line_command);

                    scopes.top().~Scope();
                    scopes.pop();

                    file_reference->clear();
                    file_reference->seekg(scopes.top().back_loop_in_position);
                    current_line = scopes.top().back_loop_in_line;
                }
                else {
                    interpreterLine(in_line_command);
                    scopes.top().execute_block = true;
                }
                scopes.top().scope_type = "condition_in_line";
                scopes.top().satisfied_condition = true;
            }
            else if (initsBlock(condition[1])) {
                Scope new_scope;
                scopes.push(new_scope);

                scopes.top().scope_type = "condition";

                scopes.top().execute_block = resolved_condition == "1";
                scopes.top().search_block_end = true;
                scopes.top().block_count++;
                scopes.top().satisfied_condition = scopes.top().execute_block;
                
            }
            else {
                scopes.top().satisfied_condition = false;
            }
        }
        else {
            Scope new_scope;
            scopes.push(new_scope);

            scopes.top().scope_type = "condition";

            scopes.top().execute_block = resolveEvaluation(condition[1].substr(0, condition[1].length() - 1)) == "1";
            scopes.top().verify_next_line = true;
            scopes.top().satisfied_condition = scopes.top().execute_block;
        }
    }

}

//Verifica se o bloco é de uma unica instrução
void verifyBlockOrSingleInstruction(string s) {
    if (s == "{") {
        if(scopes.top().scope_type == "condition")
            scopes.top().search_block_end = true;
        else if(scopes.top().scope_type == "loop")
            scopes.top().search_block_end_while = true;
        else if(scopes.top().scope_type == "loop_for")
            scopes.top().search_block_end_for = true;
        scopes.top().block_count++;
    }
    else {
        if (scopes.top().scope_type == "condition") {
            scopes.top().scope_type = "condition_single";
            interpreterLineAndFindEnd(s);

            bool satisfied = scopes.top().satisfied_condition;

            scopes.top().search_block_end = false;
            scopes.top().execute_block = false;
            scopes.top().verify_next_line = false;
            scopes.top().block_count = 0;
            scopes.top().~Scope();
            scopes.pop();

            scopes.top().satisfied_condition = satisfied;
        }
        else if (scopes.top().scope_type == "loop_for") {
            //Ajustar
            scopes.top().scope_type = "condition_single";
            interpreterLineAndFindEnd(s);

            scopes.top().search_block_end_for = false;
            scopes.top().execute_block = false;
            scopes.top().verify_next_line = false;
            scopes.top().block_count = 0;
            scopes.top().~Scope();
            scopes.pop();
        }
    }
}

//Verifica se é o bloco de WHILE
bool isWhileBlock(string params) {
    if (isParenthesesOk(params))
        return startsWith(params, "while");
    return false;
}

//Executa o bloco WHILE
void executeWhileBlock(string params) {
    vector<string> condition = splitFirst(params, '(');
    condition[1] = trim(condition[1]);

    vector<string> in_line;

    if (!endsWith(condition[1], ")"))
        in_line = splitFirst(condition[1], ')');

    if (in_line.size() == 2) {
        string in_line_command = trim(in_line[1]);
        if (resolveEvaluation(in_line[0]) == "1" && in_line_command != "{") {
            while (resolveEvaluation(in_line[0]) == "1") 
                interpreterLine(in_line_command);
        }
        else if (initsBlock(condition[1])) {
            scopes.top().back_loop_in_position = (int)(file_reference->tellg()) - (params.length() + 2);
            scopes.top().back_loop_in_line= current_line;

            Scope new_scope;
            scopes.push(new_scope);

            scopes.top().scope_type = "loop";
            scopes.top().execute_block = resolveEvaluation(in_line[0]) == "1";
            scopes.top().search_block_end_while = true;
            scopes.top().block_count++;
        }
    }
    else {
        scopes.top().back_loop_in_position = (int)(file_reference->tellg()) - (params.length() + 2);
        scopes.top().back_loop_in_line= current_line;

        Scope new_scope;
        scopes.push(new_scope);

        scopes.top().scope_type = "loop";

        scopes.top().execute_block = resolveEvaluation(condition[1].substr(0, condition[1].length() - 1)) == "1";
        scopes.top().verify_next_line = true;
    }
}

//Verifica se é o bloco FOR
bool isForLoopBlock(string params) {
    if (isParenthesesOk(params)) return startsWith(params, "for");
    return false;
}

//Executa o bloco FOR
void executeForBlock(string params) {
    vector<string> condition = splitFirst(params, '(');
    condition[1] = trim(condition[1]);

    vector<string> in_line;
    if (!endsWith(condition[1], ")"))
        in_line = splitWhenParenthesesClosed(condition[1], ')');

    if (in_line.size() == 2) {

        vector<string> loop_params = split(in_line[0], ';');
        if (!loop_params[0].empty()) {
            string var_name = trim(splitFirst((splitFirst(loop_params[0], ' ')[1]), '=')[0]);
            if (!variableExists(var_name))
                declareVariable(loop_params[0], false);
        }

        string in_line_command = trim(in_line[1]);

        if (in_line_command != "{") {
            
            for (; resolveEvaluation(trim(loop_params[1])) == "1"; updateVariable(trim(loop_params[2]) + ";")) 
                interpreterLine(in_line_command);

        }
        else if (initsBlock(condition[1])) {
            scopes.top().back_loop_in_position = (int)(file_reference->tellg()) - (params.length() + 2);
            scopes.top().back_loop_in_line= current_line;
            scopes.top().step_for = trim(loop_params[2]) + ";";

            Scope new_scope;
            scopes.push(new_scope);

            scopes.top().scope_type = "loop_for";
            scopes.top().execute_block = resolveEvaluation(trim(loop_params[1])) == "1";
            scopes.top().search_block_end_for = true;
            scopes.top().block_count++;
        }
    }
    else {
        vector<string> loop_params = split(condition[1], ';');

        if (!loop_params[0].empty()) {
            string var_name = trim(splitFirst((splitFirst(loop_params[0], ' ')[1]), '=')[0]);
            if (!variableExists(var_name))
                declareVariable(loop_params[0], false);
        }

        scopes.top().back_loop_in_position = (int)(file_reference->tellg()) - (params.length() + 2);
        scopes.top().step_for = trim(loop_params[2].substr(0, loop_params[2].length() - 1)) + ";";

        Scope new_scope;
        scopes.push(new_scope);

        scopes.top().scope_type = "loop_for";

        scopes.top().execute_block = resolveEvaluation(trim(loop_params[1])) == "1";
        scopes.top().verify_next_line = true;
    }
}

//Verifica se é uma declaração de método
bool isMethodDeclaration(string params) {
    if (isParenthesesOk(params)){
        return startsWith(params, "public static");
    }
    return false;
}

//Cadastra um método definido no código
void registerMethod(string params) {
    vector<string> method_pices = splitFirst(params, '(');
    vector<string> keywords = split(method_pices[0], ' ');
    FunctionsAssignment new_function;

    new_function.visibility = trim(keywords[0]);
    new_function.acessibility = trim(keywords[1]);
    new_function.return_type = trim(keywords[2]);
    new_function.name = trim(keywords[3]);
    new_function.start_in_line = (int)(file_reference->tellg()) - (params.length() + 2);
    new_function.params = split(method_pices[1], ')')[0];

    if (new_function.name == "main")
        throwError(errors.NAME_NOT_ALLOWED, new_function.name);

    Scope new_scope;
    scopes.push(new_scope);

    scopes.top().search_block_end_method = true;
    scopes.top().execute_block = false;
    scopes.top().method_declaration = true;
    scopes.top().block_count++;

    custom_functions[new_function.name] = new_function;
}

//Executa a função adequada para cada linha
void interpreterLine(string content) {

    if (scopes.top().verify_next_line)
        verifyBlockOrSingleInstruction(content);
    else if(trim(content) != "}" && trim(content) != "{") {
        if (isConditionBlock(content))
            executeConditionBlock(content);

        else if (isWhileBlock(content))
            executeWhileBlock(content);

        else if (isForLoopBlock(content))
            executeForBlock(content);

        else if (isMethodDeclaration(content))
            registerMethod(content);

        else if (content[content.length() - 1] != ';')
            throwError(errors.SEMICOLON, content);
        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, false) && !read_without_execute)
            executeNativeFunction(content);
    }
}

//Executa o escopo procurando seu fim
void interpreterLineAndFindEnd(string content) {

    for (char c : content) {
        if (c == '}' && scopes.top().scope_type != "condition_single") {
            scopes.top().block_count--;
            if (scopes.top().block_count == 0) {

                bool satisfied = scopes.top().satisfied_condition;

                //Limpa o escopo
                scopes.top().search_block_end = false;
                scopes.top().execute_block = false;
                scopes.top().verify_next_line = false;
                scopes.top().~Scope();
                scopes.pop();

                if(satisfied)
                    scopes.top().satisfied_condition = satisfied;
            }
        }
        else if (c == '{' && scopes.top().scope_type != "condition_single")
            scopes.top().block_count++;
    }

    if (scopes.top().execute_block && trim(content) != "}" && trim(content) != "{") {
        if (isConditionBlock(content))
            executeConditionBlock(content);

        else if (isWhileBlock(content))
            executeWhileBlock(content);

        else if (isForLoopBlock(content))
            executeForBlock(content);

        else if (content[content.length() - 1] != ';')
            throwError(errors.SEMICOLON, content);
        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, false) && !read_without_execute)
            executeNativeFunction(content);
    }

}

//
void interpreterLineInWhile(string content, ifstream * file) {
    
    bool block_next_brace_count = false;

    if (scopes.top().execute_block && trim(content) != "}" && trim(content) != "{") {
        if (isConditionBlock(content))
            executeConditionBlock(content);

        else if (isWhileBlock(content)) {
            executeWhileBlock(content);
            block_next_brace_count = true;
        }

        else if (isForLoopBlock(content)) {
            executeForBlock(content);
            block_next_brace_count = true;
        }

        else if (content[content.length() - 1] != ';')
            throwError(errors.SEMICOLON, content);
        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, false) && !read_without_execute)
            executeNativeFunction(content);
    }

    for (char c : content) {
        if (c == '}' && scopes.top().scope_type != "condition_in_line" && !block_next_brace_count) {
            scopes.top().block_count--;
            if (scopes.top().block_count == 0) {

                bool back_to_top = scopes.top().execute_block;

                scopes.top().~Scope();
                scopes.pop();

                if (back_to_top) {
                    file->clear();
                    file->seekg(scopes.top().back_loop_in_position);
                    current_line = scopes.top().back_loop_in_line;
                }
            }
        }
        else if (c == '{' && !startsWith(scopes.top().scope_type, "condition") && !block_next_brace_count)
            scopes.top().block_count++;
    }

    if (scopes.top().scope_type == "condition_in_line") scopes.top().scope_type = "";
}

//
void interpreterLineInFor(string content, ifstream* file) {

    bool block_next_brace_count = false;

    if (scopes.top().execute_block && trim(content) != "}" && trim(content) != "{") {
        if (isConditionBlock(content))
            executeConditionBlock(content);

        else if (isWhileBlock(content)) {
            executeWhileBlock(content);
            block_next_brace_count = true;
        }

        else if (isForLoopBlock(content)) {
            executeForBlock(content);
            block_next_brace_count = true;
        }

        else if (content[content.length() - 1] != ';')
            throwError(errors.SEMICOLON, content);

        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, false) && !read_without_execute)
            executeNativeFunction(content);
    }

    for (char c : content) {
        if (c == '}' && scopes.top().scope_type != "condition_in_line" && !block_next_brace_count) {
            scopes.top().block_count--;
            if (scopes.top().block_count == 0) {

                bool back_to_top = scopes.top().execute_block;

                scopes.top().~Scope();
                scopes.pop();

                updateVariable(scopes.top().step_for);
                if (back_to_top) {
                    file->clear();
                    file->seekg(scopes.top().back_loop_in_position);
                    current_line = scopes.top().back_loop_in_line;
                }
            }
        }
        else if (c == '{' && !startsWith(scopes.top().scope_type, "condition") && !block_next_brace_count)
            scopes.top().block_count++;
    }

    if (scopes.top().scope_type == "condition_in_line") scopes.top().scope_type = "";
}

//
void interpreterLineInMethod(string content) {

    bool ignore_line = isMethodDeclaration(content);
    bool block_next_brace_count = false;

    if (startsWith(content, "return") && endsWith(content, ";") && scopes.top().execute_block) {
        vector<string> returned_pieces = splitFirst(content, ' ');
        string return_type = getTypeOfVariable(returned_pieces[1].substr(0, returned_pieces[1].length()-1));

        if (return_type == "Null")
            return_type = getTypeOfData(returned_pieces[1].substr(0, returned_pieces[1].length() - 1));

        if (return_type == scopes.top().method_reference->return_type) {
            scopes.top().current_return_type = return_type;
            
            if (return_type == "Int")
                scopes.top().returned.returned_int = stoi(resolveEvaluation(returned_pieces[1].substr(0, returned_pieces[1].length() - 1)));
            else if (return_type == "Double")
                scopes.top().returned.returned_double = stod(resolveEvaluation(returned_pieces[1].substr(0, returned_pieces[1].length() - 1)));
            else if(return_type == "Boolean")
                scopes.top().returned.returned_boolean = stoi(resolveEvaluation(returned_pieces[1].substr(0, returned_pieces[1].length() - 1)));
            else if (return_type == "String")
                scopes.top().returned_string = resolveEvaluation(returned_pieces[1].substr(0, returned_pieces[1].length() - 1));
            else if(return_type == "Char")
                scopes.top().returned.returned_char = resolveEvaluation(returned_pieces[1].substr(0, returned_pieces[1].length() - 1))[0];
            //Adicionar os outros tipos VOID e etc
        }
        else {
            throwError(errors.INCORRECT_TYPE, content);
            throw - 1;
        }

        scopes.top().execute_block = false;
    }
    else if (scopes.top().execute_block && trim(content) != "}" && trim(content) != "{" && !ignore_line) {
        if (isConditionBlock(content)) {
            executeConditionBlock(content);
            block_next_brace_count = true;
        }

        else if (isWhileBlock(content)) {
            executeWhileBlock(content);
            block_next_brace_count = true;
        }

        else if (isForLoopBlock(content)) {
            executeForBlock(content);
            block_next_brace_count = true;
        }

        else if (content[content.length() - 1] != ';')
            throwError(errors.SEMICOLON, content);
        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, false) && !read_without_execute)
            executeNativeFunction(content);
    }

    for (char c : content) {
        if (c == '}' && scopes.top().scope_type != "condition_single" && !block_next_brace_count) {
            scopes.top().block_count--;
            if (scopes.top().block_count == 0) {

                bool declaration = scopes.top().method_declaration;
                string return_type = scopes.top().current_return_type;
                string returned_value_string = scopes.top().returned_string;
                FunctionsRetruns * returned_value = &scopes.top().returned;
            
                //Limpa o escopo
                scopes.top().search_block_end_method = false;
                scopes.top().execute_block = false;
                scopes.top().verify_next_line = false;
                scopes.top().~Scope();
                scopes.pop();

                if (!declaration) {
                    file_reference->clear();
                    file_reference->seekg(scopes.top().back_loop_in_position);
                    current_line = scopes.top().back_loop_in_line;
                }

                string variable_waiting = scopes.top().variable_wait_value;
                if (variable_waiting != "") {
                    string value;
                    variable_waiting = split(variable_waiting, "=")[0];

                    if (return_type == "Int")
                        value = to_string(returned_value->returned_int);
                    else if (return_type == "Double")
                        value = to_string(returned_value->returned_double);
                    else if (return_type == "String")
                        value = ("\"" + returned_value_string + "\"");


                    declareVariable(variable_waiting + " = " + value + ";", false);
                }

                scopes.top().current_return_type = return_type;
                scopes.top().returned = *returned_value;

            }
        }
        else if (c == '{' && scopes.top().scope_type != "condition_single" && !ignore_line && !block_next_brace_count)
            scopes.top().block_count++;
    }
}

//Retorna o nome do arquivo main
string getFilename() { return filename; };

//Define o arquivo de entrada
void setFilename(string f) { filename = f; };

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
}

//Ler arquivo
void readFile(string code_name) {
    ifstream file;
    string content;

    file.open(code_name);

    Scope main;
    scopes.push(main);

    if (file.good()) {
        if (file.is_open()) {

            file_reference = &file;
            while (getline(file, content)) {

                if(!content.empty()) content = trim(content);

                if (scopes.top().search_block_end) {
                    if (!startsWith(content, "//") && !content.empty())
                        interpreterLineAndFindEnd(trim(content));
                }
                else if (scopes.top().search_block_end_while) {
                    if (!startsWith(content, "//") && !content.empty())
                        interpreterLineInWhile(trim(content), &file);
                }
                else if (scopes.top().search_block_end_for) {
                    if (!startsWith(content, "//") && !content.empty())
                        interpreterLineInFor(trim(content), &file);
                }
                else if (scopes.top().search_block_end_method) {
                    if (!startsWith(content, "//") && !content.empty())
                        interpreterLineInMethod(trim(content));
                }
                else {
                    if (!startsWith(content, "//") && !content.empty())
                        interpreterLine(trim(content));
                }
                current_line++;
                
            }

            main.~Scope();
            scopes.pop();
        }
    }
    else
        cout << colors.RED << "File \"" << code_name << "\" not founded or not exists" << colors.RESET << endl;

    file.close();
}