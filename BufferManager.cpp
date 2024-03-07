#include "BufferManager.h"
#include "Kernel.h"

BufferManager g_BufferManager;

BufferManager::BufferManager(){}
BufferManager::~BufferManager(){}

void BufferManager::Initialize()
{
	this->m_DiskDriver = &Kernel::Instance().GetDiskDriver();
}