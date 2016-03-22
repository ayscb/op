#include "Operator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "util.h"
#include "doString.h"
#include "qusort.h"
#define NDEBUG    // for close assert.h
#define _ECHO
#include <assert.h>

long total_ele_size = 0;
long total_file_size = 0;

void getSimple()
{
	char * schema = "int|int|long|string";
	char * file_name = "d:\\data1.txt";
	int idcount = 10000000;

	FILE *pf = NULL;
	pf = fopen(file_name, "a+");

	int id = 0, cid = 0, pos = 0;
	long sum = 0;
	char te[100];
	memset(te, 0, 100);
	srand((unsigned)time(NULL));
	while (id != idcount){
		sprintf(te, "%d", id);
		pos = strlen(te);
		te[pos++] = '|';

		sprintf(te + pos, "%d", rand() % 30);
		pos = strlen(te);
		te[pos++] = '|';

		sprintf(te + pos, "%ld", rand() % 80);
		pos = strlen(te);
		te[pos++] = '|';

		char id_str[10];
		itoa(id, id_str, 10);

		char string_str[20] = { 0 };
		strcat(string_str, "test:");
		strcat(string_str, id_str);

		sprintf(te + pos, "%s", string_str);
		pos = strlen(te);
		te[pos++] = '\0';
		//fputs(te, pf);
		fprintf(pf, "%s\n", te);
		id++;
	}
	fclose(pf);
}

char* getSimpleMem(int size_MB){
	char * schema = "int|int|long|string";

	size_MB = size_MB ;
	char * fileMem = (char*)malloc(size_MB);
	memset(fileMem, 0, size_MB);
	char * p = fileMem;

	int id = 0;
	srand((unsigned)time(NULL));
	while ((p - fileMem) + 100 < size_MB){
	//int|int|long|string

	char id_str[10];
	itoa(id, id_str, 10);

	char string_str[20] = { 0 };
	strcat(string_str, "test:");
	strcat(string_str, id_str);

	sprintf(p, "%d|%d|%ld|%s\0", id, rand() % 100, rand() % 500, string_str);
	int len = strlen(p);
	p += len + 1;
	id++;
}
return fileMem;
}

#ifdef _FILE

