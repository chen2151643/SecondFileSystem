#include "File.h"
#include "header.h"
#include "User.h"
#include "Kernel.h"

/* =================== class File ==================== */
File::File()
{
	this->f_count = 0;
	this->f_flag = 0;
	this->f_offset = 0;
	this->f_inode = NULL;
}
File::~File()
{
	//nothing to do here
}


/* =================== class OpenFiles ==================== */
OpenFiles::OpenFiles()
{
	//nothing to do here
}
OpenFiles::~OpenFiles()
{
	//nothing to do here
}

int OpenFiles::AllocFreeSlot()
{
	int i;
	User& u = Kernel::Instance().GetUser();

	for (i = 0; i < OpenFiles::NOFILES; i++)
	{
		/* ���̴��ļ������������ҵ�������򷵻�֮ */
		if (this->ProcessOpenFileTable[i] == NULL)
		{
			/* ���ú���ջ�ֳ��������е�EAX�Ĵ�����ֵ����ϵͳ���÷���ֵ */
			u.u_ar0 = i;
			return i;
		}
	}

	u.u_ar0 = -1;   /* Open1����Ҫһ����־�������ļ��ṹ����ʧ��ʱ�����Ի���ϵͳ��Դ*/
	u.u_error = User::_EMFILE;
	return -1;
}

/*
 * @comment Ϊ�ѷ��䵽�Ŀ���������fd���ѷ���Ĵ��ļ�����
 * ����File������������ϵ
 */
void OpenFiles::SetF(int fd, File* pFile)
{
	if (fd < 0 || fd >= OpenFiles::NOFILES)
	{
		return;
	}
	/* ���̴��ļ�������ָ��ϵͳ���ļ�������Ӧ��File�ṹ */
	this->ProcessOpenFileTable[fd] = pFile;
}

/*
 * @comment �����û�ϵͳ�����ṩ���ļ�����������fd��
 * �ҵ���Ӧ�Ĵ��ļ����ƿ�File�ṹ
 */
File* OpenFiles::GetF(int fd)
{
	File* pFile;
	User& u = Kernel::Instance().GetUser();

	/* ������ļ���������ֵ�����˷�Χ */
	if (fd < 0 || fd >= OpenFiles::NOFILES)
	{
		u.u_error = User::_EBADF;
		return NULL;
	}

	pFile = this->ProcessOpenFileTable[fd];
	if (pFile == NULL)
	{
		u.u_error = User::_EBADF;
	}

	return pFile;	/* ��ʹpFile==NULLҲ���������ɵ���GetF�ĺ������жϷ���ֵ */
}

/* =================== class IOParameter ==================== */
IOParameter::IOParameter()
{
	this->m_Base = 0;
	this->m_Count = 0;
	this->m_Offset = 0;
}
IOParameter::~IOParameter()
{
	//nothing to do here
}
