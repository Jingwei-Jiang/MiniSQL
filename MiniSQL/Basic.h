#ifndef BASIC_H
#define BASIC_H
#include<string>
#include<vector>

using namespace std;

typedef struct judge_attr {
	string attr;
	string op;
	string value;
	int type;
}judge_attr;

typedef struct create_attr {
	string name;	//属性名
	int type;		//-1：int,0:float,1~255:string的长度
	bool unique;	//判断属性是否unique，是为true
	bool primary_key;//判断属性是否primary，是为true
}create_attr,attribute;

typedef struct index_info {
	string index_name;
	string attri_name;
}index;

enum class IFStype
{
	UNDEF = 0,
	INT,
	FLOAT,
	CHAR
};

enum class OP {
	UNDEF=0,
	GREATER,
	SMALLER,
	EQUAL,
	LEQ,
	GEQ,
	NEQ
};

typedef struct insert_index {
	string indexStr;//table_attri_type.idx
	int input_i;
	float input_f;
	string input_s;
}index_node;



extern string exception_save;	//报错关键信息的储存
extern string table_name;	//表名
extern string index_id;
extern vector<string>attri_name;	//返回属性
extern vector<judge_attr>judger;	//属性大小判断
extern vector<string>input_value;	//insert用属性值
extern vector<create_attr>new_table;//创建表用，储存各属性
extern string index_attr;			//创建索引的属性名
extern int tot;


//exceptions
class table_exist : public std::exception {

};

class table_not_exist : public std::exception {

};

class attribute_not_exist : public std::exception {

};
class duplicate_column :public std::exception {

};

class index_exist : public std::exception {

};

class index_not_exist : public std::exception {

};


class primary_key_conflict : public std::exception {

};

class data_type_conflict : public std::exception {

};

class attri_not_key : public std::exception {

};

class input_format_error : public std::exception {

};

class exit_command : public std::exception {

};

class unique_conflict :public std::exception {

};

class wrong_filepath :public std::exception {

};

class duplicate_key_name :public std::exception {

};

class column_too_long :public std::exception {

};

class data_too_long :public std::exception {

};

class column_not_match :public std::exception {

};

#endif // !BASIC_H