Operator* new_TextfileScanOperator(char* data_file, const char * schema_str){
#else
Operator* new_TextfileScanOperator(char* data_file, const char * schema_str, unsigned int mem_size){
#endif
	Operator *baseOp = (Operator*)malloc(sizeof(Operator));
	if (baseOp == NULL){
		return NULL;
	}

	LeafOperator *leftOp = (LeafOperator*)malloc(sizeof(LeafOperator));
	if (leftOp == NULL){
		free(baseOp);
		return NULL;
	}
	TextfileScanOperator * obj = (TextfileScanOperator*)malloc(sizeof(TextfileScanOperator));
	if (obj == NULL){
		free(baseOp);
		free(leftOp);
		return NULL;
	}
	baseOp->pDerivedObj = leftOp;
	leftOp->baseObj = baseOp;
	leftOp->pDerivedObj = obj;
	obj->baseObj = leftOp;
	obj->_isFirst = TRUE;
#ifdef _FILE
	obj->_data_path = strdup(data_file);
#else
	obj->_data_path = data_file;
	obj->_memsize = mem_size;
	total_file_size = mem_size;
#endif
	obj->_schema_datatype = new_Vector("DataType");
	get_schema(schema_str, obj->_schema_datatype);
	obj->_current_row = new_Row_from_schema(obj->_schema_datatype);
#ifdef _FILE
	obj->_pfile = NULL;
#endif
	baseOp->f_children = textfileScanOperator_children;
	baseOp->f_close = textfileScanOperator_close;
	baseOp->f_initialize = textfileScanOperator_initialize;
	baseOp->f_next = textfileScanOperator_next;
	baseOp->f_output_schema = textfileScanOperator_output_schema;
	return baseOp;
}

void textfileScanOperator_initialize(Operator* obj){
	LeafOperator* lef = (LeafOperator*)(obj->pDerivedObj);
	TextfileScanOperator * teObj = (TextfileScanOperator *)lef->pDerivedObj;
#ifdef _FILE
	teObj->_pfile = fopen(teObj->_data_path, "r");
	if (teObj->_pfile == NULL){
		printf("the file '%s ' can not open! please check again!\n", teObj->_data_path);
		system("pause");
		exit(1);
	}
	// evaluate the total_file_size
	fseek(teObj->_pfile,0,SEEK_END);
	total_file_size = ftell(teObj->_pfile);
	fseek(teObj->_pfile, 0, SEEK_SET);

	// evaluate the total_element_size
	char T[1024];
	fgets(T, 1000, teObj->_pfile);
	total_ele_size = total_file_size / strlen(T);
	fseek(teObj->_pfile, 0, SEEK_SET);

#else
	teObj->_pf = teObj->_data_path;
	total_ele_size = total_file_size / strlen(teObj->_pf);
#endif
}

Row* textfileScanOperator_next(Operator* obj){
	LeafOperator* lef = (LeafOperator*)(obj->pDerivedObj);
	TextfileScanOperator * teObj = (TextfileScanOperator *)lef->pDerivedObj;
	char  curr_line[1000] = { 0 };
	
#ifdef _FILE
	if (teObj->_pfile == NULL)	return NULL;
		
	if (fgets(curr_line, 1000, teObj->_pfile) != NULL)
	{
		if (teObj->_isFirst == TRUE){
			// the first init the file size
			total_ele_size = total_file_size / strlen(curr_line);
			teObj->_isFirst = FALSE;
		}
		teObj->_current_row->updateRow_str(teObj->_current_row, curr_line);
		return teObj->_current_row;
	}
	else{
		FILE *rp = fopen("d://data1.txt", "a");

		fprintf(rp,"data2.txt's size is :%u", total_ele_size);
		fclose(rp);
		rp =NULL;
		return NULL;
	}
#else
	if (teObj->_data_path == NULL || teObj->_pf == NULL)
		exit(0);
	if ((teObj->_pf - teObj->_data_path) + 100 < teObj->_memsize)
	{
		strcpy(curr_line, teObj->_pf);
		teObj->_current_row->updateRow_str(teObj->_current_row, curr_line);
		teObj->_pf += strlen(curr_line) + 1;
		return teObj->_current_row;
	}
	else
	{
		return NULL;
	}
#endif
}

void textfileScanOperator_close(Operator* obj){
	LeafOperator* lef = (LeafOperator*)(obj->pDerivedObj);
	TextfileScanOperator * teObj = (TextfileScanOperator *)lef->pDerivedObj;
#ifdef _FILE
	free(teObj->_data_path);
	if (teObj->_pfile != NULL){
		fclose(teObj->_pfile);
		teObj->_pfile = NULL;
	}
#else
	free(teObj->_data_path);
	teObj->_data_path = NULL;
	teObj->_pf = NULL;
	teObj->_memsize = 0;
#endif
	teObj->_schema_datatype->f_free(teObj->_schema_datatype);
	free(teObj->_schema_datatype);
	teObj->_schema_datatype = NULL;

	teObj->_current_row->f_free(teObj->_current_row);
	free(teObj->_current_row);
	teObj->_current_row = NULL;

	// free the textfile
	free(teObj);
	free(lef);
	free(obj);
}

void textfileScanOperator_children(Operator* obj, Vector* _operator){}

void textfileScanOperator_output_schema(Operator* obj, Vector* schema_out_datatype){
	LeafOperator* lef = (LeafOperator*)(obj->pDerivedObj);
	TextfileScanOperator * teObj = (TextfileScanOperator *)lef->pDerivedObj;

	for (int i = 0; i < teObj->_schema_datatype->f_size(teObj->_schema_datatype); i++){
		Vector * schma_datatype_vector = teObj->_schema_datatype;
		DataType * temp = (DataType*)(schma_datatype_vector->f_get_value(schma_datatype_vector, i));
		schema_out_datatype->f_push_back(schema_out_datatype, temp);
	}
}

//*****************************************************************************************************

Operator * new_AggregateOperator
							(const char * groupingExpression,	//"1,2"
							 const char * AggregateExpression,	// "sum(1),avg(2)"
							 Operator* child){
	Operator *baseOp = (Operator*)malloc(sizeof(Operator));
	if (baseOp == NULL){
		return NULL;
	}

	UnaryOperator *UnaryOp = (UnaryOperator*)malloc(sizeof(UnaryOperator));
	if (UnaryOp == NULL){
		free(baseOp);
		return NULL;
	}

	AggregateOperator * obj = (AggregateOperator*)malloc(sizeof(AggregateOperator));
	if (obj == NULL){
		free(baseOp);
		free(UnaryOp);
		return NULL;
	}

	baseOp->pDerivedObj = UnaryOp;
	UnaryOp->baseObj = baseOp;
	UnaryOp->pDerivedObj = obj;
	obj->baseObj = UnaryOp;

	obj->_temp_row = NULL; 
	obj->_out_row = NULL;

	obj->_child = child;
	obj->_input_schema_datatype = new_Vector("datatype");
	child->f_output_schema(child, obj->_input_schema_datatype);

	// get the group by column id
	obj->_grouping_exprs_int = new_Vector("int");
	split_str(groupingExpression, ',', obj->_grouping_exprs_int);

	// get the aggfunc
	obj->_agg_exprs_aggFunc = new_Vector("aggFunc");
	get_aggfuncs(AggregateExpression, obj->_input_schema_datatype, obj->_agg_exprs_aggFunc);
		

	baseOp->f_initialize = aggregateOperator_initialize;
	baseOp->f_next = aggregateOperator_next;
	baseOp->f_output_schema = aggregateOperator_output_schema;
	baseOp->f_children = aggregateOperator_children;
	baseOp->f_close = aggregateOperator_close;

	//init out_row for next method 
	// 放在这里而不放在next里面是为了减少反复调用next方法而一直初始化这个_out_row
	Vector* out_v_datatype = new_Vector("datatype");
	baseOp->f_output_schema(baseOp, out_v_datatype);
	obj->_out_row = new_Row_from_schema(out_v_datatype);

	return baseOp;
}

void aggregateOperator_initialize(Operator* obj){
	UnaryOperator *unaryObj = (UnaryOperator*)obj->pDerivedObj;
	AggregateOperator *aggObj = (AggregateOperator*)unaryObj->pDerivedObj;
	aggObj->_child->f_initialize(aggObj->_child);

	// init hash table

#ifdef N
	// construct the hash map  0.7 = load factor
	Settings st = new_settings(NULL, 0.7, 0.2);

	// because the hash load factor is 0.7, we believe that the real tatal element number may be lass than 0.5
	aggObj->_hash_table = new_dense_hashtable(total_ele_size * 0.5, st);	

	// for new method we should malloc the space to place the real data
	aggObj->_hash_table._acData = new_actualData(total_file_size * 0.3, total_file_size * 0.5);

#else
	// construct the hash map  0.7 = load factor
	Settings st = new_settings(NULL, 0.7, 0.2);
	// because the hash load factor is 0.7, we believe that the real tatal element number may be lass than 0.5
	aggObj->_hash_table = new_dense_hashtable(total_ele_size * 0.5, st);
#endif

	// init the empty data
	value_type empty;
	empty.key = NULL;
	empty.key_len = -1;
	empty.value = NULL;
	empty.val_len = -1;
	aggObj->_hash_table.set_empty_key(&aggObj->_hash_table, &empty);

	int start = clock();
	build_hash_table(obj);		

#ifdef _ECHO
	FILE *pf = fopen("d://res1.txt", "a");
	fprintf(pf, "agg Build HashTable's time : %d\n", clock() - start);
	fclose(pf);
#endif

	aggObj->_child->f_close(aggObj->_child);
}

void build_hash_table(Operator* obj){
	UnaryOperator *unaryObj = (UnaryOperator*)obj->pDerivedObj;
	AggregateOperator *aggObj = (AggregateOperator*)unaryObj->pDerivedObj;

	Row *curr_row;
	char key_tmp[100] = { 0 };		//每一位不固定
	char val_tmp[100] = { 0 };		//每一位固定为8B
	int key_len = 0;

	while ((curr_row = aggObj->_child->f_next(aggObj->_child)) != NULL){
	
		//combine key
		if (getCombineChar(curr_row, *aggObj->_grouping_exprs_int, key_tmp, &key_len) == NULL){
			exit(-1);
		}

		// combine value  		
		Vector * agg_fun = aggObj->_agg_exprs_aggFunc;

		// the value length is decided by aggObj , however it is a fixed length.
		if (aggObj->_hash_table._val_len == 0){
			while (agg_fun->f_hasMore(agg_fun))
			{
				AggFunc  *aggfunc = (AggFunc*)agg_fun->f_next(agg_fun);
				aggObj->_hash_table._val_len += aggfunc->f_self_size(aggfunc);
			}
		}
		
		value_type key_val;
		// 获取value值
		position pos = aggObj->_hash_table.find_pos(&aggObj->_hash_table, key_tmp, key_len);
		
		// if the key is exist ,get it
		if (pos.first != ILLEGAL_BUCKET){
			key_val = *((value_type*)aggObj->_hash_table._table + pos.first);
			memcpy(val_tmp, key_val.value, aggObj->_hash_table._val_len);
		}
		else
		{
			// init the val (because echo agg fun needs different initial value)
			while (agg_fun->f_hasMore(agg_fun)){
				AggFunc * curr_agg = (AggFunc*)agg_fun->f_next(agg_fun);
				curr_agg->f_initVal(curr_agg, val_tmp);
			}
		}

		// 将读出的row 与 value值 进行计算
		while (agg_fun->f_hasMore(agg_fun)){
			AggFunc * curr_agg = (AggFunc*)agg_fun->f_next(agg_fun);
			curr_agg->f_update(curr_agg, val_tmp, curr_row);
		}

		if (pos.first != ILLEGAL_BUCKET){
			// if exist we only update the value
			memcpy(key_val.value, val_tmp, aggObj->_hash_table._val_len);
		}
		else
		{
			
#ifdef N
			key_val = get_value_from_ad(&aggObj->_hash_table._acData, key_tmp, val_tmp, key_len, aggObj->_hash_table._val_len);
			insert_at(&aggObj->_hash_table, &key_val, pos.second);
#else
			key_val->key_len = key_len;
			key_val->key = (char*)malloc(key_len);
			memcpy(key_val, key_tmp, key_len);
			key_val->value = (char*)malloc(aggObj->_hash_table._val_len);
			key_val->val_len = aggObj->_hash_table._val_len;
			insert_at(&aggObj->_hash_table, &key_val, pos.second);
#endif
		}
		memset(key_tmp,0,100);
		memset(val_tmp,0,100);
	}
}

Row* aggregateOperator_next(Operator* obj){ 
	
	UnaryOperator *unaryObj = (UnaryOperator*)(obj->pDerivedObj);
	AggregateOperator *aggObj = (AggregateOperator*)(unaryObj->pDerivedObj);

	dense_hashtable *hash = &aggObj->_hash_table;

	// malloc the temp space
	if (aggObj->_temp_row == NULL){
		aggObj->_temp_row = (char*)malloc(1024);
		memset(aggObj->_temp_row, 0, 1024);
	}

	// 保证每一次调用next函数都是返回下一条数据 (string)
	dense_ht_iterator iter = hash->get_single_ht_iterator(hash);	
	if (iter.hase_more(&iter))
	{
		value_type kv = *(value_type*)iter.next(&iter);

		memcpy(aggObj->_temp_row, kv.key, kv.key_len);
		memcpy(aggObj->_temp_row + kv.key_len, kv.value,aggObj->_hash_table._val_len);
		aggObj->_out_row->updateRow(aggObj->_out_row, aggObj->_temp_row);
		return aggObj->_out_row;
	}
	else{
		return NULL;
	}	
}

// aggregate|aggFunc---->对应的dataType
void aggregateOperator_output_schema(Operator* obj, Vector* out_vector_datatype){
	UnaryOperator *unaryOp = (UnaryOperator*)obj->pDerivedObj;
	AggregateOperator * aggObj = (AggregateOperator*)unaryOp->pDerivedObj;

	Vector* schema_datatype_v = aggObj->_input_schema_datatype;	// schema

	// group 存放的是 1,2....  ---> 放到 out_vector_datatype 中
	Vector* group_exprs_v = aggObj->_grouping_exprs_int;
	while (group_exprs_v->f_hasMore(group_exprs_v))
	{
		int i = *(int*)group_exprs_v->f_next(group_exprs_v);
		//DataType * tem = (DataType*)(schema_datatype_v->f_get_value(schema_datatype_v, i));
		DataType * tem = new_DataType("long", sizeof(long));
		out_vector_datatype->f_push_back(out_vector_datatype, tem);
	}
	
	// aggfunc 中的 ---->out_vector_datatype
	Vector* aggf = aggObj->_agg_exprs_aggFunc;
	while (aggf->f_hasMore(aggf)){
		AggFunc* temp = (AggFunc*)aggf->f_next(aggf);
		DataType * tem = 
			(DataType*)(schema_datatype_v->f_get_value(schema_datatype_v, temp->_index));
	//	DataType *tem = new_DataType("long", sizeof(long));
		out_vector_datatype->f_push_back(out_vector_datatype, tem);
	}
}

void aggregateOperator_children(Operator* obj, Vector* _operator_out){

	UnaryOperator *unaryOp = (UnaryOperator*)obj->pDerivedObj;
	AggregateOperator * aggObj = (AggregateOperator*)unaryOp->pDerivedObj;

	_operator_out->f_push_back(_operator_out, aggObj->_child);
}

void aggregateOperator_close(Operator* obj){
	UnaryOperator *unaryObj = (UnaryOperator*)obj->pDerivedObj;
	AggregateOperator *aggObj = (AggregateOperator*)unaryObj->pDerivedObj;

	// we have call this method in initFunc , so we should not recall this any more.
	//aggObj->_child->f_close(aggObj->_child);

	if (aggObj->_out_row != NULL){
		aggObj->_out_row->f_free(aggObj->_out_row);
		free(aggObj->_out_row);
		aggObj->_out_row = NULL;
	}

	if (aggObj->_temp_row != NULL){
		free(aggObj->_temp_row);
		aggObj->_temp_row = NULL;
	}

	if (aggObj->_hash_table._num_elements != 0){
		aggObj->_hash_table.destroy_hash(&aggObj->_hash_table);
	}		

	aggObj->_agg_exprs_aggFunc->f_free(aggObj->_agg_exprs_aggFunc);
	free(aggObj->_agg_exprs_aggFunc);
	aggObj->_agg_exprs_aggFunc = NULL;

	aggObj->_grouping_exprs_int->f_free(aggObj->_grouping_exprs_int);
	free(aggObj->_grouping_exprs_int);
	aggObj->_grouping_exprs_int = NULL;

	aggObj->_input_schema_datatype->f_free(aggObj->_input_schema_datatype);
	free(aggObj->_input_schema_datatype);
	aggObj->_input_schema_datatype = NULL;
	
	// free the object
	free(aggObj);
	aggObj = NULL;
	free(unaryObj);
	unaryObj = NULL;
	free(obj);
}


//********************* filter Operator ***************************************************
void init_filter_func(Operator *filterObj){
	filterObj->f_initialize = filterOperator_init;
	filterObj->f_next = filterOperator_next;
	filterObj->f_close = filterOperator_close;
	filterObj->f_children = filterOperator_children;
	filterObj->f_output_schema = filterOperator_output_schema;

}

Operator * new_filterOperator(const char * filter_str,Operator *child){
	Operator *baseOp = (Operator*)malloc(sizeof(Operator));
	if (baseOp == NULL){
		return NULL;
	}

	LeafOperator *leafOp = (LeafOperator*)malloc(sizeof(LeafOperator));
	if (leafOp == NULL){
		free(baseOp);
		return NULL;
	}

	FilterOperator * obj = (FilterOperator*)malloc(sizeof(FilterOperator));
	if (obj == NULL){
		free(baseOp);
		free(leafOp);
		return NULL;
	}

	baseOp->pDerivedObj = leafOp;
	leafOp->baseObj = baseOp;
	leafOp->pDerivedObj = obj;
	obj->_baseObj = leafOp;

	obj->_filter_str = strdup(filter_str);
	obj->_child = child;
	init_filter_func(baseOp);
}

void filterOperator_init(Operator *obj){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	FilterOperator *filterObj = (FilterOperator*)leafObj->pDerivedObj;
	filterObj->_child->f_initialize(filterObj->_child);
}

Row* filterOperator_next(Operator * obj){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	FilterOperator *filterObj = (FilterOperator*)leafObj->pDerivedObj;

	if (filterObj->_child == NULL){
		return NULL;
	}

	Row *currRow = filterObj->_child->f_next(filterObj->_child);
	while (currRow != NULL)
	{
		if (getFilterRow(filterObj->_filter_str, currRow) == TRUE)
			return currRow;
		else
			currRow = filterObj->_child->f_next(filterObj->_child);
	}
	return NULL;	
}

void filterOperator_close(Operator* obj){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	FilterOperator *filterObj = (FilterOperator*)leafObj->pDerivedObj;
	filterObj->_child->f_close(filterObj->_child);
	free(filterObj->_filter_str);
	
	//free the object
	free(filterObj);
	filterObj = NULL;
	free(leafObj);
	leafObj = NULL;
	free(obj);
}

void filterOperator_children(Operator* obj, Vector* _operator){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	FilterOperator *filterObj = (FilterOperator*)leafObj->pDerivedObj;
	_operator->f_push_back(_operator, filterObj->_child);
}

void filterOperator_output_schema(Operator* obj, Vector* _datatype){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	FilterOperator *filterObj = (FilterOperator*)leafObj->pDerivedObj;
	assert(filterObj->_child != NULL && "the child in filterOp should not be NULL, please check!\n");

	filterObj->_child->f_output_schema(filterObj->_child,_datatype);
}

// func: judge whether or not the currRow is match with str_filter
// str_filter : 1<23 and 2>23 or 3<222 and 4 > 2345
// return: TRUE match; FALSE: not
BOOLEAN getFilterRow( const char* str_filter, Row *currRow){
	int index = 0;
	int result = 0;

	char preOp[5] = "or";
	char op[5] = { 0 };			// next op "and" , "or"
	char sub_char[20] = { 0 };
	while (index < strlen(str_filter))
	{
		index = scanOp(str_filter, index, sub_char, op);
		if (strcmp(preOp, "or") == 0){
			result += checkSubOp(sub_char, currRow);
			if (result >= 1 && strcmp(op, "or") == 0)	return result;
		}
		else if (strcmp(preOp, "and") == 0){
			result *= checkSubOp(sub_char, currRow);
		}

		if (strcmp(op, "and") == 0 || strcmp(op, "or") == 0){
			strcpy(preOp, op);
		}
		else
		{
			break;
		}
	}
	return result >=1 ? TRUE: FALSE;

}

// func :   1 > 234 and 2<333 or 3 like ---> '1>234'
// argc: 
//	in_filter_str: the filter string like " 1 > 234 and 2<333 or 3 like 'ssd'"
//	in_start_pos: the position that we will start scaning
//	out_str: get the sub_string before the 'and' or 'or' ,such as 'a<2' , 'b>3' and so on.
//	out_schema: get the 'and' or 'or'
// return :
// the position after we call the method.
int scanOp(const char const * filter_str, int in_start_pos, char *out_str, char* out_schema){
	// we only support 'and' and 'or' 
	int str_len = strlen(filter_str);
	if (in_start_pos >= str_len)	return NULL;

	int i = 0;
	for (i = in_start_pos; i < str_len; i++){
		switch (filter_str[i])
		{
		case 'a':
			if (filter_str[i + 1] == 'n'  && filter_str[i + 2] == 'd' &&
				(filter_str[i + 3] == ' '))
			{
				strcpy(out_schema, "and");
				strncpy(out_str, filter_str + in_start_pos, i - in_start_pos);
				out_str[i - in_start_pos] = NULL;
				trim(out_str);
				return i + 4;
			}

		case 'o':
			if (filter_str[i + 1] == 'r' && filter_str[i + 2] == ' ')
			{
				strcpy(out_schema, "or");
				strncpy(out_str, filter_str + in_start_pos, (i - in_start_pos));
				out_str[i - in_start_pos] = NULL;
				trim(out_str);
				return i + 3;
			}
		default:
			break;
		}
	}

	out_schema[0] = NULL;
	strncpy(out_str, filter_str + in_start_pos, i - in_start_pos);
	out_str[i - in_start_pos] = NULL;
	trim(out_str);
	return strlen(filter_str);
}


// function:  get the result from 'row' that if or not match the filter
// >,>=,<,<=,!=,Like 
// str : '2>23',
// return :T/F
BOOLEAN checkSubOp(char *str, Row *row){
	//char l[5];
	int clmid = 0;
	char r[10];
	char sign[5];
	if (getSplitStr(str, &clmid, r, sign) == TRUE){
		if (strcmp(sign, ">=") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "int") == 0){
				int val = *(int*)datatype->f_getValue(datatype, row, clmid);
				return val >= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "long") == 0){
				long val = *(long*)datatype->f_getValue(datatype, row, clmid);
				return val >= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "double") == 0){
				double val = *(double*)datatype->f_getValue(datatype, row, clmid);
				return val >= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "float") == 0){
				float val = *(float*)datatype->f_getValue(datatype, row, clmid);
				return val >= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strcmp(val, r) >= 0 ? TRUE : FALSE;
			}
		}
		else if (strcmp(sign, ">") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "int") == 0){
				int val = *(int*)datatype->f_getValue(datatype, row, clmid);
				return val > atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "long") == 0){
				long val = *(long*)datatype->f_getValue(datatype, row, clmid);
				return val > atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "double") == 0){
				double val = *(double*)datatype->f_getValue(datatype, row, clmid);
				return val > atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "float") == 0){
				float val = *(float*)datatype->f_getValue(datatype, row, clmid);
				return val > atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strcmp(val, r) > 0 ? TRUE : FALSE;
			}
		}
		else if (strcmp(sign, "<=") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "int") == 0){
				int val = *(int*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "long") == 0){
				long val = *(long*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "double") == 0){
				double val = *(double*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "float") == 0){
				float val = *(float*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strcmp(val, r) <= 0 ? TRUE : FALSE;
			}
		}
		else if (strcmp(sign, "<") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "int") == 0){
				int val = *(int*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "long") == 0){
				long val = *(long*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "double") == 0){
				double val = *(double*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "float") == 0){
				float val = *(float*)datatype->f_getValue(datatype, row, clmid);
				return val <= atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strcmp(val, r) < 0 ? TRUE : FALSE;
			}
		}
		else if (strcmp(sign, "=") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "int") == 0){
				int val = *(int*)datatype->f_getValue(datatype, row, clmid);
				return val = atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "long") == 0){
				long val = *(long*)datatype->f_getValue(datatype, row, clmid);
				return val = atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "double") == 0){
				double val = *(double*)datatype->f_getValue(datatype, row, clmid);
				return val = atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "float") == 0){
				float val = *(float*)datatype->f_getValue(datatype, row, clmid);
				return val = atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strcmp(val, r) == 0 ? TRUE : FALSE;
			}
		}
		else if (strcmp(sign, "!=") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "int") == 0){
				int val = *(int*)datatype->f_getValue(datatype, row, clmid);
				return val != atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "long") == 0){
				long val = *(long*)datatype->f_getValue(datatype, row, clmid);
				return val != atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "double") == 0){
				double val = *(double*)datatype->f_getValue(datatype, row, clmid);
				return val != atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "float") == 0){
				float val = *(float*)datatype->f_getValue(datatype, row, clmid);
				return val != atoi(r) ? TRUE : FALSE;
			}
			else if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strcmp(val, r) != 0 ? TRUE : FALSE;
			}
		}
		else if (strcmp(sign, "like") == 0){
			Vector *datatypes = row->_schema_datatype;
			DataType *datatype = (DataType*)datatypes->f_get_value(datatypes, clmid);
			if (strcmp(datatype->_name, "string") == 0){
				char* val = (char*)datatype->f_getValue(datatype, row, clmid);
				return strstr(val, r) != NULL ? TRUE : FALSE;
			}
		}
		else{
			assert("we have not support the operator!");
		}
	}
	else
	{
		return FALSE;
	}
}


