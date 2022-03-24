#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include "BPlusTreeNode.h"
#include "Basic.h"

#define pni pair<Node<T>*, int>
#define FAILED  0
#define DELETE  1
#define CHANGE  2
#define REPLACE  3
#define NONE  4

template <class T>
class BPlusTree {
typedef Node<T>* node;
	
private:
    node head, tail;
    int degree, order;
    int key_cnt, node_cnt;
    string idx_file;
	T tempkey;
	
public:
   	node root;
    BPlusTree( string TableName, string colum_name, string _idx_file_ = "",  int _degree = 4, int _order = 4 );
    ~BPlusTree();
    void Insert(T val, int record_id);
    pni Find(T val, node now);
	int Find(T val);
    int Delete(node now, node brother, T Parentkey);
	int Remove(T val);
    void SplitUpdate(node now);
    void Print();
	void Update(node now, T oldval, T newval);
	// void Write_to_file();
	// void Write_id(ofstream &fp, node now);
	// void Write(ofstream &fp, node now);
	// bool Read(ofstream &fp, node &now);
	// bool LoadFromFile();
	void Drop(node now);
	set<int> SearchRange(T val, int op);
};

template <typename T>
BPlusTree<T>::BPlusTree( string TableName, string colum_name, string _idx_file, int _degree, int _order )
    : idx_file(_idx_file), degree(_degree), order(_order) {
	key_cnt = 0;
	node_cnt = 1;
	head = new Node<T>( order, degree, 0, 0,  NULL, NULL, NULL, true);
    tail = new Node<T>( order, degree, 0, 0, NULL, NULL, NULL, true);
	root = new Node<T>( order, degree, 0, 0, NULL, tail, head, true);
    head->next = root;
    tail->prev = root;
	
	//vector<T> elements;
	//elements = Record.Find(TableName, colum_name );
	// for( int i = 0; i < elements.size(); i++ )
	// 	root->Insert( elements[i], i );
}

template <class T>
set<int> BPlusTree<T>::SearchRange(T val, int op){
	set<int> res;
	switch (op){
		case 0:{
			int pos;
			pni ret = Find(val, root);
			Node<T>* o = ret.first->prev;
			for (; o != tail; o = o->next) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] > val) res.insert(o->blkptr[j]);
			}
			return res;
		}
		case 1:{
			int pos;
			pni ret = Find(val, root);
			//cout << "pos" << pos << endl;
			Node<T>* o = ret.first->next;
			for (; o != head; o = o->prev) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] < val) res.insert(o->blkptr[j]);
			}
			return res;
		}
		case 2:{
			res.insert(Find(val));
			return res;
		}
		case 3:{
			int pos;
			pni ret = Find(val, root);
			Node<T>* o = ret.first->next;
			for (; o != head; o = o->prev) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] <= val) res.insert(o->blkptr[j]);
			}
			return res;
		}
		case 4:{
			int pos;
			pni ret = Find(val, root);
			Node<T>* o = ret.first->prev;
			for (; o != tail; o = o->next) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] >= val) res.insert(o->blkptr[j]);
			}
			return res;
		}
		case 5:{
			for (Node<T>* o = head->next; o != tail; o = o->next){
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] != val) res.insert(o->blkptr[j]);
			}
			return res;
		}
	}
	return res;
}

template <typename T>
int BPlusTree<T>::Find(T val){
	pni res = Find(val, root);
	node now = res.first;
	int pos = res.second;
	if (pos >= now->cnt	|| now->keys[pos] != val) return -1;
	else if (now->keys[pos] == val) return now->blkptr[pos];
	return -1;
}

template <typename T>
pni BPlusTree<T>::Find(T val, node now) {
	//cout << "FInd val" << val << endl;
    typename vector<T>::iterator it;
    if( now->is_leaf ){
        int pos = -1;
        now->Find(val, pos);
		//cout << "pos" << pos << endl;
        return make_pair(now, pos);
    }
    int pos = -1;
    if (now->Find(val, pos)) pos++;
	//cout << "posi " << pos << endl;
    return Find(val, now->childrens[pos]);
}

template <typename T>
void BPlusTree<T>::Insert(T val, int record_id) {
    pni res = Find(val, root);
    node cur = res.first;
    int pos = res.second;
    if( pos < cur->keys.size() && cur->keys[pos] == val ){
        return;
    }
	//cout << "ok";
    cur->Insert(val, record_id);
	//cout << "ok";
    key_cnt++;
    if( cur->IsFull() ){
		//cout << "ok";
        SplitUpdate( cur );
    }
    //return;
}

template <typename T>
void BPlusTree<T>::SplitUpdate(node now) {
    node fa = now->father;
    if (now->IsFull()) {
        T mid_val;
		node_cnt++;
        node newnow = now->Split(mid_val);
        if (!fa) {
            node newroot =
                new Node<T>( order, degree, 0, 0, NULL, NULL, NULL, false);
            root = newroot;
			node_cnt++;
            newroot->childrens.push_back(now);
            newroot->Insert(mid_val, 0,  newnow);
            newnow->father = now->father = newroot;
        } else {
            fa->Insert(mid_val, 0, newnow);
            SplitUpdate(fa);
        }
    }
    else return;
}

