#ifndef _DENSE_HASHTABLE_H_
#define _DENSE_HASHTABLE_H_

#include "hashConfig.h"

#define NDEBUG   // use for print debug infomation
#define N		// N use new method ( malloc a big memory)

// The probing method  ( 探索方法 )
// Linear probing   (线性探索)
// #define JUMP_(key, num_probes)    ( 1 )

// Quadratic probing    (二次探索)
#define JUMP_(key, num_probes)    ( num_probes )

typedef struct pair pair;
typedef struct position position;
typedef struct iter_bool iterator_bool;

typedef struct Settings Settings;

typedef struct dense_hashtable_iterator dense_ht_iterator;
typedef struct dense_hashtable dense_hashtable;

// define the data struct about pair<T,T>
//***********************************************************************

// store the position
struct position{
	size_type first;
	size_type second;
};

#ifdef N
typedef struct _actualData{
	unsigned long _base_key_offset;
	unsigned long _base_val_offset;

	char* _key_base;
	char * _val_base;

	unsigned long _key_post;
	unsigned long _val_post;

	unsigned long _key_max;	//malloc(key_max)
	unsigned long _val_max;	//malloc(val_max)


} actualData;
actualData new_actualData(size_type expectKeySize, size_type expectValSize);
void checkAndChangeKey(actualData *obj, size_type lenth);
void checkAndChangeValue(actualData * obj, size_type lenth);
value_type get_value_from_ad(actualData * obj, char* key, char* value, size_type key_len, size_type val_len);
void destroy_actualData(actualData *obj);

#endif

//define the table

//************************ settings *****************************
// define the hashFun used by setting
typedef size_type(*f_hashFun)(const key_type key, size_type key_len);

// settings
struct Settings{	// shrink 收缩
	size_type _enlarge_threshold_;  // table.size() * enlarge_factor
	size_type _shrink_threshold_;   // table.size() * shrink_factor
	float _enlarge_factor_;         // how full before resize
	float _shrink_factor_;          // how empty before resize

	BOOLEAN _use_empty_;    // used only by densehashtable, not sparsehashtable
	unsigned int _num_ht_copies_;	// num_ht_copies is a counter incremented every Copy/Move , should +1 when every Copy/Move

	f_hashFun hashFun;
	
	// public fun
	
	size_type (*enlarge_size)(Settings settingObj, size_type x);
	size_type(*shrink_size)(Settings settingObj, size_type x);

	// num_ht_copies is a counter incremented every Copy/Move
	void(*inc_num_ht_copies)(Settings *settingObj);
	
	// Reset the enlarge and shrink thresholds
	void (*reset_thresholds)(Settings *settingObj, size_type num_buckets);

	// Caller is resposible for calling reset_threshold right after
	// set_resizing_parameters.
	void(*set_resizing_parameters)(Settings *settingObj, float shrink, float grow);

	// This is the smallest size a hashtable can be without being too crowded
	// If you like, you can give a min #buckets as well as a min #elts
	size_type (*min_buckets)(Settings settingObj, size_type num_elts, size_type min_buckets_wanted);
};

Settings new_settings(f_hashFun hf, const float ht_occupancy_flt, const float ht_empty_flt);

size_type get_enlarge_size(Settings settingObj, size_type x);
size_type get_shrink_size(Settings settingObj, size_type x);
// num_ht_copies is a counter incremented every Copy/Move
void inc_num_ht_copies(Settings *settingObj);

// Reset the enlarge and shrink thresholds
void reset_thresholds(Settings *settingObj, size_type num_buckets);

// Caller is resposible for calling reset_threshold right after
// set_resizing_parameters.
void set_resizing_parameters(Settings *settingObj, float shrink, float grow);

// This is the smallest size a hashtable can be without being too crowded
// If you like, you can give a min #buckets as well as a min #elts
size_type min_buckets(Settings settingObj, size_type num_elts, size_type min_buckets_wanted);

//************************ iterator *****************************

// for iterator
struct dense_hashtable_iterator
{
	dense_hashtable * ht;
	pointer pos, end;

	BOOLEAN(*hase_more)(dense_ht_iterator *iter);
	value_type* (*next)(dense_ht_iterator * iter);
};
BOOLEAN hase_more(dense_ht_iterator *iter);
value_type* next_post(dense_ht_iterator *iter);

// "Real" constructor and default constructor
dense_ht_iterator new_dense_ht_iterator(
	dense_hashtable *ht,
	pointer it,
	pointer it_end,
	BOOLEAN advance);

// store the <iterator,bool>
struct iter_bool{
	dense_ht_iterator iter;
	BOOLEAN boolean;
};

//************************ hashtable *****************************
struct dense_hashtable
{
	// Actual data
	Settings _settings;
	value_type _emptyval;       // holds emptyval, and also the allocator [changes]
	
	size_type _num_elements;
	size_type _num_buckets;
	
	pointer _table;
	BOOLEAN _is_single_iter;	// if nor not a single iter ( for single method )
	size_type _post;			// record the position when we iterator the table ( for single method )
	size_type _val_len;		// for us ,value is fixed length

#ifdef DEBUG
	size_t elem_size_total;
#endif

	#ifdef N
	actualData _acData;
	#endif 

	dense_hashtable_iterator(*begin)(dense_hashtable *thisObj);
	dense_hashtable_iterator(*end) (dense_hashtable *thisObj);
	dense_hashtable_iterator(*get_dense_ht_iterator)( dense_hashtable *thisObj);
	dense_hashtable_iterator(*get_single_ht_iterator)(dense_hashtable *thisObj);

