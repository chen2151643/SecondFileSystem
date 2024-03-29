#include "Kernel.h"

Kernel Kernel::instance; /* Kernel 单体类实例 */
extern DiskDriver g_DiskDriver;
extern BufferManager g_BufferManager;
extern FileSystem g_FileSystem;
extern FileManager g_FileManager;
extern User g_User;
extern InodeTable g_InodeTable;

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
	// 初始化缓存
	InitBuffer();
	// 初始化文件系统
	InitFileSystem();
	// 初始化系统
	// 完善后初始化的文件结构在此处写入
	InitSystem();
}

/* 初始化 FileManager 及 User 中部分结构 */
/* 以及把初始的文件结构写入，包括/home、/etc、/bin目录 */
void Kernel::InitSystem()
{
	this->m_User = &g_User;
	cout << "Initialize System...";
	FileManager* fileManager = &Kernel::Instance().GetFileManager();
	fileManager->rootDirInode = g_InodeTable.IGet(DiskDriver::ROOTDEV, FileSystem::ROOTINO);
	Kernel::Instance().GetFileSystem().LoadSuperBlock();
	User* u = &Kernel::Instance().GetUser();
	u->u_error = User::_NOERROR;
	u->u_cdir = g_InodeTable.IGet(DiskDriver::ROOTDEV, FileSystem::ROOTINO);
	u->u_pdir = NULL;
	strcpy_s(u->u_curdir, "/");
	u->u_dirp = "/";
	memset(u->u_arg, 0, sizeof(u->u_arg));

	if (is_disk_format) {
		// 若磁盘文件是初次启动经过格式化，则为其建立初始目录结构
		u->u_arg[1] = FileManager::DEFAULT_MODE;
		u->u_dirp = "home";
		fileManager->MkNod();
		u->u_dirp = "etc";
		fileManager->MkNod();
		u->u_dirp = "bin";
		fileManager->MkNod();
		Kernel::Instance().GetFileSystem().Update(); //将脏页持久化到硬盘上
	}
	cout << "Done" << endl;
}

/* 磁盘文件初始化 */
void Kernel::InitDiskDriver() {
	this->m_DiskDriver = &g_DiskDriver;

	cout << "Initialize Disk_img...";
	is_disk_format = this->GetDiskDriver().Initialize();

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
