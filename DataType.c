#include "DataType.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

DataType* new_DataType(const char* name, int width){
	DataType * pObj = (DataType*)malloc(sizeof(DataType));
	if (pObj == NULL)
		return NULL;
	pObj->_pDerivedObj = NULL;

	pObj->_name = (char*)malloc(sizeof(char)*strlen(name) + 1);
	if (pObj->_name == NULL)
		return NULL;
	strcpy(pObj->_name, name);
	pObj->_width = width;

	// initializing interface for access to function
	pObj->f_delete_DataType = delete_DataType;
	pObj->f_toString = DataType_ToString;
	pObj->f_Name = DataType_Name;
	pObj->f_Width = DataType_width;
	pObj->f_getValue = NULL;
	return pObj;
}
void delete_DataType( DataType * const dataTypeObj){
	if (dataTypeObj != NULL)
		free(dataTypeObj);
}
char* DataType_ToString(DataType * const dataTypeObj){
	if (dataTypeObj == NULL)
		return NULL;
	char res[20];
	strcpy(res, dataTypeObj->_name);
	char wid[10];
	itoa(dataTypeObj->_width, wid, 10);
	strcpy(res + strlen(res), "\t");
	strcpy(res + strlen(res), wid);
	return res;
}
char* DataType_Name(DataType * const dataTypeObj){
	if (dataTypeObj == NULL)
		return NULL;
	return dataTypeObj->_name;
}
int DataType_width(DataType *const dataTypeObj){
	if (dataTypeObj == NULL)
		return NULL;
	return dataTypeObj->_width;
}

//***********************************************//
static DataType *intObj = NULL;
DataType* new_IntType(){
	if (intObj == NULL){
		DataType *dataBase = new_DataType("int", sizeof(int));
		if (dataBase == NULL){
			return NULL;
		}

		IntType *intObjs = (IntType*)malloc(sizeof(IntType));
		if (intObjs == NULL){
			dataBase->f_delete_DataType(dataBase);
			return NULL;
		}

		dataBase->_pDerivedObj = intObjs;
		intObjs->baseDataType = dataBase;
				
		dataBase->f_delete_DataType = delete_IntType;
		dataBase->f_getValue = getIntValue;
		intObj = intObjs->baseDataType;
	}
	return intObj;
}
void delete_IntType(DataType * const intTypeObj){
	if (intObj == NULL)
		return;
	IntType * temp = (IntType*)(intObj->_pDerivedObj);
	free(intObj);
	free(temp);
	intObj = NULL;
	temp = NULL;
}

void *getIntValue(DataType* obj, Row* row, int index){
	int value = row->f_getInt(row, index);
	return &value;
}



//*******************************************************
static DataType *longObj = NULL;
DataType * new_LongType(){
	if (longObj == NULL){
		DataType *dataBase = new_DataType("long", sizeof(long));
		if (dataBase == NULL){
			return NULL;
		}

		LongType *newlongObj = (LongType*)malloc(sizeof(LongType));
		if (newlongObj == NULL){
			dataBase->f_delete_DataType(dataBase);
			return NULL;
		}
			
		dataBase->_pDerivedObj = newlongObj;
		newlongObj->baseDataType = dataBase;

		dataBase->f_delete_DataType = delete_LongType;
		dataBase->f_getValue = getLongValue;

		longObj = newlongObj->baseDataType;
	}
	return longObj;
}
void delete_LongType(DataType * const longTypeObj){
	if (longObj == NULL)
		return;
	LongType * temp = (LongType*)longObj->_pDerivedObj;
	free(longObj);
	free(temp);
	temp = NULL;
	longObj = NULL;
}
void *getLongValue(DataType*, Row* row, int index){
	long value = row->f_getLong(row, index);
	return &value;
}

