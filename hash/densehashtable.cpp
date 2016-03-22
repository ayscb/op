#include "densehashtable.h"
#include "MurmurHash3.h"

#include <assert.h>		//use assert
#include <stdlib.h>    // use malloc realloc free exit
#include <string.h>      // use memcpy memcmp strlen
#include <stdio.h>	// use printf File


#ifdef DEBUG
#include <time.h>
#endif
// modify

// defalut hash function
size_type hashFun(const key_type key, size_type key_len){
	size_type resu[1];
	if ( key_len == 0)
		MurmurHash3_x86_32(key, strlen(key), 11, resu);
	else
		MurmurHash3_x86_32(key, key_len, 11, resu);
	return resu[0];
}
#ifdef DEBUG
void show_test_hash(dense_hashtable ht, size_type needed_size){
	double e_size_hash = sizeof(dense_hashtable);
	double e_size_buck = ht._num_buckets * sizeof(value_type);
	double e_size_elem = ht.elem_size_total *( 60*2 + 12 * 2);
	double e_size_total = e_size_buck + e_size_elem + e_size_hash ;
	
	printf("\n");
	printf("bucket num: %ld \nelement num: %ld\n", ht._num_buckets, ht._num_elements);
	printf("expect e_size_hash：%lf B\n", e_size_hash);
	printf("expect e_size_buck：%lf M\n", e_size_buck / 1024 / 1024);
	printf("expect e_size_elem：%lf M\n", e_size_elem / 1024 / 1024);
	printf("expect e_size_total：%lf M\n", e_size_total / 1024 / 1024);

	double incr = (needed_size - ht._num_buckets) * sizeof(value_type);
	printf("新增内存:%lf M\n", incr / 1024 /1024);
}
#endif
//******************** actualData**************************
#ifdef N
actualData new_actualData( size_type expectKeySize, size_type expectValSize){
	actualData ad;
	ad._key_max = expectKeySize != 0 ? expectKeySize : BASE_SIZE_MINI;
	ad._val_max = expectValSize != 0 ? expectValSize : BASE_SIZE_MINI;

	ad._base_key_offset = 0;
	ad._base_val_offset = 0;

	ad._key_post = 0;
	ad._val_post = 0;

	ad._key_base = (char*)malloc(ad._key_max);
	ad._val_base = (char*)malloc(ad._val_max);
	memset(ad._key_base, 0, ad._key_max);
	memset(ad._val_base, 0, ad._val_max);

	return ad;
}

void checkAndChangeKey( actualData *obj, size_type lenth)
{
	if (obj->_key_post + lenth >= obj->_key_max)
	{
		char * p = (char*)realloc((char*)obj->_key_base + obj->_base_key_offset, obj->_key_max * 2);
		assert(p != NULL);
		obj->_base_key_offset = p - obj->_key_base;
		obj->_key_max *= 2;
		memset( p + obj->_key_post, 0, obj->_key_max - obj->_key_post);
		//obj->_key_base = p;
	}
}

void checkAndChangeValue( actualData * obj, size_type lenth)
{
	if (obj->_val_post + lenth >= obj->_val_max)
	{
		char * p = (char*)realloc((char*)obj->_val_base + obj->_base_val_offset, obj->_val_max * 2);
		assert(p != NULL);
		obj->_base_val_offset = p - obj->_val_base;
		obj->_val_max *= 2;
		memset(p + obj->_val_post, 0, obj->_val_max - obj->_val_post);
	}
}

void destroy_actualData(actualData *obj){
	char * repk = (char*)obj->_key_base + obj->_base_key_offset;
	char * repv = (char*)obj->_val_base + obj->_base_val_offset;
	if (repk != NULL)
	{
		free(repk);
		obj->_key_base = NULL;
		obj->_key_post = 0;
		obj->_base_key_offset = 0;
	}
	if (repv != NULL)
	{
		free(repv);
		obj->_val_base = NULL;
		obj->_val_post = 0;
		obj->_base_val_offset = 0;
	}
}

value_type get_value_from_ad(actualData * obj, char* key, char* value, size_type key_len, size_type val_len){

	if (obj->_key_max == 0)
		*obj = new_actualData(obj->_key_max * key_len, obj->_val_max * val_len);

	value_type val;
	checkAndChangeKey(obj, key_len);
	checkAndChangeValue(obj, val_len);

	val.key_len = key_len;
	val.val_len = val_len;

	val.key = (char*)obj->_key_base + obj->_base_key_offset + obj->_key_post;
	val.value = (char*)obj->_val_base + obj->_base_val_offset + obj->_val_post;

	memcpy(val.key, key, key_len);
	memcpy(val.value, value, val_len);

	obj->_key_post += key_len;
	obj->_val_post += val_len;
	return val;
}

