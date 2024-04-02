#include "BufferManager.h"
#include "Kernel.h"
#include "Utility.h"

BufferManager g_BufferManager;

BufferManager::BufferManager(){}
BufferManager::~BufferManager(){}

/* ��ʼ�����ɻ�����У�Buf��b_addrָ�򻺴�� */
void BufferManager::Initialize()
{
	int i;
	Buf* bp;

	this->bFreeList.b_forw = this->bFreeList.b_back = &(this->bFreeList);
	this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList);
	for (i = 0; i < NBUF; i++)
	{
		bp = &(this->m_Buf[i]);
		bp->b_dev = -1;
		bp->b_addr = this->Buffer[i];
		/* ��ʼ��NODEV���� */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		/* ��ʼ�����ɶ��� */
		bp->b_flags = Buf::B_BUSY;
		Brelse(bp);
	}
	this->m_DiskDriver = &Kernel::Instance().GetDiskDriver();
}

void BufferManager::NotAvail(Buf* bp)
{
	/* �����ɶ�����ȡ�� */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* ����B_BUSY��־ */
	bp->b_flags |= Buf::B_BUSY;
	return;
}

/* �ͷŻ�����ƿ�buf */
/* ��Ҫ����Ϊ��Buf��B_WANTED��B_BUSY��B_ASYNC��λ */
/* �������ɶ��� */
void BufferManager::Brelse(Buf* bp)
{
	/* ע�����²�����û�����B_DELWRI��B_WRITE��B_READ��B_DONE��־
	 * B_DELWRI��ʾ��Ȼ���ÿ��ƿ��ͷŵ����ɶ������棬�����п��ܻ�û��д�������ϡ�
	 * B_DONE����ָ�û����������ȷ�ط�ӳ�˴洢�ڻ�Ӧ�洢�ڴ����ϵ���Ϣ
	 */
	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);
	(this->bFreeList.av_back)->av_forw = bp; //���β
	bp->av_back = this->bFreeList.av_back;
	bp->av_forw = &(this->bFreeList);
	this->bFreeList.av_back = bp;

	return;
}

/* ����һ�黺�棬���ڶ�д�豸dev�ϵ��ַ���blkno��
* �����ֳɵ���ֱ�ӷ��أ���û�У��򷵻�һ�����ɻ����
* ����GetBlk�ĺ���ͨ��B_Done��ʶ�ж�
*/
Buf* BufferManager::GetBlk(short dev, int blkno)
{
	Buf* bp;
	Devtab* dp;

	/* ������豸�ų�����ϵͳ�п��豸���� */
	if (dev >= this->m_DiskDriver->GetNBlkDev())
	{
		//cerr << "�豸�ų���ϵͳ���豸������error" << endl;
		Utility::Panic("out of dev number, error");
	}
	/*
	 * ����豸�������Ѿ�������Ӧ���棬�򷵻ظû��棻
	 * ��������ɶ����з����µĻ��������ַ����д��
	 */
loop:
	if (dev < 0) // ����ʲô�����������
	{
		dp = (Devtab*)(&this->bFreeList);
	}
	else {
		/* �������豸�Ż�ÿ��豸�� */
		dp = this->m_DiskDriver->GetDevtab(dev);
		if (dp == NULL)
		{
			Utility::Panic("Null devtab!");
		}
		/* �����ڸ��豸�����������Ƿ�����Ӧ�Ļ��� */
		/* b_forw���豸���У���Buf��devΪ����豸��һ�����л� */
		for (bp = dp->b_forw; bp != (Buf*)dp; bp = bp->b_forw)
		{
			/* ����Ҫ�ҵĻ��棬����� */
			if (bp->b_blkno != blkno || bp->b_dev != dev)
				continue;
			this->NotAvail(bp);
			return bp;
		}
	}
	//�豸����û��Ҫ�ҵĿ�

	/* ������ɶ���Ϊ�� */
	if (this->bFreeList.av_forw == &this->bFreeList)
	{
		this->bFreeList.b_flags |= Buf::B_WANTED;
		// ��ϵͳ��������������
		//goto loop;
	}

	/* ȡ���ɶ��е�һ�����п� */
	bp = this->bFreeList.av_forw;
	this->NotAvail(bp);//�����ɶ�����ȡ��

	/* ������ַ������ӳ�д�������첽д�������� */
	if (bp->b_flags & Buf::B_DELWRI)
	{
		this->Bwrite(bp);
		goto loop;
	}

	/* ע��: �����������������λ��ֻ����B_BUSY */
	bp->b_flags = Buf::B_BUSY;

	/* ��ԭ�豸�����г�� */
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	/* �����µ��豸���� */
	bp->b_forw = dp->b_forw;
	bp->b_back = (Buf*)dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;

	bp->b_dev = dev;
	bp->b_blkno = blkno;
	return bp;
}


