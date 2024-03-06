#pragma once

#include "DirectoryEntry.h"
#include "File.h"

class User
{
public:
	// static const int EAX = 0; 
	/* u.u_ar0[EAX]�������ֳ��������� EAX �Ĵ�����ƫ���� */
	enum ErrorCode {
		NOERROR = 0, /* No error */
	};
public:
	/* ϵͳ������س�Ա */
	int u_ar0; //�˴���,���ϵͳ���÷���ֵ
	int u_arg[5]; /* ��ŵ�ǰϵͳ���ò��� */
	char* u_dirp; /* ϵͳ���ò���(һ������ Pathname)��ָ�� */
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
