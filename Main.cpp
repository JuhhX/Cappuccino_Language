#include <iostream>
#include <cstdio>
#include <string>

#include <windows.h>

#include <direct.h>
#include <limits.h>

#include "BasicUtilities.h"
#include "General.h"

using namespace std;

int main() {

    // Executa o analisador Rust
    char buff[MAX_PATH];
    char* v = _getcwd(buff, MAX_PATH);
    string current_working_dir(buff);
    string current_folder = ".\\cappuccino_analyzer\\cappuccino_analyzer " + current_working_dir;

    FILE* pipe = _popen(current_folder.c_str(), "r");

    if (!pipe) {
        throwError(errors.ERRORS_ANALYZING, "");
        return -1;
    }

    //Obtem a saída do Rust
    char buffer[128];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    int status = _pclose(pipe);

    //Executa o código
    if (status == 0) {

        initializeFunctions();

        setFilename("Main.cappuccino");

        try {
            readFile(getFilename());
        }
        catch (int e) {
            if (e == -1)
                cerr << colors.RED << endl << "CRITICAL ERROR!" << colors.RESET << endl;
        }
    }
    else if (status == 1) 
        cout << result << endl;

	return 0;
}