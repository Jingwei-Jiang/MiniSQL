#ifndef BLOCKMAN_H
#define BLOCKMAN_H
#pragma once
#include <iostream>
#include <cassert>
#include <fstream>
#include <map>
#include <string>
#include <cmath>
#include "Block.h"
using namespace std;
class Buffer{
    Block* Buf;
public:
    Buffer();
    ~Buffer();
    Buffer(const Buffer& b);
    Block& ReadBlock(string& filename,int id);
    Block& LRU();
    void BufToFile(string& filename);
    void FlushBuf();
};
#endif