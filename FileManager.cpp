#include "FileManager.h"
#include "DirectoryEntry.h"
#include "Kernel.h"

FileManager g_FileManager;

FileManager::FileManager(){}
FileManager::~FileManager(){}

extern InodeTable g_InodeTable;
extern OpenFileTable g_OpenFileTable;

void FileManager::Initialize()
{
	this->m_FileSystem = &Kernel::Instance().GetFileSystem();

	this->m_InodeTable = &g_InodeTable;
	this->m_OpenFileTable = &g_OpenFileTable;

	this->m_InodeTable->Initialize();
}
