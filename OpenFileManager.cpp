#include "OpenFileManager.h"
#include "Kernel.h"

/* =================== class OpenFileTable ==================== */
OpenFileTable g_OpenFileTable;

OpenFileTable::OpenFileTable()
{
	//nothing to do here
}
OpenFileTable::~OpenFileTable()
{
	//nothing to do here
}



/* =================== class InodeTable ==================== */
InodeTable g_InodeTable;

InodeTable::InodeTable()
{
	//nothing to do here
}
InodeTable::~InodeTable()
{
	//nothing to do here
}

void InodeTable::Initialize()
{
	/* 获取对g_FileSystem的引用 */
	this->m_FileSystem = &Kernel::Instance().GetFileSystem();
}