#ifndef _QUSORT_H_
#define _QUSORT_H_
#include "Vector.h"

/* �ӿ������ٶȵ��Ż�ѡ�� */  
#pragma optimize("t", on)  

typedef int(*com) (const void * _key1, const void * _key2, Vector clmAE, Vector clmIdLen);

static void __cdecl shortsort(char *lo, char *hi, size_t width, com comp, Vector clmAE, Vector clmIdLen);
static void __cdecl swap(char *p, char *q, size_t width);

#define CUTOFF 8    /* testing shows that this is good value */ 

static void __cdecl shortsort(char *lo, char *hi, size_t width, com comp,Vector clmAE, Vector clmIdLen)
{
	char *p, *max;
	/* Note: in assertions below, i and j are alway inside original bound of array to sort. */
	while (hi > lo) {
		max = lo;
		/*�������forѭ�������Ǵ�lo��hi��Ԫ���У�ѡ������һ����maxָ��ָ����������*/
		for (p = lo + width; p <= hi; p += width) {
			if (comp(p, max,clmAE,clmIdLen) > 0) {
				max = p;
			}
		}
		/*�����������hiָ������򽻻�*/
		swap(max, hi, width);
		/*hi��ǰ�ƶ�һ��ָ�롣������һ������hi��������Ѿ��ź���ı�δ���򲿷����е���Ҫ�������*/
		hi -= width;
	}
}


/*�������swap������
��������Ƚϼ򵥣����ǽ���������Ĳ�������������ָ����ʵ�ֵġ�
*/

static void __cdecl swap(
	char *a,
	char *b,
	size_t width
	)
{
	char tmp;
	if (a != b)
		/* Do the swap one character at a time to avoid potential alignment
		problems. */
		while (width--) {
		tmp = *a;
		*a++ = *b;
		*b++ = tmp;
		}
}


/*����������Ҫ�Ĳ��֣�qsort������*/
/*ʹ�õ��Ƿǵݹ鷽ʽ������������һ���Զ����ջʽ�ṹ���������������ջ�Ĵ�С
*/

#define STKSIZ (8*sizeof(void*) - 2)  

