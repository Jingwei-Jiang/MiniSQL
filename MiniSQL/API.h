#ifndef API_H
#define API_H
#include <set>
#include <vector>
#include <string>
#include "SharedDataStructures.h"
#include "CatalogManager.h"
#include "Exception.h"
#include "IndexManager.h"
#include "RecordMan.h"
using namespace std;

class API {
private:
    CatalogManager* Catalog;
    IndexManager* Indexer;
    RecordManager* Recorder;
public:
    API(CatalogManager* catalog);
    void CreateTable(TableCatalog &table);
    void DropTable(string table_name);
    void CreateIndex(string index_name, string table_name, string column_name);
    void DropIndex(string index_name);
    void InsertValue(string table_name, Record &record);
    void Select(Condition condition);
    void SelectAll(string table_name);
    void Delete(Condition condition);
    void ClearTable(string table_name);
    void PrintTable(TableCatalog &catalog, vector<Record> &records);
    TableCatalog GetTableCatalog(string table_name);
};

#endif