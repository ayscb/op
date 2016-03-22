// OpC.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include "string.h"
#include "doString.h"
#include "agg1.h"
#include "Row.h"
#include "Operator.h"
#include <time.h>
#include <stdlib.h>

void showAgg(char * str, int sum);
void showRow(Row * row);
void showVector(Vector * vector);

void testDataType(int isfree){
	DataType *data = new_DataType("int", 4);
	printf("%s\n", data->f_Name(data));
	printf("%d\n", data->f_Width(data));
	printf("the adderss: %d\n", data);
	printf("\n");
	if (isfree == 1){
		data->f_delete_DataType(data);
		printf("内存已释放");
	}
		
}

void testIntType(int isfree){
	DataType * int1 = new_IntType();
	printf("the type name is : %s\n", int1->f_Name(int1));
	printf("the type length is : %d\n", int1->f_Width(int1));
	printf("the base type adderss is : %x\n", int1);
	printf("the child type adderss is : %x\n", int1->_pDerivedObj);	
	printf("\n");
	if (isfree == 1)
		int1->f_delete_DataType(int1);
}

void testDouble(int isfree){
	DataType *dou1 = new_DoubleType();
	printf("the type name is : %s\n", dou1->f_Name(dou1));
	printf("the type length is : %d\n", dou1->f_Width(dou1));
	printf("the type type adderss is : %x\n", dou1);
	printf("\n");
	if (isfree == 1)
		dou1->f_delete_DataType(dou1);
}

void testLong(int isfree){
	DataType *long1 = new_LongType();
	printf("the type name is : %s\n", long1->f_Name(long1));
	printf("the type length is : %d\n", long1->f_Width(long1));
	printf("the type type adderss is : %x\n", long1);
	printf("\n");
	if (isfree == 1)
		long1->f_delete_DataType(long1);
}

void testString(int isfree){
	DataType *str1 = new_StringType();
	printf("the type name is : %s\n", str1->f_Name(str1));
	printf("the type length is : %d\n", str1->f_Width(str1));
	printf("the type type adderss is : %x\n", str1);
	printf("\n");
	if (isfree == 1)
		str1->f_delete_DataType(str1);
}

void testUnknowType(int isfree){
	DataType *nuku1 = new_UnknowType();
	printf("the type name is : %s\n", nuku1->f_Name(nuku1));
	printf("the type length is : %d\n", nuku1->f_Width(nuku1));
	printf("the type type adderss is : %x\n", nuku1);
	printf("\n");
	if (isfree == 1)
		nuku1->f_delete_DataType(nuku1);
}

void testfree(){
	for (int i = 0; i < 10; i++){
		DataType * int2 = new_IntType();
		int2->f_delete_DataType(int2);
	}
}

void testRow(){
	char * schema = "int|string|long|int";
	void *rows[4];
	int a = 11;
	char * b = "1strings";
	int c = 12;
	long d = 13;

	rows[0] = &a;
	rows[1] = b;
	rows[2] = &c;
	rows[3] = &d;

	Row * row = new_Row((void**)rows, 4);
	printf("%d\n", row->f_getInt(row, 0));
	printf("%s\n", row->f_getString(row, 1));
	printf("%d\n", row->f_getLong(row, 2));
	printf("%d\n\n", row->f_getInt(row, 3));
	row->f_putInt(row, 0, 21);
	//修改值 
	char * ss = "ss";
	row->f_putString(row, 1, "2ss");
	row->f_putLong(row, 2, 211111);
	row->f_putInt(row, 3, 22);
	printf("%d\n", row->f_getInt(row, 0));
	printf("%s\n", row->f_getString(row, 1));
	printf("%d\n", row->f_getLong(row, 2));
	printf("%d\n", row->f_getInt(row, 3));

	//*********************************************
	//char * schema = "int|string|long|int";
	Vector * schema_dataType = new_Vector("datatype");
	get_schema(schema, schema_dataType);
	Row * row1 = new_Row_from_schema(schema_dataType);
	int asss = row1->f_getInt(row1, 0);
	row1->f_putInt(row1, 0, 3123);
	row1->f_putString(row1, 1, "3ss");
	row1->f_putLong(row1, 2, 31234);
	row1->f_putInt(row1, 3, 31823);
	printf("%d\n", row1->f_getInt(row1, 0));
	printf("%s\n", row1->f_getString(row1, 1));
	printf("%d\n", row1->f_getLong(row1, 2));
	printf("%d\n", row1->f_getInt(row1, 3));

	char * txt_str = "41|4qwe|4444|456";
	row->f_setSchema(row, schema_dataType);
	row->updateRow(row, txt_str);
	printf("%d\n", row->f_getInt(row, 0));
	printf("%s\n", row->f_getString(row, 1));
	printf("%d\n", row->f_getLong(row, 2));
	printf("%d\n", row->f_getInt(row, 3));

	row1->updateRow(row1, txt_str);
	printf("%d\n", row->f_getInt(row, 0));
	printf("%s\n", row->f_getString(row, 1));
	printf("%d\n", row->f_getLong(row, 2));
	printf("%d\n", row->f_getInt(row, 3));

}
//******************************************

