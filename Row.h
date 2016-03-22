#ifndef __aggregation__row__
#define __aggregation__row__
//#include "DataType.h"
#include "Vector.h"

//这里row** 保存的是值
typedef struct _Row Row;
struct _Row
{
	void ** _row;
	unsigned int _size;
	Vector * _schema_datatype;

	int (*f_getInt)(Row *, int);
	void (*f_putInt)(Row *, int, int);

	long (*f_getLong)(Row *,int);
	void (*f_putLong)(Row *,int, long);

	double (*f_getDouble)(Row *,int);
	void (*f_putDouble)(Row *,int, double);

	float (*f_getFloat)(Row *,int);
	void (*f_putFloat)(Row *,int, float);

	char* (*f_getString)(Row *,int);
	void (*f_putString)(Row *,int, char*);

	void (*updateRow)(Row * row,char * txt_str);				// 123qweqre32435235    这种单纯拼接的
	void (*updateRow_str)(Row * row, char * txt_str);			// "sdasd|1234|2|sss"	这种字符串
	void (*f_setSchema)(Row* row, Vector* schema_datatype);

	void (*getUncharRow)(Row * row, char * out_char, unsigned int *out_len);					// 123qweqre32435235    这种单纯拼接的
	void (*getCharRow)(Row* row, char * out_char);

	void(*f_free)(Row* row);
};

	Row * new_Row(void ** data, int size);
	Row * new_Row_from_schema(Vector* in_schema_datatype);	

	int getInt(Row *,int);
	void putInt(Row *, int, int);

	long getLong(Row *, int);
	void putLong(Row *, int, long);

	float getFloat(Row *, int);
	void putFloat(Row *, int, float);

	double getDouble(Row *, int);
	void putDouble(Row *, int, double);

	char * getString(Row *, int);
	void putString(Row *, int, char *);

	void updateRow(Row *, char *);
	void updateRow_str(Row *, char *);
	void setSchema(Row* row, Vector* schema_datatype);

	void getUncharRow(Row * row, char * out_str, unsigned int *out_len);
	void getCharRow(Row *row, char * out_str);		// for test ,get "sad|2334|...."

	void Rfree(Row *row);
#endif
