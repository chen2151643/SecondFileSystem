#pragma once

#include "INode.h"
#include "BufferManager.h"

/*
 * �ļ�ϵͳ�洢��Դ�����(Super Block)�Ķ��塣
 */
class SuperBlock
{
	/* Functions */
public:
	SuperBlock();
	~SuperBlock();

	/* Members */
public:
	int		s_isize;		/* ���Inode��ռ�õ��̿��� */
	int		s_fsize;		/* �̿����� */

	int		s_nfree;		/* ֱ�ӹ���Ŀ����̿����� */
	int		s_free[100];	/* ֱ�ӹ���Ŀ����̿������� */

	int		s_ninode;		/* ֱ�ӹ���Ŀ������Inode���� */
	int		s_inode[100];	/* ֱ�ӹ���Ŀ������Inode������ */

	//int		s_flock;		/* ���������̿��������־ */
	//int		s_ilock;		/* ��������Inode���־ */

	int		s_fmod;			/* �ڴ���super block�������޸ı�־����ζ����Ҫ��������Ӧ��Super Block */
	int		s_ronly;		/* ���ļ�ϵͳֻ�ܶ��� */
	int		s_time;			/* ���һ�θ���ʱ�� */
	int		padding[49];	/* ���ʹSuperBlock���С����1024�ֽڣ�ռ��2������ */
};

/*
 * �ļ�ϵͳװ���(Mount)�Ķ��塣
 * װ�������ʵ�����ļ�ϵͳ��
 * ���ļ�ϵͳ�����ӡ�
 */

/*
 * �ļ�ϵͳ��(FileSystem)�����ļ��洢�豸��
 * �ĸ���洢��Դ�����̿顢���INode�ķ��䡢
 * �ͷš�
 */
class FileSystem
{
public:
	/* static consts */
	static const int NMOUNT = 5;			/* ϵͳ�����ڹ������ļ�ϵͳ��װ������� */

	static const int SUPER_BLOCK_SECTOR_NUMBER = 0;	/* ����SuperBlockλ�ڴ����ϵ������ţ�ռ��0��1���������� */

	static const int ROOTINO = 1;			/* �ļ�ϵͳ��Ŀ¼���Inode��� 0#inode���ã�ʹ��1#inode */

	static const int INODE_NUMBER_PER_SECTOR = 8;		/* ���INode���󳤶�Ϊ64�ֽڣ�ÿ�����̿���Դ��512/64 = 8�����Inode */
	static const int INODE_ZONE_START_SECTOR = 2;		/* ���Inode��λ�ڴ����ϵ���ʼ������ */
	static const int INODE_ZONE_SIZE = 512 - 2;		/* ���������Inode��ռ�ݵ������� */

	static const int DATA_ZONE_START_SECTOR = 512;		/* ����������ʼ������ */
	static const int DATA_ZONE_END_SECTOR = 18000 - 1;	/* �������Ľ��������� */
	static const int DATA_ZONE_SIZE = 18000 - DATA_ZONE_START_SECTOR;	/* ������ռ�ݵ��������� */

	/* Ϊ��ʼ�����Ŀ¼�ȳ�ʼĿ¼�ṹ��������� */
	static const int DATA_INIT_SECTOR = 4;
	static const int INODE_INIT_NUM = 4;


	/* Functions */
public:
	/* Constructors */
	FileSystem();
	/* Destructors */
	~FileSystem();

	/*
	 * @comment ��ʼ����Ա����
	 */
	void Initialize();

	/*
	* @comment ϵͳ��ʼ��ʱ����SuperBlock
	*/
	void LoadSuperBlock();

	/*
	 * @comment �����ļ��洢�豸���豸��dev��ȡ
	 * ���ļ�ϵͳ��SuperBlock
	 */
	SuperBlock* GetFS(short dev);
	/*
	 * @comment ��SuperBlock������ڴ渱�����µ�
	 * �洢�豸��SuperBlock��ȥ
	 */
	void Update();

	/*
	 * @comment  �ڴ洢�豸dev�Ϸ���һ������
	 * ���INode��һ�����ڴ����µ��ļ���
	 */
	Inode* IAlloc(short dev);
	/*
	 * @comment  �ͷŴ洢�豸dev�ϱ��Ϊnumber
	 * �����INode��һ������ɾ���ļ���
	 */
	void IFree(short dev, int number);

	/*
	 * @comment �ڴ洢�豸dev�Ϸ�����д��̿�
	 */
	Buf* Alloc(short dev);
	/*
	 * @comment �ͷŴ洢�豸dev�ϱ��Ϊblkno�Ĵ��̿�
	 */
	void Free(short dev, int blkno);

private:
	BufferManager* m_BufferManager;		/* FileSystem����Ҫ�������ģ��(BufferManager)�ṩ�Ľӿ� */
	//�����ļ�ϵͳΪ�����̣��ʲ��ظ���
	//int updlock;				/* Update()�����������ú�������ͬ���ڴ����SuperBlock�����Լ���
	// ���޸Ĺ����ڴ�Inode����һʱ��ֻ����һ�����̵��øú��� */
};
