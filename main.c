#include "stdafx.h"
#include "string.h"
#include "doString.h"
#include "agg1.h"
#include "Row.h"
#include "Operator.h"
#include "testFun.h"
#include <time.h>
#include <stdlib.h>


void testOp(){
	Row *currRow;

	__int64 start = clock();
	// fileScan Operator
	char * schema_str = "int|int|long|string";
#ifdef _FILE
	char * file_name = "/home/result.txt";
	Operator * scanOp = new_TextfileScanOperator(file_name, schema_str);
#else
	char * file_mem = getSimpleMem(400);
	Operator * scanOp = new_TextfileScanOperator(file_mem, schema_str, 400);
#endif

	//filter Operator
	Operator *filterOp = new_filterOperator("1<10 and 2 >30 or 3 like '5'", scanOp);

	/*filterOp->f_initialize(filterOp);
	while (( currRow = filterOp->f_next(filterOp)) != NULL)
	{
		int a;
		a = 3;
	}*/
	//sort Operator      //  1 desc, 2 desc,3 asc  (asc as defalut)
	Operator *sortOp = new_sortOperator(" 2 desc , 1 asc  ", filterOp);

	//sortOp->f_initialize(sortOp);
	//test_printFile("d://result.txt", sortOp, clock() - start);

	//Groupby Operator  group by "1,2"   "sum(1),avg(2)"
	Operator * aggOp = new_AggregateOperator(" 1,2 ", "max(2),sum(2)", filterOp);

	//aggOp->f_initialize(aggOp);
	//test_printFile("d://result.txt", aggOp, clock() - start);

	// sortOp    0 desc, 2 asc
	Operator *sort2 = new_sortOperator("3 desc", aggOp);
	sort2->f_initialize(sort2);
	test_printFile("d://result.txt", sort2, clock() - start);
	sort2->f_close(sort2);

	printf("run ok\n");
	getch();
}

int main(void){
	testOp();
	//system("pause");
}