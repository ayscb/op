#include "agg1.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "util.h"
#include "doString.h"
// base
AggFunc * new_AggFunc(int index){
	return new_AggFunc_pro(index, 0);
}
AggFunc * new_AggFunc_pro(int index , int position ){
	AggFunc *aggObj = (AggFunc*)malloc(sizeof(AggFunc));
	if (aggObj == NULL)
		return NULL;
	aggObj->_index = index;
	aggObj->_position = position;
	aggObj->f_set_position = set_position;
	aggObj->f_self_size = NULL;
	aggObj->f_update = NULL;
	return aggObj;
}
void set_position(AggFunc *agg, int position){
	agg->_position = position;
}

//SumLong
AggFunc * new_SumLong(int index){
	
	return new_SumLong_pro(index, 0);
}
AggFunc * new_SumLong_pro(int index, int position){
	SumLong *sumObj = (SumLong*)malloc(sizeof(SumLong));
	if (sumObj == NULL)
		return NULL;
	sumObj->baseFunc = new_AggFunc_pro(index, position);
	if (sumObj->baseFunc == NULL){
		free(sumObj);
		return NULL;
	}
	sumObj->baseFunc->_pDerivedObj = sumObj;
	sumObj->baseFunc->f_update = sum_update;
	sumObj->baseFunc->f_initVal = sum_initVal;
	sumObj->baseFunc->f_self_size = sum_self_size;
	return sumObj->baseFunc;
}
void sum_update(AggFunc *agg, char * cur_val, Row * cur_row){
	long cur_sum, tmp;
	memcpy(&cur_sum, cur_val + agg->_position, sizeof(long));
	tmp = cur_sum + cur_row->f_getLong(cur_row,agg->_index);
	memcpy(cur_val + agg->_position, &tmp, sizeof(long));
}
void sum_initVal(AggFunc *agg, char * cur_val){
	long init_val = 0;
	memcpy(cur_val + agg->_position, &init_val, sizeof(long));
}
int sum_self_size(AggFunc *agg){ return sizeof(long); }



//MaxLong
AggFunc * new_MaxLong(int index){
	return new_MaxLong_pro(index, 0);
}
AggFunc * new_MaxLong_pro(int index, int position){
	MaxLong *Obj = (MaxLong*)malloc(sizeof(MaxLong));
	if (Obj == NULL)
		return NULL;

	Obj->baseFunc = new_AggFunc_pro(index, position);
	if (Obj->baseFunc == NULL){
		free(Obj);
		return NULL;
	}

	Obj->baseFunc->_pDerivedObj = Obj;
	Obj->baseFunc->f_update = max_update;
	Obj->baseFunc->f_initVal = max_initVal;
	Obj->baseFunc->f_self_size = max_self_size;
	return Obj->baseFunc;
}
void max_update(AggFunc *agg, char * out_cur_val, Row * cur_row){
	long cur_max, tmp;
	memcpy(&cur_max, out_cur_val + agg->_position, sizeof(long));
	tmp = cur_row->f_getLong(cur_row,agg->_index);
	if (cur_max < tmp) {
		memcpy(out_cur_val + agg->_position, &tmp, sizeof(long));
	}
}
void max_initVal(AggFunc *agg, char *out_cur_val){
	long init_max_val = LLONG_MIN;
	memcpy(out_cur_val + agg->_position, &init_max_val, sizeof(long));
}
int max_self_size(AggFunc *agg){ return sizeof(long); }


//MinLong
AggFunc * new_MinLong(int index){
	return new_MinLong_pro(index, 0);
}
AggFunc * new_MinLong_pro(int index, int position){
	MinLong *Obj = (MinLong*)malloc(sizeof(MinLong));
	if (Obj == NULL)
		return NULL;

	Obj->baseFunc = new_AggFunc_pro(index, position);
	if (Obj->baseFunc == NULL){
		free(Obj);
		return NULL;
	}

	Obj->baseFunc->_pDerivedObj = Obj;
	Obj->baseFunc->f_update = min_update;
	Obj->baseFunc->f_initVal = min_initVal;
	Obj->baseFunc->f_self_size = min_self_size;
	return Obj->baseFunc;
}
void min_update(AggFunc *agg, char * cur_val, Row * cur_row){
	long cur_min, tmp;
	memcpy(&cur_min, cur_val + agg->_position, sizeof(long));
	tmp = cur_row->f_getLong(cur_row, agg->_index);
	if (cur_min > tmp) {
		memcpy(cur_val + agg->_position, &tmp, sizeof(long));
	}
}
void min_initVal(AggFunc * agg, char * out_cur_val){
	long init_min_val = LONG_MAX;
	memcpy(out_cur_val + agg->_position, &init_min_val, sizeof(long));
}
int min_self_size(AggFunc *agg){ return sizeof(long); }