void testutil(){
	//****************// test get_type ok
	printf("test method get_type(int):\n");
	DataType *p = get_type("INt");
	printf("name: %s\t length : %d\n", p->f_Name(p), p->f_Width(p));
	p->f_delete_DataType(p);
	p = NULL;
	printf("\n");

	printf("test method get_type(string):\n");
	p = get_type("string");
	printf("name: %s\t length : %d\n", p->f_Name(p),p->f_Width(p));
	p->f_delete_DataType(p);
	p = NULL;
	printf("\n");

	//***************// test method "get_schema" ok
	printf("test method get_schema\n");
	char * schema1 = "string|int|long|string|double";	
	printf("原始字符序列：%s\n", schema1);
	Vector *dataTypeVector = new_Vector("dataType");
	get_schema(schema1, dataTypeVector);
	
	while (dataTypeVector->f_hasMore(dataTypeVector)){
		DataType * p = (DataType*)(dataTypeVector->f_next(dataTypeVector));
		printf("name: %s\t length : %d\n", p->f_Name(p), p->f_Width(p));
	}
	printf("\n");

	//***************// test method "split_str" ok
	printf("test method split_str\n");
	char * schema3 = "1|2|3|5|7";
	printf("原始字符序列：%s\n", schema3);
	Vector *intVector = new_Vector("int");
	split_str(schema3, '|', intVector);
	while (intVector->f_hasMore(intVector)){
		int *a = (int*)(intVector->f_next(intVector));
		printf("%d\t", *a);
	}
	printf("\n");


}

void testVector(){
	// test ok!!!!!!!!!!!!!!!!!!!!!!!!!
	Vector *vex = new_Vector("int");
	int a = 123;
	int b = 456;
	vex->f_push_back(vex, &a);
	vex->f_push_back(vex, &b);
	int in1 = *(int*)vex->f_get_value(vex, 0);
	int in2 = *(int*)vex->f_get_value(vex, 1);
	printf("%d,%d\n", in1, in2);

	Vector *vexStr = new_Vector("string");
	char * s1 = "asdf";
	char * s2 = "qwe\0";
	vexStr->f_push_back(vexStr, s1);
	vexStr->f_push_back(vexStr, s2);
	char * rs1 = strdup((char*)vexStr->f_get_value(vexStr, 0));
	char * rs2 = (char*)vexStr->f_get_value(vexStr, 1);	
	printf("存放的数据：%s,%s\n", rs1, rs2);

	//当指向内部数据时 ，依靠指针可以改变里面的内容
	memcpy(rs2, "ss", 3);
	char *rs3 = (char*)vexStr->f_get_value(vexStr, 1);
	printf("%s,%s\n", rs1, rs2);

	AggFunc * agg_max = new_MaxLong_pro(1,1);
	AggFunc * agg_sum = new_SumLong_pro(1, 2);
	AggFunc * agg_avg = new_AvgLong_pro(1,3);

	printf("%d\n", agg_max->f_self_size(agg_max));
	printf("%d\n", agg_sum->f_self_size(agg_sum));
	printf("%d\n", agg_avg->f_self_size(agg_avg));

	Vector *vexagg = new_Vector("aggfunc");
	vexagg->f_push_back(vexagg, agg_max);
	vexagg->f_push_back(vexagg, agg_sum);
	vexagg->f_push_back(vexagg, agg_avg);


	AggFunc * re1 = (AggFunc*)vexagg->f_get_value(vexagg, 0);
	AggFunc * re2 = (AggFunc*)vexagg->f_get_value(vexagg, 1);
	AggFunc * re3 = (AggFunc*)vexagg->f_get_value(vexagg, 2);

	printf("%d\n", re1->f_self_size(re1));
	printf("%d\n", re2->f_self_size(re2));
	printf("%d\n", re3->f_self_size(re3));

	while (vexagg->f_hasMore(vexagg) == TRUE)
	{
		AggFunc * t = (AggFunc*)(vex->f_next(vexagg));
		printf("%d\n", t->f_self_size(t));
	}

}

