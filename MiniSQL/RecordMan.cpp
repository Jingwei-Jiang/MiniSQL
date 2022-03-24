#include "RecordMan.h"
extern Buffer global_buffer;
RecordManager::RecordManager(){

}
int RecordManager::GetRecordSize(TableCatalog table){
    int size_per_record=0;
    for (auto i:table.Columns){
        if (i.Type==TableCatalog::ColumnType::Int){
            size_per_record+=sizeof(int);
        }
        if (i.Type==TableCatalog::ColumnType::Float){
            size_per_record+=sizeof(float);
        }
        if (i.Type==TableCatalog::ColumnType::Char){
            size_per_record+=sizeof(char)*i.Size;
        }
    }
    size_per_record+=1;//1 byte for vaild
    return size_per_record; 
}
bool RecordManager::CreateTable(TableCatalog table){
    string file_path="Tables\\" + table.TableName+".data";
    ofstream file_in;
    file_in.open(file_path/*.c_str(),ios::in|ios::binary*/);
    if (file_in.is_open()){
        return true;
    }else{
        cerr<<"Create file error"<<endl;
        return false;
    }
}
void RecordManager::DropTable(string filename){
    global_buffer.BufToFile(filename);
    string file_path="Tables\\" + filename+".data";
    remove(file_path.c_str());
}
bool RecordManager::InsertRecord(TableCatalog table,Record R){
    int record_per_block;
    int size_per_record;
    size_per_record=GetRecordSize(table);
    record_per_block=BLOCK_SIZE/size_per_record;
    int block_id = R.ID/record_per_block;
    Block & block=global_buffer.ReadBlock(table.TableName,block_id);
    block.SetReadinTag(true);
    int offset=(R.ID%record_per_block)*size_per_record;
    char vaild=1;
    block.WriteVaild(vaild,sizeof(char),offset);
    offset+=sizeof(char);
    //   byte 1st   2nd.....
    //content vaild other elements
    int iteration=0;
    for (auto i:R.Content){
        if(i.GetType()==Record::ValueType::Int){
            int int_data=i.GetInt();
            block.WriteInt(int_data,sizeof(int),offset);
            offset+=sizeof(int);
        }
        if(i.GetType()==Record::ValueType::Float){
            float float_data=i.GetFloat();
            block.WriteFloat(float_data,sizeof(float),offset);
            offset+=sizeof(float);
        }
        if(i.GetType()==Record::ValueType::Char){
            string char_data=i.GetChar();
            int length=char_data.length();
            block.WriteString(char_data,sizeof(char)*length,offset);
            offset+=sizeof(char)*length;
            TableCatalog::Column c=table.Columns[iteration];
            int size=c.Size;
            if (length<size){
            string alignment(size-length,0);
            block.WriteString(alignment,sizeof(char)*(size-length),offset);
            offset+=sizeof(char)*(size-length);
            }
        }
        iteration++;
    }
    block.WritetoFile();
    return 1;
}
int RecordManager::DeleteRecord(TableCatalog table,set<int> IDSet){
    int size_per_record=GetRecordSize(table);
    int record_per_block=BLOCK_SIZE/size_per_record;
    int deleted_records_num=0;
    for (auto i:IDSet){
        int block_id=i/record_per_block;
        Block & block=global_buffer.ReadBlock(table.TableName,block_id);
        int offset=(i%record_per_block)*size_per_record;
        char vaild;
        block.ReadVaild(vaild,sizeof(char),offset);
        if (vaild==1){
            vaild=0;
            deleted_records_num++;
            block.WriteVaild(vaild,sizeof(char),offset);
            block.WritetoFile();
            block.clear();
        }
    }
    return deleted_records_num;
}
bool RecordManager::DeleteAll(TableCatalog table){
    int size_per_record=GetRecordSize(table);
    int record_per_block=BLOCK_SIZE/size_per_record;
    int block_num=table.RecordNumber/record_per_block;
    for (int i=0;i<=block_num;i++){
        Block & block=global_buffer.ReadBlock(table.TableName,i);
        if (i!=block_num){
            for (int j=0;j<record_per_block;j++){
                int offset=j*size_per_record;
                char vaild=0;
                block.WriteVaild(vaild,sizeof(char),offset);
                }
        }else{
            for (int j=0;j<table.RecordNumber-i*record_per_block;j++){
                int offset=j*size_per_record;
                char vaild=0;
                block.WriteVaild(vaild,sizeof(char),offset);
            }
        }
        block.WritetoFile();
        block.clear();
    }
    return 1;   
}
set<int> RecordManager::SelectRecord(TableCatalog table,Condition::Value cond){
    set<int> Record_ID;
    string column_name=cond.GetColumnName();
    string condition_operator=cond.GetOperator();
    int column_index=table.ColumnID[column_name];
    TableCatalog::Column c=table.Columns[column_index];
    int ofst=1;
    for (auto i:table.Columns){
        if (table.ColumnID[i.Name]<column_index){
            if (i.Type==TableCatalog::ColumnType::Int){
                ofst+=sizeof(int);
            }
            if (i.Type==TableCatalog::ColumnType::Float){
                ofst+=sizeof(float);
            }
            if (i.Type==TableCatalog::ColumnType::Char){
                ofst+=sizeof(char)*i.Size;
            }
        }
    }
    int size_per_record=GetRecordSize(table);
    int record_per_block=BLOCK_SIZE/size_per_record;
    int block_num=table.RecordNumber/record_per_block;
    if (c.Type==TableCatalog::ColumnType::Int){
        int int_data=cond.GetInt();
        for (int i=0;i<=block_num;i++){
            Block & block=global_buffer.ReadBlock(table.TableName,i);
            if (i!=block_num){
                for (int j=0;j<record_per_block;j++){
                    int offset=j*size_per_record;
                    char v;
                    block.ReadVaild(v,sizeof(char),offset);
                    if (v==1){
                        offset+=ofst;
                        int column_data;
                        block.ReadInt(column_data,sizeof(int),offset);
                        if (Comparison(column_data,int_data,condition_operator)==true){
                            Record_ID.insert(i*record_per_block+j);
                        }
                    }
                }
            }else{
                for (int j=0;j<table.RecordNumber-i*record_per_block;j++){
                    int offset=j*size_per_record;
                    char v;
                    block.ReadVaild(v,sizeof(char),offset);
                    if (v==1){
                        offset+=ofst;
                        int column_data;
                        block.ReadInt(column_data,sizeof(int),offset);
                        if (Comparison(column_data,int_data,condition_operator)==true){
                            Record_ID.insert(i*record_per_block+j);
                        }
                }
                }
            }
        }
    }
    if (c.Type==TableCatalog::ColumnType::Float){
        float float_data=cond.GetFloat();
    for (int i=0;i<=block_num;i++){
        Block & block=global_buffer.ReadBlock(table.TableName,i);
        if (i!=block_num){
            for (int j=0;j<record_per_block;j++){
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    offset+=ofst;
                    float column_data;
                    block.ReadFloat(column_data,sizeof(float),offset);
                    if (Comparison(column_data,float_data,condition_operator)==true){
                        Record_ID.insert(i*record_per_block+j);
                    }
                }
            }
        }else{
            for (int j=0;j<table.RecordNumber-i*record_per_block;j++){
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    offset+=ofst;
                    float column_data;
                    block.ReadFloat(column_data,sizeof(float),offset);
                    if (Comparison(column_data,float_data,condition_operator)==true){
                        Record_ID.insert(i*record_per_block+j);
                    }
                }
            }
        }
    }
    }
    if (c.Type==TableCatalog::ColumnType::Char){
        string char_data=cond.GetChar();
    for (int i=0;i<=block_num;i++){
        Block & block=global_buffer.ReadBlock(table.TableName,i);
        if (i!=block_num){
            for (int j=0;j<record_per_block;j++){
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    offset+=ofst;
                    string column_data;
                    block.ReadString(column_data,sizeof(char)*c.Size,offset);
                    if (Comparison(column_data,char_data,condition_operator)==true){
                        Record_ID.insert(i*record_per_block+j);
                    }
                }
            }
        }else{
            for (int j=0;j<table.RecordNumber-i*record_per_block;j++){
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    offset+=ofst;
                    string column_data;
                    block.ReadString(column_data,sizeof(char)*c.Size,offset);
                    if (Comparison(column_data,char_data,condition_operator)==true){
                        Record_ID.insert(i*record_per_block+j);
                    }
                }
            }
        }
    }
    }
    return Record_ID;
}

