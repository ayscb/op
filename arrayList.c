
#include "arrayList.h"
#include <stdlib.h>
#include <assert.h>
ValueArray new_ValueArray(int expectNumSize){
	ValueArray val;
	val._max_num = expectNumSize;
	val._ele_num = 0;
	val._post = 0;
	val._value = (value_type*)malloc(sizeof(value_type) * expectNumSize);


	val.destroy = destroy_V;
	val.insert = insert_V;
	return val;
}

void checkSpace(ValueArray *obj, size_type incr_num);

#ifdef N
void insert_V(ValueArray * obj, value_type value, actualData acdata){
	checkSpace(obj, 1);
	value.key -= acdata._base_key_offset;
	value.value -= acdata._base_val_offset;
	obj->_value[obj->_ele_num] = value;
	obj->_ele_num++;
}
#else
void insert_V(ValueArray * obj, value_type value)
{
	checkSpace(obj, 1);
	obj->_value[obj->_ele_num] = value;
	obj->_ele_num++;
}
#endif

void destroy_V(ValueArray * obj){
	destroy_actualData(&obj->_acData);
	obj->_acData = { 0 };
	if (obj->_value != NULL){
		free(obj->_value);
	}
}

void checkSpace(ValueArray *obj,size_type incr_num){
	if (obj->_ele_num + incr_num > obj->_max_num){
		obj->_value = (value_type*)realloc(obj->_value, obj->_max_num * 2);
		assert(obj->_value != NULL && "the realloc space is error");
		obj->_max_num *= 2;
	}
}