void testVoid(){
	void *datas[10];
	int a1 = 1;
	int a2 = 2;
	short b1 = 21;
	short b2 = 22;
	char c1 = 'q';
	char c2 = 's';

	int a = sizeof(datas);
	int b = sizeof(datas[0]);
	datas[0] = &b1;
	datas[1] = &a1;
	datas[2] = &c1;

	printf("21 : %d \n", *(short*)datas[0]);
	printf("1 : %d \n", *(int*)datas[1]);
	printf("q : %c \n", *(char*)datas[2]);
}

void testdoString(){
	char * a = "char|string|int\0int|int|int\0String|string|string|string\0"; //56
	char * b = "char|string|int|short";	//22
	char * c = "sum(1),avg(2)";	//13
	char * d = "sum(1)";	//6

	char *aa = (char*)malloc(sizeof(char) * 56);
	char *bb = (char*)malloc(sizeof(char) * 23);
	char *cc = (char*)malloc(sizeof(char) * 13);
	char *dd = (char*)malloc(sizeof(char) * 7);
	memcpy(aa, a, 56);
	strncpy(bb, b, 22);
	strncpy(cc, c, 14);
	strncpy(dd, d, 7);

	// 查找从 start位置开始的 str中 第一次出现 lime的位置(index from 0)
	printf("%s the first '|' position is : %d\n", a, getLimePosition(a, '|', 0));
	printf("%s the first '|' position is : %d\n", aa, getLimePosition(aa, '|', 0));

	printf("%s the second '|' position is : %d\n", a, getLimePosition(a, '|', getLimePosition(a, '|', 0) + 1));
	printf("%s the second '|' position is : %d\n", aa, getLimePosition(aa, '|', getLimePosition(aa, '|', 0) +1));
	
	// for "ssss\0ssss\0addd\0" --> ssss
	int i = 0;
	for (; i < 5; i++){
		char *tem1 = getStringFromCharArray(a, i, 3);
		printf("%s the %d 's data is %s\n", a, i, tem1);
		free(tem1);

		tem1 = getStringFromCharArray(aa, i, 3);
		printf("%s the %d 's data is %s\n", aa, i, tem1);
		free(tem1);
	}

	//sum(1) -->sum 1
	char **tem2 = getStringFromSchem(d);
	//printf("%s ---> %s,%s", d, tem2[0], tem2[1]);
	free(tem2[0]);
	free(tem2[1]);

	//sub
	char tem3[40];
	substring(tem3, a, 3, 23);

}

void testStringFun(){
	char * a = "char|string|int\0";
	char *p = strchr(a, '|');
	if (p == NULL)
		printf("NULL");
	else
		printf("%d", p - a);

	char *b = strdup(a);
	strcpy(b, a);
	char *q = strtok(b, "|");;
	while (q != NULL)
	{
		printf("%s\n", q);
		q = strtok(NULL, "|");
	}
}

void testAgg1(){
	AggFunc * base = new_AggFunc(1);
	AggFunc * sum = new_SumLong(2);
	AggFunc* max = new_MaxLong(3);
	AggFunc * avg = new_AvgLong(4);

	printf("before position: %d\n", base->_position);
	base->f_set_position(base,1);
	printf("after position: %d\n", base->_position);

	char * txt_str = "12|stri|345|34";
	char * schema_str = "int|string|long|int";
	Vector *schema_vector_dataType = new_Vector("datatype");
	get_schema(schema_str, schema_vector_dataType);
	Row *row = new_Row_from_schema(schema_vector_dataType);

	AggFunc * sum0 = new_SumLong(0);
	sum0->f_set_position(sum0,0);
	AggFunc * sum2 = new_SumLong_pro(sizeof(long) * 1,2);
	AggFunc * sum3 = new_SumLong_pro(sizeof(long) * 2,3);

	// test update
	char * updata_str = (char*)malloc(sizeof(char) * 3 * 8);
	memset(updata_str, 0, 24);
	showAgg(updata_str, 3);
	printf("\n");

	printf("test aggfunc's update method\n");
	char * txt1 = "10|stri|100|1000";
	char * txt2 = "20|stri|200|2000";
	row->updateRow(row, txt1);
	showRow(row);
	sum0->f_update(sum0, updata_str, row);
	showAgg(updata_str, 3);
	sum2->f_update(sum2, updata_str, row);
	showAgg(updata_str, 3);
	sum3->f_update(sum3, updata_str, row);
	showAgg(updata_str, 3);
	printf("\n");

	row->updateRow(row, txt2);
	showRow(row);
	sum0->f_update(sum0, updata_str, row);
	showAgg(updata_str, 3);
	sum2->f_update(sum2, updata_str, row);
	showAgg(updata_str, 3);
	sum3->f_update(sum3, updata_str, row);
	showAgg(updata_str, 3);
	printf("\n");

	//test get_aggfuncs
	printf("test aggfunc's get_aggfuncs method\n");
	char * schemas = "sum(0),sum(2),sum(3)";
	Vector* aggfunc_vector = new_Vector("aggfunc");
	get_aggfuncs(schemas, NULL, aggfunc_vector);
	row->updateRow(row, txt1);
	showRow(row);
	while (aggfunc_vector->f_hasMore(aggfunc_vector)){
		AggFunc * temp = (AggFunc*)(aggfunc_vector->f_next(aggfunc_vector));
		temp->f_update(temp, updata_str, row);
		showAgg(updata_str,3);
	}
	printf("\n");
}

