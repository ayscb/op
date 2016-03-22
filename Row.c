#include "Row.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doString.h"
#include "util.h"
void loadFunc(Row * obj);

Row * new_Row(void ** a, int size){
	Row *obj = (Row*)malloc(sizeof(Row));
	if (obj == NULL)
		return NULL;
	obj->_row = a;
	obj->_size = size;
	obj->_schema_datatype = NULL;
	loadFunc(obj);
	return obj;
}
Row * new_Row_from_schema(Vector* in_schema_datatype){
	Row *obj = (Row*)malloc(sizeof(Row));
	if (obj == NULL)
		return NULL;
	obj->_size = in_schema_datatype->f_size(in_schema_datatype);

	obj->_row = (void**)malloc(obj->_size * sizeof(void*));
	if (obj->_row == NULL)
		return NULL;
	obj->_schema_datatype = new_Vector("DataType");

	for (int i = 0; i < obj->_size; i++){
		DataType * data = (DataType*)in_schema_datatype->f_get_value(in_schema_datatype, i);
		
		obj->_schema_datatype->f_push_back(obj->_schema_datatype, data);

		if ( strcmpi( data->f_Name(data) ,"int") == 0) {
			obj->_row[i] = (int*)malloc(sizeof(int));
		}
		else if (strcmpi(data->f_Name(data), "long") == 0) {
			obj->_row[i] = (long*)malloc(sizeof(long));
		}
		else if (strcmpi(data->f_Name(data), "float") == 0) {
			obj->_row[i] = (float*)malloc(sizeof(float));
		}
		else if (strcmpi(data->f_Name(data), "double") == 0) {
			obj->_row[i] = (double*)malloc(sizeof(double));
		}
		else if (strcmpi(data->f_Name(data), "string") == 0) {
			obj->_row[i] = (char*)malloc(sizeof(char)* 1024);
		}
	}
	loadFunc(obj);
	return obj;
}

void setSchema(Row* row, Vector* schema_datatype){
	if (row != NULL)
		row->_schema_datatype = new_Vector("DataType");
	while (schema_datatype->f_hasMore(schema_datatype))
	{
		DataType * tem = (DataType*)(schema_datatype->f_next(schema_datatype));
		row->_schema_datatype->f_push_back(row->_schema_datatype,tem);
	}
}
int getInt(Row * obj, int i){ return *((int *)(obj->_row[i])); }
void putInt(Row * obj, int index, int new_val){ *(int*)(obj->_row[index]) = new_val; }

long getLong(Row * obj, int i){ return *((long *)(obj->_row[i])); }
void putLong(Row * obj, int index, long new_val){ *((long *)obj->_row[index]) = new_val; }

float getFloat(Row * obj, int i){ return *((float *)(obj->_row[i])); }
void putFloat(Row * obj, int index, float new_val){ *((float *)obj->_row[index]) = new_val; }

double getDouble(Row * obj, int i){ return *((double *)(obj->_row[i])); }
void putDouble(Row * obj, int index, double new_val){ *((double *)obj->_row[index]) = new_val; }

char * getString(Row * obj, int i){ return ((char *)(obj->_row[i])); }
void putString(Row * obj, int index, char* new_val){ 
	void * te = *(obj->_row + index);
	strcpy((char*)te, new_val);
	//(obj->_row[index]) = strdup(new_val); 
}

// 123qweqre32435235    这种单纯拼接的
//void updateRow(Row * obj, char * txt_row, int txt_row_len){
void updateRow(Row * obj, char * txt_row){

	int pos = 0;  // txt_row position
	int column_id = 0;

	Vector * datatype = obj->_schema_datatype;
	while (column_id < datatype->f_size(datatype)) {

		DataType * datatype = (DataType*)obj->_schema_datatype->f_get_value(obj->_schema_datatype, column_id);
		if (strcmpi(datatype->f_Name(datatype), "int") == 0) {
			obj->f_putInt(obj, column_id, *(int*)(txt_row + pos));
			pos += sizeof(int);
		}
		else if (strcmpi(datatype->f_Name(datatype), "float") == 0) {
			obj->f_putFloat(obj, column_id, *(float*)(txt_row + pos));
			pos += sizeof(float);
		}
		else if (strcmpi(datatype->f_Name(datatype), "double") == 0) {
			obj->f_putDouble(obj, column_id, *(double*)(txt_row + pos));
			pos += sizeof(double);
		}
		else if (strcmpi(datatype->f_Name(datatype), "long") == 0) {
			obj->f_putLong(obj, column_id, *(long*)(txt_row + pos));
			pos += sizeof(long);
		}
		else if (strcmpi(datatype->f_Name(datatype), "string") == 0) {
			obj->f_putString(obj, column_id, txt_row + pos);
			pos += strlen(txt_row + pos) + 1;
		}
		column_id++;
	}
}

