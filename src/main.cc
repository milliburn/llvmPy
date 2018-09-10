#include <llvmPy/AST.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
using namespace llvmPy::AST;
using namespace std;

int
main(int argc, char * argv[])
{
    Expr * expr;
    char line[256];

    while (true) {
        string input;
        cout << "> ";
        getline(cin, input);

        if (input == "exit") {
            break;
        }

        istringstream stream(input);
        expr = Expr::parse(stream);

        if (expr == nullptr) {
            continue;
        }

        string out = expr->toString();
        cout << out << endl;
    }

    return 0;
}
