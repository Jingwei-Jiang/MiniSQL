#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <set>
#include <vector>
#include <string>
#include "CatalogManager.h"
#include "Exception.h"
#include "API.h"
using namespace std;

class Interpreter {
private:
    API* Api;
    CatalogManager* Catalog;
    bool Quitted;
public:
    const set<string> KEYWORD_LIST
    {
        "create", "drop", "select", "insert", "delete", "quit", "execfile",
        "table", "index", 
        "int", "char", "float", "primary", "key", "unique",
        "from", "where", "into", "values", "and", "on"
    };
    const set<string> OPERATOR_LIST
    {
        "*", "(", ")", ",",
        "<", "<=", "=", ">=", ">", "!="
    };
    enum TokenType {
        Keyword,
        Int,
        Float,
        String,
        Name,
        Operator
    };
    
    class Token {
    public:
        string Name;
        TokenType Type;
        Token(string name, TokenType type): Name(name), Type(type) {}
    };
    
    Interpreter();
    void GetQuery();
    void Run(string SQLpath);
    string ReadFile(string SQLpath);
    vector<Token> Tokenize(string query);
    void AnalyseToken(vector<Token> &tokens);
    void ExecuteQuery(string query);
    void Quit();
    
    string Beautify(string raw) const;
    vector<string> Split(string str, char ch) const;
    bool IsRunning() const;

};

#endif