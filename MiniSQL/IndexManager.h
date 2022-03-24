#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <set>

#include "BPlusTree.h"
#include "Basic.h"
#include "SharedDataStructures.h"
#define INTSIZE  4
#define FLOATSIZE 4
#define CHARSIZE 4

class IndexManager{
private:
    map<int, BPlusTree<int>*> IntIndexMap;
    map<int, BPlusTree<float>*> FloatIndexMap;
    map<int, BPlusTree<string>*> StringIndexMap;
	map<string, int> MapIndex;
    static int index_num ;
public:
    IndexManager();
    ~IndexManager();
    //template< class T >
    void CreateIndex(
        TableCatalog Table, vector<pair<Record::Value, int> >&Elements, 
        string column_name, string index_name 
    );
    void DropIndex( string index_name );
	void ShowIndex( string index_name );
    set<int> Search( Condition::Value k, string index_name ) ;
    //template<class T>
    void Insert( string index_name, int record_id , int key );
    void Insert( string index_name, int record_id , float key );
    void Insert( string index_name, int record_id , string key );
    template<class T>
	void Delete( string index_name, int record_id , T key );
    int OpType( string op );
};
/*
template<class T>
void IndexManager::Insert(string index_name, int record_id , T key ) {
    int index_num = MapIndex[index_name];
    if( IntIndexMap.count(index_num) == 1 ){
        BPlusTree<int>* newtree = IntIndexMap[index_num];
        newtree->Insert<T>( key, record_id );
    }else if( FloatIndexMap.count(index_num) == 1 ){
        BPlusTree<float>* newtree = FloatIndexMap[index_num];
        newtree->Insert<T>( key, record_id );
    }else{
        BPlusTree<string>* newtree = StringIndexMap[index_num];
        newtree->Insert<T>( key, record_id );
    }
}*/

template<class T>
void IndexManager::Delete( string index_name, int record_id, T key ) {

    int index_num = MapIndex[index_name];
    if( IntIndexMap.count(index_num) == 1 ){
        BPlusTree<int>* newtree = IntIndexMap[index_num];
        newtree->Remove( key, record_id );
    }else if( FloatIndexMap.count(index_num) == 1 ){
        BPlusTree<float>* newtree = FloatIndexMap[index_num];
        newtree->Remove( key, record_id );
    }else{
        BPlusTree<string>* newtree = StringIndexMap[index_num];
        newtree->Remove( key, record_id );
    }
        
    
    

}

#endif