#include "header.h"
#include "FileSystem.h"
#include "Kernel.h"

SuperBlock::SuperBlock(){}
SuperBlock::~SuperBlock(){}

/*==============================class FileSystem===================================*/
FileSystem::FileSystem(){}
FileSystem::~FileSystem(){}

void FileSystem::Initialize()
{
	this->m_BufferManager = &Kernel::Instance().GetBufferManager();
}


//Mount::Mount(){}
//Mount::~Mount(){}