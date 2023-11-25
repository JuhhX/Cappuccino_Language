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
    int pos_inicial = 0;
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