//*******************************************************
static DataType *floatObj = NULL;
DataType * new_FloatType(){
	if (floatObj == NULL){
		DataType *dataBase = new_DataType("float", sizeof(float));
		if (dataBase == NULL){
			return NULL;
		}

		FloatType * newfloatObj = (FloatType*)malloc(sizeof(FloatType));
		if (newfloatObj == NULL){
			dataBase->f_delete_DataType(dataBase);
			return NULL;
		}
		dataBase->_pDerivedObj = newfloatObj;
		newfloatObj->baseDataType = dataBase;

		dataBase->f_delete_DataType = delete_FloatType;
		dataBase->f_getValue = getFloatValue;

		floatObj = newfloatObj->baseDataType;
	}
	return floatObj;
}
void delete_FloatType(DataType * const floatTypeObj){
	if (floatObj == NULL)
		return;
	DoubleType * temp = (DoubleType*)(floatObj->_pDerivedObj);
	free(floatObj);
	free(temp);
	floatObj = NULL;
	temp = NULL;
}
void *getFloatValue(DataType*, Row* row, int index){
	float value = row->f_getFloat(row, index);
	return &value;
}

//*******************************************************
static DataType *doubleObj = NULL;
DataType * new_DoubleType(){
	if (doubleObj == NULL){
		DataType *dataBase = new_DataType("double", sizeof(double));
		if (dataBase == NULL){
			return NULL;
		}

		DoubleType * newdoubleObj = (DoubleType*)malloc(sizeof(DoubleType));
		if (newdoubleObj == NULL){
			dataBase->f_delete_DataType(dataBase);
			return NULL;
		}
		dataBase->_pDerivedObj = newdoubleObj;
		newdoubleObj->baseDataType = dataBase;

		dataBase->f_delete_DataType = delete_DoubleType;
		dataBase->f_getValue = getDoubleValue;
		doubleObj = newdoubleObj->baseDataType;
	}
	return doubleObj;
}
void delete_DoubleType(DataType * const doubleTypeObj){
	if (doubleObj == NULL)
		return;
	DoubleType * temp = (DoubleType*)(doubleObj->_pDerivedObj);
	free(doubleObj);
	free(temp);
	doubleObj = NULL;
	temp = NULL;
}
void *getDoubleValue(DataType*, Row* row, int index){
	double value = row->f_getDouble(row, index);
	return &value;
}

//***********************************************************
static DataType *stringObj = NULL;
DataType * new_StringType(){
	if (stringObj == NULL){
		DataType *dataBase = new_DataType("string", 0);
		if (dataBase == NULL){
			return NULL;
		}

		StringType *newstringObj = (StringType*)malloc(sizeof(StringType));
		if (newstringObj == NULL){
			dataBase->f_delete_DataType(dataBase);
			return NULL;
		}
		dataBase->_pDerivedObj = newstringObj;
		newstringObj->baseDataType = dataBase;

		dataBase->f_delete_DataType = delete_StringType;
		dataBase->f_getValue = getStringValue;
		stringObj = dataBase;
	}
	return stringObj;
}
void delete_StringType(DataType * stringTypeObj){
	if (stringObj == NULL)
		return;
	StringType * temp = (StringType*)(stringObj->_pDerivedObj);
	free(stringObj);
	free(temp);
	temp = NULL;
	stringObj = NULL;

}
void *getStringValue(DataType*, Row* row, int index){
	char* value = row->f_getString(row, index);
	return value;
}
//***********************************************************
static DataType *unknowObj = NULL;
DataType * new_UnknowType(){
	if (unknowObj == NULL){
		DataType *dataBase = new_DataType("unknow", -1);
		if (dataBase == NULL) return NULL;

		UnknowType* newunknowObj = (UnknowType*)malloc(sizeof(UnknowType));
		if (newunknowObj == NULL){
			dataBase->f_delete_DataType(dataBase);
			return NULL;
		}
		dataBase->_pDerivedObj = newunknowObj;
		newunknowObj->baseDataType = dataBase;

		dataBase->f_delete_DataType = delete_UnknowType;

		unknowObj = newunknowObj->baseDataType;
	}
	return unknowObj;
}
void delete_UnknowType(DataType * unknowObj){
	if (unknowObj == NULL)
		return;
	UnknowType * tem = (UnknowType*)(unknowObj->_pDerivedObj);
	free(unknowObj);
	free(tem);
	tem = NULL;
	unknowObj = NULL;
}