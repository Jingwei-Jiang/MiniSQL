#include "BlockMan.h"
Buffer global_buffer;    
Buffer::Buffer(){
    Buf = new Block[BLOCK_SIZE];
}
Buffer::~Buffer(){
    delete[] Buf;
}
Buffer::Buffer(const Buffer& b){
    this->Buf=b.Buf;
}
Block& Buffer::ReadBlock(string& filename,int id){
    for (int i=0;i<BLOCK_NUM;i++){
        Buf[i].SetFlagQueue(Buf[i].GetFlagQueue()+1);
        if (Buf[i].GetFileName()==filename && Buf[i].GetBlockId()==id){
            Buf[i].SetFlagQueue(0);
            return Buf[i];
        }
    }
    Block &cur = LRU();
    cur.SetFileName(filename);
    cur.SetBlockId(id);
    string file_path="Tables\\" + filename+".data";
    ifstream file_in;
    file_in.open(file_path.c_str(),ios::in|ios::binary);
    if (!file_in.is_open()){
        file_in.close();
        cerr<<"The file does not exists!"<<endl;
        ofstream file_out;
        file_out.open(file_path.c_str(),ios::out|ios::binary);
        file_out.close();
        file_in.open(file_path.c_str(),ios::in|ios::binary);
    }
    file_in.seekg(id*BLOCK_SIZE,ios::beg);
    if (file_in.tellg()==id*BLOCK_SIZE){
        file_in.read((char*)(cur.GetData()),BLOCK_SIZE);
    }else{
        cerr<<"Pointer Located Falure"<<endl;
    }
        file_in.close();
    return cur;
}
Block& Buffer::LRU(){
    unsigned int max=0;
    int Block_id=-1;
    int i;
    for (i=BLOCK_NUM-1;i>=0;i--){
        if (this->Buf[i].GetFlagQueue()>max && this->Buf[i].GetPinTag()==false){
            max=this->Buf[i].GetFlagQueue();
            Block_id=i;
        }
    }
    if (Block_id==-1){
        cerr<<"LRU Failure"<<endl;
    }
    if (this->Buf[Block_id].GetReadinTag()==true){
        this->Buf[Block_id].WritetoFile();
    }
    this->Buf[Block_id].clear();
    return this->Buf[Block_id];
}
void Buffer::BufToFile(string& filename){
    for (int i=0;i<BLOCK_NUM;i++){
        if (this->Buf[i].GetFileName()==filename){
            this->Buf[i].WritetoFile();
            this->Buf[i].clear();
        }
    }
}
void Buffer::FlushBuf(){
    for (int i=BLOCK_NUM-1;i>=0;i--){
        this->Buf[i].clear();
    }
}