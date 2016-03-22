#include "doString.h"
#include <string.h>
#include <stdlib.h>

//function : 123|45|6789|  start=0 --> 3 ; start = 4 --> 6
// note : 从0开始
// return : -1 , >=0
int getLimePosition( char* str, char lime, int start){
	char *point = str + start;
	char *q = strchr(point, lime);
	if (q)
		return q - str;
	else
		return -1;
}


// function : "ssss\0ssss\0addd\0" --> ssss 
// note : 这里申请了一个空间返回子串，需要释放
// args : index   --> 获取第几组字符串   
// args : maxSize --> 一共有几组字符串
// return : char* 子串
char* getStringFromCharArray(const char*str, int index,int maxSize){
	if (index >= maxSize)
		return NULL;
	const char * p = str;
	if (index == 0)
		return strdup(p);

	int point = 0;
	int i = 0;
	while (i != index && i < maxSize)
	{
		point = strlen(p);
		p += point + 1;
		i++;
	}
	if (index == maxSize)
		return NULL;
	else
		return strdup(p);
}

// function : sum(1) --> sum ,1
// note : 这里申请了两个空间，需要释放
// call : free(tem2[0]);free(tem2[1]);
// return : char** 字符串数组 -->char[0] char[1]
char** getStringFromSchem(char * str ){
	char delim1 = '(';
	char delim2 = ')';
	char *first = strchr(str, delim1);
	char *second = strchr(str, delim2);
	char * a[2];
	a[0] = (char*)malloc(sizeof(char) * (first - str));
	a[1] = (char*)malloc(sizeof(char) * (second - first));
	substring(a[0], str, 0, (first - str));
	substring(a[1], str, (first - str)+1, (second - str));
	return a;
}

// function : substring
// note : dest 需要提前申请
// args : start (包含) end (不包含) dest:已申请空间
void substring(char *dest, const char *src, int start, int end )
{
	int i = start;
	if ( start > strlen(src) )
		return;
	if ( end > strlen(src) )
		end = strlen(src);
	while ( i < end )
	{
		dest[i - start] = src[i];
		i++;
	}
	dest[i - start] = '\0';
	return;
}

// get lower string
// src 不能为常量
void lower_str(char * src){
	int str_len = strlen(src);
	for (int i = 0; i < str_len; i++){
		if (src[i] > 'A' && src[i] < 'Z')
		{
			src[i] += 32;
		}
	}
}

// " sdf "  ==> "sdf"
void trim(char * str){
	_trim(str, ' ');
	_trim(str, '\'');
	_trim(str, '\"');
}

void _trim(char *str, char sign){
	int s1 = 0, s2 = 0;
	int len = strlen(str);
	while (str[s1] == sign)	s1++;
	memmove(str, str + s1, strlen(str) - s1);
	str[len - s1] = NULL;

	s2 = strlen(str);
	while (str[s2] == sign || str[s2] == '\0') s2--;
	str[s2 + 1] = NULL;
}
