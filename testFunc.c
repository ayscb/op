#include "testFun.h"

void test_printFile(const char * filename, Operator *op, unsigned int times){
	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
	{
		printf("can not open the file :%s", filename);
		return;
	}

	fprintf(fp, "the total time is : %d\n", times);

	Row *curr = op->f_next(op);
	char T[1024];
	while (curr != NULL)
	{
		curr->getCharRow(curr, T);
		fprintf(fp, "%s\n", T);
		curr = op->f_next(op);
	}
	fclose(fp);
}