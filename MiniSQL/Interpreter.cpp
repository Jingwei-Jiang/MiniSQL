#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <string>
#include <cstring>
#include <vector>
#include "time.h"
#include "SharedDataStructures.h"
#include "Interpreter.h"
#include "CatalogManager.h"
#include "Exception.h"
#include "API.h"
using namespace std;
using colType = TableCatalog::ColumnType;

Interpreter::Interpreter() {
    Catalog = new CatalogManager();
    Api = new API(Catalog);
    Quitted = false;
    cout << "MiniSQL v1.0" << endl
         << "Made by Chen Feng, Jiang Jingwei and Shen Ao, 2021." << endl;
}

void Interpreter::GetQuery() {
    if (Quitted)
        return;
    clock_t time_start;
    try {
        cout << endl << "Input query: (semicolon required)" << endl;
        string sql_raw = "";
        while (sql_raw.find(';') == sql_raw.npos) {
            string tmp;
            getline(cin, tmp);
            sql_raw += tmp;
        }
        string sql_beautified = Beautify(sql_raw);
        
        vector<string> sqls = Split(sql_beautified, ';');
        time_start = clock();
        for (string query: sqls) {
            vector<Interpreter::Token> tokens = Tokenize(query);
            if (tokens.empty())
                continue;
            AnalyseToken(tokens);
        }
        clock_t time_end = clock();
        double duration = time_end - time_start;
        cout << "Executed in " << duration << " miliseconds." << endl;
    }
    catch(Exception& ex) {
        ex.ShowMessage();
        clock_t time_end = clock();
        double duration = time_end - time_start;
        cout << "Executed in " << duration << " miliseconds." << endl;
        if (ex.Message == "Program quitted.")
            Quitted = true;
        return;
    }
}

void Interpreter::Run(string SQLpath) {
    //clock_t time_start = clock();
    try {
        string sql_file = ReadFile(SQLpath);
        string sql_beautified = Beautify(sql_file);
        vector<string> sqls = Split(sql_beautified, ';');

        for (string query: sqls) {
            vector<Interpreter::Token> tokens = Tokenize(query);
            if (tokens.empty())
                continue;
            AnalyseToken(tokens);
        }
    }
    catch(Exception& ex) {
        throw;
    }
}

