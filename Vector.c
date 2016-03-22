#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Vector.h"
#include "agg1.h"

int reallocSize(Vector *vector){
	vector->_base =
		(char*)realloc(vector->_base, sizeof(char*)*(vector->_capality + INCREASE_SIZE));
	if (vector->_base == NULL)
		return FALSE;
	vector->_capality += INCREASE_SIZE;
	return TRUE;
}

int checkSpace(Vector *vector, int typelength){
	if (vector->_index + typelength > vector->_capality)
		if (reallocSize(vector) == FALSE)
			return FALSE;
	return TRUE;
}

Vector* new_Vector(const char * _typename){
	Vector * vectObj = (Vector*)malloc(sizeof(Vector));
	if (vectObj == NULL)
		return NULL;

	strcpy(vectObj->_type, _typename);

	vectObj->_base = (char*)malloc(sizeof(char) * DEFALUT_SIZE);
	if (vectObj->_base == NULL){
		free(vectObj);
		return NULL;
	}

	if (strcmpi(_typename, "string") == 0){		
		memset(vectObj->_stringPoint, 0, sizeof(int) * 100);
	}

	vectObj->_Num = 0;
	vectObj->_index = 0;
	vectObj->_limit = vectObj->_capality = DEFALUT_SIZE;

	vectObj->f_push_back = push_bace;
	vectObj->f_empty = empty;
	vectObj->f_get_value = get_value;
	vectObj->f_size = size;
	vectObj->f_hasMore = hasMore;
	vectObj->f_next = next;
	vectObj->f_free = f_free;
}

void push_bace(Vector *vector, const void * data){
	if (vector->_limit != vector->_capality){
		vector->_index = vector->_limit;
		vector->_limit = vector->_capality;
	}
	char *p = NULL;
	p = vector->_base;
	p += vector->_index;
	
	if (strcmpi(vector->_type, "int") == 0){
		if (checkSpace(vector, sizeof(int)) == FALSE)
			return;
		memcpy(p, (int*)data, sizeof(int));
		vector->_index += sizeof(int);
	}
	else if (strcmpi(vector->_type, "long") == 0){
		if (checkSpace(vector, sizeof(long)) == FALSE)
			return;
		memcpy(p, (long*)data, sizeof(long));
		vector->_index += sizeof(long);
	}
	else if (strcmpi(vector->_type, "string") == 0){
		int strLen = strlen((char*)data) + 1;
		if (checkSpace(vector, strLen) == FALSE )
			return;
		// for the first record this point (0)
		if (vector->_index == 0 && vector->_Num == 0){
			vector->_stringPoint[vector->_Num] = vector->_index;
		}		
		memcpy(p, (char*)data, strLen);
		vector->_index += strLen;
		// record the next index
		vector->_stringPoint[vector->_Num + 1] = vector->_index;
	}
	else if (strcmpi(vector->_type, "DataType") == 0){
		if (checkSpace(vector, sizeof(DataType)) == FALSE)
			return;
		memcpy(p, (DataType*)data, sizeof(DataType));
		vector->_index += sizeof(DataType);
	}
	else if (strcmpi(vector->_type, "AggFunc") == 0){
		if (checkSpace(vector, sizeof(DataType)) == FALSE)
			return;
		memcpy(p, (AggFunc*)data, sizeof(AggFunc));
		vector->_index += sizeof(AggFunc);
	}
	vector->_Num++;
}

int hasMore( Vector *vector){
	// change the mode to read ( like buffer)
	if (vector->_limit == vector->_capality){
		vector->_limit = vector->_index;
		vector->_index = 0;
		vector->_stringCurrIndex = 0;	
	}
	// only for string
	if (strcmpi(vector->_type, "string") == 0){
		if (vector->_stringCurrIndex == vector->_Num){
			vector->_limit = vector->_capality;
			vector->_index = vector->_stringPoint[vector->_Num];
			return FALSE;
		}
			
	}
	// read over ,so return null( expect for string)
	if (vector->_index == vector->_limit){
		vector->_limit = vector->_capality;
		return FALSE;
	}		
	else
		return TRUE;
}

void * next(Vector *vector){	
	
	//read the data and move the index
	//char * p = vector->_base + vector->_index;
	char * p = vector->_base;

	// caculate the next index
	if (strcmpi(vector->_type, "int") == 0){
		p += vector->_index;
		vector->_index += sizeof(int);	
	}
	else if (strcmpi(vector->_type, "long") == 0){
		p += vector->_index;
		vector->_index += sizeof(long);
	}
	else if (strcmpi(vector->_type, "string") == 0){
		p += vector->_stringPoint[vector->_stringCurrIndex++];
	}
	else if (strcmpi(vector->_type, "datatype") == 0){
		p += vector->_index;
		vector->_index += sizeof(DataType);
	}
	else if (strcmpi(vector->_type, "AggFunc") == 0){
		p += vector->_index;
		vector->_index += sizeof(AggFunc);
	}

	return p;
}

// 获取下标的值
void * get_value(const Vector *vector, int index){
	char *p = vector->_base;
	if (strcmpi(vector->_type, "int") == 0){
		p += index * sizeof(int);
	}
	else if (strcmpi(vector->_type, "long") == 0){
		p += index * sizeof(int);
	}
	else if (strcmpi(vector->_type, "string") == 0){
		p += vector->_stringPoint[index];
	}
	else if (strcmpi(vector->_type, "datatype") == 0){
		p += index * sizeof(DataType);
	}
	else if (strcmpi(vector->_type, "AggFunc") == 0){
		p += index * sizeof(AggFunc);
	}
	return p;
}

int empty(const Vector *vector){
	return vector->_index == 0 ? TRUE : FALSE;
}

int size(const Vector* vector){
	return vector->_Num;
}

void f_free(Vector * vector){
	if (vector->_base != NULL)
	{
		free(vector->_base);
		vector->_base = NULL;
	}
}

