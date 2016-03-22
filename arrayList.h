#ifndef _ARRAY_LIST_H
#define _ARRAY_LIES_H
#include "hash\densehashtable.h"
#include "hash\hashConfig.h"

typedef struct ValueArray{
	size_type _max_num;
	size_type _ele_num;
	pointer _value;
	size_type _post;		//store the position when we iterat the data

#ifdef N
	actualData _acData;
	void(*insert)(ValueArray * obj, value_type value, actualData acdata);
#else
	void(*insert)(ValueArray * obj, value_type value);
#endif

	void(*destroy)(ValueArray * obj);
}ValueArray;

ValueArray new_ValueArray(int expectSize);
#ifdef N
void insert_V(ValueArray * obj, value_type value, actualData acdata);
#else
void insert_V(ValueArray * obj, value_type value );
#endif
void destroy_V(ValueArray * obj);

#endif