#include <iostream>
#include "General.h"
#include "StringUtilities.h"
#include "BasicUtilities.h"

using namespace std;

//Dispara erros
void throwError(string error_type, string line_content) {
    cout << endl << colors.RED << (filename + ":" + to_string(current_line) + ": ") << error_type << colors.RESET << endl;
    cout << "\t" << line_content << endl;
    read_without_execute = true;
}

//Executa m�todos definidos no c�digo
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
        current_line = static_cast<int>(custom_functions[trim(command_parameters[0])].start_in_line);

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
            current_line = static_cast<int>(custom_functions[trim(command_parameters[0])].start_in_line);

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

//Executa m�todos definidos no c�digo
void executeCustomFunction(string content, Object * c) {
    vector<string> command_parameters = splitFirst(content, '(');
    string method_name = trim(splitFirst(command_parameters[0], '.')[1]);

    //Sem parametros
    if (startsWith(command_parameters[1], ")") && c->custom_functions[trim(method_name)].params == "") {

        scopes.top().back_loop_in_position = (int)(file_reference->tellg());
        scopes.top().back_loop_in_line = current_line;

        Scope new_scope;
        scopes.push(new_scope);

        file_reference->clear();
        file_reference->seekg(c->custom_functions[trim(method_name)].start_in_line);
        current_line = static_cast<int>(c->custom_functions[trim(method_name)].start_in_line);

        scopes.top().class_reference = c;
        scopes.top().method_reference = &c->custom_functions[trim(method_name)];
        scopes.top().search_block_end_method = true;
        scopes.top().execute_block = true;
        scopes.top().block_count++;
    }
    //Com parametros
    else {

        scopes.top().back_loop_in_position = (int)(file_reference->tellg());
        scopes.top().back_loop_in_line = current_line;
        command_parameters[1] = command_parameters[1].substr(0, command_parameters[1].length() - 2);

        vector<string> informed_params = split(command_parameters[1], ',');
        vector<string> method_params = split(c->custom_functions[trim(method_name)].params, ',');

        if (informed_params.size() == method_params.size()) {
            Scope new_scope;
            scopes.push(new_scope);

            for (int x = 0; x < method_params.size(); x++) {
                declareVariable(trim(method_params[x]) + "=" + trim(informed_params[x]) + ";", false);
            }

            file_reference->clear();
            file_reference->seekg(c->custom_functions[trim(method_name)].start_in_line);
            current_line = static_cast<int>(c->custom_functions[trim(method_name)].start_in_line);

            scopes.top().class_reference = c;
            scopes.top().method_reference = &c->custom_functions[trim(method_name)];
            scopes.top().search_block_end_method = true;
            scopes.top().execute_block = true;
            scopes.top().block_count++;
        }
        else {
            throwError(errors.MISSING_PARAMETERS, c->custom_functions[trim(command_parameters[0])].params);
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

//Verifica se � um m�todo definido no c�digo
bool isCustomFunction(string content, bool ignore_error) {
    if (isParenthesesOk(content)) {
        vector<string> command = splitFirst(content, '(');
        if (!command[0].empty()) {
            if (custom_functions.count(trim(command[0])))
                return true;
            else {
                if(!ignore_error)
                    throwError(errors.UNDEFINED_FUNCTION, content);
            }
        }
    }
    else {
        if (!ignore_error)
            throwError(errors.UNCLOSED_PARENTHESIS, content);
    }
    
    return false;
}

//Verifica se � um bloco de condi��o
bool isConditionBlock(string params) {
    params = trim(params);
    if (isParenthesesOk(params)){
        if (startsWith(params, "else"))
            return true;
        return startsWith(params, "if");
    }
    return startsWith(params, "else");
}

//Verifica se est� iniciando um escopo
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
        
            //Divide o final da condi��o do inicio do comando
            if (!endsWith(condition[1], ")"))
                in_line = splitWhenParenthesesClosed(condition[1], ')');

            if (in_line.size() == 2) {
                string in_line_command = trim(in_line[1]);

                string resolved_condition = resolveEvaluation(in_line[0]);
                if (resolved_condition == "1" && in_line_command != "{" && !satisfied) {
                    if (isCustomFunction(in_line_command, true)) {
                        string line;
                        scopes.top().back_loop_in_position = (int)(file_reference->tellg());
                        executeCustomFunction(in_line_command);

                        while (getline(*file_reference, line) && scopes.top().search_block_end_method) {
                            if (line.empty() || startsWith(line, "//")) continue;
                            if (!line.empty()) line = trim(line);

                            interpreterLineInMethod(line);
                        }

                        file_reference->clear();
                        file_reference->seekg(scopes.top().back_loop_in_position);
                        current_line = scopes.top().back_loop_in_line;
                    }
                    else 
                        interpreterLine(in_line_command);
                    scopes.top().scope_type = "condition_in_line";
                    scopes.top().satisfied_condition = true;
                }
                else if (initsBlock(condition[1])) {
                    //Se resolved for false ele est� caindo aqui e considerado inicio de bloco por causa do printf
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
    
        //Divide o final da condi��o do inicio do comando
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
                    string line;
                    scopes.top().back_loop_in_position = (int)(file_reference->tellg());
                    executeCustomFunction(in_line_command);

                    while (getline(*file_reference, line) && scopes.top().search_block_end_method) {
                        if (line.empty() || startsWith(line, "//")) continue;
                        if (!line.empty()) line = trim(line);

                        interpreterLineInMethod(line);
                    }

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

//Verifica se o bloco � de uma unica instru��o
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

//Verifica se � o bloco de WHILE
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

//Verifica se � o bloco FOR
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

//Verifica se � uma declara��o de m�todo
bool isMethodDeclaration(string params) {
    if (isParenthesesOk(params)){
        return startsWith(params, "public");
    }
    return false;
}

//Cadastra um m�todo definido no c�digo
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


//Executa a fun��o adequada para cada linha
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

        else if (isClassDeclaration(content))
            registerClass(content);

        else if (isVariableDeclaration(content))
            declareVariable(content, false);
        else if (isObjectDeclaration(content))
            declareObject(content, false);

        else if (isObjectVariableUpdate(content))
            updateObjectVariable(content);

        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isObjectMethod(content))
            executeObjectMethod(content);
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

        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isObjectDeclaration(content))
            declareObject(content, false);

        else if (isObjectVariableUpdate(content))
            updateObjectVariable(content);

        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isObjectMethod(content))
            executeObjectMethod(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, false) && !read_without_execute)
            executeNativeFunction(content);
    }

}

