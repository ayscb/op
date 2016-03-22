#ifndef __aggregation__DataType__
#define __aggregation__DataType__

#include "Row.h"
/*
* Method :CaseCase
* property : caseCase
*/

typedef struct _DataType DataType;
struct _DataType
{
	void * _pDerivedObj;
	char *_name;
	int _width;

	// function   method caseCase  
	void(*f_delete_DataType)(DataType *);
	char* (*f_toString)(DataType *);
	char* (*f_Name)(DataType *);
	int(*f_Width)(DataType *);
	void * (*f_getValue)(DataType*, Row* row, int index);

};

DataType* new_DataType(const char* name, int width);
void delete_DataType(DataType * const dataTypeObj);
char* DataType_ToString(DataType * const dataTypeObj);
char* DataType_Name(DataType * const dataTypeObj);
int DataType_width(DataType *const dataTypeObj);

// class IntType
typedef struct _IntType IntType;
struct _IntType
{
	DataType *baseDataType;
};

DataType* new_IntType();
void delete_IntType(DataType * intTypeObj);
void *getIntValue(DataType*, Row* row, int index);

// class LongType
typedef struct _LongType LongType;
struct _LongType {
	DataType *baseDataType;
};

DataType * new_LongType();
void delete_LongType(DataType * longTypeObj);
void *getLongValue(DataType*, Row* row, int index);

// class FloatType
typedef struct _FloatType FloatType;
struct _FloatType {
	DataType *baseDataType;
};

DataType * new_FloatType();
void delete_FloatType(DataType * floatTypeObj);
void *getFloatValue(DataType*, Row* row, int index);

// class DoubleType
typedef struct _DoubleType DoubleType;
struct _DoubleType {
	DataType *baseDataType;
};

DataType * new_DoubleType();
void delete_DoubleType(DataType * doubleTypeObj);
void *getDoubleValue(DataType*, Row* row, int index);


//class string
typedef struct _StringType StringType;
typedef struct _StringType{
	DataType *baseDataType;
} StringType;

DataType * new_StringType();
void delete_StringType(DataType * doubleTypeObj);
void *getStringValue(DataType*, Row* row, int index);

//class unknowType
typedef struct _UnknowType UnknowType;
typedef struct _UnknowType{
	DataType *baseDataType;
} UnknowType;

DataType * new_UnknowType();
void delete_UnknowType(DataType * doubleTypeObj);

#endif