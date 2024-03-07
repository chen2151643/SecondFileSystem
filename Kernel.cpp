#include "Kernel.h"

Kernel Kernel::instance; /* Kernel ������ʵ�� */
extern DiskDriver g_DiskDriver;
extern BufferManager g_BufferManager;
extern FileSystem g_FileSystem;
extern FileManager g_FileManager;
extern User g_User;

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
	// ��ʼ������
	InitBuffer();
	// ��ʼ���ļ�ϵͳ
	InitFileSystem();
}

/* �����ļ���ʼ�� */
void Kernel::InitDiskDriver() {
	this->m_DiskDriver = &g_DiskDriver;

	cout << "Initialize Disk_img...";
	this->GetDiskDriver().Initialize();

	cout << "Done" << endl;
}

void Kernel::InitBuffer()
{
	this->m_BufferManager = &g_BufferManager;

	cout << "Initialize Buffer...";
	this->GetBufferManager().Initialize();
	cout << "Done" << endl;
}


void Kernel::InitFileSystem()
{
	this->m_FileSystem = &g_FileSystem;
	this->m_FileManager = &g_FileManager;

	cout << "Initialize File System...";
	this->GetFileSystem().Initialize();
	cout << "Done" << endl;

	cout << "Initialize File Manager...";
	this->GetFileManager().Initialize();
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