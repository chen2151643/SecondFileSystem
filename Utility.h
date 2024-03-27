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
	/* 用于在读、写文件时，高速缓存与用户指定目标内存区域之间数据传送 */
	static void IOMove(unsigned char* from, unsigned char* to, int count);
	/* 以src为源地址，dst为目的地址，复制count个双字 */
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