void __cdecl qusort(void *base, size_t num, size_t width, com comp, Vector clmid, Vector clmAE, Vector clmIdLen)
{
	/*����ʹ����ĳЩ���ɣ�����ὲ������ʹ��ջ��С�����󲻻����1+log2(num)�����30��ջ��СӦ�����㹻�ˡ�Ϊʲô˵��30�أ�
	��ʵ������STKSIZ�Ķ����п��Լ����sizeof(void*)��4������8*4-2��30*/
	char *lo, *hi;              /* ends of sub-array currently sorting   �����������ָ�룬����ָ��������Ͻ���½�*/
	char *mid;                  /* points to middle of subarray  ������м���ָ��*/
	char *loguy, *higuy;        /* traveling pointers for partition step  ѭ���е��ζ�ָ��*/
	size_t size;                /* size of the sub-array  ����Ĵ�С*/
	char *lostk[STKSIZ], *histk[STKSIZ];
	int stkptr;                 /* stack for saving sub-array to be processed  ջ��ָ��*/

	/*���ֻ��һ�������µ�Ԫ�أ����˳�*/
	if (num < 2 || width == 0)
		return;                 /* nothing to do */

	stkptr = 0;                 /* initialize stack */

	lo = (char*)base;
	hi = (char *)base + width * (num - 1);        /* initialize limits */

	/*�����ǩ��α�ݹ�Ŀ�ʼ*/
recurse:

	size = (hi - lo) / width + 1;        /* number of el's to sort */
	/*��sizeС��CUTOFFʱ��ʹ��ѡ�������㷨����*/
	if (size <= CUTOFF) {
		shortsort(lo, hi, width, comp,clmAE,clmIdLen);
	}
	else {
		/*��������Ҫѡ��һ��������㷨�ĸ�Ч��Ҫ�������ҵ�һ�����������м�ֵ
		���������Ҫ��֤�ܹ��ܿ��ҵ���������ѡ������ĵ�һ��м�������һ�����
		��ֵ�������������µĵ�Ч�ʡ����Ա�����ѡ�����������м�ֵ���ȵ���ѡ������
		���м����Ч��Ҫ�ߡ�

		���ǽ���һ��ΪʲôҪ�����������������⡣�������£����������㷨
		������ʱ�临�Ӷ���O(n^2)�����������һ���������Ѿ�������ļ����������ѡ����
		��һ������Ϊ�����Ҳ���������������е��������Ƿ����Ľ���Ƿֳ���һ����
		СΪN-1�������һ����СΪ1�����飬�����Ļ���������Ҫ�ıȽϴ�����N + N-1 + N-2
		+ N-3 +...+2+1=(N+1)N/2=O(n^2)�������ѡ��ǰ �� �����������м�ֵ������������
		����Ҳ�ܹ��õ��ܺõĴ���*/

		mid = lo + (size / 2) * width;      /* find middle element */
		/*��һ�� �м��� �����������Ԫ������*/

		/* Sort the first, middle, last elements into order */
		if (comp(lo, mid,clmAE, clmIdLen) > 0) {
			swap(lo, mid, width);
		}
		if (comp(lo, hi,clmAE, clmIdLen) > 0) {
			swap(lo, hi, width);
		}
		if (comp(mid, hi, clmAE, clmIdLen) > 0) {
			swap(mid, hi, width);
		}

		/*����Ҫ��������������飬һ����С�ڷ�����ģ�һ���ǵ��ڷ�����ģ�����һ���Ǵ��ڷ�����ġ�*/
		/*�����ʼ����loguy �� higuy����ָ�룬����ѭ���������ƶ���ָʾ��Ҫ����������Ԫ�صġ�
		higuy�ݼ���loguy���������������forѭ�����ǿ�����ֹ��*/

		loguy = lo; /* traveling pointers for partition step  ѭ���е��ζ�ָ��*/
		higuy = hi; /* traveling pointers for partition step  ѭ���е��ζ�ָ��*/

		/* Note that higuy decreases and loguy increases on every iteration,
		so loop must terminate. */
		for (;;) {
			/*��ʼ�ƶ�loguyָ�룬ֱ��A[loguy]>A[mid]*/
			if (mid > loguy) {
				do  {
					loguy += width;
				} while (loguy < mid && comp(loguy, mid, clmAE, clmIdLen) <= 0);
			}

			/*����ƶ���loguy>=mid��ʱ�򣬾ͼ�������ƶ���ʹ��A[loguy]>a[mid]��
			��һ��ʵ�������þ���ʹ���ƶ���loguy֮��loguyָ��֮ǰ��Ԫ�ض��ǲ����ڻ���ֵ��Ԫ�ء�*/
			if (mid <= loguy) {
				do  {
					loguy += width;
				} while (loguy <= hi && comp(loguy, mid,clmAE, clmIdLen) <= 0);
			}

			/*ִ�е������ʱ��loguyָ��֮ǰ�����A[mid]ҪС���ߵ�����*/

			/*�����ƶ�higuyָ�룬ֱ��A[higuy]<��A[mid]*/
			do  {
				higuy -= width;
			} while (higuy > mid && comp(higuy, mid, clmAE, clmIdLen) > 0);

			/*�������ָ�뽻���ˣ����˳�ѭ����*/

			if (higuy < loguy)
				break;

			/* ��ʱA[loguy]>A[mid],A[higuy]<=A[mid],loguy<=hi��higuy>lo��*/
			/*��������ָ��ָ���Ԫ��*/
			swap(loguy, higuy, width);

			/* If the partition element was moved, follow it.  Only need
			to check for mid == higuy, since before the swap,
			A[loguy] > A[mid] implies loguy != mid. */

			/*�������Ԫ�ص�λ���ƶ��ˣ�����Ҫ��������

			��Ϊ��ǰ���loguy���������ѭ���еĵڶ���ѭ���Ѿ���֤��loguy>mid,

			��loguyָ�벻��midָ����ȡ�

			��������ֻ��Ҫ��һ��higuyָ���Ƿ����midָ�룬

			���ԭ����mid==higuy�����ˣ���ô�����ղŵĽ������м�ֵ���Ѿ�����

			loguyָ���λ�ã�ע�⣺�ղ���ֵ�����ˣ����ǲ�û�н���ָ�롣��higuy��mid��ȣ�����higuy��loguyָ������ݣ�higuy��Ȼ����mid����������mid��loguy�����¸����м�ֵ��*/

			if (mid == higuy)
				mid = loguy;

			/* A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
			of loop is re-established */

			/*���ѭ��һֱ���е�����ָ�뽻��Ϊֹ*/
		}

		/*     A[i] <= A[mid] for lo <= i < loguy,
		A[i] > A[mid] for higuy < i < hi,
		A[hi] >= A[mid]
		higuy < loguy
		implying:
		higuy == loguy-1
		or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid] */

		/*��һ��ѭ������֮����Ϊ��û��ִ��loguyָ���higuyָ�����ݵĽ���������loguyָ���ǰ�������Ԫ�ض������ڻ���ֵ����higuyָ��֮�������Ԫ�ض����ڻ���ֵ�����Դ�ʱ�����������

		1)  higuy��loguy��1

		2)  higuy��hi��1��loguy��hi+1

		���еڶ������������һ��ʼѡ������Ԫ�ص�ʱ��hiָ���Ԫ�غ�midָ���Ԫ��ֵ��ȣ���hiǰ���Ԫ��ȫ���������ڻ���ֵ��ʹ���ƶ�loguyָ���ʱ��һֱ�ƶ�����hi+1��ֹͣ�����ƶ�higuyָ���ʱ��higuyָ���ƶ�һ����ֹͣ�ˣ�ͣ��hi��1����

		*/

		/* Find adjacent elements equal to the partition element.  The
		doubled loop is to avoid calling comp(mid,mid), since some
		existing comparison funcs don't work when passed the same value
		for both pointers. */

		higuy += width;
		if (mid < higuy) {
			do  {
				higuy -= width;
			} while (higuy > mid && comp(higuy, mid, clmAE, clmIdLen) == 0);
		}
		if (mid >= higuy) {
			do  {
				higuy -= width;
			} while (higuy > lo && comp(higuy, mid, clmAE, clmIdLen) == 0);
		}

		/* OK, now we have the following:
		higuy < loguy
		lo <= higuy <= hi
		A[i]  <= A[mid] for lo <= i <= higuy
		A[i]  == A[mid] for higuy < i < loguy
		A[i]  >  A[mid] for loguy <= i < hi
		A[hi] >= A[mid] */

		/* We've finished the partition, now we want to sort the subarrays
		[lo, higuy] and [loguy, hi].
		We do the smaller one first to minimize stack usage.
		We only sort arrays of length 2 or more.*/

		/*
		���ǿ�������һ�£�����һ���Ѿ���������飬���ÿ�ηֳ�N-1��1�����飬
		��������ÿ�ζ��ȴ���N-1��һ�룬��ô���ǵĵݹ���Ⱦ��Ǻ�N�ɱ������������ڴ�N��ջ�ռ�Ŀ����Ǻܴ�ġ�
		����ȴ���1����һ�룬ջ�������ֻ��2�������Ԫ�ظպ��������м�ʱ��ջ�ĳ�����logN��
		����ջ�Ĳ����������ȰѴ��������Ϣ��ջ��
		*/

		if (higuy - lo >= hi - loguy) {
			if (lo < higuy) {
				lostk[stkptr] = lo;
				histk[stkptr] = higuy;
				++stkptr;
			}                           /* save big recursion for later */

			if (loguy < hi) {
				lo = loguy;
				goto recurse;           /* do small recursion */
			}
		}
		else {
			if (loguy < hi) {
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;               /* save big recursion for later */
			}

			if (lo < higuy) {
				hi = higuy;
				goto recurse;           /* do small recursion */
			}
		}
	}

	/* We have sorted the array, except for any pending sorts on the stack.
	Check if there are any, and do them. */

	/*��ջ������ֱ��ջΪ�գ��˳�ѭ��*/

	--stkptr;
	if (stkptr >= 0) {
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;           /* pop subarray from stack */
	}
	else
		return;                 /* all subarrays done */
}


#endif