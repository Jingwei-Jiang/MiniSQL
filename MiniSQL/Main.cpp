#include <iostream>
#include <string>
#include "Interpreter.h"
using namespace std;

int main() {
    Interpreter itp;
    while (itp.IsRunning()) {
        itp.GetQuery();
    }
    system("pause");
}