void Interpreter::AnalyseToken(vector<Token> &tokens) {
    try {
        if (tokens[0].Name == "create") {
            if (tokens[1].Name == "table") {
                if (tokens[2].Type != Name)
                    throw Exception("Interpreter Error: Invalid table name.");
                string table_name = tokens[2].Name;

                if (Catalog->ExistTable(table_name)) {
                    throw Exception(
                        "Catalog Error: Table \'" + table_name + "\'" +
                        "already exists."
                    );
                }

                TableCatalog table(table_name);
                string primary_key = "";
                if (tokens[3].Name != "(") {
                    throw Exception(
                        "Interpreter Error:" \
                        "Illegal syntax for \'create table\'."
                    );
                }
                int pos = 4;
                while (pos < tokens.size() && tokens[pos].Name != ")") {
                    if (tokens[pos].Name == "primary" && tokens[pos+1].Name == "key") {
                        if (tokens[pos+2].Name != "(" || 
                            tokens[pos+3].Type != Name || 
                            tokens[pos+4].Name != ")") {
                            throw Exception(
                                "Interpreter Error:" \
                                "Illegal syntax for defining primary key."
                            );
                        }
                        string col_name = tokens[pos+3].Name;
                        if (!table.ExistColumn(col_name)) {
                            throw Exception(
                                "Catalog Error: Column \'" + col_name + "\'" +
                                "does not exist."
                            );
                        }
                        table.SetPrimaryKey(col_name);
                        primary_key = col_name;
                        pos += 5;
                        continue;
                    }

                    if (tokens[pos].Type != Name) {
                        throw Exception(
                            "Interpreter Error: Invalid column name.");
                    }
                    string col_name = tokens[pos++].Name;
                    if (table.ExistColumn(col_name)) {
                        throw Exception(
                            "Catalog Error: Column \'" + col_name + "\'" +
                            "already exists."
                        );
                    }
                    colType type;
                    int size = 0;
                    bool is_unique = false, is_primary = false;
                    if (tokens[pos].Name == "int")
                        type = colType::Int;
                    else if (tokens[pos].Name == "float")
                        type = colType::Float;
                    else if (tokens[pos].Name == "char") {
                        type = colType::Char;
                        if (tokens[pos+1].Name != "(" || 
                            tokens[pos+2].Type != Int || 
                            tokens[pos+3].Name != ")") {
                            throw Exception(
                                "Interpreter Error:" \
                                "Illegal syntax for defining columns."
                            );
                        }
                        try {
                            size = stoi(tokens[pos+2].Name);
                        }
                        catch(...) {
                            size = 255;
                        }
                        if (size > 255)
                            size = 255;
                        pos += 3;
                    }
                    else {
                        throw Exception(
                            "Interpreter Error: Illegal type for columns."
                        );
                    }
                    pos++;
                    if (tokens[pos].Name == "unique") {
                        is_unique = true;
                        pos++;
                    }
                    
                    table.AddColumn(TableCatalog::Column(
                        col_name, type, size, is_unique, is_primary
                    ));

                    if (tokens[pos].Name == ")")
                        break;
                    pos++;
                }
                Api->CreateTable(table);
                if (primary_key != "") {
                    Api->CreateIndex(
                        "%ind_primary_" + table.TableName + "_" + primary_key,
                        table.TableName, 
                        primary_key
                    );
                }
            }
            else if (tokens[1].Name == "index") {
                if (tokens[2].Type != Name)
                    throw Exception("Interpreter Error: Invalid index name.");
                string index_name = tokens[2].Name;
                if (Catalog->ExistIndex(index_name)) {
                    throw Exception(
                        "Catalog Error: Index \'" + index_name + "\'" +
                        "already exists."
                    );
                }
                if (tokens[3].Name != "on") {
                    throw Exception(
                        "Interpreter Error: Illegal syntax for creating index.");
                }
                string table_name = tokens[4].Name;
                if (tokens[5].Name != "(" || 
                    tokens[6].Type != Name || 
                    tokens[7].Name != ")") {
                    throw Exception(
                        "Interpreter Error: Illegal syntax for creating index.");
                }
                string column_name = tokens[6].Name;
                try {
                    Api->CreateIndex(index_name, table_name, column_name);
                }
                catch (Exception& ex) {
                    throw;
                }
            }
        }
        else if (tokens[0].Name == "drop") {
            if (tokens[1].Name == "table") {
                if (tokens[2].Type != Name)
                    throw Exception("Interpreter Error: Invalid table name.");
                string table_name = tokens[2].Name;
                /*
                if (!Catalog->ExistTable(table_name)) {
                    throw Exception(
                        "Catalog Error: Table \'" + table_name + "\'" +
                        "does not exist."
                    );
                }*/
                Api->DropTable(table_name);
            }
            else if (tokens[1].Name == "index") {
                if (tokens[2].Type != Name)
                    throw Exception("Interpreter Error: Invalid index name.");
                string index_name = tokens[2].Name;
                Api->DropIndex(index_name);
            }
        }
        else if (tokens[0].Name == "insert" && tokens[1].Name == "into") {
            if (tokens[2].Type != Name)
                throw Exception("Interpreter Error: Invalid table name.");
            string table_name = tokens[2].Name;
            if (!Catalog->ExistTable(table_name)) {
                throw Exception(
                    "Catalog Error: Table \'" + table_name + "\'" +
                    "does not exist."
                );
            }
            if (tokens[3].Name != "values" || tokens[4].Name != "(") {
                throw Exception(
                    "Interpreter Error:" \
                    "Illegal syntax for \'insert into\'."
                );
            }
            int pos = 5;
            Record record;
            try {
                TableCatalog cat = Api->GetTableCatalog(table_name);
                for (auto &col: cat.Columns) {
                    if (col.Type == colType::Int && tokens[pos].Type != Int || 
                        col.Type == colType::Float && 
                            tokens[pos].Type != Int && tokens[pos].Type != Float ||
                        col.Type == colType::Char && tokens[pos].Type != String) 
                    {
                        throw Exception(
                            "Interpreter Error:" \
                            "Value type mismatch for \'insert into\'."
                        );
                    }
                    if (col.Type == colType::Int) {
                        record.Add(stoi(tokens[pos].Name));
                    }
                    else if (col.Type == colType::Float) {
                        record.Add((float) atof(tokens[pos].Name.c_str()));
                    }
                    else {
                        record.Add(tokens[pos].Name.substr(0, col.Size));
                    }
                    pos += 2;
                }
                Api->InsertValue(table_name, record);
            }
            catch (Exception& ex) {
                throw;
            }
            catch (...) {
                throw Exception(
                    "Interpreter Error:" \
                    "Illegal syntax for \'insert into\'."
                );
            }
        }
        else if (tokens[0].Name == "select" && tokens[1].Name == "*" &&
                 tokens[2].Name == "from" ||
                 tokens[0].Name == "delete" && tokens[1].Name == "from")
        {
            int pos = tokens[0].Name == "select" ? 3 : 2;

            if (tokens[pos].Type != Name)
                throw Exception("Interpreter Error: Invalid table name.");
            string table_name = tokens[pos].Name;
            if (!Catalog->ExistTable(table_name)) {
                throw Exception(
                    "Catalog Error: Table \'" + table_name + "\'" +
                    "does not exist."
                );
            }
            if (tokens.size() == 3 && tokens[0].Name == "delete") {
                Api->ClearTable(table_name);
                return;
            }
            else if (tokens.size() == 4 && tokens[0].Name == "select") {
                Api->SelectAll(table_name);
                return;
            }
            if (tokens[pos+1].Name != "where") {
                throw Exception(
                    "Interpreter Error:" \
                    "Illegal syntax for \'" + tokens[0].Name + "\'."
                );
            }
            pos += 2;

            try {
                TableCatalog cat = Api->GetTableCatalog(table_name);
                Condition condition(cat);
                while (pos < tokens.size()) {
                    string column_name = tokens[pos].Name;
                    if (!cat.ExistColumn(column_name)) {
                        throw Exception(
                            "Catalog Error: Column \'" + column_name + "\'" +
                            "does not exist."
                        );
                    }
                    if (tokens[pos+1].Type != Operator) {
                        throw Exception(
                            "Interpreter Error: Invalid operator \'" + 
                            tokens[pos+1].Name + "\'."
                        );
                    }
                    string op = tokens[pos+1].Name;
                    pos += 2;
                    TableCatalog::Column col = cat.GetColumnInformation(column_name);
                    if (col.Type == colType::Int && tokens[pos].Type != Int || 
                        col.Type == colType::Float && 
                            tokens[pos].Type != Int && tokens[pos].Type != Float ||
                        col.Type == colType::Char && tokens[pos].Type != String) 
                    {
                        throw Exception(
                            "Interpreter Error:" \
                            "Value type mismatch for \'select\'."
                        );
                    }
                    if (col.Type == colType::Int) {
                        condition.Add(column_name, op, stoi(tokens[pos].Name));
                    }
                    else if (col.Type == colType::Float) {
                        condition.Add(
                            column_name, op, 
                            (float) atof(tokens[pos].Name.c_str())
                        );
                    }
                    else {
                        condition.Add(
                            column_name, op, 
                            tokens[pos].Name.substr(0, col.Size)
                        );
                    }
                    pos++;
                    if (pos >= tokens.size())
                        break;
                    if (tokens[pos].Name != "and") {
                        throw Exception(
                            "Interpreter Error:" \
                            "Illegal syntax for \'" + tokens[0].Name + "\'."
                        );
                    }
                    pos++;
                }
                if (tokens[0].Name == "select")
                    Api->Select(condition);
                else
                    Api->Delete(condition);
            }
            catch (Exception& ex) {
                throw;
            }
            catch (...) {
                throw Exception(
                    "Interpreter Error:" \
                    "Illegal syntax for \'" + tokens[0].Name + "\'."
                );
            }
        }
        else if (tokens[0].Name == "execfile") {
            Run(tokens[1].Name);
        }
        else if (tokens[0].Name == "quit") {
            //cout << "Program quitted." << endl;
            throw Exception("Program quitted.");
        }
        else {
            throw Exception(
                "Interpreter Error: Illegal query.");
        }
    }
    catch (Exception& ex) {
        throw;
    }
    catch(...) {
        throw Exception("Interpreter Error: Illegal query.");
    }
}

