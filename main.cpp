#include"header.h"
#include "Kernel.h"

/*
* 文件系统FileManager初始化及User结构
*/
void InitSystem()
{	
	cout << "Initialize File System...";
	FileManager* fileManager = &Kernel::Instance().GetFileManager();	
	cout << "Done" << endl;
}

int main()
{
	/* 内核初始化，主要是内核全局对象的引用关系建立
	以及镜像磁盘的创建及初始化，若已存在跳过这一步*/
	Kernel::Instance().Initialize();
	/* 初始化系统 */
	InitSystem();

}