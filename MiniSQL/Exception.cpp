#include <iostream>
#include <string>
#include "Exception.h"
using namespace std;

Exception::Exception(): Message("An exception occured during runtime.") {}

Exception::Exception(string message): Message(message) {}

void Exception::ShowMessage() {
    cout << Message << endl;
}