#endif
//****************** setting **************************

//setting
Settings new_settings(f_hashFun hf,
	const float ht_occupancy_flt,
	const float ht_empty_flt){
	struct Settings sett;
	sett._enlarge_threshold_ = 0;
	sett._shrink_threshold_ = 0;
	//sett._consider_shrink_ = FALSE;
	sett._use_empty_ = FALSE;

	if (ht_occupancy_flt == 0)
		sett._enlarge_factor_ = HT_OCCUPANCY_PCT / 100.0f;
	else
		sett._enlarge_factor_ = ht_occupancy_flt;
	if (ht_empty_flt == 0)
		sett._shrink_factor_ = HT_EMPTY_PCT / 100.0f;
	else
		sett._shrink_factor_ = ht_empty_flt;

	sett._num_ht_copies_ = 0;

	// define the hash function
	if (hf == NULL)
		sett.hashFun = hashFun;
	else
		sett.hashFun = hf;

	//func		
	sett.enlarge_size = get_enlarge_size;
	sett.shrink_size = get_shrink_size;
	sett.inc_num_ht_copies = inc_num_ht_copies;
	sett.reset_thresholds = reset_thresholds;
	sett.set_resizing_parameters = set_resizing_parameters;
	sett.min_buckets = min_buckets;

	return sett;
}


size_type get_enlarge_size(Settings settingObj, size_type x)
{
	return (size_type)(settingObj._enlarge_factor_ * x);
}
size_type get_shrink_size(Settings settingObj, size_type x){
	return (size_type)(settingObj._shrink_factor_ * x);
}
// num_ht_copies is a counter incremented every Copy/Move
void inc_num_ht_copies(Settings *settingObj){
	++settingObj->_num_ht_copies_;
}

// Reset the enlarge and shrink thresholds
void reset_thresholds(Settings *settingObj, size_type num_buckets) {
	settingObj->_enlarge_threshold_ = settingObj->enlarge_size(*settingObj, num_buckets);
	settingObj->_shrink_threshold_ = settingObj->shrink_size(*settingObj, num_buckets);
}

// 当调用 “set_resizing_parameters ”之后应该再调用 “reset_thresholds”。
//     shrink : 收缩因子 （ >=0.0 ; 当=0时 ，不收缩 ）
//     grow	  : 扩展因子  ( <=1.0 ; 当=1时 ，表示空间全部占满才会进行扩容 ）
void set_resizing_parameters(Settings *settingObj, float shrink, float grow) {
	assert(shrink >= 0.0);
	assert(grow <= 1.0);
	if (shrink > grow / 2.0f)
		shrink = grow / 2.0f;     // otherwise we thrash hashtable size
	settingObj->_shrink_factor_ = shrink;
	settingObj->_enlarge_factor_ = grow;
}

// This is the smallest size a hashtable can be without being too crowded
// If you like, you can give a min #buckets as well as a min #elts
size_type min_buckets(Settings settingObj, size_type num_elts, size_type min_buckets_wanted) {
	float enlarge = settingObj._enlarge_factor_;
	size_type sz = HT_MIN_BUCKETS;             // min buckets allowed
	while (sz < min_buckets_wanted ||
		num_elts >= (size_type)(sz * enlarge)) {
		// This just prevents overflowing size_type, 
		// since sz can exceed max_size() here.
		if ((size_type)(sz * 2) < sz) {
			// protect against overflow
			printf("resize overflow");
			exit(0);
		}
		sz *= 2;
	}
	return sz;
}

//****************** iterator **************************

// for iterator
void iter_initFunc(dense_ht_iterator *iter){
	iter->hase_more = hase_more;
	iter->next = next_post;
}

