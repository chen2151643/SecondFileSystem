#include "BufferManager.h"
#include "Kernel.h"
#include "Utility.h"

BufferManager g_BufferManager;

BufferManager::BufferManager(){}
BufferManager::~BufferManager(){}

/* 初始化自由缓存队列，Buf的b_addr指向缓存块 */
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
		/* 初始化NODEV队列 */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		/* 初始化自由队列 */
		bp->b_flags = Buf::B_BUSY;
		Brelse(bp);
	}
	this->m_DiskDriver = &Kernel::Instance().GetDiskDriver();
}

void BufferManager::NotAvail(Buf* bp)
{
	/* 从自由队列中取出 */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* 设置B_BUSY标志 */
	bp->b_flags |= Buf::B_BUSY;
	return;
}

/* 释放缓存控制块buf */
/* 主要功能为清Buf的B_WANTED、B_BUSY、B_ASYNC三位 */
/* 插入自由队列 */
void BufferManager::Brelse(Buf* bp)
{
	/* 注意以下操作并没有清除B_DELWRI、B_WRITE、B_READ、B_DONE标志
	 * B_DELWRI表示虽然将该控制块释放到自由队列里面，但是有可能还没有写到磁盘上。
	 * B_DONE则是指该缓存的内容正确地反映了存储在或应存储在磁盘上的信息
	 */
	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);
	(this->bFreeList.av_back)->av_forw = bp; //插队尾
	bp->av_back = this->bFreeList.av_back;
	bp->av_forw = &(this->bFreeList);
	this->bFreeList.av_back = bp;

	return;
}

/* 申请一块缓存，用于读写设备dev上的字符块blkno。
* 若有现成的则直接返回，若没有，则返回一个自由缓存块
* 调用GetBlk的函数通过B_Done标识判断
*/
Buf* BufferManager::GetBlk(short dev, int blkno)
{
	Buf* bp;
	Devtab* dp;

	/* 如果主设备号超出了系统中块设备数量 */
	if (dev >= this->m_DiskDriver->GetNBlkDev())
	{
		//cerr << "设备号超过系统中设备数量，error" << endl;
		Utility::Panic("out of dev number, error");
	}
	/*
	 * 如果设备队列中已经存在相应缓存，则返回该缓存；
	 * 否则从自由队列中分配新的缓存用于字符块读写。
	 */
loop:
	if (dev < 0) // 这是什么情况，还不懂
	{
		dp = (Devtab*)(&this->bFreeList);
	}
	else {
		/* 根据主设备号获得块设备表 */
		dp = this->m_DiskDriver->GetDevtab(dev);
		if (dp == NULL)
		{
			Utility::Panic("Null devtab!");
		}
		/* 首先在该设备队列中搜索是否有相应的缓存 */
		/* b_forw是设备队列，是Buf里dev为这个设备的一个队列环 */
		for (bp = dp->b_forw; bp != (Buf*)dp; bp = bp->b_forw)
		{
			/* 不是要找的缓存，则继续 */
			if (bp->b_blkno != blkno || bp->b_dev != dev)
				continue;
			this->NotAvail(bp);
			return bp;
		}
	}
	//设备表中没有要找的块

	/* 如果自由队列为空 */
	if (this->bFreeList.av_forw == &this->bFreeList)
	{
		this->bFreeList.b_flags |= Buf::B_WANTED;
		// 本系统不会出现这种情况
		//goto loop;
	}

	/* 取自由队列第一个空闲块 */
	bp = this->bFreeList.av_forw;
	this->NotAvail(bp);//从自由队列中取出

	/* 如果该字符块是延迟写，将其异步写到磁盘上 */
	if (bp->b_flags & Buf::B_DELWRI)
	{
		this->Bwrite(bp);
		goto loop;
	}

	/* 注意: 这里清除了所有其他位，只设了B_BUSY */
	bp->b_flags = Buf::B_BUSY;

	/* 从原设备队列中抽出 */
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	/* 加入新的设备队列 */
	bp->b_forw = dp->b_forw;
	bp->b_back = (Buf*)dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;

	bp->b_dev = dev;
	bp->b_blkno = blkno;
	return bp;
}


/* 获取自由缓存队列控制块Buf对象引用 */
Buf& BufferManager::GetBFreeList()
{
	return this->bFreeList;
}

/*
* 同步读，调用GetBlk
* 读完返回Buf* bp
*/
Buf* BufferManager::Bread(short dev, int blkno)
{
	Buf* bp;
	/* 根据设备号，字符块号申请缓存 */
	bp = this->GetBlk(dev, blkno);
	/* 如果在设备队列中找到所需缓存，即B_DONE已设置，就不需进行I/O操作 */
	if (bp->b_flags & Buf::B_DONE)
	{
		return bp;
	}
	/* 没有找到相应缓存，构成I/O读请求块 */
	bp->b_flags |= Buf::B_READ;
	bp->b_wcount = BufferManager::BUFFER_SIZE;

	/*
	 * 将I/O请求块送入相应设备I/O请求队列，如无其它I/O请求，则将立即执行本次I/O请求；
	 * 否则等待当前I/O请求执行完毕后，由中断处理程序启动执行此请求。
	 * 注：Strategy()函数将I/O请求块送入设备请求队列后，不等I/O操作执行完毕，就直接返回。
	 */
	this->m_DiskDriver->IO(bp);
	/* 同步读，等待I/O操作结束 */

	return bp;
}

void BufferManager::Bwrite(Buf* bp)
{
	unsigned int flags;

	flags = bp->b_flags;
	bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
	bp->b_wcount = BufferManager::BUFFER_SIZE;		/* 512字节 */

	this->m_DiskDriver->IO(bp); //完成bp这个Buf块的写任务，并完成Done的置位

		/* 同步写，需要等待I/O操作结束 */
		// this->IOWait(bp);
	// 因为全是同步写，故不做条件判断了
	this->Brelse(bp);

	return;
}

void BufferManager::Bdwrite(Buf* bp)
{
	/* 置上B_DONE允许其它进程使用该磁盘块内容 */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

/* 将dev指定设备队列中延迟写的缓存全部输出到磁盘 */
void BufferManager::Bflush(short dev)
{
	Buf* bp;
	/* 注意：这里之所以要在搜索到一个块之后重新开始搜索，
	 * 因为在bwite()进入到驱动程序中时有开中断的操作，所以
	 * 等到bwrite执行完成后，CPU已处于开中断状态，所以很
	 * 有可能在这期间产生磁盘中断，使得bfreelist队列出现变化，
	 * 如果这里继续往下搜索，而不是重新开始搜索那么很可能在
	 * 操作bfreelist队列的时候出现错误。
	 */
loop:
	for (bp = this->bFreeList.av_forw; bp != &(this->bFreeList); bp = bp->av_forw)
	{
		/* 找出自由队列中所有延迟写的块 */
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
* 清空缓冲区内容
*/
void BufferManager::ClrBuf(Buf* bp)
{
	int* pInt = (int*)bp->b_addr;

	/* 将缓冲区中数据清零 */
	for (unsigned int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
	{
		pInt[i] = 0;
	}
	return;
}