#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <cassert>
#include <fstream>
#include <map>
#include <string>
#include <cmath>
using namespace std;
#define BLOCK_SIZE 4096
#define BLOCK_NUM 1024
class Block{
    bool Pin,Readin,Busy;
    string FileName;
    unsigned char *Data;
    int BlockId;
    unsigned int FlagQueue;
public:
    Block();
    Block(const Block &b);
    ~Block();
    void clear();
    bool GetPinTag();
    bool GetReadinTag();
    bool GetBusyTag();
    string GetFileName();
    int GetBlockId();
    unsigned char* GetData();
    unsigned int GetFlagQueue();
    void SetFileName(string FN);
    void SetPinTag(bool PT);
    void SetReadinTag(bool RT);
    void SetBusyTag(bool BT);
    void SetBlockId(int ID);
    void SetFlagQueue(unsigned int FQ);
    void WritetoFile();
    void ReadString(string &s, int len, int offset);
    void ReadInt(int &i, int len, int offset);
    void ReadFloat(float &f, int len, int offset);
    void ReadVaild(char &v, int len, int offset);
    void WriteString(string s, int len, int offset);
    void WriteInt(int &i, int len, int offset);
    void WriteFloat(float &f,int len, int offset);
    void WriteVaild(char &v, int len, int offset);
};
#endif