BOOLEAN hase_more(dense_ht_iterator *iter){
	advance_past_empty_and_deleted(iter);
	if (iter->pos == iter->end){
		iter->ht->_is_single_iter = FALSE;
		return FALSE;
	}	
	else{
		return TRUE;
	}
}
value_type* next_post(dense_ht_iterator *iter){
#ifdef N
	value_type *p = iter->pos;

	// single 模式
	if (iter->ht->_is_single_iter == TRUE){
		iter->ht->_post = iter->pos - iter->ht->_table + 1;
	}
	else
	{			
		iter->pos++;	
	}
	p->key = p->key + iter->ht->_acData._base_key_offset;
	p->value = p->value + iter->ht->_acData._base_val_offset;
	return p;

#else
	// single 模式
	if (iter->ht->_is_single_iter == TRUE){
		iter->ht->_post = iter->pos - iter->ht->_table + 1;
		return iter->pos;
	}
	else
	{
		value_type *p = iter->pos;
		iter->pos++;
		return p;
	}
#endif
}

// "Real" constructor and default constructor
dense_ht_iterator new_dense_ht_iterator(
	dense_hashtable *ht,
	pointer it,
	pointer it_end,
	BOOLEAN advance)
{
	dense_ht_iterator iter;
	iter.ht = ht;
	iter.pos = it;
	iter.end = it_end;
	iter_initFunc(&iter);
	if (advance)
		advance_past_empty_and_deleted(&iter);
	return iter;
}



//****************** hashtable **************************

void initFunc(dense_hashtable * ht){
	//func
	ht->begin = begin;
	ht->end = end;
	ht->find = find;
	ht->find_pos = find_pos;
	ht->get_dense_ht_iterator = get_dense_ht_iterator;
	ht->get_single_ht_iterator = get_single_ht_iterator;
	ht->insert = insert;
	ht->set_empty_key = set_empty_key;
	ht->clear = clear;
	ht->destroy_hash = destroy_hash;
}

// defalut :size=0
dense_hashtable new_dense_hashtable(size_type expected_max_items_in_table, Settings st){
	dense_hashtable ht;
	
	ht._settings = st;

	ht._num_elements = 0;
	ht._num_buckets = expected_max_items_in_table == 0 ?
	HT_DEFAULT_STARTING_BUCKETS :
								ht._settings.min_buckets(ht._settings, expected_max_items_in_table, 0);

	ht._table = NULL;
	ht._val_len = 0;

	ht._is_single_iter = FALSE;
	ht._post = 0;

#ifdef DEBUG
	ht.elem_size_total = 0;

	FILE *fw = fopen(resize_file_probe, "w");
	fprintf(fw, "key\t value\t probes_times\t insert_time\t total_bucket \t current_element\t insert_buck\n");
	fclose(fw);
	fw = NULL;

	fw = fopen(find_file,"w");
	fprintf(fw, "key\t num_probes\t probes_time\t total_bucket\t exist\n");
	fclose(fw);
	fw = NULL;

	fw = fopen(insert_file, "w");
	fprintf(fw, "key\t value\t insert_time\t total_bucket \t current_element\t exist\t place\n");
	fclose(fw);
	fw = NULL;
#endif

	ht._settings.reset_thresholds(&ht._settings, ht._num_buckets);

	//func
	initFunc(&ht);
	return ht;
}

// As a convenience for resize(), we allow an optional second argument
// which lets you make this new hashtable a different size than ht
dense_hashtable get_dense_hashtable(dense_hashtable * ht, size_type min_buckets_wanted)
{
	dense_hashtable newObj;

	newObj._settings = ht->_settings;

	if (newObj._settings._use_empty_)
		set_value(&newObj._emptyval, &ht->_emptyval);

	newObj._table = NULL;
	newObj._num_buckets = 0;
	newObj._num_elements = 0;
	newObj._post = 0;

	if (!ht->_settings._use_empty_){
		// If use_empty isn't set, copy_from will crash, so we do our own copying.
		assert(ht->_num_elements == 0);
		newObj._num_buckets
			= newObj._settings.min_buckets(ht->_settings, ht->_num_elements, min_buckets_wanted);
		newObj._settings.reset_thresholds(&newObj._settings, newObj._num_buckets);
		
		#ifdef N
				// 因为election = 0 ，所以 这里key_base 和 val_base 不应该有值
				if (ht->_acData._key_base != NULL )
				{
					free(ht->_acData._key_base);
					ht->_acData._key_base = NULL;
					ht->_acData._base_key_offset = 0;
				}
				if (ht->_acData._key_base != NULL){
					free(ht->_acData._val_base);
					ht->_acData._val_base = NULL;
					ht->_acData._base_val_offset = 0;
				}
				newObj._acData._key_base = (char*)malloc(0);
				newObj._acData._val_base = (char*)malloc(0);
		#endif
		
				initFunc(&newObj);
		return newObj;
	}
	newObj._settings.reset_thresholds(&newObj._settings, newObj._num_buckets);
	initFunc(&newObj);
	#ifdef N
		//这里说明 ht 里面有值，也就是说需要原key_base 和 val_base
		newObj._acData._key_base = ht->_acData._key_base;
		newObj._acData._val_base = ht->_acData._val_base;
		newObj._acData._base_key_offset = ht->_acData._base_key_offset;
		newObj._acData._base_val_offset = ht->_acData._base_val_offset;
	#endif
	// rehash 新的值
	copy_from(&newObj, ht, min_buckets_wanted); 
	return newObj;
}