string Interpreter::ReadFile(string SQLpath) {
    string sql_file;
    try {
        
        /*
        ifstream f;
        f.open(SQLpath);
        f.seekg(0, ios::end);
        int length = f.tellg();
        f.seekg(0, ios::beg);
        char *buffer = new char[length];
        f.read(buffer, length);
        f.close();
        
        FILE *f = fopen(SQLpath.c_str(), "r");
        fseek(f, 0, SEEK_END);
        long length = ftell(f);
        char *buffer = new char[length+1];
        rewind(f);
        fread(buffer, sizeof(char), length, f);
        fclose(f);
        buffer[length] = '\0';
        cout << buffer << endl;
        sql_file = buffer;
        cout << sql_file << endl;
        */
       
        ifstream fin(SQLpath);
        if (!fin.is_open())
            throw Exception("Read error: Cannot open file \'" + SQLpath + "\'.");
        stringstream ss;
        ss << fin.rdbuf();
        sql_file = ss.str();
        //cout << sql_file << endl;
    }
    catch (Exception& ex) {
        throw;
    }
    catch(...) {
        throw Exception("Read error: Cannot open file \'" + SQLpath + "\'.");
    }
    return sql_file;
}

vector<Interpreter::Token> Interpreter::Tokenize(string query) {
    vector<Interpreter::Token> tokens;

    auto is_digit = [=](char ch)->bool { 
        return ch >= '0' && ch <= '9' || ch == '.'; 
    };
    auto is_name = [=](char ch)->bool { 
        return ch >= 'a' && ch <= 'z' || is_digit(ch) || ch == '_'; 
    };
    auto is_white = [=](char ch)->bool { 
        return ch == ' ' || ch <= '\n' ||
               ch == '\t' || ch <= '\r'; 
    };
    auto is_valid_digit = [=](string num)->bool {
        int point_num = 0;
        for (char ch: num) {
            if (!is_digit(ch))
                return false;
            point_num += ch == '.';
        }
        return point_num <= 1;
    };
    auto is_valid_name = [=](string name)->bool {
        return name.find('.') == string::npos && 
               (name[0] < '0' || name[0] > '9');
    };
    auto is_keyword = [=](string name)->bool {
        return KEYWORD_LIST.find(name) != KEYWORD_LIST.end();
    };
    auto is_operator = [=](string name)->bool {
        return OPERATOR_LIST.find(name) != OPERATOR_LIST.end();
    };

    string tmp = "";
    string query_with_end = query + ' ';
    bool adding_name = false;
    bool adding_string = false;

    for (int i=0; i<query_with_end.length(); i++) {
        char ch = query_with_end[i];
        if (ch == '\'') {
            if (adding_name) {
                throw Exception(
                    "Interpreter Error: Invalid word \'" + tmp + "\'.");
            }
            adding_string = !adding_string;
            if (!adding_string)  {
                tokens.push_back(Token(tmp, String));
                tmp = "";
            }
            continue;
        }
        if (adding_string) {
            tmp += ch;
            continue;
        }
        if (ch >= 'A' && ch <= 'Z')
            ch += 'a' - 'A';
        if (is_name(ch)) {
            adding_name = true;
            tmp += ch;
            continue;
        }
        if (adding_name) {
            if (is_valid_digit(tmp)) {
                if (tmp.find('.') == string::npos)
                    tokens.push_back(Token(tmp, Int));
                else
                    tokens.push_back(Token(tmp, Float));
            }
            else if (is_valid_name(tmp)) {
                if (is_keyword(tmp)) {
                    tokens.push_back(Token(tmp, Keyword));
                    if (tmp == "execfile") {
                        tokens.push_back(Token(
                            query.substr(i+1, query.length()-i-1), String
                        ));
                        break;
                    }
                }
                else
                    tokens.push_back(Token(tmp, Name));
            }
            else {
                throw Exception(
                    "Interpreter Error: Invalid word \'" + tmp + "\'.");
            }
            adding_name = false;
            tmp = "";
        }
        string op = query_with_end.substr(i, 2);
        if (i < query_with_end.length()-1 && is_operator(op)) {
            tokens.push_back(Token(op, Operator));
            i += 1;
            continue;
        }
        op = ch;
        if (is_operator(op)) {
            tokens.push_back(Token(op, Operator));
            continue;
        }
        if (!is_white(ch)) throw Exception(
            "Interpreter Error: Invalid operator \'" + op + "\'.");
    }
    return tokens;
}

