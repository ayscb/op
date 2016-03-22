#ifndef __aggregation__Vector__
#define __aggregation__Vector__
//#include "DataType.h"

#define DEFALUT_SIZE 1024
#define INCREASE_SIZE 512
#define TRUE 1
#define FALSE 0

// 说明：调用get方法返回的是base数组的指针，默认是void*。需要进行转化
//support for int string DataType AggFunc 
typedef struct _Vector
{
	char  _type[20];		//save type
	char *_base;			//save data
	int _index,_limit, _capality;
	int _Num;
	int _stringPoint[100];
	int _stringCurrIndex;	//for next method which is record the index;

	void (*f_push_back)(struct _Vector *, const void *);	
	void * (*f_get_value)(const struct _Vector *, int);
	int(*f_empty)(const struct _Vector *);
	int(*f_size)(const struct _Vector *);
	int(*f_hasMore)( struct _Vector *);
	void * (*f_next)( struct _Vector *);
	void(*f_free)(struct _Vector *);
} Vector;

Vector * new_Vector(const char * _typename);
void push_bace(Vector *vector, const void * data);	// add the data into the last position
void * get_value(const Vector *vector, int index);
int empty(const Vector *vector);
int size(const Vector * vector);  // the number of data
int hasMore( Vector *vector);
void * next( Vector * vector);
void * pop_back(Vector * vector);	//delete the last element and return this element
void f_free(Vector * vector);
#endif
