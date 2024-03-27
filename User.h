#pragma once

#include "DirectoryEntry.h"
#include "File.h"

class User
{
public:
	// static const int EAX = 0; 
	/* u.u_ar0[EAX]�������ֳ��������� EAX �Ĵ�����ƫ���� */
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
	/* ϵͳ������س�Ա */
	int u_ar0; //�˴���,���ϵͳ���÷���ֵ
	int u_arg[5]; /* ��ŵ�ǰϵͳ���ò��� */
	const char*  u_dirp; /* ϵͳ���ò���(һ������ Pathname)��ָ�� */
	/* �ļ�ϵͳ��س�Ա */
	Inode* u_cdir; /* ָ��ǰĿ¼�� Inode ָ�� */
	Inode* u_pdir; /* ָ��Ŀ¼�� Inode ָ�� */
	DirectoryEntry u_dent; /* ��ǰĿ¼��Ŀ¼�� */
	char u_dbuf[DirectoryEntry::DIRSIZ]; /* ��ǰ·������ */
	char u_curdir[128]; /* ��ǰ����Ŀ¼����·�� */
	ErrorCode u_error; /* ��Ŵ����� */
	/* �ļ�ϵͳ��س�Ա */
	OpenFiles u_ofiles; /* ���̴��ļ������������ */
	/* �ļ� I/O ���� */
	IOParameter u_IOParam; /* ��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ��
	�ֽ������� */

	/* Constructors */
	User();
	/* Destructors */
	~User();

};