//AvgLong
AggFunc * new_AvgLong(int index){
	return new_AggFunc_pro(index, 0);
}
AggFunc * new_AvgLong_pro(int index, int position){
	AvgLong *Obj = (AvgLong*)malloc(sizeof(AvgLong));
	if (Obj == NULL)
		return NULL;

	Obj->baseFunc = new_AggFunc_pro(index, position);
	if (Obj->baseFunc == NULL){
		free(Obj);
		return NULL;
	}

	Obj->baseFunc->_pDerivedObj = Obj;
	Obj->baseFunc->f_update = avg_update;
	Obj->baseFunc->f_initVal = avg_initVal;
	Obj->baseFunc->f_self_size = avg_self_size;
	return Obj->baseFunc;
}

void avg_update(AggFunc *agg, char * cur_val, Row * cur_row){
	long cur_sum, tmp;
	int cur_count;
	memcpy(&cur_sum, cur_val + agg->_position, sizeof(long));
	tmp = cur_sum + cur_row->f_getLong(cur_row,agg->_index);
	memcpy(cur_val + agg->_position, &tmp, sizeof(long));

	memcpy(&cur_count, cur_val + agg->_position + sizeof(long), sizeof(int));
	cur_count++;
	memcpy(cur_val + agg->_position + sizeof(long), &cur_count, sizeof(int));
}

void avg_initVal(AggFunc *agg, char *out_cur_val){
	long avg_init_val = 0;
	int avg_init_cot = 0;
	memcpy(out_cur_val + agg->_position, &avg_init_val, sizeof(long));
	memcpy(out_cur_val + agg->_position + sizeof(long),&avg_init_cot, sizeof(int));
}
int avg_self_size(AggFunc *agg){ return sizeof(long) + sizeof(int); }


//CountLong
AggFunc * new_CountLong(int index){
	return new_CountLong_pro(index, 0);
}
AggFunc * new_CountLong_pro(int index, int position){
	CountLong *Obj = (CountLong*)malloc(sizeof(CountLong));
	if (Obj == NULL)
		return NULL;

	Obj->baseFunc = new_AggFunc_pro(index, position);
	if (Obj->baseFunc == NULL){
		free(Obj);
		return NULL;
	}

	Obj->baseFunc->_pDerivedObj = Obj;
	Obj->baseFunc->f_update = cnt_update;
	Obj->baseFunc->f_initVal = cnt_initVal;
	Obj->baseFunc->f_self_size = cnt_self_size;
	return Obj->baseFunc;
}
void cnt_update(AggFunc *agg, char * out_cur_val, Row * cur_row){
	long cur_cnt;
	memcpy(&cur_cnt, out_cur_val + agg->_position, sizeof(long));
	cur_cnt++;
	memcpy(out_cur_val + agg->_position, &cur_cnt, sizeof(long));
}
void cnt_initVal(AggFunc *agg, char *out_cur_val){
	long init_cnt_val = 0;
	memcpy(out_cur_val + agg->_position, &init_cnt_val, sizeof(long));
}
int cnt_self_size(AggFunc *agg){ return sizeof(long); }



// sum(1),avg(2)   
void get_aggfuncs(const char * aggexpr, Vector * schema_dataType, Vector* funcs_aggfunc){

	Vector * str_schema = new_Vector("string");
	split_str(aggexpr, ',', str_schema);		//sum(1)  avg(2)
	
	int current_pos = 0;
	while (str_schema->f_hasMore(str_schema)){
		char * temp = (char*)(str_schema->f_next(str_schema));	// sum(1)
		char ** result = getStringFromSchem(temp);
		char * name = result[0];
		int i = atoi(result[1]);
		AggFunc * aggFunc = get_aggfunc(name, NULL, i);
		aggFunc->f_set_position(aggFunc,current_pos);
		current_pos += aggFunc->f_self_size(aggFunc);

		funcs_aggfunc->f_push_back(funcs_aggfunc,aggFunc);
	}
}
AggFunc* get_aggfunc(const char * func_name, DataType* type, int index){
	if (strcmpi(func_name, "sum") == 0) {
		AggFunc *obj = new_SumLong(index);
		return obj;
	}
	else if (strcmpi(func_name, "avg") == 0) {
		AggFunc * obj = new_AvgLong(index);
		return obj;
	}
	else if (strcmpi(func_name, "max") == 0) {
		AggFunc * obj = new_MaxLong(index);
		return obj;
	}
	else if (strcmpi(func_name, "min") == 0) {
		AggFunc * obj = new_MinLong(index);
		return obj;
	}
	else if (strcmpi(func_name, "count") == 0) {
		AggFunc * obj = new_CountLong(index);
		return obj;
	}
	else {
		return NULL;
	}
}