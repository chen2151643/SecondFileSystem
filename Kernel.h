#pragma once
#include "FileManager.h"
#include "DiskDriver.h"
#include "User.h"

class Kernel
{
public:
	Kernel();
	~Kernel();
	static Kernel& Instance();
	void Initialize(); /* �ú�����ɳ�ʼ���ں˴󲿷����ݽṹ�ĳ�ʼ�� */
	BufferManager& GetBufferManager();
	FileSystem & GetFileSystem();
	FileManager& GetFileManager();
	DiskDriver& GetDiskDriver();
	User& GetUser(); /* ��ȡ��ǰ���̵� User �ṹ */
private:
	void InitDiskDriver();
	void InitBuffer();
	void InitFileSystem();
private:
	static Kernel instance; /* Kernel ������ʵ�� */
	BufferManager* m_BufferManager;
	FileSystem* m_FileSystem;
	FileManager* m_FileManager;
	DiskDriver* m_DiskDriver;
	User* m_User;
};