// function : 2>=23 -----> 2 , >= , 23
// 这个是破坏性的，将in_src修改
BOOLEAN getSplitStr(char * in_src, int * out_clm_id, char * out_str_result, char * out_sign){
	trim(in_src);
	char * signs[] = { ">=", ">", "<=", "<", "!=", "=", "like" };
	int sin_len = 7;
	for (int i = 0; i < sin_len; i++){
		char * pos = strstr(in_src, signs[i]);
		if (pos != NULL){
			// get the column id
			int index = pos - in_src;
			in_src[index] = NULL;
			*out_clm_id = atoi(in_src);

			// get the values
			index += strlen(signs[i]);	
			strcpy(out_str_result, in_src + index);
			trim(out_str_result);

			// get the signs
			strcpy(out_sign, signs[i]);
			return TRUE;
		}
	}
	return FALSE;
}

//*******************************************************************************************************
//sortOperator

void init_sort_func(Operator * obj){
	obj->f_children = sortOperator_children;
	obj->f_close = sortOPerator_close;
	obj->f_initialize = initSortOp;
	obj->f_next = sortOperator_next;
	obj->f_output_schema = sortOperator_output_schema;
}

Operator* new_sortOperator(const char* sort_str, Operator*child){
	Operator *baseOp = (Operator*)malloc(sizeof(Operator));
	if (baseOp == NULL){
		return NULL;
	}

	LeafOperator *leafOp = (LeafOperator*)malloc(sizeof(LeafOperator));
	if (leafOp == NULL){
		free(baseOp);
		return NULL;
	}

	SortOperator * obj = (SortOperator*)malloc(sizeof(SortOperator));
	if (obj == NULL){
		free(baseOp);
		free(leafOp);
		return NULL;
	}

	baseOp->pDerivedObj = leafOp;
	leafOp->baseObj = baseOp;
	leafOp->pDerivedObj = obj;
	obj->_baseObj = leafOp;

	obj->_sort_str = strdup(sort_str);
	obj->_child = child;
	init_sort_func(baseOp);

	return baseOp;
}

