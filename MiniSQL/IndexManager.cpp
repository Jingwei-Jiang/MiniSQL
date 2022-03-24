#include "IndexManager.h"
using namespace std;

int IndexManager::index_num = 0;

IndexManager::IndexManager() {}
IndexManager::~IndexManager() {}

//template< class T >
void IndexManager::CreateIndex( 
    TableCatalog Table, vector<pair<Record::Value, int> >&Elements, 
    string column_name, string index_name
) 
{
    TableCatalog::Column cur;
    //vector<Record> Elements = GetColumn( Table, column_name ); 
    int size = 0;
    for( auto i: Table.Columns ){
        if( i.Type == 2 ) size += i.Size;
        else size += 4;
        if( i.Name == column_name )
            cur = i;
    }
    if( size > 0 ) size = 4096 / (size+1);
    int t = cur.Type;
    switch (t) {
    case TableCatalog::ColumnType::Int: {
        BPlusTree<int>* newtree =
            new BPlusTree<int>( Table.TableName, column_name, index_name, size, size);
        for( int i = 0; i < Elements.size(); i++ )
            newtree->Insert( Elements[i].first.GetInt(), Elements[i].second );
        MapIndex[index_name] = index_num;
        IntIndexMap[index_num++] = newtree;
        break;
    }
    case TableCatalog::ColumnType::Float: {
        BPlusTree<float>* newtree =
            new BPlusTree<float>( Table.TableName, column_name, index_name, size, size);
        for( int i = 0; i < Elements.size(); i++ )
            newtree->Insert( Elements[i].first.GetFloat(), Elements[i].second );
        MapIndex[index_name] = index_num;
        FloatIndexMap[index_num++] = newtree;
        break;
    }
    case TableCatalog::ColumnType::Char: {
        BPlusTree<string>* newtree =
            new BPlusTree<string>( Table.TableName, column_name, index_name, size, size);
        for( int i = 0; i < Elements.size(); i++ )
            newtree->Insert( Elements[i].first.GetChar(), Elements[i].second );
        MapIndex[index_name] = index_num;
        StringIndexMap[index_num++] = newtree;
        break;
    }
    }
}

void IndexManager::DropIndex( string index_name ) {
    int index_num = MapIndex[index_name];
    if( IntIndexMap.count(index_num) == 1 ){
        auto it = IntIndexMap.find(index_num);
        if (it != IntIndexMap.end()) {
            delete it->second;
            IntIndexMap.erase(it);
            auto id_it =MapIndex.find(index_name);
            MapIndex.erase(id_it);   
        }
        remove(index_name.c_str());
    }else if( FloatIndexMap.count(index_num) == 1 ){
        auto it = FloatIndexMap.find(index_num);
        if (it != FloatIndexMap.end()) {
            delete it->second;
            FloatIndexMap.erase(it);
            auto id_it =MapIndex.find(index_name);
            MapIndex.erase(id_it);   
        }
        remove(index_name.c_str());
    }else{   
        auto it = StringIndexMap.find(index_num);
        if (it != StringIndexMap.end()) {
            delete it->second;
            StringIndexMap.erase(it);
            auto id_it =MapIndex.find(index_name);
            MapIndex.erase(id_it);   
        }
        remove(index_name.c_str());
    }
}

int IndexManager::OpType( string op ){
    if( op == ">") return 0;
    if( op == "<") return 1;
    if( op == "=") return 2;
    if( op == "<=") return 3;
    if( op == ">=") return 4;
    if( op == "!=") return 5;
    return 0;
}

set<int> IndexManager::Search( Condition::Value k , string index_name ) {
    set<int> res; res.clear();
    Condition::ValueType t = k.GetType();
    int op = OpType(k.GetOperator());
    switch (t){
    case Condition::ValueType::Int:{
        int val = k.GetInt();
        int index_num = MapIndex[ index_name ];
        BPlusTree<int>* newtree = IntIndexMap[ index_num ];
        res = newtree->SearchRange(val, op);
    }
    case Condition::ValueType::Float:{
        float val = k.GetFloat();
        int index_num = MapIndex[ index_name ];
        BPlusTree<float>* newtree = FloatIndexMap[ index_num ];
        res = newtree->SearchRange(val, op);
    }
    case Condition::ValueType::Char:{
        string val = k.GetChar();
        int index_num = MapIndex[ index_name ];
        BPlusTree<string>* newtree = StringIndexMap[ index_num ];
        res = newtree->SearchRange(val, op);
    }
    }
    return res;
}

void IndexManager::ShowIndex( string index_name ){

    int index_num = MapIndex[index_name];
    if( IntIndexMap.count(index_num) == 1 ){
        auto it = IntIndexMap.find(index_num);
        it->second->Print();
    }else if( FloatIndexMap.count(index_num) == 1 ){
        auto it = FloatIndexMap.find(index_num);
        it->second->Print();
    }else{   
        auto it = StringIndexMap.find(index_num);
        it->second->Print();
    }
    
}

void IndexManager::Insert( string index_name, int record_id , int key ) {
    int index_num = MapIndex[index_name];
    BPlusTree<int>* newtree = IntIndexMap[index_num];
    newtree->Insert( key, record_id );
}

void IndexManager::Insert( string index_name, int record_id , float key ) {
    int index_num = MapIndex[index_name];
    BPlusTree<float>* newtree = FloatIndexMap[index_num];
    newtree->Insert( key, record_id );
}

void IndexManager::Insert( string index_name, int record_id , string key ) {
    int index_num = MapIndex[index_name];
    BPlusTree<string>* newtree = StringIndexMap[index_num];
    newtree->Insert( key, record_id );
}

// void IndexManager::SaveToFile() {
//     for (auto i : IntIndexMap)
//         if (i.second) {
//             i.second->write_to_file();
//             delete i.second;
//         }
//     for (auto i : FloatIndexMap)
//         if (i.second) {
//             i.second->write_to_file();
//             delete i.second;
//         }
//     for (auto i : StringIndexMap)
//         if (i.second) {
//             i.second->write_to_file();
//             delete i.second;
//         }
// }