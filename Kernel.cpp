#include "Kernel.h"

Kernel Kernel::instance; /* Kernel 单体类实例 */
DiskDriver g_DiskDriver;
BufferManager g_BufferManager;
FileSystem g_FileSystem;
FileManager g_FileManager;
User g_User;

Kernel::Kernel(){}
Kernel::~Kernel(){}

Kernel& Kernel::Instance()
{
	return Kernel::instance;
}

/* 该函数完成初始化内核大部分数据结构的初始化 */
void Kernel::Initialize()
{
	// 初始化磁盘镜像文件
	InitDiskDriver();

}

/* 磁盘文件初始化 */
void Kernel::InitDiskDriver() {
	this->m_DiskDriver = &g_DiskDriver;

	cout << "Initialize Disk_img...";
	this->GetDiskDriver().Initialize();

	cout << "Done" << endl;
}

BufferManager& Kernel::GetBufferManager()
{
	return *(this->m_BufferManager);
}
FileSystem& Kernel::GetFileSystem()
{
	return *(this->m_FileSystem);
}
FileManager& Kernel::GetFileManager()
{
	return *(this->m_FileManager);
}
DiskDriver& Kernel::GetDiskDriver()
{
	return *(this->m_DiskDriver);
}
User& Kernel::GetUser()
{
	return *(this->m_User);
}