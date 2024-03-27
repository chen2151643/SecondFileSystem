#pragma once
#include"header.h"
using namespace std;

class Utility
{
public:
	static int Min(int a, int b);
	static int StringLength(char* pString);
	static void StringCopy(const char* src, char* dst);
	static void Panic(const char* str);
	/* �����ڶ���д�ļ�ʱ�����ٻ������û�ָ��Ŀ���ڴ�����֮�����ݴ��� */
	static void IOMove(unsigned char* from, unsigned char* to, int count);
	/* ��srcΪԴ��ַ��dstΪĿ�ĵ�ַ������count��˫�� */
	static void DWordCopy(int* src, int* dst, int count);
	static int GetTime();
	static void Usage(int usage_id);
	static const int NUM_COMMAND = 14;
	enum Usage_ID {
		u_LS = 0,
		u_FOPEN = 1,
		u_FCLOSE = 2,
		u_FREAD = 3,
		u_FWRITE = 4,
		u_LSEEK = 5,
		u_CREATE = 6,
		u_MKDIR = 7,
		u_FUNLINK = 8,
		u_FLINK = 9,
		u_CD = 10,
		u_FIN = 11,
		u_FOUT = 12,
		u_HELP = 13,
	};
private:
	const static char* usage[NUM_COMMAND];
};

