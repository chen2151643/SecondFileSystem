#include "header.h"
#include "FileSystem.h"
#include "Kernel.h"

SuperBlock::SuperBlock(){}
SuperBlock::~SuperBlock(){}

/*==============================class FileSystem===================================*/
FileSystem g_FileSystem;

FileSystem::FileSystem(){}
FileSystem::~FileSystem(){}

/* �ļ�ϵͳ��ʼ�� */
void FileSystem::Initialize()
{
	this->m_BufferManager = &Kernel::Instance().GetBufferManager();
}


//Mount::Mount(){}
//Mount::~Mount(){}