// f1 (ASC), f2  DESC  ---> 1 (ASC), 3 DESC      ASC=T
void initSortOp(Operator *obj){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	SortOperator *sortObj = (SortOperator*)leafObj->pDerivedObj;
	sortObj->_child->f_initialize(sortObj->_child);

	// compute the vectors
	computeVectors(sortObj);

	initCurrRow(sortObj);
	// init table
	int start = clock();
	buildTable(sortObj);
#ifdef _ECHO
	FILE *pf = fopen("d://res1.txt", "a");
	fprintf(pf, "sort's time : %d s\n", clock() - start);
	fclose(pf);
#endif
	sortObj->_child->f_close(sortObj->_child);
}

Row* sortOperator_next(Operator *obj){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	SortOperator *sortObj = (SortOperator*)leafObj->pDerivedObj;
	if (sortObj->_table._post != sortObj->_table._ele_num)
	{
		value_type *vals = sortObj->_table._value + sortObj->_table._post;
		sortObj->row->updateRow(sortObj->row, vals->value);
		sortObj->_table._post++;
		return sortObj->row;
	}
	else
	{
		return NULL;
	}
}

void sortOPerator_close(Operator * obj){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	SortOperator *sortObj = (SortOperator*)leafObj->pDerivedObj;

	// we have call this method in the function 'initFunc' so we sould not recall this again.
	//sortObj->_child->f_close(sortObj->_child);

	free(sortObj->_sort_str);

	sortObj->_sort_v_i_clmAE->f_free(sortObj->_sort_v_i_clmAE);
	free(sortObj->_sort_v_i_clmAE);
	sortObj->_sort_v_i_clmAE = NULL;

	sortObj->_sort_v_i_clmId->f_free(sortObj->_sort_v_i_clmId);
	free(sortObj->_sort_v_i_clmId);
	sortObj->_sort_v_i_clmId = NULL;

	sortObj->_sort_v_i_clmIdLen->f_free(sortObj->_sort_v_i_clmIdLen);
	free(sortObj->_sort_v_i_clmIdLen);
	sortObj->_sort_v_i_clmIdLen = NULL;

	sortObj->_table.destroy(&sortObj->_table);

	// free the object
	free(sortObj);
	sortObj = NULL;
	free(leafObj);
	leafObj = NULL;
	free(obj);
}

