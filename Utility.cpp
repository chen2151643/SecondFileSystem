#include "Utility.h"
#include <chrono>

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

void Utility::StringCopy(const char* src, char* dst)
{
	int i = 0;
	//while ((dst[i++] = src[i++]) != 0);
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

	/* 返回字符串长度 */
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

void Utility::Panic(const char* str)
{
	cerr << str << endl;
	//exit(-1); 怕直接exit
}

int Utility::GetTime()
{
	// 获取当前时间点
	auto now = chrono::system_clock::now();

	// 将时间点转换为时间戳，以秒为单位
	auto timestamp = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();

	// 将时间戳转换为int类型
	int intTimestamp = static_cast<int>(timestamp);

	return intTimestamp;
}

/*
const char* Utility::usage[14] = {
	"ls\n\t Usage:ls\n\t Description:显示当前目录列表\n",
	"fopen\n\t Usage:fopen [name]\n\t Description:打开名为name的文件，返回fd\n",
	"fclose\n\t Usage:fclose [fd]\n\t Description:关闭文件描述符为fd的文件\n",
	"fread\n\t Usage:fread [fd] [length]\n\t Description:从文件描述符为fd的文件中读取length个字节，输出读取的内容\n",
	"fwrite\n\t Usage:fwrite [fd] [string] [length]\n\t Description:向文件描述符为fd的文件写入内容为string的length个字节（不足截断，超过补0）\n",
	"fseek\n\t Usage:fseek [fd] [offset]\n\t Description:将文件描述符为fd的文件的读写指针调整到距文件开头偏移量为offset的位置\n",
	"fcreate\n\t Usage:fcreate [name]\n\t Description:创建名为name的普通文件\n",
	"mkdir\n\t Usage:mkdir [name]\n\t Description:创建名为name的目录文件\n",
	"funlink\n\t Usage:funlink [name]\n\t Description:删除名为name的文件\n",
	"flink\n\t Usage:flink [src] [dst]\n\t Description:建立dst->src的硬链接\n",
	"cd\n\t Usage:cd [name]\n\t Description:改变工作目录为name\n",
	"fin\n\t Usage:fin [extername] [intername]\n\t Description:将外部名为extername的文件内容存入文件系统内部名为intername的文件\n",
	"fout\n\t Usage:fout [intername] [extername]\n\t Description:将内部名为intername的文件内容存入文件系统外部名为extername的文件\n",
	"help\n\t Usage:help\n\t Description:给出命令提示信息\n",
};
*/

const char* Utility::usage[14] = {
	"ls\n\t Usage: ls\n\t Description: display the list of files in the current directory\n",
	"fopen\n\t Usage: fopen [name]\n\t Description: open the file named [name] and return the file descriptor\n",
	"fclose\n\t Usage: fclose [fd]\n\t Description: close the file associated with file descriptor [fd]\n",
	"fread\n\t Usage: fread [fd] [length]\n\t Description: read [length] bytes from the file associated with file descriptor [fd] and output the content\n",
	"fwrite\n\t Usage: fwrite [fd] [string] [length]\n\t Description: write [length] bytes of [string] to the file associated with file descriptor [fd] (truncate if length exceeds, pad with 0 if less)\n",
	"fseek\n\t Usage: fseek [fd] [offset]\n\t Description: set the read/write pointer of the file associated with file descriptor [fd] to the position [offset] from the beginning of the file\n",
	"fcreate\n\t Usage: fcreate [name]\n\t Description: create a regular file named [name]\n",
	"mkdir\n\t Usage: mkdir [name]\n\t Description: create a directory named [name]\n",
	"funlink\n\t Usage: funlink [name]\n\t Description: delete the file named [name]\n",
	"flink\n\t Usage: flink [src] [dst]\n\t Description: create a hard link from [src] to [dst]\n",
	"cd\n\t Usage: cd [name]\n\t Description: change the current working directory to [name]\n",
	"fin\n\t Usage: fin [extername] [intername]\n\t Description: store the content of the file with external name [extername] into the file system with internal name [intername]\n",
	"fout\n\t Usage: fout [intername] [extername]\n\t Description: store the content of the file with internal name [intername] into the external file with name [extername]\n",
	"help\n\t Usage: help\n\t Description: provide command prompt information\n",
};


void Utility::Usage(int usage_id)
{
	if (usage_id == -1) {
		for (int i = 0; i < NUM_COMMAND; i++) {
			cout << i+1 << ".";
			Usage(i);
		}
	}
	else
		cout << usage[usage_id];
}