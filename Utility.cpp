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

	/* �����ַ������� */
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
	//exit(-1); ��ֱ��exit
}

int Utility::GetTime()
{
	// ��ȡ��ǰʱ���
	auto now = chrono::system_clock::now();

	// ��ʱ���ת��Ϊʱ���������Ϊ��λ
	auto timestamp = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();

	// ��ʱ���ת��Ϊint����
	int intTimestamp = static_cast<int>(timestamp);

	return intTimestamp;
}

/*
const char* Utility::usage[14] = {
	"ls\n\t Usage:ls\n\t Description:��ʾ��ǰĿ¼�б�\n",
	"fopen\n\t Usage:fopen [name]\n\t Description:����Ϊname���ļ�������fd\n",
	"fclose\n\t Usage:fclose [fd]\n\t Description:�ر��ļ�������Ϊfd���ļ�\n",
	"fread\n\t Usage:fread [fd] [length]\n\t Description:���ļ�������Ϊfd���ļ��ж�ȡlength���ֽڣ������ȡ������\n",
	"fwrite\n\t Usage:fwrite [fd] [string] [length]\n\t Description:���ļ�������Ϊfd���ļ�д������Ϊstring��length���ֽڣ�����ضϣ�������0��\n",
	"fseek\n\t Usage:fseek [fd] [offset]\n\t Description:���ļ�������Ϊfd���ļ��Ķ�дָ����������ļ���ͷƫ����Ϊoffset��λ��\n",
	"fcreate\n\t Usage:fcreate [name]\n\t Description:������Ϊname����ͨ�ļ�\n",
	"mkdir\n\t Usage:mkdir [name]\n\t Description:������Ϊname��Ŀ¼�ļ�\n",
	"funlink\n\t Usage:funlink [name]\n\t Description:ɾ����Ϊname���ļ�\n",
	"flink\n\t Usage:flink [src] [dst]\n\t Description:����dst->src��Ӳ����\n",
	"cd\n\t Usage:cd [name]\n\t Description:�ı乤��Ŀ¼Ϊname\n",
	"fin\n\t Usage:fin [extername] [intername]\n\t Description:���ⲿ��Ϊextername���ļ����ݴ����ļ�ϵͳ�ڲ���Ϊintername���ļ�\n",
	"fout\n\t Usage:fout [intername] [extername]\n\t Description:���ڲ���Ϊintername���ļ����ݴ����ļ�ϵͳ�ⲿ��Ϊextername���ļ�\n",
	"help\n\t Usage:help\n\t Description:����������ʾ��Ϣ\n",
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