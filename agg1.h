#ifndef __aggregation__agg1__
#define __aggregation__agg1__
#include "Vector.h"
#include "DataType.h"
#include "Row.h"

typedef struct _AggFunc
{
	int _position = 0;	//ptr pos in char array
	int _index = 0;		//column index

	void * _pDerivedObj;
	void (*f_set_position)(struct _AggFunc* obj, int position);
	void(*f_initVal)(struct _AggFunc* obj, char * curr_val);
	void (*f_update)(struct _AggFunc* obj, char * curr_val, Row * curr_row);	// curr_val 更新值，curr_row 一条记录
	int  (*f_self_size)(struct _AggFunc*);
}AggFunc;

AggFunc * new_AggFunc(int index);
AggFunc * new_AggFunc_pro(int index, int position);
void set_position(AggFunc* agg, int position);

//SumLong
typedef	struct _SumLong
{
	AggFunc * baseFunc;
} SumLong;
AggFunc * new_SumLong(int);
AggFunc * new_SumLong_pro(int index, int position);
void sum_initVal(AggFunc *agg, char * cur_val);
void sum_update(AggFunc* agg, char * curr_, Row *);
int sum_self_size(AggFunc* agg);

//MaxLong
typedef	struct _MaxLong
{
	AggFunc * baseFunc;
} MaxLong;
AggFunc * new_MaxLong(int);
AggFunc * new_MaxLong_pro(int index, int position);
void max_initVal(AggFunc *agg, char *out_cur_val);
void max_update(AggFunc* agg, char *, Row *);
int max_self_size(AggFunc* agg);

//MinLong
typedef	struct _MinLong
{
	AggFunc * baseFunc;
} MinLong;
AggFunc * new_MinLong(int);
AggFunc * new_MinLong_pro(int index, int position);
void min_initVal(AggFunc * agg, char * out_cur_val);
void min_update(AggFunc* agg, char *, Row *);
int min_self_size(AggFunc* agg);

//AvgLong
typedef	struct _AvgLong
{
	AggFunc * baseFunc;
} AvgLong;
AggFunc * new_AvgLong(int);
AggFunc * new_AvgLong_pro(int index, int position);
void avg_initVal(AggFunc *agg, char *out_cur_val);
void avg_update(AggFunc* agg, char *, Row *);
int avg_self_size(AggFunc* agg);

//CountLong
typedef	struct _CountLong
{
	AggFunc * baseFunc;
} CountLong;
AggFunc * new_CountLong(int);
AggFunc * new_CountLong_pro(int index, int position);
void cnt_initVal(AggFunc *agg, char *out_cur_val);
void cnt_update(AggFunc* agg, char *, Row *);
int cnt_self_size(AggFunc* agg);


// vector<DataType>  vector<AggFunc>
void get_aggfuncs(const char * str, Vector * dataType, Vector* aggFunc);
AggFunc* get_aggfunc(const char *, DataType*, int index);

#endif