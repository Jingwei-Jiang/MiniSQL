#ifndef RECORDMAN_H
#define RECORDMAN_H
#pragma once
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include "SharedDataStructures.h"
#include "BlockMan.h"
#include "Block.h"
using namespace std;
class RecordManager{
public:
    RecordManager();
    int GetRecordSize(TableCatalog table);
    bool CreateTable(TableCatalog table);
    void DropTable(string filename);
    bool InsertRecord(TableCatalog table,Record R);
    int DeleteRecord(TableCatalog table,set<int> IDSet);
    bool DeleteAll(TableCatalog table);
    set<int> SelectRecord(TableCatalog table,Condition::Value cond);
    set<int> SelectAll(TableCatalog table);
    vector<Record> GetRecord(TableCatalog table,set<int> IDSet);
    vector<pair<Record::Value,int> > GetColumn(TableCatalog table,string columnname);
};

template<typename T> bool Comparison(T& data1, T& data2, string condition_op){
    if (condition_op=="="){
        if (data1==data2){
            return true;
        }else{
            return false;
        }
    }
    if (condition_op=="!="){
        if (data1!=data2){
            return true;
        }else{
            return false;
        }
    }
    if (condition_op=="<"){
        if (data1<data2){
            return true;
        }else{
            return false;
        }
    }
    if (condition_op==">"){
        if (data1>data2){
            return true;
        }else{
            return false;
        }   
    }
    if (condition_op=="<="){
        if (data1<=data2){
            return true;
        }else{
            return false;
        }
    }
    if (condition_op==">="){
        if (data1>=data2){
            return true;
        }else{
            return false;
        }
    }
    return false;
}

#endif

