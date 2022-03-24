#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <string>
#include "API.h"
#include "Interpreter.h"
#include "CatalogManager.h"
#include "SharedDataStructures.h"
#include "Exception.h"
#include "IndexManager.h"
#include "RecordMan.h"
using namespace std;
using colType = TableCatalog::ColumnType;
using recType = Record::ValueType;

API::API(CatalogManager* catalog): Catalog(catalog) {
    try {
        Indexer = new IndexManager();
        Recorder = new RecordManager();

        auto &index_map = catalog->GetIndexMap();
        for (auto &it: index_map) {
            string index_name = it.first;
            string table_name = it.second.first;
            string column_name = it.second.second;
            TableCatalog table = GetTableCatalog(table_name);
            auto elements = Recorder->GetColumn(table, column_name);
            Indexer->CreateIndex(table, elements, column_name, index_name);
        }
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when initializing.");
    }
}

void API::CreateTable(TableCatalog &table) {
    try {
        Recorder->CreateTable(table);
        Catalog->SaveTable(table);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when creating table.");
    }
    cout << "Created table \'" << table.TableName << "\':" << endl;
    table._Print();
}

void API::DropTable(string table_name) {
    if (!Catalog->ExistTable(table_name))
        return;
    try {
        Recorder->DropTable(table_name);
        Catalog->DropTable(table_name);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when dropping table.");
    }
    cout << "Dropped table \'" << table_name << "\'." << endl;
}

void API::CreateIndex(string index_name, string table_name, string column_name) {
    try {
        TableCatalog table = GetTableCatalog(table_name);
        auto elements = Recorder->GetColumn(table, column_name);
        Indexer->CreateIndex(table, elements, column_name, index_name);
        Catalog->CreateIndex(index_name, table_name, column_name);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when creating index.");
    }
    cout << "Created index \'" << index_name << "\' on table \'" 
         << table_name << "\', column \'" << column_name << "\'." << endl;
}

void API::DropIndex(string index_name) {
    try {
        Indexer->DropIndex(index_name);
        Catalog->DropIndex(index_name);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when dropping index.");
    }
    cout << "Dropped index \'" << index_name << "\'." << endl;
}

void API::InsertValue(string table_name, Record &record) {
    try {
        TableCatalog table = GetTableCatalog(table_name);
        record.ID = table.RecordNumber;
        /*
        for (int i=0; i<table.Columns.size(); i++) {
            auto &col = table.Columns[i];
            if (col.IsPrimary || col.IsUnique) {
                Condition cond(table);
                Record::Value &val = record.Content[i];
                switch (col.Type) {
                    case colType::Int:
                        cond.Add(col.Name, "=", val.GetInt());
                    break;
                    case colType::Float:
                        cond.Add(col.Name, "=", val.GetFloat());
                    break;
                    case colType::Char:
                        cond.Add(col.Name, "=", val.GetChar());
                    break;
                }
                set<int> selected = 
                    Recorder->SelectRecord(table, cond.Content[0]);
                if (!selected.empty()) {
                    throw Exception(
                        "Record Manager Error: " \
                        "Values of column \'" + col.Name + "\' " \
                        "must be unique."
                    );
                }
            }
        }
        */
        bool res = Recorder->InsertRecord(table, record);
        if (!res)
            throw Exception("API Error: Error occured when inserting value.");
        for (int i=0; i<table.Columns.size(); i++) {
            auto &col = table.Columns[i];
            string index_name = Catalog->GetIndex(table_name, col.Name);
            if (index_name != "") {
                Record::Value &val = record.Content[i];
                switch (col.Type) {
                    case colType::Int:
                        Indexer->Insert(index_name, record.ID, val.GetInt());
                    break;
                    case colType::Float:
                        Indexer->Insert(index_name, record.ID, val.GetFloat());
                    break;
                    case colType::Char:
                        Indexer->Insert(index_name, record.ID, val.GetChar());
                    break;
                }
            }
        }
        table.RecordNumber++;
        Catalog->SaveTable(table);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when inserting value.");
    }
    cout << "Inserted value into table \'" << table_name << "\':" << endl;
    record._Print();
}

