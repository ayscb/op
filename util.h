#ifndef __aggregation__Util__
#define __aggregation__Util__

#include "DataType.h"
#include "Vector.h"

enum AggFuncName
{
	sum, max, min, avg
};

void get_schema(const char* str, Vector* Schema_datatype);
void split_str(const char * str, char delim, Vector* result);
DataType* get_type(const char * typeStrName);


// 在build_hash_table中使用
// func: get the 'string' we combine the some columns .( in fact ,the 'string' is a binary string)
// argc: 
//	row[in] : the row that store the data and schema
//	comb_ids[in] : the ids 's vector that we want to combine ( the type is 'int')
//	out_str[out] : the string we will get by this method.
//	out_strlen[out]: the string's length 
char * getCombineChar(Row* row, Vector comb_ids, char * out_str, int * out_strlen);

unsigned   __int64   GetCycleCount();
#endif /* defined(__aggregation__util__) */