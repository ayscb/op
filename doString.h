#ifndef __aggregation__doString__
#define __aggregation__doString__



// ���Ҵ� startλ�ÿ�ʼ�� str�� ��һ�γ��� lime��λ��
int getLimePosition(char* str, char lime, int start);

// for "ssss\0ssss\0addd\0" --> ssss 
// index : ��ȡ�ڼ����ַ���   
// maxSize: һ���м����ַ���
char* getStringFromCharArray(const char*str, int index, int maxSize);

// sum(1) --> sum 1
// ������
char** getStringFromSchem(char * str);

void substring(char *dest, const char *src, int start, int end);

void trim(char * str);


// private
void _trim(char *str, char sign);
void lower_str(char * src);
#endif