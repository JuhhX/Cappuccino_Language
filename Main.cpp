#include <iostream>
#include <string>

#include "BasicUtilities.h"
#include "GeneralFunctions.h"

using namespace std;

int main() {

    initializeFunctions();

    setFilename("Main.capuccino");

    try {
        readFile(getFilename());
    }
    catch (int e) {
        if (e == -1) 
            cerr << colors.RED << endl << "CRITICAL ERROR!" << colors.RESET << endl;
    }

	return 0;
}