void testAperator(){
	printf("test operator 's next method\n");
	char * str_schema = "int|string|int|long|int";
#ifdef _FILE
	Operator * op = new_TextfileScanOperator("d:/sqldata.txt", str_schema);
#else
	char *file_mem = getSimpleMem(24);
	Operator * op = new_TextfileScanOperator(file_mem, str_schema,24 * 1024 * 1024);
#endif
	op->f_initialize(op);
	Row * row = NULL;
	while ((row = op->f_next(op)) != NULL){
		showRow(row);
	}
	printf("\n");

	printf("test operator's output_schema\n");
	Vector * schema = new_Vector("dataType");
	op->f_output_schema(op,schema);
	showVector(schema);
	printf("\n");

	printf("test agg output_schema\n");
	char * groupingExpression = "0|2";
	char * AggregateExpression = "sum(0),avg(2),max(3)";
	Operator * agg = new_AggregateOperator(groupingExpression, AggregateExpression, op);

}
void showAgg(char * str, int sum){
	printf("解析后的数值：");
	for (int i = 0; i < sum; i++){
		long curr;
		memcpy(&curr, str + sizeof(long) * i, sizeof(long));
		printf("%d\t",curr);
	}
	printf("\n");

}

void showRow(Row * row){
	Vector * dataType_vector = row->_schema_datatype;
	int i = 0;
	printf("新行共%d列，数据为：",row->_size);
	while (dataType_vector->f_hasMore(dataType_vector))
	{
		DataType * tem = (DataType*)(dataType_vector->f_next(dataType_vector));
		if (strcmpi(tem->f_Name(tem), "int") == 0){
			printf("%d\t", row->f_getInt(row, i));
		}
		else if (strcmpi(tem->f_Name(tem), "long") == 0){
			printf("%d\t", row->f_getLong(row, i));
		}
		else if (strcmpi(tem->f_Name(tem), "double") == 0){
			printf("%d\t", row->f_getDouble(row, i));
		}
		else if (strcmpi(tem->f_Name(tem), "float") == 0){
			printf("%d\t", row->f_getFloat(row, i));
		}
		else if (strcmpi(tem->f_Name(tem), "string") == 0){
			printf("%s\t", row->f_getString(row, i));
		}
		i++;
	}
	printf("\n");
}

void showVector(Vector * vector){
	while (vector->f_hasMore(vector)){
		if (strcmpi(vector->_type, "datatype") == 0){
			DataType * tem = (DataType*)(vector->f_next(vector));
			printf("%s\t%d\n", tem->f_Name(tem), tem->f_Width(tem));
		}
		else if (strcmpi(vector->_type, "string") == 0){
			printf("%s\n", (char*)vector->f_next(vector));
		}
		else if (strcmpi(vector->_type, "int") == 0){
			printf("%d\n", *(int*)vector->f_next(vector));
		}
		else if (strcmpi(vector->_type, "long") == 0){
			printf("%l\n", *(long*)vector->f_next(vector));
		}	
	}
}