//Executa o escopo do WHILE
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

        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isObjectDeclaration(content))
            declareObject(content, false);

        else if (isObjectVariableUpdate(content))
            updateObjectVariable(content);

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

//Executa o escopo do FOR
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

        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isObjectDeclaration(content))
            declareObject(content, false);

        else if (isObjectVariableUpdate(content))
            updateObjectVariable(content);

        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isObjectMethod(content))
            executeObjectMethod(content);
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
                else
                    scopes.top().pop_last_variable();
            }
        }
        else if (c == '{' && !startsWith(scopes.top().scope_type, "condition") && !block_next_brace_count)
            scopes.top().block_count++;
    }

    if (scopes.top().scope_type == "condition_in_line") scopes.top().scope_type = "";
}

//Executa o escopo de um m�todo
void interpreterLineInMethod(string content) {

    bool ignore_line = isMethodDeclaration(content);
    bool block_next_brace_count = false;
    bool is_native_function = false;

    if (startsWith(content, "return") && endsWith(content, ";") && scopes.top().execute_block) {
        vector<string> returned_pieces = splitFirst(content, ' ');
        string return_type;

        if (startsWith(trim(returned_pieces[1]), "this.")) {
            string var_name = trim(splitFirst(returned_pieces[1], '.')[1]);
            var_name = var_name.substr(0, var_name.length() - 1);
            return_type = scopes.top().class_reference->getTypeOfVariable(var_name);
        }
        else {
            return_type = getTypeOfVariable(returned_pieces[1].substr(0, returned_pieces[1].length() - 1));
        }

        if (return_type == "Null")
            return_type = getTypeOfData(returned_pieces[1].substr(0, returned_pieces[1].length() - 1));

        if (return_type == "Null") {
            if (isNativeFunction(returned_pieces[1], true)) {
                executeNativeFunction(returned_pieces[1]);
                return_type = scopes.top().current_return_type;
                is_native_function = true;
            }
        }

        if (return_type == scopes.top().method_reference->return_type) {
            if (!is_native_function) {
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

        /*else if (content[content.length() - 1] != ';')
            throwError(errors.SEMICOLON, content);*/
        else if (isVariableDeclaration(content) && !read_without_execute)
            declareVariable(content, false);
        else if (isObjectDeclaration(content))
            declareObject(content, false);

        else if (isObjectVariableUpdate(content))
            updateObjectVariable(content);

        else if (isVariableUpdate(content) && !read_without_execute)
            updateVariable(content);
        else if (isCustomFunction(content, true) && !read_without_execute)
            executeCustomFunction(content);
        else if (isNativeFunction(content, true) && !read_without_execute)
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

                scopes.top().current_return_type = return_type;
                scopes.top().returned = *returned_value;
                scopes.top().returned_string = returned_value_string;

            }
        }
        else if (c == '{' && scopes.top().scope_type != "condition_single" && !ignore_line && !block_next_brace_count)
            scopes.top().block_count++;
    }
}

