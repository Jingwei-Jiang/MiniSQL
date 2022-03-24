#include"Block.h"
Block::Block():Pin(false), Readin(false), Busy(false), FileName(""), BlockId(-1), FlagQueue(0){
    Data = new unsigned char [BLOCK_SIZE];
    for (int i=0;i< BLOCK_SIZE ;i++){
        Data[i]=0;
    }
}
Block::Block(const Block& b){
    this->BlockId=b.BlockId;
    this->Busy=b.Busy;
    this->Readin=b.Readin;
    this->Pin=b.Pin;
    this->Data=b.Data;
    this->FileName=b.FileName;
}
Block::~Block(){

}
void Block::clear(){
    this->BlockId=-1;
    for (int i=0;i< BLOCK_SIZE ;i++){
        (this->Data)[i]=0;
    }
    this->Busy=false;this->Pin=false;
    this->Readin=false;
    this->FileName="";
    this->FlagQueue=0;
}
bool Block::GetPinTag(){
    return this->Pin;
}
bool Block::GetReadinTag(){
    return this->Readin;
}
bool Block::GetBusyTag(){
    return this->Busy;
}
string Block::GetFileName(){
    return this->FileName;
}
int Block::GetBlockId(){
    return this->BlockId;
}
unsigned char* Block::GetData(){
    return this->Data;
}
unsigned int Block::GetFlagQueue(){
    return this->FlagQueue;
}
void Block::SetFileName(string FN){
    this->FileName=FN;
}
void Block::SetPinTag(bool PT){
    this->Pin=PT;
}
void Block::SetReadinTag(bool RT){
    this->Readin=RT;
}
void Block::SetBusyTag(bool BT){
    this->Busy=BT;
}
void Block::SetBlockId(int ID){
    this->BlockId=ID;
}
void Block::SetFlagQueue(unsigned int FQ){
    this->FlagQueue=FQ;
}
void Block::WritetoFile(){
    if (this->Readin!=false && this->FileName!=""){
        string file_path;
        file_path="Tables\\" + this->FileName+".data";
        FILE* file;
        file=fopen(file_path.c_str(),"rb+");
        if (file){
            fseek(file,this->BlockId*BLOCK_SIZE,SEEK_SET);
            fwrite(this->Data,BLOCK_SIZE,1,file);
            fclose(file);
        }else{
            cerr<<"Can not access the file"<<endl;
        }
    }
}
void Block::ReadString(string &s, int len, int offset){
    const char *c=s.c_str();
    memcpy((unsigned char*)c,this->Data+offset,len);
    int length=0;
    while(c[length]&&length<len){
        length++;
    }
    s = string(c, c+length);
}
void Block::ReadInt(int &i, int len, int offset){
    memcpy((unsigned char*)&i,this->Data+offset,len);
}
void Block::ReadFloat(float &f, int len, int offset){
    memcpy((unsigned char *)&f,this->Data+offset,len);
}
void Block::ReadVaild(char &v, int len, int offset){
    memcpy((unsigned char *)&v,this->Data+offset,len);
}
void Block::WriteString(string s, int len, int offset){
    this->Readin=true;
    const char *c=s.c_str();
    memcpy(this->Data+offset,c,len);    
}
void Block::WriteInt(int &i, int len, int offset){
    this->Readin=true;
    memcpy(this->Data+offset,(unsigned char*)&i,len);
}
void Block::WriteFloat(float &f,int len, int offset){
    this->Readin=true;
    memcpy(this->Data+offset,(unsigned char*)&f,len);
}
void Block::WriteVaild(char &v, int len, int offset){
    this->Readin=true;
    memcpy(this->Data+offset,(unsigned char*)&v,len);
}