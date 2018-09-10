#include <llvmPy/AST.h>
#include <llvmPy/Codegen.h>
#include <llvm/Support/raw_ostream.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
using namespace llvmPy::AST;
using namespace std;

int
main(int argc, char * argv[])
{
    char line[256];

    while (true) {
        string input;
        cout << "> ";
        getline(cin, input);

        if (input == "exit") {
            break;
        }

        istringstream stream(input);
        Expr& expr = *Expr::parse(stream);
        llvmPy::Codegen cg;
        auto& ir = *expr.codegen(cg);
        ir.print(llvm::outs());
        cout << endl;
    }

    return 0;
}