set<int> RecordManager::SelectAll(TableCatalog table){
    set<int> Record_ID;
    int size_per_record=GetRecordSize(table);
    int record_per_block=BLOCK_SIZE/size_per_record;
    int block_num=table.RecordNumber/record_per_block;
    for (int i=0;i<=block_num;i++){
        Block & block=global_buffer.ReadBlock(table.TableName,i);
        if (i!=block_num){
            for (int j=0;j<record_per_block;j++){
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    Record_ID.insert(i*record_per_block+j);
                }
            }
        }else{
            for (int j=0;j<table.RecordNumber-i*record_per_block;j++){
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    Record_ID.insert(i*record_per_block+j);
                }
            }
        }
    }
    return Record_ID;
}

vector<Record> RecordManager::GetRecord(TableCatalog table,set<int> IDSet){
    vector<Record> records;
    int size_per_record=GetRecordSize(table);
    int record_per_block=BLOCK_SIZE/size_per_record;
    for(auto i:IDSet){
        Record R;
        int block_id=i/record_per_block;
        Block & block=global_buffer.ReadBlock(table.TableName,block_id);
        int offset=(i%record_per_block)*size_per_record;
        char valid;
        block.ReadVaild(valid,sizeof(char),offset);
        if (valid==1){
            offset++;
            for(auto j:table.Columns){
                if (j.Type==TableCatalog::ColumnType::Int){
                    int int_data;
                    block.ReadInt(int_data,sizeof(int),offset);
                    R.Add(int_data);
                    offset+=sizeof(int);
                }
                if (j.Type==TableCatalog::ColumnType::Float){
                    float float_data;
                    block.ReadFloat(float_data,sizeof(float),offset);
                    R.Add(float_data);
                    offset+=sizeof(float);
                }
                if (j.Type==TableCatalog::ColumnType::Char){
                    string char_data;
                    block.ReadString(char_data,sizeof(char)*j.Size,offset);
                    R.Add(char_data);
                    offset+=sizeof(char)*j.Size;
                }
            }
            records.push_back(R);
        }
    }
    return records;
}