/* ��ȡ���ɻ�����п��ƿ�Buf�������� */
Buf& BufferManager::GetBFreeList()
{
	return this->bFreeList;
}

/*
* ͬ����������GetBlk
* ���귵��Buf* bp
*/
Buf* BufferManager::Bread(short dev, int blkno)
{
	Buf* bp;
	/* �����豸�ţ��ַ�������뻺�� */
	bp = this->GetBlk(dev, blkno);
	/* ������豸�������ҵ����軺�棬��B_DONE�����ã��Ͳ������I/O���� */
	if (bp->b_flags & Buf::B_DONE)
	{
		return bp;
	}
	/* û���ҵ���Ӧ���棬����I/O������� */
	bp->b_flags |= Buf::B_READ;
	bp->b_wcount = BufferManager::BUFFER_SIZE;

	/*
	 * ��I/O�����������Ӧ�豸I/O������У���������I/O����������ִ�б���I/O����
	 * ����ȴ���ǰI/O����ִ����Ϻ����жϴ����������ִ�д�����
	 * ע��Strategy()������I/O����������豸������к󣬲���I/O����ִ����ϣ���ֱ�ӷ��ء�
	 */
	this->m_DiskDriver->IO(bp);
	/* ͬ�������ȴ�I/O�������� */

	return bp;
}

void BufferManager::Bwrite(Buf* bp)
{
	unsigned int flags;

	flags = bp->b_flags;
	bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
	bp->b_wcount = BufferManager::BUFFER_SIZE;		/* 512�ֽ� */

	this->m_DiskDriver->IO(bp); //���bp���Buf���д���񣬲����Done����λ

		/* ͬ��д����Ҫ�ȴ�I/O�������� */
		// this->IOWait(bp);
	// ��Ϊȫ��ͬ��д���ʲ��������ж���
	this->Brelse(bp);

	return;
}

void BufferManager::Bdwrite(Buf* bp)
{
	/* ����B_DONE������������ʹ�øô��̿����� */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

/* ��devָ���豸�������ӳ�д�Ļ���ȫ����������� */
void BufferManager::Bflush(short dev)
{
	Buf* bp;
	/* ע�⣺����֮����Ҫ��������һ����֮�����¿�ʼ������
	 * ��Ϊ��bwite()���뵽����������ʱ�п��жϵĲ���������
	 * �ȵ�bwriteִ����ɺ�CPU�Ѵ��ڿ��ж�״̬�����Ժ�
	 * �п��������ڼ���������жϣ�ʹ��bfreelist���г��ֱ仯��
	 * �����������������������������¿�ʼ������ô�ܿ�����
	 * ����bfreelist���е�ʱ����ִ���
	 */
loop:
	for (bp = this->bFreeList.av_forw; bp != &(this->bFreeList); bp = bp->av_forw)
	{
		/* �ҳ����ɶ����������ӳ�д�Ŀ� */
		if ((bp->b_flags & Buf::B_DELWRI) && (dev == DiskDriver::NODEV || dev == bp->b_dev))
		{
			this->NotAvail(bp);
			this->Bwrite(bp);
			goto loop;
		}
	}
	return;
}

/*
* ��ջ���������
*/
void BufferManager::ClrBuf(Buf* bp)
{
	int* pInt = (int*)bp->b_addr;

	/* ������������������ */
	for (unsigned int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
	{
		pInt[i] = 0;
	}
	return;
}