#ifndef SHARED_DATA_STRUCTURES_H
#define SHARED_DATA_STRUCTURES_H
#include <string>
#include <map>
#include <vector>
using namespace std;

class TableCatalog {
public:
    enum ColumnType {       
        Int = 0,
        Float = 1,
        Char = 2
    };
    class Column {
    public:
        string Name;
        int Type;
        int Size;           //用于char类型
        bool IsUnique;
        bool IsPrimary;
        Column() {}
        Column(
            string name, ColumnType type, int size=0, 
            bool is_unique=false, bool is_primary=false
        );
    };
    
    string TableName;
    vector<Column> Columns;
    map<string, int> ColumnID;
    int RecordNumber;
    TableCatalog() {}
    TableCatalog(string table_name);
    void AddColumn(Column col);
    bool ExistColumn(string column_name);
    void SetPrimaryKey(string column_name);
    Column GetColumnInformation(string column_name);
    void _Print();
    
};

class Record {
public:
    enum ValueType {
        Int,
        Float,
        Char
    };
    
    class Value {
    private:
        int ValInt;
        float ValFloat;
        string ValChar;
        ValueType Type;
    public:
        Value() {}
        Value(int val);
        Value(float val);
        Value(string val);
        int GetInt();
        float GetFloat();
        string GetChar();
        ValueType GetType();
    };

    vector<Value> Content;
    int ID;
    Record() {}
    void _Print();
    template <typename T>
    void Add(T val);
};

template <typename T>
void Record::Add(T val) {
    Content.push_back(Value(val));
}

class Condition {
public:
    enum ValueType {
        Int,
        Float,
        Char
    };
    
    class Value {
    private:
        int ValInt;
        float ValFloat;
        string ValChar;
        ValueType Type;
        string ColumnName;
        string Operator;
    public:
        Value(string column_name, string op, int val);
        Value(string column_name, string op, float val);
        Value(string column_name, string op, string val);
        int GetInt();
        float GetFloat();
        string GetChar();
        ValueType GetType();
        string GetColumnName();
        string GetOperator();
    };

    TableCatalog Table;
    vector<Value> Content;

    Condition(TableCatalog table);
    void _Print();
    template <typename T>
    void Add(string column_name, string op, T val);
};

template <typename T>
void Condition::Add(string column_name, string op, T val) {
    Content.push_back(Value(column_name, op, val));
}

#endif

