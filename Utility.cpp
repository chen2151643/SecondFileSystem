#include "Utility.h"
int Utility::Min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

void Utility::DWordCopy(int* src, int* dst, int count)
{
	while (count--)
	{
		*dst++ = *src++;
	}
	return;
}

void Utility::StringCopy(char* src, char* dst)
{
	while ((*dst++ = *src++) != 0);
}

int Utility::StringLength(char* pString)
{
	int length = 0;
	char* pChar = pString;

	while (*pChar++)
	{
		length++;
	}

	/* ·µ»Ø×Ö·û´®³¤¶È */
	return length;
}

void Utility::IOMove(unsigned char* from, unsigned char* to, int count)
{
	while (count--)
	{
		*to++ = *from++;
	}
	return;
}