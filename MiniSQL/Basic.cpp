#include"basic.h"
string exception_save;
string table_name;	//表名
string index_id;
vector<string>attri_name;	//返回属性
vector<judge_attr>judger;	//属性大小判断
vector<string>input_value;	//insert用属性值
vector<create_attr>new_table;//创建表用，储存各属性
string index_attr;			//创建索引的属性名
int tot;