void sortOperator_children(Operator *obj, Vector* _operator){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	SortOperator *sortObj = (SortOperator*)leafObj->pDerivedObj;
	assert(sortObj->_child != NULL);
	_operator->f_push_back(_operator, sortObj->_child);

}

void sortOperator_output_schema(Operator *obj, Vector * _out_schema_v){
	LeafOperator *leafObj = (LeafOperator*)obj->pDerivedObj;
	SortOperator *sortObj = (SortOperator*)leafObj->pDerivedObj;
	sortObj->_child->f_output_schema(sortObj->_child,_out_schema_v);
}

void buildTable(SortOperator *sortObj){

	// evaluate the size
#ifdef N
	sortObj->_table = new_ValueArray(total_ele_size);
	//Vector *datatype = new_Vector("datatype");
//	obj->f_output_schema(obj, datatype);

	sortObj->_table._acData = new_actualData(total_file_size * 0.3, total_file_size);
#else
	sortObj->_table = new_ValueArray(total_file_size);
#endif

	Row  *curr;
	value_type val;
	while ((curr = sortObj->_child->f_next(sortObj->_child)) != NULL)
	{
#ifdef N
		val = getValuesFromRow(curr, &sortObj->_table._acData, *sortObj->_sort_v_i_clmId);
		sortObj->_table.insert(&sortObj->_table, val, sortObj->_table._acData);
#else
		val = getValuesFromRow(curr, *sortObj->_sort_v_i_clmId);
		sortObj->_table.insert(&sortObj->_table, val);
#endif
	}
#ifdef N
	// modify the data offset
	while (sortObj->_table._post != sortObj->_table._ele_num)
	{
		(sortObj->_table._value + sortObj->_table._post)->key += sortObj->_table._acData._base_key_offset;
		(sortObj->_table._value + sortObj->_table._post)->value += sortObj->_table._acData._base_val_offset;
		sortObj->_table._post++;
	}
	sortObj->_table._post = 0;

#endif
	qusort(sortObj->_table._value, sortObj->_table._ele_num, sizeof(value_type), compare,
		*sortObj->_sort_v_i_clmId, *sortObj->_sort_v_i_clmAE, *sortObj->_sort_v_i_clmIdLen);

}