template <typename T>
void BPlusTree<T>::Update(node now, T oldval, T newval){
	if (!now) return;
	int pos = 0;
	if (now->Find(oldval, pos)) {now->keys[pos] = newval; return;}
	Update(now->father, oldval, newval);
}

template <typename T>
int BPlusTree<T>::Delete(node now, node brother, T Parentkey){
	int pos = upper_bound(now->keys.begin(), now->keys.end(), tempkey) - now->keys.begin();
	bool find_flag = pos == 0 ? false : now->keys[pos - 1] == tempkey;
	int res;
	if (now->is_leaf) {
		res = FAILED;
		if (find_flag) res = DELETE;
	}
	else {
		if (find_flag) res = Delete(now->childrens[pos], now->childrens[pos - 1], now->keys[pos - 1]);
		else {
			if (pos == now->cnt) res = Delete(now->childrens[pos], now->childrens[pos - 1], now->keys[pos - 1]);
			else res = Delete(now->childrens[pos], now->childrens[pos + 1], now->keys[pos]);
		}
	}
	if (pos == now->cnt || find_flag) pos--;
	int ret = NONE;
	T entry_replace_val = tempkey;
	bool flag = find_flag && pos == 0 && now->is_leaf;

	switch (res)
	{
	case FAILED:
		ret = FAILED;
		break;
	case CHANGE:
		now->keys[pos] = tempkey;
		ret = NONE;
		break;
	case DELETE:
		now->Delete(pos);
		if (now == root){
			if (now->cnt == 0) {
				if (now->is_leaf) {}
				else {
					root = now->childrens.front();
					root->father = NULL;
					node_cnt--;
					delete now;
				}
				
				
			}
			ret = NONE;
		}
		else {
			int lbound = (order + 1) / 2 - 1;
			if (now->cnt < lbound) {
				if (brother->cnt > lbound) { // append
					if (now->next == brother) { //right brother
						if (now->is_leaf) {
							now->keys.push_back(brother->keys.front());
							now->blkptr.push_back(brother->blkptr.front());
							now->cnt++;
							brother->keys.erase(brother->keys.begin());
							brother->blkptr.erase(brother->blkptr.begin());
							brother->cnt--;

							tempkey = brother->keys.front();
							if (flag) Update(now->father, entry_replace_val, now->keys.front());
						}
						else {
							//now->insert(Parentkey, brother->childrens.front());
							now->keys.push_back(Parentkey);
							now->childrens.push_back(brother->childrens.front());
							brother->childrens.front()->father = now;
							now->cnt++;

							tempkey = brother->keys.front();

							brother->keys.erase(brother->keys.begin());
							brother->childrens.erase(brother->childrens.begin());
							brother->cnt--;
						}
						
					}
					else { // left brother
						if (now->is_leaf){
							tempkey = brother->keys.back();
							
							now->keys.insert(now->keys.begin(), tempkey);
							now->blkptr.insert(now->blkptr.begin(), brother->blkptr.back());
							now->cnt++;
							
							brother->keys.pop_back();
							brother->blkptr.pop_back();
							brother->cnt--;
							if (flag) Update(now->father, entry_replace_val, now->keys.front());
						}
						else {
							tempkey = brother->keys.back();
							//now->insert(Parentkey, brother->childrens.back());
							now->keys.insert(now->keys.begin(), Parentkey);
							now->childrens.insert(now->childrens.begin(), brother->childrens.back());
							brother->childrens.back()->father = now;
							now->cnt++;

							brother->keys.pop_back();
							brother->childrens.pop_back();
							brother->cnt--;
						}
					}
					ret = CHANGE;
				}
				else { // merge
					if (now->next == brother) {
						if (!now->is_leaf) {now->keys.push_back(Parentkey); now->cnt++;}
						now->Merge();
						delete brother;
						node_cnt--; 
						ret = DELETE;
						if (now->is_leaf && flag) Update(now->father, entry_replace_val, now->keys.front());
					}
					else {
						
						if (!now->is_leaf) { brother->keys.push_back(Parentkey); brother->cnt++;}
						
						brother->Merge();
						delete now;
						node_cnt--; 
						ret = DELETE;
						if (brother->is_leaf && flag) Update(brother->father, entry_replace_val, brother->keys.front());
					}
				}
			}
			else {
				if (flag) Update(now->father,entry_replace_val, now->keys.front());
			}
		}
			break;
		default:
			ret = NONE;
			break;
	}
	return ret;
}

template <typename T>
int BPlusTree<T>::Remove(T val){
	tempkey = val;
	if (!root) return 0;
	T x = T();
	return Delete(root, NULL, x);
}

template <typename T>
BPlusTree<T>::~BPlusTree() {
	Drop(root);
}

template <typename T>
queue<pni> q;

template <typename T>
void BPlusTree<T>::Print() {
	for (Node<T>* o = head->next; o != tail; o = o->next) {
		for (int j = 0; j < o->cnt; j++)
			cout << o->keys[j] << " ";
	}
	cout << endl;
}

template <typename T>
void BPlusTree<T>::Drop(node now){
	if (!now) return;
	if (!now->is_leaf) {
		for (int i = 0; i <= now->cnt; i++)
			Drop(now->childrens[i]);
	}
	delete now;
	node_cnt--;
	return;
}

#endif