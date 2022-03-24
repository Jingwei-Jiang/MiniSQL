#include <iostream>
#include <vector>
#include <set>
#include "BPlusTree.h"
//#include "BPlusTreeNode.h"

using namespace std;

int main(){
    vector<int> s;
    
    string name("abc");
    string name2("abcindex");
    string name3("abcc");
    int size = 4;
    int order = 4;
    BPlusTree<int>* newtree =
            new BPlusTree<int>( name, name2,name3, size, order);
    for( int i = 0; i < 100; i++ ){
        newtree->Insert( 50-i, i );
    }
    for( int i = 200; i < 300; i++ )
        newtree->Insert( i, i-100 );
    newtree->Print();
    newtree->Remove(200);
    cout << endl;
    newtree->Print();
    int a = 0;
    char op = '<';
    set<int>k = newtree->SearchRange(a, 1);
    //cout << "ok" << endl;
    cout << k.size();
    for( auto i : k )
        cout << i << " ";
}