void initCurrRow(SortOperator *sortObj){
	Vector * datatype = new_Vector("datatype");
	sortObj->_child->f_output_schema(sortObj->_child, datatype);
	sortObj->row = new_Row_from_schema(datatype);
}

void computeVectors(SortOperator *sortObj){

	sortObj->_sort_v_i_clmId = new_Vector("int");
	sortObj->_sort_v_i_clmAE = new_Vector("int");
	sortObj->_sort_v_i_clmIdLen = new_Vector("int");

	char *str = sortObj->_sort_str;
	int str_len = strlen(str);
	int index = 0, pre_start = 0;
	char sub_str[50] = { 0 };

	// deal with "2 asc, 4 desc" to get the sort clumn ids and echo clumn 's aesc or acs
	while (index < str_len){
		index = getLimePosition(str, ',', index);
		strset(sub_str, NULL);
		if (index != -1)	// not last sub_str
		{
			strncpy(sub_str, str + pre_start, index - pre_start);
			sub_str[index] = NULL;
			trim(sub_str);
			getVectory(sub_str, sortObj->_sort_v_i_clmId, sortObj->_sort_v_i_clmAE);
		}
		else{
			// the last sub_str
			strncpy(sub_str, str + pre_start, str_len - pre_start);
			sub_str[str_len - pre_start] = NULL;
			trim(sub_str);
			getVectory(sub_str, sortObj->_sort_v_i_clmId, sortObj->_sort_v_i_clmAE);
			break;
		}
		pre_start = index + 1;
		index++;
	}

	// get the clm length
	Vector *datatype = new_Vector("datatype");
	sortObj->_child->f_output_schema(sortObj->_child, datatype);
	getVectory_len(datatype, sortObj->_sort_v_i_clmId, sortObj->_sort_v_i_clmIdLen);
}