void resize_copy(dense_hashtable * ht,size_type bucket_num){

	int num = 0;
	pointer newtb = allocate(bucket_num);
	fill_range_with_empty(newtb, newtb + bucket_num, &ht->_emptyval);
	dense_ht_iterator iter = ht->get_dense_ht_iterator(ht);

	while (iter.hase_more(&iter))
	{
		size_type num_probes = 0;              // how many times we've probed
		size_type bucknum;
		const size_type bucket_count_minus_one = ht->_num_buckets - 1;

		value_type *value = (value_type*)iter.next(&iter);

		key_type key = get_key(*value);
		size_type hashcode = ht->_settings.hashFun(key, value->key_len);

#ifdef DEBUG
		FILE *fw = fopen(resize_file_probe, "a+");
		unsigned long start, end;
		start = clock();
#endif
		for (bucknum = hashcode & bucket_count_minus_one;
			!test_empty_point(newtb, bucknum);                               // not empty
			bucknum = (bucknum + JUMP_(key, num_probes)) & bucket_count_minus_one) {
			++num_probes;
			assert(num_probes < bucket_num
				&& "Hashtable is full: an error in key_equal<> or hash<>");
		}
#ifdef DEBUG		
		end = clock();
#ifdef N
		char keys[50]={0};
		char vals[50]={0};
		memcpy(keys,value->key,value->key_len);
		keys[value->key_len] = '\0';
		memcpy(vals,value->value, value->val_len-1);
		vals[value->val_len] = '\0';
		fprintf(fw, "%s\t%s\t%ld\t%ld\t%ld\t\t%ld\t%ld\n", keys, vals, num_probes, end - start, bucket_num, num + 1, bucknum);
		fclose(fw);
		fw=NULL;
#else
		fprintf(fw, "%s\t%s\t%ld\t%ld\t%ld\t\t%ld\t%ld\n", value->key, value->value, num_probes, end - start, bucket_num, num + 1, bucknum);
		fclose(fw);
		fw = NULL;
#endif
		
#endif

#ifdef N
		set_offset_value(newtb + bucknum, value, &ht->_acData);       // copies the value to here
#else
		set_value(newtb + bucknum,value);
#endif
		num++;
	}
	ht->_settings.inc_num_ht_copies(&ht->_settings);

	free(ht->_table);
	ht->_table = newtb;
	ht->_num_buckets = bucket_num;
	ht->_num_elements = num;
}
void resize_hashtable(dense_hashtable * ht, size_type min_buckets_wanted)
{
	if (!ht->_settings._use_empty_){
		// If use_empty isn't set, copy_from will crash, so we do our own copying.
		assert(ht->_num_elements == 0);
		ht->_num_buckets
			= ht->_settings.min_buckets(ht->_settings, ht->_num_elements, min_buckets_wanted);
		ht->_settings.reset_thresholds(&ht->_settings, ht->_num_buckets);
		ht->_table = (pointer)realloc(ht->_table, ht->_num_buckets);
		assert(ht->_table);

#ifdef N
		// 因为election = 0 ，所以 这里key_base 和 val_base 不应该有值
		if (ht->_acData._key_base != NULL)
		{
			free(ht->_acData._key_base);
			ht->_acData._key_base = NULL;
			ht->_acData._base_key_offset = 0;
		}
		if (ht->_acData._key_base != NULL){
			free(ht->_acData._val_base);
			ht->_acData._val_base = NULL;
			ht->_acData._base_val_offset = 0;
		}
		ht->_acData._key_base = (char*)malloc(0);
		ht->_acData._val_base = (char*)malloc(0);
#endif
	}
	else
	{
		size_type new_num_buck =
			min_buckets(ht->_settings, ht->_num_elements, min_buckets_wanted);
		// ht->_num_buckets = new_num_buck; bucket 还要做为后边遍历的终点了，不能修改
		ht->_settings.reset_thresholds(&ht->_settings, new_num_buck);
		assert((new_num_buck & (new_num_buck - 1)) == 0);      // a power of two

		// rehash 新的值
#ifdef DEBUG		
		unsigned long start, end;
		start = clock();
#endif
		resize_copy( ht , new_num_buck);
#ifdef DEBUG
		end = clock();
		FILE *fw = fopen(resize_file_total, "a+");
		fprintf(fw, "resize table  from old bucket :%ld to new bucket:%ld cost time : %ld\n", ht->_num_buckets, new_num_buck,end-start);		
#endif
	}
}


