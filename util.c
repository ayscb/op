#include "util.h"
#include <string.h>
#include <stdlib.h>

// function: int|long|char --> int long char 
// note : the vector must be defined first
void get_schema(const char* str, Vector* Schema_datatype){
	char delim = '|';
	Vector * schema_str = new_Vector("string");
	split_str(str, delim, schema_str);
	while (schema_str->f_hasMore(schema_str)){
		char * str = (char*)(schema_str->f_next(schema_str));
		DataType * temp = get_type(str);
		Schema_datatype->f_push_back(Schema_datatype,temp);
	}
}

// result : string , int
// func : split the string into vector (sipport int , string)
void split_str(const char * str, char delim, Vector* result){
	char *temp = strdup(str);
	char *q = strtok(temp, &delim);;
	while (q != NULL)
	{
		if (strcmpi(result->_type, "int") == 0)
		{
			const int a = atoi(q);
			result->f_push_back(result, &a);
		}
		else if (strcmpi(result->_type, "string") == 0)
			result->f_push_back(result, q);
		q = strtok(NULL, &delim);
	}
	free(temp);
	temp = NULL;

}

char * getSplitStr( char * str, char delim, int start){
	if (start > strlen(str)){
		return NULL;
	}

	char * p = str;
	p += start;
	char *post = strchr(p, delim);
	if (post == NULL)
		return NULL;

	return NULL;
}
// function : get the DataType
// note : you must free datatype after used
DataType* get_type(const char * typeStrName){
	if (strcmpi(typeStrName, "short") == 0){
		return NULL;
	}
	else if (strcmpi(typeStrName, "int") == 0){
		return new_IntType();
	}
	else if (strcmpi(typeStrName, "long") == 0){
		return new_LongType();
	}
	else if (strcmpi(typeStrName, "double") == 0){
		return new_DoubleType();
	}
	else if (strcmpi(typeStrName, "float") == 0){
		return new_FloatType();
	}
	else if (strcmpi(typeStrName, "char") == 0){
		return NULL;
	}
	else if (strcmpi(typeStrName, "string") == 0){
		return new_StringType();
	}
	else{
		return NULL;
	}
}

// 在build_hash_table，getValuesFromRow中使用
// func: get the 'string' that we combine the some columns .( in fact ,the 'string' is a binary string)
// argc: 
//	row[in] : the row that store the data and schema
//	comb_ids[in] : the ids 's vector that we want to combine ( the type is 'int')
//	out_str[out] : the string we will get by this method.
//	out_strlen[out]: the string's length 
char * getCombineChar(Row* row, Vector comb_ids, char * out_str, int * out_strlen){
	unsigned int str_pos = 0;
	Vector* datatype_v = row->_schema_datatype;	// get the datatype schema

	int size = comb_ids.f_size(&comb_ids);	
	for (int i = 0; i < size; i++)		//
	{
		int idx = *(int*)comb_ids.f_get_value(&comb_ids, i);
		DataType* datatype = (DataType*)datatype_v->f_get_value(datatype_v, idx);	// get the datatype

		if (strcmpi(datatype->f_Name(datatype), "int") == 0){
			int value = row->f_getInt(row, idx);
			memcpy(out_str + str_pos, &value, sizeof(int));
			str_pos += sizeof(int);
		}
		else if (strcmpi(datatype->f_Name(datatype), "long") == 0){
			long value = row->f_getLong(row, idx);
			memcpy(out_str + str_pos, &value, sizeof(long));
			str_pos += sizeof(long);
		}
		else if (strcmpi(datatype->f_Name(datatype), "double") == 0){
			double value = row->f_getDouble(row, idx);
			memcpy(out_str + str_pos, &value, sizeof(double));
			str_pos += sizeof(double);
		}
		else if (strcmpi(datatype->f_Name(datatype), "float") == 0){
			float value = row->f_getFloat(row, idx);
			memcpy(out_str + str_pos, &value, sizeof(float));
			str_pos += sizeof(float);
		}
		else if (strcmpi(datatype->f_Name(datatype), "string") == 0){
			char* value = row->f_getString(row, idx);
			memcpy(out_str + str_pos, value, strlen(value));
			str_pos += strlen(value) +1;
		}
		else{
			return NULL;
		}
	}
	*out_strlen = str_pos;
	return out_str;
}

void getClmLength(Vector * datatype, Vector* in_vec_clm, Vector * out_vec_clmLen){
	while (in_vec_clm->f_hasMore(in_vec_clm))
	{
		int index = *(int*)in_vec_clm->f_next(in_vec_clm);
		DataType *data = (DataType*)datatype->f_get_value(datatype, index);
		out_vec_clmLen->f_push_back(out_vec_clmLen, &data->_width);
	}
}

// get test time ns
unsigned   __int64   GetCycleCount()
{
	__asm   _emit   0FH
	__asm   _emit   31H
}