// get the vectory from "0 desc" ,vec_1 store '0' and vec_2 stores 'T/F'
void getVectory(char *sub_str, Vector *vec_clmid, Vector *vec_clmAE){
	int i = getLimePosition(sub_str, ' ', 0);
	char tem[20];
	int s1 = -1;
	int s2 = -1;

	if (i == -1){
		s1 = atoi(sub_str);
		s2 = TRUE;
		vec_clmid->f_push_back(vec_clmid, &s1);
		vec_clmAE->f_push_back(vec_clmAE, &s2);
		return;
	}

	strncpy(tem, sub_str, i);
	tem[i] = NULL;
	s1 = atoi(tem);

	strncpy(tem, sub_str + i + 1, strlen(sub_str) - i - 1);
	tem[strlen(sub_str) - i - 1] = NULL;
	trim(tem);
	if (strcmpi(tem, "asc") == 0)
		s2 = TRUE;
	else
		s2 = FALSE;
	vec_clmid->f_push_back(vec_clmid, &s1);
	vec_clmAE->f_push_back(vec_clmAE, &s2);
}

void getVectory_len(Vector * datatype,Vector* vec_clm, Vector * vec_clmLen){
	while (vec_clm->f_hasMore(vec_clm))
	{
		int index = *(int*)vec_clm->f_next(vec_clm);
		DataType *data = (DataType*)datatype->f_get_value(datatype, index);
		vec_clmLen->f_push_back(vec_clmLen, &data->_width);
	}
}