void API::Select(Condition condition) {
    try {
        set<int> records;
        string table_name = condition.Table.TableName;
        for (auto &cond: condition.Content) {
            string column_name = cond.GetColumnName();
            string index_name = Catalog->GetIndex(table_name, column_name);
            set<int> selected_records = index_name == "" ?
                Recorder->SelectRecord(condition.Table, cond) :
                Indexer->Search(cond, index_name);
            if (records.empty())
                records = selected_records;
            else {
                set<int> tmp;
                set_intersection(
                    records.begin(), records.end(),
                    selected_records.begin(), selected_records.end(),
                    inserter(tmp, tmp.begin())
                );
                records = tmp;
            }
        }
        vector<Record> select_res = Recorder->GetRecord(condition.Table, records);
        cout << "Selected result (" 
             << select_res.size() << " record(s)):" << endl;
        PrintTable(condition.Table, select_res);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when selecting value.");
    }
    //cout << "Selected from table \'" << condition.Table.TableName << "\'where" << endl;
    //condition._Print();
}

void API::SelectAll(string table_name) {
    try {
        TableCatalog table = GetTableCatalog(table_name);
        set<int> records = Recorder->SelectAll(table);
        vector<Record> select_res = Recorder->GetRecord(table, records);
        cout << "Selected result (" 
             << records.size() << " record(s)):" << endl;
        PrintTable(table, select_res);
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when dropping index.");
    }
    //cout << "Selected table \'" << table_name << "\'." << endl;
}

void API::Delete(Condition condition) {
    try {
        set<int> records;
        string table_name = condition.Table.TableName;
        for (auto &cond: condition.Content) {
            string column_name = cond.GetColumnName();
            string index_name = Catalog->GetIndex(table_name, column_name);
            set<int> selected_records = index_name == "" ?
                Recorder->SelectRecord(condition.Table, cond) :
                Indexer->Search(cond, index_name);
            if (records.empty())
                records = selected_records;
            else {
                set<int> tmp;
                set_intersection(
                    records.begin(), records.end(),
                    selected_records.begin(), selected_records.end(),
                    inserter(tmp, tmp.begin())
                );
                records = tmp;
            }
        }
        if (records.empty())
            return;
        int deleted_num = Recorder->DeleteRecord(condition.Table, records);
        //if (!res)
        //    throw Exception("Record Error: Error occured when deleting value.");
        cout << "Deleted " 
             << deleted_num << " record(s) from table \'"
             << condition.Table.TableName << "\'." << endl;
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when deleting value.");
    }
    //cout << "Deleted from table \'" << condition.Table.TableName << "\'." << endl;
    //condition._Print();
}

void API::ClearTable(string table_name) {
    try {
        TableCatalog table = GetTableCatalog(table_name);
        bool res = Recorder->DeleteAll(table);
        if (!res)
            throw Exception("Record Error: Error occured when deleting value.");
        cout << "Cleared table \'"
             << table_name << "\'." << endl;
    }
    catch (Exception& ex) {
        throw;
    }
    catch (...) {
        throw Exception("API Error: Error occured when clearing table.");
    }
    //cout << "Cleared table \'" << table_name << "\'." << endl;
}

void API::PrintTable(TableCatalog &catalog, vector<Record> &records) {
    if (!records.empty() && catalog.Columns.size() != records[0].Content.size()) {
        throw Exception("API Error: Columns mismatch while printing table.");
    }
    for (auto &col: catalog.Columns) {
        cout << "|\t" << col.Name;
    }
    cout <<  "|" << endl;
    for (auto &rec: records) {
        for (auto &val: rec.Content) {
            cout << "|\t";
            switch (val.GetType()) {
                case recType::Int:      cout << val.GetInt();       break;
                case recType::Float:    cout << val.GetFloat();     break;
                case recType::Char:     cout << val.GetChar();      break;
            }
        }
        cout << "|" << endl;
    }
}

TableCatalog API::GetTableCatalog(string table_name) {
    try {
        return Catalog->LoadTable(table_name);
    }
    catch (Exception& ex) {
        throw;
    }
}