void testAll(){
	
	// fileScan Operator
	char * schema_str = "int|int|long|string";
#ifdef _FILE
	char * file_name = "d://data0.txt";
	Operator * scanOp = new_TextfileScanOperator(file_name, schema_str);
#else
	char * file_mem = getSimpleMem(1);
	Operator * scanOp = new_TextfileScanOperator(file_mem, schema_str,1 * 1024 * 1024);
#endif
	//scanOp->f_initialize(scanOp);

	//filter Operator
	Operator *filterOp = new_filterOperator("1<10 and 2 <40 or 3 like '5'", scanOp);

	Operator *sortOp = new_sortOperator(" 1 desc, 2 desc,3 asc", filterOp);

	//Groupby Operator  group by "1,2"   "sum(1),avg(2)"
	Operator * aggOp = new_AggregateOperator(" 1,2", "sum(2)", sortOp);

//	aggOp->f_initialize(aggOp);

	// sortOp
	Operator *sort2 = new_sortOperator("0 desc", aggOp);
	sort2->f_initialize(sort2);

	Row *currRow;
	currRow = sort2->f_next(sort2);
	
	printf("group by\n 1 \t sum (2)\n");
	while (currRow != NULL)
	{
		char T[100];
		currRow->getCharRow(currRow, T);
		printf("%s\n", T);
		currRow = sort2->f_next(sort2);
	}

}

void testCompare(){
	Vector *clmid = new_Vector("int");
	Vector *clmAE = new_Vector("int");
	Vector *clmIdLen = new_Vector("int");

	// id= 2,4,5  T,F,F 4,0,4
	int id[] = { 2, 4, 5 };
	int ae[] = { 1, 0, 0 };
	int le[] = { 0, 0, 4 };
	for (int i = 0; i < 3; i++){
		clmid->f_push_back(clmid, &id[i]);
		clmAE->f_push_back(clmAE, &ae[i]);
		clmIdLen->f_push_back(clmIdLen, &le[i]);
	}

	char  _key1[20] = { 0 }, _key2[20] = { 0 };


	value_type key1, key2;
	key1.key = _key1;
	
	key2.key = _key2;
	
	
	// key1 : qwe|asdf|11
	// key2 : qwe|asdf|12
	int k11 = 11;
	char * _ck11 = "asdf";
	char * _ck12 = "qwee";
	memcpy(_key1, &k11, sizeof(int));
	strcpy(_key1 + sizeof(int), _ck11);
	strcpy(_key1 + sizeof(int) + strlen(_ck11) + 1, _ck12);
	key1.key_len = sizeof(int) + strlen(_ck11) + strlen(_ck12) + 2;

	int k21 = 12;
	char * _ck21 = "asdf";
	char * _ck22 = "qweee";
	memcpy(_key2, &k21, 4);
	strcpy(_key2 + 4, _ck21);
	strcpy(_key2 + 4 + strlen(_ck21) + 1, _ck22);
	key2.key_len = sizeof(int) + strlen(_ck21) + strlen(_ck22) + 2;

	int re = compare(&key1, &key2, *clmAE, *clmIdLen);
	
}

int _tmains(int argc, _TCHAR* argv[])
{
	for (int i = 0; i < 0; i++){
		testCompare();
	}

	//char *resu = getSimpleMem(1);
	// test all
	//getSimple();
	for (int i = 0; i < 1; i++){
		testAll();
	}
	// test 单例模式  OK
	for (int i = 0; i < 0; i++)
	{
		testIntType(0);
	}
	// test 所有DataType
	for (int i = 0; i < 0; i++){
		testDataType(0);
		testIntType(0);
		testDouble(0);
		testLong(0);
		testString(0);
		testUnknowType(0);
		testfree();
	}

	for (int i = 0; i < 0; i++){
		testutil();
	}

	for (int i = 0; i < 0; i++){
		testVoid();
	}

	for (int i = 0; i < 0; i++){
		testVector();
	}

	// you 
	for (int i = 0; i < 0; i++){
		testdoString();
	}
	for (int i = 0; i < 0; i++){
		testRow();
	}
	for (int i = 0; i < 0; i++){
		testAgg1();
	}
	for (int i = 0; i < 0; i++){
		testAperator();
	}
	for (int i = 0; i < 0; i++){
		// 有问题 free 与 printf 冲突
		char * d = "sum(11)";	//6
		char **tem2 = getStringFromSchem(d);
		printf("%s ---> %s,%s", d, *tem2, *(tem2+1));
		//free(tem2);
		//free((tem2+1)	);
		
	}

	// other test
	for (int i = 0; i < 0; i++){
		testStringFun();
	}
	printf("ok!");
	getchar();
	return 0;
}
