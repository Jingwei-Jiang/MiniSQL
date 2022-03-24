#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include "API.h"
#include "SharedDataStructures.h"
#include "CatalogManager.h"
#include "Exception.h"
using namespace std;
using colType = TableCatalog::ColumnType;

CatalogManager::CatalogManager() {
    LoadIndex();
}

bool CatalogManager::ExistTable(string table_name) {
    ifstream fin("Catalog\\" + table_name + ".tab");
    return fin.good();
}

bool CatalogManager::ExistTableColumn(string table_name, string column_name) {
    if (!ExistTable(table_name)) {
        return false;
    }
    TableCatalog table = LoadTable(table_name);
    if (!table.ExistColumn(column_name)) {
        return false;
    }
    return true;
}

void CatalogManager::SaveTable(TableCatalog &table) {
    if (ExistTable(table.TableName)) {
        DropTable(table.TableName);
    }
    ofstream fout;
    fout.open("Catalog\\" + table.TableName + ".tab");
    if (!fout.is_open()) {
        throw Exception("Catalog Error: \'Catalog\' folder not found.");
    }
    fout << table.Columns.size() << " " << table.RecordNumber << endl;
    for (auto &col: table.Columns) {
        fout << col.Name << " " << col.Type << " " << col.Size << " "
             << col.IsUnique << " " << col.IsPrimary << endl;
    }
    fout.close();
}

TableCatalog CatalogManager::LoadTable(string table_name) {
    if (!ExistTable(table_name)) {
        throw Exception("Catalog error: Table \'" + table_name + "\' does not exist.");
    }
    TableCatalog table(table_name);
    
    ifstream fin("Catalog\\" + table_name + ".tab");
    int col_num, record_num;
    fin >> col_num >> record_num;
    table.RecordNumber = record_num;
    
    string name;
    int type;
    int size;
    bool is_unique, is_primary;
    for (int i=0; i<col_num; i++) {
        fin >> name >> type >> size >> is_unique >> is_primary;
        TableCatalog::Column col(
            name, (TableCatalog::ColumnType) type, 
            size, is_unique, is_primary
        );
        table.AddColumn(col);
    }
    return table;
}

void CatalogManager::DropTable(string table_name) {
    string path = "Catalog\\" + table_name + ".tab";
    
    vector<string> index_to_be_dropped;
    for (auto &it: Index) {
        if (it.second.first == table_name) {
            index_to_be_dropped.push_back(it.first);
        }
    }
    for (string index_name: index_to_be_dropped) {
        DropIndex(index_name);
    }
    remove(path.c_str());
}

void CatalogManager::CreateIndex(
    string index_name, string table_name, string column_name) {
    if (!ExistTableColumn(table_name, column_name)) {
        throw Exception(
            "Catalog error: Column \'" + column_name + "\' in table \'" + 
            table_name + "\' does not exist.");
    }
    Index[index_name] = make_pair(table_name, column_name);
    Column2Index[make_pair(table_name, column_name)] = index_name;
    SaveIndex();
}

bool CatalogManager::ExistIndex(string index_name) {
    return Index.count(index_name) > 0;
}

void CatalogManager::DropIndex(string index_name) {
    if (!ExistIndex(index_name))
        return;
    auto pr = Index[index_name];
    Index.erase(index_name);
    Column2Index.erase(pr);
    SaveIndex();
}

string CatalogManager::GetIndex(string table_name, string column_name) {
    auto key = make_pair(table_name, column_name);
    if (Column2Index.count(key) > 0)
        return Column2Index[key];
    else
        return "";
}

map<string, pair<string, string> >& CatalogManager::GetIndexMap() {
    return Index;
}

void CatalogManager::SaveIndex() {
    ofstream fout;
    fout.open("Catalog\\indices.ind");
    if (!fout.is_open()) {
        throw Exception("Catalog Error: \'Catalog\' folder not found.");
    }
    fout << Index.size() << endl;
    for (auto &it: Index) {
        fout << it.first << " " << it.second.first << " " << it.second.second << endl;
    }
    fout.close();
}

void CatalogManager::LoadIndex() {
    ifstream fin("Catalog\\indices.ind");
    if (!fin.good()) {
        SaveIndex();
        return;
    }
    Index.clear();
    int num;
    fin >> num;
    for (int i=0; i<num; i++) {
        string index_name, table_name, column_name;
        fin >> index_name >> table_name >> column_name;
        Index[index_name] = make_pair(table_name, column_name);
        Column2Index[make_pair(table_name, column_name)] = index_name;
    }
    fin.close();
}