	iterator_bool (*insert)(dense_hashtable *thisObj, value_type *obj, BOOLEAN overwrite);
	void(*set_empty_key)(dense_hashtable *ht, value_type *val);
	dense_ht_iterator(*find)(dense_hashtable thisObj, const key_type key, size_type key_len);
	position (*find_pos)(dense_hashtable *thisObj, const key_type key, size_type key_len);
	void (*clear)(dense_hashtable * ht);
	void (*destroy_hash)(dense_hashtable *ht);
};

// defalut :size=0
dense_hashtable new_dense_hashtable(size_type expected_max_items_in_table, Settings st);

// As a convenience for resize(), we allow an optional second argument
// which lets you make this new hashtable a different size than ht
dense_hashtable get_dense_hashtable(dense_hashtable * ht, size_type min_buckets_wanted);

dense_hashtable_iterator begin(dense_hashtable *ht);
dense_hashtable_iterator end(dense_hashtable *thisObj);
dense_hashtable_iterator get_dense_ht_iterator( dense_hashtable *thisObj);
dense_ht_iterator get_single_ht_iterator(dense_hashtable *thisObj);
iterator_bool insert(dense_hashtable *thisObj, value_type *obj, BOOLEAN overwrite);
void set_empty_key(dense_hashtable *ht, value_type *val);
dense_ht_iterator find(dense_hashtable thisObj, const key_type key, size_type key_len);
position find_pos(dense_hashtable *thisObj, const key_type key, size_type key_len);
void clear(dense_hashtable *ht);
void destroy_hash(dense_hashtable *ht);



//************************ private func *****************************

//// used for method 'new_dense_ht_iterator'
key_type get_key(value_type Obj);

void advance_past_empty_and_deleted(dense_ht_iterator *iter);
BOOLEAN equals_iter(dense_ht_iterator iter1, dense_ht_iterator iter2);

BOOLEAN equals_keys(key_type key1, key_type key2, size_type key1_len, size_type key2_len);

#ifdef N
BOOLEAN equals_key_value(key_type key, size_type key_len, value_type val, actualData Offset);
void set_offset_value(pointer point, value_type* val, actualData *acData);		// when insert a new values ,we use it
#else
BOOLEAN equals_key_value(key_type key, size_type key_len, value_type val);
#endif
void set_value(pointer point, value_type* val);

BOOLEAN test_empty_values(value_type val1, value_type val2);
BOOLEAN equals_key_value(key_type key, size_type key_len, value_type val);
BOOLEAN equals_values(value_type val1, value_type val2);
BOOLEAN test_empty_iter(dense_hashtable ht, dense_ht_iterator iter);
BOOLEAN test_empty_post(dense_hashtable thisObj, size_type bucknum);
BOOLEAN test_empty_point(pointer point, size_type bucknum);


position find_position(dense_hashtable thisObj, const key_type key, size_type key_len);
dense_ht_iterator insert_at(dense_hashtable *thisObj, value_type * obj, size_type pos);
iterator_bool insert_noresize(dense_hashtable *thisObj, value_type* obj, BOOLEAN overwrite);
BOOLEAN resize_delta(dense_hashtable *thisObj, size_type delta);
void copy_from(dense_hashtable *newObj,dense_hashtable *src,size_type min_buckets_wanted);
void clear_to_size(dense_hashtable *newObj, size_type new_num_buckets);
void destroy_buckets(pointer p, size_type start, size_type end);
void resize_table(dense_hashtable Obj, size_type /*old_size*/, size_type new_size);
void fill_range_with_empty(pointer start, pointer end, value_type * emptyval);
void swap(dense_hashtable *dest_ht, dense_hashtable src_ht);


pointer allocate(size_type n);
pointer reallocate(pointer p, size_type n);
void deallocate(pointer p, size_type);
size_type max_size();
void destroy(pointer p);

#ifdef DEBUG
void show_test_hash(dense_hashtable ht, size_type needed_size);
#endif
//************************ static val *****************************
// How full we let the table get before we resize, by default.
// Knuth says .8 is good -- higher causes us to probe too much,
// though it saves memory.
static const int HT_OCCUPANCY_PCT = 70;  // defined at the bottom of this file

// How empty we let the table get before we resize lower, by default
// (0.0 means never resize lower.)
// It should be less than OCCUPANCY_PCT / 2 or we thrash resizing
static const int HT_EMPTY_PCT = 0.4 * 70;      // defined at the bottom of this file

// Minimum size we're willing to let hashtables be.
// Must be a power of two, and at least 4.
// Note, however, that for a given hashtable, the initial size is a
// function of the first constructor arg, and may be >HT_MIN_BUCKETS.
static const int HT_MIN_BUCKETS = 8;

// By default, if you don't specify a hashtable size at
// construction-time, we use this size.  Must be a power of two, and
// at least HT_MIN_BUCKETS.
static const int HT_DEFAULT_STARTING_BUCKETS = 320;

static const size_type ILLEGAL_BUCKET = -1;

#ifdef N
static const unsigned char ILLEGAL_SIZE = -1;
static const size_type BASE_SIZE_MINI = 10 * 1024 * 1024;	//init actual data size 10M
static const size_type BASE_SIZE_INC = 10 * 1024 * 1024;
#endif

#ifdef DEBUG
static const char * find_file = "D://find.txt";
static const char * resize_file_total = "D://resize.txt";
static const char * resize_file_probe = "D://resize_probe.txt";
static const char* insert_file = "D://insert.txt";
#endif
#endif