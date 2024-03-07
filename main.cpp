#include"header.h"
#include "Kernel.h"

/*
* �ļ�ϵͳFileManager��ʼ����User�ṹ
*/
void InitSystem()
{	
	cout << "Initialize File System...";
	FileManager* fileManager = &Kernel::Instance().GetFileManager();	
	cout << "Done" << endl;
}

int main()
{
	/* �ں˳�ʼ������Ҫ���ں�ȫ�ֶ�������ù�ϵ����
	�Լ�������̵Ĵ�������ʼ�������Ѵ���������һ��*/
	Kernel::Instance().Initialize();
	/* ��ʼ��ϵͳ */
	InitSystem();

}