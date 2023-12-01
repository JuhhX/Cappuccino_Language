#include "StringUtilities.h"

string trim(string s) {

	while (s[0] == ' ' || s[0] == '\t')
		s = s.substr(1, s.size());

	while (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\t')
		s = s.substr(0, s.length() - 1);

	return s;
}

string trimEnd(string s) {
	while (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\t')
		s = s.substr(0, s.length() - 1);

	return s;
}

string trimStart(string s) {
	while (s[0] == ' ' || s[0] == '\t')
		s = s.substr(1, s.size());
	return s;
}

vector<string> split(string s, char separator) {
    vector<string> retorno;
    int pos_inicial = 0;
    int pos_caract = (int)s.find(separator);

    while (pos_caract != string::npos) {
        retorno.push_back(s.substr(pos_inicial, pos_caract - pos_inicial));
        pos_inicial = pos_caract + 1;
        pos_caract = (int)s.find(separator, pos_inicial);
    }

    retorno.push_back(s.substr(pos_inicial));

    return retorno;
}

vector<string> split(string s, string separator) {
    vector<string> retorno;
    size_t pos_inicial = 0;
    int pos_caract = (int)s.find(separator);

    while (pos_caract != string::npos) {
        retorno.push_back(s.substr(pos_inicial, pos_caract - pos_inicial));
        pos_inicial = pos_caract + separator.length();
        pos_caract = (int)s.find(separator, pos_inicial);
    }

    retorno.push_back(s.substr(pos_inicial));

    return retorno;
}

vector<string> splitFirst(string s, char separator) {
    vector<string> retorno;
    int pos_caract = (int)s.find(separator);

    if (pos_caract != string::npos) {
        retorno.push_back(s.substr(0, pos_caract));
        retorno.push_back(trim(s.substr(pos_caract + 1)));
    }

    return retorno;
}

vector<string> splitWhenParenthesesClosed(string s, char separator) {
    vector<string> retorno;
    int parentheses_count = 1;
    int pos_caract = 0;

    for (char c : s) {
        if (c == '(') 
            parentheses_count++;
        else if (c == ')')
            parentheses_count--;

        if (parentheses_count == 0) {
            retorno.push_back(s.substr(0, pos_caract));
            retorno.push_back(trim(s.substr(pos_caract + 1)));
            break;
        }

        pos_caract++;
    }

    return retorno;
}

vector<string> splitLast(string s, char separator) {
    vector<string> retorno;

    int pos_caract = (int)s.rfind(separator);

    if (pos_caract != string::npos) {
        retorno.push_back(s.substr(0, pos_caract));
        retorno.push_back(trim(s.substr(pos_caract + 1)));
    }

    return retorno;
}

bool startsWith(string s, string t) {
    return s.find(t) == 0;
}

bool endsWith(string s, string t) {
    return s.rfind(t) == s.length() - 1;
}

bool isString(string s) {
    return s.find("\"") == 0 && s.rfind("\"") == s.length() - 1;
}

//Resolve uma expressão dentro de uma string
string resolveFormatExpression(string params) {
    if (isVector(params)) {
        int slots = getVectorSlots(trim(params.substr(0, params.length() - 1)));
        params = splitFirst(params, '[')[0];

        if (variableExists(params + "[" + to_string(slots) + "]")) {
            return (getVariableValue(params + "[" + to_string(slots) + "]", true));
        }
    }
    else if (variableExists(params))
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

        if (s[current_pos] == '{') {
            expression_init = current_pos + 1;
            is_reading = true;
        }
        else if (s[current_pos] == '}') {
            result += resolveFormatExpression(s.substr(expression_init, current_pos - expression_init));
            is_reading = false;
        }
        else if (!is_reading)
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
            if (count == 0)  cout << endl;
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
