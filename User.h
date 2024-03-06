#pragma once

#include "DirectoryEntry.h"
#include "File.h"

class User
{
public:
	// static const int EAX = 0; 
	/* u.u_ar0[EAX]；访问现场保护区中 EAX 寄存器的偏移量 */
	enum ErrorCode {
		NOERROR = 0, /* No error */
	};
public:
	/* 系统调用相关成员 */
	int u_ar0; //此处简化,存放系统调用返回值
	int u_arg[5]; /* 存放当前系统调用参数 */
	char* u_dirp; /* 系统调用参数(一般用于 Pathname)的指针 */
	/* 文件系统相关成员 */
	Inode* u_cdir; /* 指向当前目录的 Inode 指针 */
	Inode* u_pdir; /* 指向父目录的 Inode 指针 */
	DirectoryEntry u_dent; /* 当前目录的目录项 */
	char u_dbuf[DirectoryEntry::DIRSIZ]; /* 当前路径分量 */
	char u_curdir[128]; /* 当前工作目录完整路径 */
	ErrorCode u_error; /* 存放错误码 */
	/* 文件系统相关成员 */
	OpenFiles u_ofiles; /* 进程打开文件描述符表对象 */
	/* 文件 I/O 操作 */
	IOParameter u_IOParam; /* 记录当前读、写文件的偏移量，用户目标区域和剩余
	字节数参数 */

	/* Constructors */
	User();
	/* Destructors */
	~User();

};