//
void interpreterLineInClass(string content) {
    for (char c : content) {
        if (c == ' ') continue;

        if (c == '}') {
            scopes.top().block_count--;
            if (scopes.top().block_count == 0) {


                //Limpa o escopo
                scopes.top().search_block_end_class = false;
                scopes.top().execute_block = false;
                scopes.top().~Scope();
                scopes.pop();

            }
        }
        else if (c == '{')
            scopes.top().block_count++;
    }
}

//
void interpreterLineInClass(string content, Object * c) {
    interpreterLineInClass(content);

    if (scopes.top().execute_block && trim(content) != "}" && trim(content) != "{" && !(content.empty() || content == "\t")) {
        vector<string> declaration_pieces = splitFirst(content, ' ');

        if (declaration_pieces.size() > 1) {

            string accessibility = trim(declaration_pieces[0]);

            //Ignorar variaveis dentro de m�todos

            if (endsWith(declaration_pieces[1], ";")) {
                if (isVariableDeclaration(content) && !read_without_execute) {
                    if (scopes.top().isClassMethod)
                        declareVariable(content, false);
                }
                else if (isNativeFunction(content, true) && !read_without_execute) {
                    if (scopes.top().isClassMethod)
                        executeNativeFunction(content);
                }
                else {
                    if(accessibility == "public" || accessibility == "private")
                        c->declareVariable(declaration_pieces[1], accessibility, false);
                }
            }
            else if (isParenthesesOk(content)) {
                if (!startsWith(content, "for") && !startsWith(content, "while") && !startsWith(content, "if") && !startsWith(content, "else")) {
                    c->declareMethod(content);
                }
            }
        }
    }

}

//Retorna o nome do arquivo main
string getFilename() { return filename; };

//Define o arquivo de entrada
void setFilename(string f) { filename = f; };

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

                if (content.empty() || startsWith(content, "//")) continue;
                if (!content.empty()) content = trim(content);

                if (scopes.top().search_block_end) {    
                    interpreterLineAndFindEnd(trim(content));
                }
                else if (scopes.top().search_block_end_while) {    
                    interpreterLineInWhile(trim(content), &file);
                }
                else if (scopes.top().search_block_end_for) {    
                    interpreterLineInFor(trim(content), &file);
                }
                else if (scopes.top().search_block_end_method) {    
                    interpreterLineInMethod(trim(content));
                }
                else if (scopes.top().search_block_end_class) {    
                    interpreterLineInClass(trim(content));
                }
                else 
                    interpreterLine(content);
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