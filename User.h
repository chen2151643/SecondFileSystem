#pragma once

#include "DirectoryEntry.h"
#include "File.h"

class User
{
public:
	// static const int EAX = 0; 
	/* u.u_ar0[EAX]；访问现场保护区中 EAX 寄存器的偏移量 */
	enum ErrorCode {
		_NOERROR = 0, /* No error */
		_EPERM = 1,	/* Operation not permitted */
		_ENOENT = 2,	/* No such file or directory */
		_ESRCH = 3,	/* No such process */
		_EINTR = 4,	/* Interrupted system call */
		_EIO = 5,	/* I/O error */
		_ENXIO = 6,	/* No such device or address */
		_E2BIG = 7,	/* Arg list too long */
		_ENOEXEC = 8,	/* Exec format error */
		_EBADF = 9,	/* Bad file number */
		_ECHILD = 10,	/* No child processes */
		_EAGAIN = 11,	/* Try again */
		_ENOMEM = 12,	/* Out of memory */
		_EACCES = 13,	/* Permission denied */
		_EFAULT = 14,	/* Bad address */
		_ENOTBLK = 15,	/* Block device required */
		_EBUSY = 16,	/* Device or resource busy */
		_EEXIST = 17,	/* File exists */
		_EXDEV = 18,	/* Cross-device link */
		_ENODEV = 19,	/* No such device */
		_ENOTDIR = 20,	/* Not a directory */
		_EISDIR = 21,	/* Is a directory */
		_EINVAL = 22,	/* Invalid argument */
		_ENFILE = 23,	/* File table overflow */
		_EMFILE = 24,	/* Too many open files */
		_ENOTTY = 25,	/* Not a typewriter(terminal) */
		_ETXTBSY = 26,	/* Text file busy */
		_EFBIG = 27,	/* File too large */
		_ENOSPC = 28,	/* No space left on device */
		_ESPIPE = 29,	/* Illegal seek */
		_EROFS = 30,	/* Read-only file system */
		_EMLINK = 31,	/* Too many links */
		_EPIPE = 32,	/* Broken pipe */
		_ENOSYS = 100,
		//EFAULT	= 106
	};
public:
	/* 系统调用相关成员 */
	int u_ar0; //此处简化,存放系统调用返回值
	int u_arg[5]; /* 存放当前系统调用参数 */
	const char*  u_dirp; /* 系统调用参数(一般用于 Pathname)的指针 */
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
