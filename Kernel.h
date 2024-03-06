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
	void Initialize(); /* 该函数完成初始化内核大部分数据结构的初始化 */
	BufferManager& GetBufferManager();
	FileSystem & GetFileSystem();
	FileManager& GetFileManager();
	DiskDriver& GetDiskDriver();
	User& GetUser(); /* 获取当前进程的 User 结构 */
private:
	void InitDiskDriver();
	void InitBuffer();
	void InitFileSystem();
private:
	static Kernel instance; /* Kernel 单体类实例 */
	BufferManager* m_BufferManager;
	FileSystem* m_FileSystem;
	FileManager* m_FileManager;
	DiskDriver* m_DiskDriver;
	User* m_User;
};