vector<pair<Record::Value,int>> RecordManager::GetColumn(TableCatalog table,string columnname){
    int size_per_record=GetRecordSize(table);
    int record_per_block=BLOCK_SIZE/size_per_record;
    int column_index=table.ColumnID[columnname];
    int block_num=table.RecordNumber/record_per_block;
    int ofst=1;
    vector<pair<Record::Value,int>>column_data;
    TableCatalog::Column c;
    for (auto i:table.Columns){
        if (table.ColumnID[i.Name]<column_index){
            if (i.Type==TableCatalog::ColumnType::Int){
                ofst+=sizeof(int);
            }
            if (i.Type==TableCatalog::ColumnType::Float){
                ofst+=sizeof(float);
            }
            if (i.Type==TableCatalog::ColumnType::Char){
                ofst+=sizeof(char)*i.Size;
            }
        }
        if (i.Name==columnname){
            c=i;
            break;
        }
    }
    for (int i=0;i<=block_num;i++){
        Block & block=global_buffer.ReadBlock(table.TableName,i);
        if (i!=block_num){
            for (int j=0;j<record_per_block;j++){
                pair<Record::Value,int>key_pair;
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    offset+=ofst;
                    if (c.Type==TableCatalog::ColumnType::Int){
                        int int_data;
                        block.ReadInt(int_data,sizeof(int),offset);
                        key_pair.first=Record::Value(int_data);
                    }
                    if (c.Type==TableCatalog::ColumnType::Float){
                        float float_data;
                        block.ReadFloat(float_data,sizeof(float),offset);
                        key_pair.first=Record::Value(float_data);
                    }
                    if (c.Type==TableCatalog::ColumnType::Char){
                        string char_data;
                        block.ReadString(char_data,sizeof(char)*c.Size,offset);
                        key_pair.first=Record::Value(char_data);
                    }
                }
                key_pair.second=i*record_per_block+j;
                column_data.push_back(key_pair);
            }
        }else{
            for (int j=0;j<table.RecordNumber-i*record_per_block;j++){
                pair<Record::Value,int>key_pair;
                int offset=j*size_per_record;
                char v;
                block.ReadVaild(v,sizeof(char),offset);
                if (v==1){
                    offset+=ofst;
                    if (c.Type==TableCatalog::ColumnType::Int){
                        int int_data;
                        block.ReadInt(int_data,sizeof(int),offset);
                        key_pair.first=Record::Value(int_data);
                    }
                    if (c.Type==TableCatalog::ColumnType::Float){
                        float float_data;
                        block.ReadFloat(float_data,sizeof(float),offset);
                        key_pair.first=Record::Value(float_data);
                    }
                    if (c.Type==TableCatalog::ColumnType::Char){
                        string char_data;
                        block.ReadString(char_data,sizeof(char)*c.Size,offset);
                        key_pair.first=Record::Value(char_data);
                    }
                }
                key_pair.second=i*record_per_block+j;
                column_data.push_back(key_pair);
            }
        }
    }
    return column_data;
}