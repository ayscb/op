#ifndef __aggregation__doString__
#define __aggregation__doString__



// 查找从 start位置开始的 str中 第一次出现 lime的位置
int getLimePosition(char* str, char lime, int start);

// for "ssss\0ssss\0addd\0" --> ssss 
// index : 获取第几组字符串   
// maxSize: 一共有几组字符串
char* getStringFromCharArray(const char*str, int index, int maxSize);

// sum(1) --> sum 1
// 有疑问
char** getStringFromSchem(char * str);

void substring(char *dest, const char *src, int start, int end);

void trim(char * str);


// private
void _trim(char *str, char sign);
void lower_str(char * src);
#endif