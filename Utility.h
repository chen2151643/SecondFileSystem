#pragma once
class Utility
{
public:
	static int Min(int a, int b);
	static int StringLength(char* pString);
	static void StringCopy(char* src, char* dst);

	/* �����ڶ���д�ļ�ʱ�����ٻ������û�ָ��Ŀ���ڴ�����֮�����ݴ��� */
	static void IOMove(unsigned char* from, unsigned char* to, int count);
	/* ��srcΪԴ��ַ��dstΪĿ�ĵ�ַ������count��˫�� */
	static void DWordCopy(int* src, int* dst, int count);
};