// "sdasd|1234|2|sss"	这种字符串
void updateRow_str(Row * obj, char * txt_row){
	if (obj->_schema_datatype == NULL)
		return;
	Vector * str_string = new_Vector("string");
	split_str(txt_row, '|', str_string);
	int i = 0;

	Vector * datatype = obj->_schema_datatype;
	
	while (str_string->f_hasMore(str_string) && i < obj->_size){
			DataType * temp = (DataType*)(datatype->f_get_value(datatype, i));
			if (strcmpi(temp->f_Name(temp), "int") == 0) {
				char * resu = (char*)(str_string->f_get_value(str_string, i));
				obj->f_putInt(obj, i, atoi(resu));
			}
			else if (strcmpi(temp->f_Name(temp), "float") == 0) {
				char * resu = (char*)(str_string->f_get_value(str_string, i));
				obj->f_putFloat(obj, i, atof(resu));
			}
			else if (strcmpi(temp->f_Name(temp), "double") == 0) {
				char * resu = (char*)(str_string->f_get_value(str_string, i));
				obj->f_putDouble(obj, i, strtod(resu, NULL));
			}
			else if (strcmpi(temp->f_Name(temp), "long") == 0) {
				char * resu = (char*)(str_string->f_get_value(str_string, i));
				obj->f_putLong(obj, i, atol(resu));
			}
			else if (strcmpi(temp->f_Name(temp), "string") == 0) {
				char * resu = (char*)(str_string->f_get_value(str_string, i));
				obj->f_putString(obj, i, resu);
			}
			i++;
	}
	str_string->f_free(str_string);
	free(str_string);
	str_string = NULL;

}
// get the char from row ,'out_len; is the chars's length
void getUncharRow(Row * row, char * out_str, unsigned int *out_len){
	if (row->_schema_datatype == NULL)
	{
		printf("The row' s schema should be set first.");
		exit(0);
	}

	int pos = 0, i = 0;;
	while ( row->_schema_datatype->f_hasMore(row->_schema_datatype) ){
		DataType * datatype = (DataType*)(row->_schema_datatype->f_next(row->_schema_datatype));
		if (strcmpi(datatype->_name, "int") == 0){
			int val = row->f_getInt(row, i);
			memcpy(out_str + pos, &val, sizeof(int));
			pos += sizeof(int);
		}
		else if (strcmpi(datatype->_name, "long") == 0){
			long val = row->f_getLong(row, i);
			memcpy(out_str + pos, &val, sizeof(long));
			pos += sizeof(long);
		}
		else if (strcmpi(datatype->_name, "float") == 0){
			float val = row->f_getFloat(row, i);
			memcpy(out_str + pos, &val, sizeof(float));
			pos += sizeof(float);
		}
		else if (strcmpi(datatype->_name, "double") == 0){
			double val = row->f_getDouble(row, i);
			memcpy(out_str + pos, &val, sizeof(double));
			pos += sizeof(double);
		}
		else if (strcmpi(datatype->_name, "string") == 0){
			char* val = row->f_getString(row, i);
			memcpy(out_str + pos, val, strlen(val) + 1);
			pos += strlen(val)+1;
		}
		i++;
	}
	*out_len = pos;
}

void getCharRow(Row *row, char * out_str){
	if (row->_schema_datatype == NULL)
	{
		printf("The row' s schema should be set first.");
		exit(0);
	}
	
	int pos = 0, i = 0;;
	while (row->_schema_datatype->f_hasMore(row->_schema_datatype)){
		DataType * datatype = (DataType*)(row->_schema_datatype->f_next(row->_schema_datatype));
		if (strcmpi(datatype->_name, "string") == 0){
			char* val = row->f_getString(row, i);
			memcpy(out_str + pos, val, strlen(val));
			pos += strlen(val);
			out_str[pos++] = '|';
		}
		else
		{
			char temp[100] = { 0 };
			if (strcmpi(datatype->_name, "int") == 0){
				int val = row->f_getInt(row, i);
				sprintf(temp, "%d", val);
			}
			else if (strcmpi(datatype->_name, "long") == 0){
				long val = row->f_getLong(row, i);
				sprintf(temp, "%ld", val);

			}
			else if (strcmpi(datatype->_name, "float") == 0){
				float val = row->f_getFloat(row, i);
				gcvt(val, 6, temp);

			}
			else if (strcmpi(datatype->_name, "double") == 0){
				double val = row->f_getDouble(row, i);
				gcvt(val, 6, temp);

			}

			memcpy(out_str + pos, temp, strlen(temp));
			pos += strlen(temp);
			out_str[pos++] = '|';
			i++;
		}
	}		
	out_str[pos - 1] = NULL;
}

void Rfree(Row * row){
	row->_schema_datatype->f_free(row->_schema_datatype);
	row->_schema_datatype = NULL;

	for (int i = 0; i < row->_size; i++){
		if (row->_row[i] != NULL){
			free(row->_row[i]);
			row->_row[i] = NULL;
		}		
	}
	free(row->_row);
	row->_row = NULL;

}
void loadFunc(Row * obj){
	obj->f_setSchema = setSchema;

	obj->f_getDouble = getDouble;
	obj->f_putDouble = putDouble;

	obj->f_getInt = getInt;
	obj->f_putInt = putInt;

	obj->f_getLong = getLong;
	obj->f_putLong = putLong;

	obj->f_putString = putString;
	obj->f_getString = getString;

	obj->f_getFloat = getFloat;
	obj->f_putFloat = putFloat;

	obj->updateRow = updateRow;
	obj->updateRow_str = updateRow_str;

	obj->getUncharRow = getUncharRow;
	obj->getCharRow = getCharRow;

	obj->f_free = Rfree;
}