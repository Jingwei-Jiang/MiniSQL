
#ifndef BPLUSTREENODE_H
#define BPLUSTREENODE_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "Basic.h"


template <typename T>
class Node {
private:
    int order, degree;
	
public:
    int cnt;
	int id;
    Node* father;
    vector<T> keys;
	vector<int> blkptr;
    vector<Node*> childrens;
    Node* next;
    Node* prev;
    bool is_leaf;

    Node(int _order = 3, int _degree = 3, int _cnt = 0, int _id = 0,  Node* _father = NULL, Node* _next = NULL, Node* _prev = NULL, bool is_new_leaf = false);
    ~Node();
    bool IsFull();
    bool Find(T key, int& pos);
    Node* Split(T& val);
    void Insert(T key, int blockoffset = 0, Node* ch = NULL);
    void Delete(int pos);
	void Merge();
    // friend ostream& operator<<(ostream& os, const Node<T>& rhs) {
    //     if (&rhs == NULL)
    //         os << "NULL";
    //     else {
    //         os << "[";
	// 		os << "id:" << rhs.id << " " <<"nxt:" << (rhs.next == NULL ? 0 : rhs.next->id) <<"; ";
    //         os << "ch:(";
	// 		for (auto i : rhs.childrens) os << i->id << ","; os <<"), ";
    //         if (!rhs.is_leaf){
	// 			for (int i = 0; i < rhs.keys.size(); i++)
    //             os << rhs.keys[i] << (i + 1 == rhs.keys.size() ? ']' : ',');
	// 		}
	// 		else {
	// 			for (int i = 0; i < rhs.keys.size(); i++)
    //             os << rhs.keys[i] << ":" << rhs.blkptr[i] << (i + 1 == rhs.keys.size() ? ']' : ',');
	// 		}
	// 	}   
	// 	return os;
    // }
};

template <typename T>
Node<T>::Node(int _order, int _degree, int _cnt, int _id ,  Node* _father, Node* _next, Node* _prev, bool _is_leaf ):
    order(_order), 
    degree(_degree),
    cnt(_cnt),
    father(_father),
    next(_next),
    prev(_prev),
    is_leaf(_is_leaf)
{
	if (_id == 0) id = 1;
	else id = _id;
    keys.clear();
	blkptr.clear(); 
    childrens.clear();
}


template <typename T>
bool Node<T>::Find(T key, int& pos) {
    pos = 0;
    typename vector<T>::iterator it;
    it = lower_bound(keys.begin(), keys.end(), key);
    pos = it - keys.begin();

    if (it == keys.end()) return false;
    if (*it == key) return true;
    else return false;
}

template <typename T>
void Node<T>::Merge(){
	Node<T>* brother = next;
	if (is_leaf)
		for (int i = 0; i < brother->cnt; i++) {
			keys.push_back(brother->keys[i]);
			blkptr.push_back(brother->blkptr[i]);
		}
	else {
		for (int i = 0; i < brother->cnt; i++) {
			keys.push_back(brother->keys[i]);
		}
		for (int i = 0; i < brother->cnt + 1; i++) {
			childrens.push_back(brother->childrens[i]);
			brother->childrens[i]->father = this;
		}
	}
	cnt += brother->cnt;
	if (brother->next) brother->next->prev = this;
	next = brother->next;
	return;
}

template <typename T>
Node<T>* Node<T>::Split(T& val) {
    int leftsize = cnt / 2;
    Node* newNode = new Node<T>(order, degree, cnt - leftsize - (!is_leaf), 0,  father, this->next, this, is_leaf);
	if (next) next->prev =newNode;
	next = newNode;
    if (is_leaf) {
        for (int i = leftsize; i < cnt; i++) {
            newNode->keys.push_back(keys[i]);
			newNode->blkptr.push_back(blkptr[i]);
		}
        while (keys.size() != leftsize) {
			keys.pop_back();
			blkptr.pop_back();
		}
		cnt = leftsize;
        val = *(newNode->keys.begin());
    } else {
        val = (keys[leftsize]);
        for (int i = leftsize + 1; i < cnt; i++) {
            newNode->keys.push_back(keys[i]);
		}
        for (int i = leftsize + 1; i < childrens.size(); i++)
            childrens[i]->father = newNode,
            newNode->childrens.push_back(childrens[i]);
        while (childrens.size() != leftsize + 1) childrens.pop_back();
        while (keys.size() != leftsize) keys.pop_back();
        cnt = leftsize;
    }
    return newNode;
}

template <typename T>
void Node<T>::Insert(T val, int record_id, Node* ch) {
   // cout << "cnt " << cnt << endl ;
    typename vector<T>::iterator it;
    if (is_leaf) {
        it = lower_bound(keys.begin(), keys.end(), val);
		int pos = it - keys.begin();
        keys.insert(it, val);
		blkptr.insert(blkptr.begin() + pos, record_id);
        //cout << "node" << pos ;
    } else {
        it = lower_bound(keys.begin(), keys.end(), val);
        int tmppos = it - keys.begin();
        tmppos++;
        keys.insert(it, val);
        childrens.insert(childrens.begin() + tmppos, ch);
		ch->father = this;
    }
    cnt++; 
    //cout << endl << cnt <<"ok";
}

template <typename T>
bool Node<T>::IsFull() {
    if (is_leaf)
        return cnt > order;
    else
        return cnt > degree;
}

template <typename T>
void Node<T>::Delete(int pos) {
	if (is_leaf) {
		keys.erase(keys.begin() + pos);
		blkptr.erase(blkptr.begin() + pos);
	}
	else {
		keys.erase(keys.begin() + pos);
		childrens.erase(childrens.begin() + pos + 1);
	}
	cnt--;
	return;
}

template <typename T>
Node<T>::~Node() {}

#endif