#include "Kernel.h"

Kernel Kernel::instance; /* Kernel ������ʵ�� */
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

/* �ú�����ɳ�ʼ���ں˴󲿷����ݽṹ�ĳ�ʼ�� */
void Kernel::Initialize()
{
	// ��ʼ�����̾����ļ�
	InitDiskDriver();

}

/* �����ļ���ʼ�� */
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