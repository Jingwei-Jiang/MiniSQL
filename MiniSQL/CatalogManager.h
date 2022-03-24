#ifndef CATALOG_MANAGER_H
#define CATALOG_MANAGER_H
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include "SharedDataStructures.h"
using namespace std;

class CatalogManager {
private:
    map<string, pair<string, string> > Index;
    map<pair<string, string>, string > Column2Index;
public:
    CatalogManager();
    bool ExistTable(string table_name);
    bool ExistTableColumn(string table_name, string column_name);
    void SaveTable(TableCatalog &table);
    TableCatalog LoadTable(string table_name);
    void DropTable(string table_name);
    void CreateIndex(string index_name, string table_name, string column_name);
    bool ExistIndex(string index_name);
    void DropIndex(string index_name);
    string GetIndex(string table_name, string column_name);
    map<string, pair<string, string> >& GetIndexMap();
    void SaveIndex();
    void LoadIndex();
};

#endif