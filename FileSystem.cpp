#include "header.h"
#include "FileSystem.h"
#include "Kernel.h"

SuperBlock::SuperBlock(){}
SuperBlock::~SuperBlock(){}

/*==============================class FileSystem===================================*/
FileSystem g_FileSystem;

FileSystem::FileSystem(){}
FileSystem::~FileSystem(){}

/* 文件系统初始化 */
void FileSystem::Initialize()
{
	this->m_BufferManager = &Kernel::Instance().GetBufferManager();
}


//Mount::Mount(){}
//Mount::~Mount(){}