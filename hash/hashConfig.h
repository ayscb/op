#ifndef _HASH_CONFIG_H_
#define _HASH_CONFIG_H_

#define TRUE 1
#define FALSE 0
#define max_size_t (( 1 << sizeof(size_t) * 8 ) -1)


// define const value
typedef size_t size_type;
typedef size_t max_type_size;


typedef struct MyStruct
{
	unsigned char key_len;
	unsigned int val_len;
	char * key;
	char * value;
} T;


typedef char* key_type;
typedef T value_type;

typedef T* pointer;
typedef const T* const_pointer;


// define bool value
typedef int BOOLEAN;



#endif