#include "DirectoryEntry.h"
#include <cstring> 

DirectoryEntry::DirectoryEntry()
{
	this->m_ino = 0;
	//this->m_name[0] = '\0';
	memset(m_name, 0, DirectoryEntry::DIRSIZ);
}


DirectoryEntry::DirectoryEntry(int ino, const char* name)
{
	this->m_ino = ino;
	memset(m_name, 0, DirectoryEntry::DIRSIZ);
	char* p_name = m_name;
	while ((*p_name++ = *name++) != 0);
}

DirectoryEntry::~DirectoryEntry()
{
}
