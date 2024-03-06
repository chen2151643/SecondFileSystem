#include "DirectoryEntry.h"

DirectoryEntry::DirectoryEntry()
{
	this->m_ino = 0;
	this->m_name[0] = '\0';
}


DirectoryEntry::DirectoryEntry(int ino, const char* name)
{
	this->m_ino = 0;
	char* p_name = m_name;
	while ((*p_name++ = *name++) != 0);
}

DirectoryEntry::~DirectoryEntry()
{
}