dense_hashtable_iterator begin(dense_hashtable *ht){
	return new_dense_ht_iterator(ht,
		ht->_table,
		ht->_table + ht->_num_buckets,
		TRUE);
}

dense_hashtable_iterator end(dense_hashtable *thisObj){
	return new_dense_ht_iterator(thisObj,
		thisObj->_table + thisObj->_num_buckets,
		thisObj->_table + thisObj->_num_buckets,
		TRUE);
}

// 从某一个位置开始
dense_ht_iterator post(dense_hashtable *ht, size_type _post){
	return new_dense_ht_iterator(ht,
		ht->_table + _post,
		ht->_table + ht->_num_buckets,
		TRUE);
}

dense_hashtable_iterator get_dense_ht_iterator(dense_hashtable *thisObj){
	return begin(thisObj);
}

// 利用 hashtable 中的 post 向后遍历，与调用这个方法无关了。（新增）
dense_ht_iterator get_single_ht_iterator(dense_hashtable *thisObj){

	dense_hashtable_iterator it = post(thisObj, thisObj->_post);

	// 我们开始记录post的位置这里只是一个开始的标志，具体增加是在next方法中。
	thisObj->_is_single_iter = TRUE;

	return it;
}

// This is the normal insert routine, used by the outside world
iterator_bool insert(dense_hashtable *thisObj, value_type *obj, BOOLEAN overwrite) {
	resize_delta(thisObj,1);                      // adding an object, grow if need be
	return insert_noresize(thisObj, obj, overwrite);
}

void set_empty_key(dense_hashtable *ht, value_type *val)
{
	// Once you set the empty key, you can't change it
	assert(!ht->_settings._use_empty_&& "Calling set_empty_key multiple times");
	
	ht->_settings._use_empty_ = TRUE;
	set_value(&ht->_emptyval, val);

	assert(!ht->_table);                  // must set before first use
	// num_buckets was set in constructor even though table was NULL
	ht->_table = allocate(ht->_num_buckets);

	assert(ht->_table);
	fill_range_with_empty(ht->_table, ht->_table + ht->_num_buckets,&ht->_emptyval);


}

dense_ht_iterator find(dense_hashtable thisObj, const key_type key, size_type key_len){
	if (thisObj._num_elements == 0) return thisObj.end(&thisObj);
	position pos = find_position(thisObj, key, key_len);
	if (pos.first == ILLEGAL_BUCKET)     // alas, not there
		return thisObj.end(&thisObj);
	else
		return new_dense_ht_iterator(&thisObj,
		thisObj._table + pos.first,
		thisObj._table + thisObj._num_buckets, false);
}

// new by ayscb
position find_pos(dense_hashtable *thisObj, const key_type key, size_type key_len){
	resize_delta(thisObj, 1);
	position pos = find_position(*thisObj, key, key_len);
	return pos;
}

void clear(dense_hashtable *ht) {
	// If the table is already empty, and the number of buckets is
	// already as we desire, there's nothing to do.
	const size_type new_num_buckets = ht->_settings.min_buckets(ht->_settings,0, 0);
	if (ht->_num_elements == 0 && new_num_buckets == ht->_num_buckets) {
		return;
	}
	clear_to_size(ht,new_num_buckets);
}

void destroy_hash(dense_hashtable *ht){

#ifdef N
	if (ht->_table != NULL)
		free(ht->_table);
	free(ht->_emptyval.key);
	free(ht->_emptyval.value);

	destroy_actualData(&ht->_acData);
	ht->_acData = { 0 };


#else
	if (ht->_num_elements > 0 )
		destroy_buckets(ht->_table, 0, ht->_num_buckets);
#endif
}

//**************************************************************************
// private func

key_type get_key(value_type Obj){
	return Obj.key;
}