string Interpreter::Beautify(string raw) const {
    string beautified = "";
    char prev_ch = '\0';
    for (char ch: raw) {
        if (ch >= ' ' && ch <= 126 || 
            ch == '\n' || ch == '\r' || ch == '\t')
        {
            char tmpch;
            switch (ch) {
                case '\"':  tmpch = '\'';   break;
                case '\n':
                case '\r':
                case '\t':
                    tmpch = ' ';
                break;
                default:    tmpch = ch;     break;
            }
            //if (ch >= 'A' && ch <= 'Z')
            //    tmpch = ch + 'a' - 'z';
            if (prev_ch != ' ' || tmpch != ' ')
                beautified += tmpch;
            prev_ch = tmpch;
        }
    }
    return beautified;
}

vector<string> Interpreter::Split(string str, char ch) const {
    string tmpstr = str;
    vector<string> sqls;
    
    int pos;
    while ((pos = tmpstr.find(ch)) != tmpstr.npos) {
        //cout << tmpstr << endl;
        sqls.push_back(tmpstr.substr(0, pos));
        tmpstr.erase(0, pos+1);
    }

    if (tmpstr.length() > 1)
        sqls.push_back(tmpstr);
    return sqls;
}

bool Interpreter::IsRunning() const {
    return !Quitted;
}
