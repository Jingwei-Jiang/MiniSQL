#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <string>
using namespace std;

class Exception {
public:
    string Message;
    Exception();
    Exception(string message);
    void ShowMessage();
};

#endif