// Arithmetic.  The only hard part is making sure that
// we're not on an empty or marked-deleted array element
void advance_past_empty_and_deleted(dense_ht_iterator *iter) {
	while (iter->pos != iter->end  && test_empty_iter(*(iter->ht), *iter))
		++iter->pos;
}

BOOLEAN equals_iter(dense_ht_iterator iter1, dense_ht_iterator iter2){
	if (iter1.pos == iter2.pos)
		return TRUE;
	else
		return FALSE;
}

BOOLEAN equals_keys(key_type key1, key_type key2, size_type key1_len, size_type key2_len){
	if (key1_len != key2_len)
		return FALSE;
	if (memcmp(key1, key2, key1_len) == 0)
		return TRUE;
	else
		return FALSE;
}


#ifdef N
BOOLEAN equals_key_value(key_type key, size_type key_len, value_type val, actualData offset){
	return equals_keys(key, (char*)val.key + offset._base_key_offset, key_len, val.key_len);
}
#else
BOOLEAN equals_key_value(key_type key, size_type key_len, value_type val){
	return equals_keys(key, val.key, key_len, val.key_len);
}
#endif

BOOLEAN test_empty_values(value_type val1, value_type val2){
	return equals_keys(val1.key, val2.key, val1.key_len, val2.key_len);
}

BOOLEAN test_empty_iter(dense_hashtable ht, dense_ht_iterator iter){
	assert(ht._settings._use_empty_);  // we always need to know what's empty!
	value_type val = *(value_type*)iter.pos;
	if (val.key_len == ILLEGAL_SIZE)
		return TRUE;
	else
		return FALSE;
}

BOOLEAN test_empty_post(dense_hashtable thisObj, size_type bucknum){
	assert(thisObj._settings._use_empty_);  // we always need to know what's empty!
	if (thisObj._table[bucknum].key_len == ILLEGAL_SIZE)
		return TRUE;
	else
		return FALSE;
}

BOOLEAN test_empty_point(pointer point, size_type bucknum){
	assert(point != NULL && point + bucknum != NULL);
	value_type val = *(value_type*)(point + bucknum);
	if (val.key == NULL &&val.key_len == ILLEGAL_SIZE)
		return TRUE;
	else
		return FALSE;
}

#ifdef N
void set_offset_value(pointer point, value_type * val, actualData *acdata){
	point->key_len = val->key_len;
	point->val_len = val->val_len;
	point->key = val->key -  acdata->_base_key_offset ;
	point->value = val->value - acdata->_base_val_offset;
}
#endif

// free the already value and point to the new val
void set_value(pointer point, value_type* val){
#ifndef N
	destroy(point);
#endif
	point->key_len = val->key_len;
	point->val_len = val->val_len;
	point->key = val->key;
	point->value = val->value;
}


