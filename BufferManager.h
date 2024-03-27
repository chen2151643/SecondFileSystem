#pragma once

#include "Buf.h"
#include "DiskDriver.h"

class BufferManager
{
public:
	/* static const member */
	static const int NBUF = 15;			/* ������ƿ顢������������ */
	static const int BUFFER_SIZE = 512;	/* ��������С�� ���ֽ�Ϊ��λ */

public:
	BufferManager();
	~BufferManager();

	void Initialize();					/* ������ƿ���еĳ�ʼ������������ƿ���b_addrָ����Ӧ�������׵�ַ��*/

	Buf* GetBlk(short dev, int blkno);	/* ����һ�黺�棬���ڶ�д�豸dev�ϵ��ַ���blkno��*/
	void Brelse(Buf* bp);				/* �ͷŻ�����ƿ�buf */

	Buf* Bread(short dev, int blkno);	/* ��һ�����̿顣devΪ�������豸�ţ�blknoΪĿ����̿��߼���š� */
	
	void Bwrite(Buf* bp);				/* дһ�����̿� */
	void Bdwrite(Buf* bp);				/* �ӳ�д���̿� */

	void ClrBuf(Buf* bp);				/* ��ջ��������� */
	void Bflush(short dev);				/* ��devָ���豸�������ӳ�д�Ļ���ȫ����������� */
	
	Buf& GetBFreeList();				/* ��ȡ���ɻ�����п��ƿ�Buf�������� */
	void NotAvail(Buf* bp);

private:
	Buf bFreeList;						/* ���ɻ�����п��ƿ� */
	Buf m_Buf[NBUF];					/* ������ƿ����� */
	unsigned char Buffer[NBUF][BUFFER_SIZE];	/* ���������� */

	DiskDriver* m_DiskDriver;
//	DeviceManager* m_DeviceManager;		/* ָ���豸����ģ��ȫ�ֶ��� */
};

