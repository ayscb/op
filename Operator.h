#ifndef __aggregation__operator__
#define __aggregation__operator__

#include "row.h"
#include "util.h"
#include "agg1.h"
#include "hash\densehashtable.h"
#include "arrayList.h"
#include <stdio.h>

#define _FILE

// test func
char* getSimpleMem(int size_MB);
void getSimple();

typedef struct _Operator Operator;
typedef struct _Operator
{
	void* pDerivedObj;
	void(*f_initialize)(Operator*);
	Row* (*f_next)(Operator*);
	void(*f_close)(Operator*);
	void(*f_children)(Operator*, Vector*);
	void(*f_output_schema)(Operator*, Vector*);
} Operator;


typedef struct LeafOperator
{
	void* pDerivedObj;
	Operator * baseObj;

}LeafOperator;

typedef struct _UnaryOperator
{
	void* pDerivedObj;
	Operator * baseObj;
	Operator * (*f_child)(struct _UnaryOperator*);

}UnaryOperator;

typedef struct _BinaryOperator
{
	void* pDerivedObj;
	Operator * baseObj;
	Operator* (*f_left)( struct _BinaryOperator*);
	Operator* (*f_right)(struct _BinaryOperator*);

}BinaryOperator;

//***************************************************************************************
typedef struct _TextfileScanOperator
{
	LeafOperator * baseObj;
	char * _data_path;
	Vector* _schema_datatype;		// datatype
	Row* _current_row;
	BOOLEAN _isFirst;
#ifdef _FILE
	FILE * _pfile;
#else
	int _memsize;		// total mem size
	char * _pf;			// current mem position
#endif
}TextfileScanOperator;

#ifdef _FILE
Operator* new_TextfileScanOperator(char* data_file, const char * schema_str);
#else
Operator* new_TextfileScanOperator(char* data_file, const char * schema_str, unsigned int mem_size);
#endif

void textfileScanOperator_initialize(Operator* obj);
Row* textfileScanOperator_next(Operator* obj);
void textfileScanOperator_close(Operator* obj);
void textfileScanOperator_children(Operator* obj, Vector* _operator);
void textfileScanOperator_output_schema(Operator* obj, Vector* _datatype);

//***********************************************************************************
typedef struct _AggregateOperator
{
	UnaryOperator * baseObj;
	dense_hashtable _hash_table;
	//相当于 临时变量在堆中创建
	char * _temp_row;					// record(temp) the row from _hash_table 's key + value  ( only for reduce malloc's time )
	Row* _out_row;						// record(temp) thr row from _temp_row ( used by method next )

	Operator * _child;
	Vector * _grouping_exprs_int;  
	Vector * _agg_exprs_aggFunc;
	Vector * _input_schema_datatype;

	void(*f_build_hash_table)( struct _AggregateOperator*);
}AggregateOperator;
Operator * new_AggregateOperator(
				const char * groupingExpression,
				const char *AggregateExpression,
				Operator* child);
void aggregateOperator_initialize(Operator* obj);
Row* aggregateOperator_next(Operator* obj);
void aggregateOperator_close(Operator* obj);
void aggregateOperator_children(Operator* obj, Vector* _operator);
void aggregateOperator_output_schema(Operator* obj, Vector* _datatype);
void build_hash_table(Operator * obj);

//**************************************************************************************
// flterOperator
typedef struct _FilterOperator
{
	LeafOperator * _baseObj;
	char * _filter_str;		// 0 > 23 and 2<33 or 3 like 'sdf' and 4 like "sdf"
	Operator *_child;

	//Vector _and_vector_str;			// and , or
	//Vector _filter_vector_str;		// a>1 , b<2

}FilterOperator;
Operator * new_filterOperator(const char * filter_str, Operator *child);
void filterOperator_init(Operator *obj);
Row* filterOperator_next(Operator *obj);
void filterOperator_close(Operator* obj);
void filterOperator_children(Operator* obj, Vector* _operator);
void filterOperator_output_schema(Operator* obj, Vector* _datatype);

//************************************************************************************************
// private func
BOOLEAN getFilterRow(const char* str_filter, Row *currRow);
int scanOp(const char const * filter_str, int in_start_pos, char *out_str, char* out_schema);
BOOLEAN checkSubOp(char *str, Row *row);
BOOLEAN getSplitStr(char * in_src, int * out_clm_id, char * out_str_result, char * out_sign);

// sortOperator ( order by )	ORDER BY  f1 (ASC), f2  DESC
typedef struct _SortOperator{
	LeafOperator * _baseObj;
	char * _sort_str;				// store 1 asc, 3 desc......		
	Operator *_child;
	Row *row;						// store current row
	ValueArray _table;				// store the  whole data ( array list)
	Vector *_sort_v_i_clmId;		// store 3,5,... as column id
	Vector *_sort_v_i_clmAE;		// store ASC(sheng) / DESC (ASC=1,DESC=0)
	Vector *_sort_v_i_clmIdLen;		// store the columnId 's length (int:4)
}SortOperator;

Operator* new_sortOperator(const char* sort_str, Operator*child);
void initSortOp(Operator *Obj);
Row* sortOperator_next(Operator *Obj);
void sortOPerator_close(Operator * Obj);
void sortOperator_children(Operator *Obj, Vector* _operator);
void sortOperator_output_schema(Operator *obj, Vector * _out_schema_v);

int compare(const void * _key1, const void * _key2, Vector clmAE, Vector clmIdLen);

void initCurrRow(SortOperator *sortObj);
void buildTable(SortOperator *sortObj);
void computeVectors(SortOperator *sortObj);
void getVectory(char *sub_str, Vector *vec_clmid, Vector *vec_clmAE);
void getVectory_len(Vector * datatype, Vector* vec_clm, Vector * vec_clmLen);
#ifdef N
value_type getValuesFromRow(Row * row, actualData *acta, Vector clmids);
#else
value_type getValuesFromRow(Row * row, Vector clmids);
#endif
#endif /* defined(__aggregation__operator__) */