#ifdef N
value_type getValuesFromRow(Row * row,actualData *acta, Vector clmids){
#else
value_type getValuesFromRow(Row * row, Vector clmids){
#endif
	char key[100] = { 0 };
	int pos = 0;
	getCombineChar(row, clmids, key, &pos);
	
#ifdef N
	unsigned int val_len = 0;
	char valTem[1024] = { 0 };
	row->getUncharRow(row, valTem,&val_len);
	value_type values = get_value_from_ad(acta, key, valTem, pos, val_len);
	return values;

#else
	value_type values;
	values.key_len = pos;
	values.key = (char*)malloc(values.key_len);
	memcpy(values.key, key, pos);

	char * tem = row->getUncharRow(row, &values.val_len);
	values.value = (char*)malloc(values.val_len);
	memcpy(values.value, tem, values.val_len);
	return values;
#endif
}

// 正向比较
int compare(const void * _key1, const void * _key2, Vector clmAE, Vector clmIdLen){

	value_type * key1 = (value_type*)_key1;
	value_type *key2 = (value_type*)_key2;

	int pos1 = 0, pos2 = 0;

	int idLen = 0;
	int size = clmIdLen.f_size(&clmIdLen);
	for (size_t i = 0; i < size; i++)
	{
		int resu = 0;
		idLen = *(int*)clmIdLen.f_get_value(&clmIdLen, i);	// get the length 
		if (idLen == 0){
			// string
			resu = strcmpi(key1->key + pos1, key2->key + pos2);
			pos1 += strlen(key1->key + pos1) + 1;
			pos2 += strlen(key2->key + pos2) + 1;

		}
		else{
			// int|long|			
			resu = memicmp(key1->key + pos1, key2->key + pos2, idLen);
			pos1 += idLen;
			pos2 += idLen;			
		}

		if (resu == 0) continue;

		int AE = *(int*)clmAE.f_get_value(&clmAE, i);
		if (AE == TRUE){
			return resu;
		}			
		else{
			return -1 * resu;
		}
	}
	return 0;
}
