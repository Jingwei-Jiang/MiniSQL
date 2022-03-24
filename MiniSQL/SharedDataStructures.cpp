#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include "SharedDataStructures.h"
#include "Exception.h"
using namespace std;

TableCatalog::Column::Column(
    string name, ColumnType type,
    int size, bool is_unique, bool is_primary
):  Name(name), Type(type), Size(size), 
    IsUnique(is_unique), IsPrimary(is_primary) {}

TableCatalog::TableCatalog(string table_name): 
    TableName(table_name), RecordNumber(0) {}

void TableCatalog::AddColumn(Column col) {
    ColumnID[col.Name] = Columns.size();
    Columns.push_back(col);
}

bool TableCatalog::ExistColumn(string column_name) {
    return ColumnID.count(column_name) > 0;
}

void TableCatalog::SetPrimaryKey(string column_name) {
    if (!ExistColumn(column_name)) {
        throw Exception(
            "Catalog error: Column \'" + column_name + "\' does not exist.");
    }
    Columns[ColumnID[column_name]].IsPrimary = true;
}

TableCatalog::Column TableCatalog::GetColumnInformation(string column_name) {
    if (!ExistColumn(column_name)) {
        throw Exception(
            "Catalog error: Column \'" + column_name + "\' does not exist.");
    }
    return Columns[ColumnID[column_name]];
}

void TableCatalog::_Print() {
    cout << TableName << ".tab:" << endl;
    for (auto &column: Columns) {
        //Column column = it.second;
        cout << "\t" << column.Name << "\tType" << column.Type
             << "(" << column.Size << ")\t";
        if (column.IsUnique)
            cout << "unique\t";
        if (column.IsPrimary)
            cout << "primary";
        cout << endl;
    }
}

Record::Value::Value(int val):      ValInt(val),    Type(Int)   {}
Record::Value::Value(float val):    ValFloat(val),  Type(Float) {}
Record::Value::Value(string val):   ValChar(val),   Type(Char)  {}

int Record::Value::GetInt() {
    return ValInt;
}

float Record::Value::GetFloat() {
    return ValFloat;
}

string Record::Value::GetChar() {
    return ValChar;
}

Record::ValueType Record::Value::GetType() {
    return Type;
}

void Record::_Print() {
    for (Value& val: Content) {
        switch (val.GetType()) {
            case Int:
                cout << "|\t" << val.GetInt();
            break;
            case Float:
                cout << "|\t" << val.GetFloat();
            break;
            case Char:
                cout << "|\t" << val.GetChar();
            break;
        }
    }
    cout << endl;
}

Condition::Condition(TableCatalog table): Table(table) {}

Condition::Value::Value(string column_name, string op, int val):
    ColumnName(column_name), Operator(op), ValInt(val), Type(Int) {}

Condition::Value::Value(string column_name, string op, float val):
    ColumnName(column_name), Operator(op), ValFloat(val), Type(Float) {}

Condition::Value::Value(string column_name, string op, string val):
    ColumnName(column_name), Operator(op), ValChar(val), Type(Char) {}

int Condition::Value::GetInt() {
    return ValInt;
}

float Condition::Value::GetFloat() {
    return ValFloat;
}

string Condition::Value::GetChar() {
    return ValChar;
}

Condition::ValueType Condition::Value::GetType() {
    return Type;
}

string Condition::Value::GetColumnName() {
    return ColumnName;
}

string Condition::Value::GetOperator() {
    return Operator;
}

void Condition::_Print() {
    for (Value& cond: Content) {
        cout << "|\t" << cond.GetColumnName() << cond.GetOperator();
        switch (cond.GetType()) {
            case Int:
                cout << cond.GetInt();
            break;
            case Float:
                cout << cond.GetFloat();
            break;
            case Char:
                cout << cond.GetChar();
            break;
        }
    }
    cout << endl;
}