position find_position(dense_hashtable thisObj, const key_type key, size_type key_len){

#ifdef DEBUG
	unsigned long start, end;
	start = clock();
#endif
	size_type num_probes = 0;              // how many times we've probed(探测)
	const size_type bucket_count_minus_one = thisObj._num_buckets - 1;
	size_type hashcode = hashFun(key, key_len);
	size_type bucknum = hashcode & bucket_count_minus_one;				// get the result in [0,num_buckets - 1]
	size_type insert_pos = ILLEGAL_BUCKET; // where we would insert

	position ps;

	while (1) {                          // probe until something happens		
		if (test_empty_post(thisObj, bucknum)) {         // bucket is empty
			ps.first = ILLEGAL_BUCKET;
			ps.second = bucknum;
#ifdef DEBUG
			end = clock();
			FILE *fw = fopen(find_file, "a+");
			fprintf(fw, "%s\t%ld\t%ld\t%ld\t%s\n", key, num_probes, end - start, thisObj._num_buckets, ps.first == ILLEGAL_BUCKET ? "F" : "T");
			fclose(fw);
			fw = NULL;
#endif
			return ps;
		}
#ifdef N
		else if (equals_key_value(key, key_len, (value_type)thisObj._table[bucknum],thisObj._acData) ){	//already here
#else
		else if (equals_key_value(key, key_len, (value_type)thisObj._table[bucknum])) {	//already here
#endif
			ps.first = bucknum;
			ps.second = ILLEGAL_BUCKET;
#ifdef DEBUG
			end = clock();
			FILE *fw = fopen(find_file, "a+");
			fprintf(fw, "%s\t%ld\t%ld\t%ld\t%s\n", key, num_probes, end - start, thisObj._num_buckets, ps.first == ILLEGAL_BUCKET ? "F" : "T");
			fclose(fw);
			fw = NULL;
#endif
			return ps;
		}
		++num_probes;                        // we're doing another probe
		bucknum = (bucknum + JUMP_(key, num_probes)) & bucket_count_minus_one;
		assert(num_probes < thisObj._num_buckets
			&& "Hashtable is full: an error in key_equal<> or hash<>");
	}
}

// Private method used by "insert_noresize" and "find_or_insert".
dense_ht_iterator insert_at(dense_hashtable *thisObj, value_type * obj, size_type pos) {
	if (thisObj->_num_elements >= max_size()) {
		printf("insert overflow");
		exit(0);
	}
	if (test_empty_post(*thisObj, pos))
		thisObj->_num_elements++;               // replacing an empty bucket

#ifdef N
	set_offset_value(thisObj->_table + pos, obj, &thisObj->_acData);
#else
	set_value(thisObj->_table + pos, obj);
#endif
	
	return new_dense_ht_iterator(thisObj, thisObj->_table + pos, thisObj->_table + thisObj->_num_buckets, TRUE);
}

// 直接插入而不用考虑扩容
iterator_bool insert_noresize(dense_hashtable *thisObj, value_type* obj, BOOLEAN overwrite) {
	// First, double-check we're not inserting delkey or emptyval
#ifdef DEBUG	
	unsigned long start, end;
	start = clock();
#endif

	assert(!thisObj->_settings._use_empty_ || !test_empty_values(*obj, thisObj->_emptyval)
		&& "Inserting the empty key");

	const position pos = find_position(*thisObj, obj->key, obj->key_len);

	iterator_bool iter;
	if (pos.first != ILLEGAL_BUCKET) {      // object was already there
		if (overwrite){
			iter.iter = insert_at(thisObj, obj, pos.first);
			iter.boolean = true;
		}
		else
		{
			iter.iter = new_dense_ht_iterator(thisObj,
				thisObj->_table + pos.first,
				thisObj->_table + thisObj->_num_buckets,
				TRUE);
			iter.boolean = false;	// false: we didn't insert
		}
	}
	else {   // pos.second says where to put it
		iter.iter = insert_at(thisObj, obj, pos.second);
		iter.boolean = true;
	}
	
#ifdef DEBUG
	end = clock();
	FILE *fw = fopen(insert_file, "a+");	
	//fprintf(fw, "key\t value\t insert_time\t total_bucket \t current_element\t exist\t place\n");
	fprintf(fw, "%s\t%s\t\t%ld\t%ld\t%ld\t%s\t%d\n",
		obj->key, obj->value, end - start, thisObj->_num_buckets, thisObj->_num_elements,
		pos.first == ILLEGAL_BUCKET ? "F" : "T",
		pos.second == ILLEGAL_BUCKET ? 0 : pos.second);

	fclose(fw);
	fw = NULL;
#endif
	return iter;
}

// We'll let you resize a hashtable -- though this makes us copy all!
// When you resize, you say, "make it big enough for this many more elements"
// Returns true if we actually resized, false if size was already ok.
BOOLEAN resize_delta(dense_hashtable *thisObj, size_type delta) {
	BOOLEAN did_resize = FALSE;

	if (thisObj->_num_elements >= max_size_t - delta ){
		printf("resize overflow");
		exit(0);
	}
	if (thisObj->_num_buckets >= HT_MIN_BUCKETS &&
		(thisObj->_num_elements + delta) <= thisObj->_settings._enlarge_threshold_)
		return did_resize;                          // we're ok as we are

	const size_type needed_size =
		thisObj->_settings.min_buckets(thisObj->_settings, thisObj->_num_elements + delta, 0);
#ifdef DEBUG
	show_test_hash(*thisObj, needed_size);
#endif
	assert(needed_size > thisObj->_num_buckets && "here should be ' > ' ");

	resize_hashtable(thisObj, needed_size);
	return TRUE;

	/*dense_hashtable tmp = get_dense_hashtable(thisObj, needed_size);

	swap(thisObj, tmp);                             // now we are tmp
	return TRUE;*/
}

// 将 ht 中的值hash 到 newObj中
void copy_from(dense_hashtable *newObj,  dense_hashtable *src, size_type min_buckets_wanted) {
	size_type new_num_buck = 
		min_buckets(newObj->_settings, src->_num_elements, min_buckets_wanted);
	clear_to_size(newObj, new_num_buck);

	// We use a normal iterator to get non-deleted bcks from ht
	// We could use insert() here, but since we know there are
	// no duplicates and no deleted items, we can be more efficient

	assert((newObj->_num_buckets & (newObj->_num_buckets - 1)) == 0);      // a power of two
 
	dense_ht_iterator iter = src->get_dense_ht_iterator(src);
	while (iter.hase_more(&iter))
	{
		size_type num_probes = 0;              // how many times we've probed
		size_type bucknum;
		const size_type bucket_count_minus_one = newObj->_num_buckets - 1;

		value_type *value = (value_type*)iter.next(&iter);
		key_type key = get_key(*value);

		size_type hashcode = newObj->_settings.hashFun(key, value->key_len);
		for (bucknum = hashcode & bucket_count_minus_one;
			!test_empty_post(*newObj, bucknum);                               // not empty
			bucknum = (bucknum + JUMP_(key, num_probes)) & bucket_count_minus_one) {
			++num_probes;
			assert(num_probes < newObj->_num_buckets
				&& "Hashtable is full: an error in key_equal<> or hash<>");
		}
#ifdef N
		newObj->_acData = src->_acData;
		set_offset_value(newObj->_table + bucknum, value,&src->_acData);
#else
		set_value(newObj->_table + bucknum, value);       // copies the value to here
#endif
		newObj->_num_elements++;
	}
	newObj->_settings.inc_num_ht_copies(&newObj->_settings);
}

//  将newObj 中的_table rellocate新的大小
void clear_to_size(dense_hashtable *newObj, size_type new_num_buckets) {
	
	if (!newObj->_table) {
		newObj->_table = allocate(new_num_buckets);
	}
	else {
		#ifndef N
			destroy_buckets(newObj->_table,0, newObj->_num_buckets);
		#endif
		if (new_num_buckets != newObj->_num_buckets) {   // resize, if necessary
			resize_table(*newObj,newObj->_num_buckets, new_num_buckets);
		}
	}
	assert(newObj->_table && newObj->_settings._use_empty_);
	fill_range_with_empty(newObj->_table, newObj->_table + new_num_buckets, &newObj->_emptyval);

	newObj->_num_elements = 0;
	newObj->_num_buckets = new_num_buckets;          // our new size
	newObj->_settings.reset_thresholds(&newObj->_settings,newObj->_num_buckets);
	
}
void destroy_buckets(pointer p, size_type start, size_type end){
	while (start != end)
	{
		destroy(p + start);
		start++;
	}
}
// used by 'clear_to_size' . 这里删除_table ，并重新malloc
void resize_table(dense_hashtable Obj, size_type /*old_size*/, size_type new_size) {
	deallocate(Obj._table, NULL);
	Obj._table = allocate(new_size);
}

void fill_range_with_empty( pointer start, pointer end , value_type * emptyval){
	while (start != end){
		start->key_len = emptyval->key_len;
		start->val_len = emptyval->val_len;
		start->key = emptyval->key;
		start->value = emptyval->value;
		start++;
	}
}

//将 src_ht 中的成员变量全部复制到dest中。其中table 和 empty 复制的是指针
void swap(dense_hashtable *dest_ht, dense_hashtable src_ht){

	dest_ht->_num_buckets = src_ht._num_buckets;
	dest_ht->_num_elements = src_ht._num_elements;
	dest_ht->_table = src_ht._table;

	dest_ht->_settings = src_ht._settings;
	dest_ht->_post = src_ht._post;

	if (src_ht._settings._use_empty_)
		dest_ht->_emptyval = src_ht._emptyval;

	dest_ht->_settings.reset_thresholds(&dest_ht->_settings, dest_ht->_num_buckets);  // also resets consider_shrink

}

//************************* malloc/free **************************

pointer allocate(size_type n){
	return (pointer)malloc(sizeof(value_type) * n);
}

pointer reallocate(pointer p, size_type n){
	return (pointer)realloc(p, sizeof(value_type) * n);
}

void deallocate(pointer p, size_type) {
	free(p);
}

size_type max_size()  {
	return (size_type)(-1) / sizeof(value_type);
}

// 这个是释放p指向的对象里的东西
void destroy(pointer p){
	if (p->key != NULL)
	{
		assert(p->key_len > 0 && "the destroyd key's size should not be 0 ,when the point is not NULL");
		free(p->key);
		p->key = NULL;
	}
	if (p->value != NULL)
	{
		free(p->value);
		p->value = NULL;
	}
		
}

