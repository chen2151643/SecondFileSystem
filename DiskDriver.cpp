#include "DiskDriver.h"
#include "DirectoryEntry.h"
#include "FileSystem.h"
#include "INode.h"
#include "Utility.h"

DiskDriver g_DiskDriver;

DiskDriver::DiskDriver(){
	nblkdev = 0;
	for (int i = 0; i < DiskDriver::DISK_MAX_CNT; i++) {
		tab_name[i] = new char[29]; // 预留一个字节用于存储字符串结尾的空字符 '\0'
		std::memset(tab_name[i], 0, 29); // 将分配的内存清零
	}
}
DiskDriver::~DiskDriver(){
	for (int i = 0; i < DiskDriver::DISK_MAX_CNT; i++) {
		if (d_tab[i] != NULL)
			delete d_tab[i];
		if (tab_name[i] != NULL)
			delete tab_name[i];
	}
}

/*
void show_size()
{
	std::ifstream file("disk_image.img", std::ios::binary | std::ios::ate); // 打开文件并将文件指针移到文件末尾

	if (file.is_open()) {
		std::streampos size = file.tellg(); // 获取文件指针的位置，即文件大小
		std::cout << "文件大小为：" << size << " 字节" << std::endl;
		file.close();
	}
	else {
		std::cout << "无法打开文件" << std::endl;
	}
	exit(-1);
}
*/

const char* DiskDriver::DISK_FILE_NAME = "disk_image.img";
/*
* 初始化s_isize，s_fsize
* 空闲块管理初始化
* 空闲inode管理
* 这里考虑了主硬盘初始文件结构的构造，提前写死了用掉的inode
* 之后完善后我们初始化只让文件结构初始有一个根目录
* 其余目录再写入
*/
void init_spb(SuperBlock& spb)
{
	spb.s_isize = FileSystem::INODE_ZONE_SIZE; /* INode区所占数据块数量*/
	spb.s_fsize = FileSystem::DATA_ZONE_END_SECTOR + 1;  /* 磁盘数据块总数 */

	//第一组99块（区分栈底） 其他都是一百块一组 剩下的被超级快直接管理
	spb.s_nfree = (FileSystem::DATA_ZONE_SIZE - FileSystem::DATA_INIT_SECTOR - 99) % 100;

	//超级快直接管理的空闲盘块的第一个盘块的盘块号
	//成组链表法
	int start_last_datablk = FileSystem::DATA_ZONE_START_SECTOR;
	//将末尾3块用作初始目录的存储
	for (;;)
		if ((start_last_datablk + 100 - 1) < FileSystem::DATA_ZONE_END_SECTOR - FileSystem::DATA_INIT_SECTOR)//判断剩下盘块是否还有100个
			start_last_datablk += 100;
		else
			break;
	start_last_datablk--; // 由于第一组只有99块
	for (int i = 0; i < spb.s_nfree; i++)
		spb.s_free[i] = start_last_datablk + i;

	spb.s_ninode = 100;
	// 留出INODE_INIT_NUM个INODE用作初始目录文件的inode
	for (int i = 1; i <= spb.s_ninode; i++)
		spb.s_inode[i-1] = FileSystem::INODE_INIT_NUM+i;//注：这里只是diskinode的编号，真正取用的时候要进行盘块的转换
	// 此处需要修改，等文件系统完善后，写入初始文件和目录不放在这些固定初始化流程的函数中

	spb.s_fmod = 0;
	spb.s_ronly = 0;
}

/*
* 把数据区空闲块管理起来
*/
void init_datablock(char* data)
{
	struct {
		int nfree;//本组空闲的个数
		int free[100];//本组空闲的索引表
	}tmp_table;

	int last_datablk_num = FileSystem::DATA_ZONE_SIZE - FileSystem::DATA_INIT_SECTOR;//未加入索引的盘块的数量
	//注:成组连接法,必须的初始化索引表
	for (int i = 0;; i++)
	{
		if (last_datablk_num >= 100)
			tmp_table.nfree = 100;
		else
			tmp_table.nfree = last_datablk_num;
		last_datablk_num -= tmp_table.nfree;

		if (last_datablk_num == 0)
			break;

		for (int j = 0; j < tmp_table.nfree; j++)
		{
			if (i == 0 && j == 0)
				tmp_table.free[j] = 0;//栈底
			else
			{
				tmp_table.free[j] = 100 * i + j + FileSystem::DATA_ZONE_START_SECTOR - 1;
			}
		}
		memcpy(&data[99 * 512 + i * 100 * 512], (void*)&tmp_table, sizeof(tmp_table));
	}
}

/*
* disk_init是基于已打开的img_file的文件流，对打开的磁盘进行初始化的函数
* 完善后此处写死的初始目录结构分离出，初始化只含根目录即可，如此可用于非主硬盘
*/
int DiskDriver::disk_init()
{
	SuperBlock spb;
	init_spb(spb);
	DiskInode* dinode = new DiskInode[FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR];

	//设置rootDiskInode的初始值 和初始的home目录的初始值
	// 根目录
	dinode[1].d_mode = Inode::IFDIR; //文件类型为目录
	dinode[1].d_mode |= Inode::IRWXU;
	dinode[1].d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR;
	dinode[1].d_nlink = 1;
	dinode[1].d_size = 2 * sizeof(DirectoryEntry);
	dinode[1].d_atime = Utility::GetTime();
	dinode[1].d_mtime = Utility::GetTime();

	DirectoryEntry dir1[2] = {
		{1,"."},
		{1,".."},
	};

	char* datablock = new char[FileSystem::DATA_ZONE_SIZE * 512];
	memset(datablock, 0, FileSystem::DATA_ZONE_SIZE * 512);
	init_datablock(datablock);

	//装填初始目录文件信息
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 1), dir1, dinode[1].d_size);

	// 写入superblock
	// 写入inode区
	// 写入数据块区
	img_file.write((char*)&spb, sizeof(SuperBlock)); 
	img_file.write((char*)dinode, FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR * sizeof(DiskInode));
	img_file.write(datablock, FileSystem::DATA_ZONE_SIZE * 512);

	return 0;
}

// 由Kernel的InitDiskDriver调用
// 进行磁盘的格式化
bool DiskDriver::Initialize()
{
	bool is_disk_format = true;
	img_file.open(DiskDriver::DISK_FILE_NAME, ios::in | ios::binary);
	// 尝试打开文件，若失败则说明未存在磁盘镜像文件，需创建并初始化
	if (!img_file.good()) {
		// File does not exist or could not be opened
		// 磁盘镜像文件不存在
		img_file.clear();
		// 创建磁盘镜像文件
		img_file.open(DiskDriver::DISK_FILE_NAME, ios::out | ios::binary);
		if (!img_file.good()) {
			cout << "failed to create disk image file" << endl;
			exit(-1);
		}
		// 此处为创建主硬盘的块设备表
		d_tab[nblkdev++] =new Devtab();
		Utility::StringCopy(DiskDriver::DISK_FILE_NAME,tab_name[0]);

		disk_init(); // 磁盘格式化
		img_file.close(); //关闭并保存
	}
	else {
		// 文件已存在，则说明有一块初始化过的磁盘文件
		// 磁盘初始化已完成
		// 若存在则成功打开，需要关闭
		/*
			由于考虑加入挂载功能，为了在初始化时获取已挂载的硬盘信息
			本系统将挂载信息放在主硬盘根目录下  "/etc/mount.txt" 中
			所以之后在完成文件读写后，需在此处补上读挂载配置文件的内容
			主要是nblkdev的更新和new Devtab的创建，文件驱动是磁盘文件名
		*/
		is_disk_format = false;
		d_tab[nblkdev++] = new Devtab();
		Utility::StringCopy(DiskDriver::DISK_FILE_NAME, tab_name[0]);
		img_file.close();
	}
	return is_disk_format;
}

int DiskDriver::GetNBlkDev()
{
	return this->nblkdev;
}

/*
* 根据设备号dev返回指向对应Devtab结构的指针
*/
Devtab* DiskDriver::GetDevtab(short dev)
{
	if (dev < 0 || dev >= this->nblkdev) {
		//cerr << "设备号不合法" << endl;
		Utility::Panic("dev number illegal");
	}
	return this->d_tab[dev];
}

/* 本单进程单用户系统读写皆为同步，让一次IO就清空IO队列的任务
 实际上，每次IO队列里只有一个IO请求块，长度为1，因为来一个完成一个
 */
void DiskDriver::IO(Buf* bp)
{
	/* 判断块号是否合法 */
	if (bp->b_blkno >= DiskDriver::NSECTOR)
	{
		/* 设置出错标志 */
		bp->b_flags |= Buf::B_ERROR;
		Utility::Panic("out of number of block, error");
	}
	
	/* 将bp加入I/O请求队列的队尾，此时I/O队列已经退化到单链表形式，将bp->av_forw == NULL标志着链表结尾 */
	bp->av_forw = NULL;
	short dp = bp->b_dev;
	if (this->d_tab[dp]->d_actf == NULL)
	{
		this->d_tab[dp]->d_actf = bp;
	}
	else
	{
		this->d_tab[dp]->d_actl->av_forw = bp;
	}
	this->d_tab[dp]->d_actl = bp;

	/* 接下来把IO队列全部清空 */
	if (this->d_tab[dp]->d_active == 0)		/* 磁盘空闲 */
	{
		this->d_tab[dp]->d_active++;	/* I/O请求队列不空，设置控制器忙标志 */
		/* 遍历该块设备的IO请求队列 */

		for (Buf* bp_i = this->d_tab[dp]->d_actf ;bp_i!= NULL;bp_i = bp_i->av_forw) {
			/* 完成这个Buf块的读写任务 */
			/* 一次IO */
			int io_dev= bp_i->b_dev;
			int offset = bp_i->b_blkno * DiskDriver::BLOCK_SIZE; 
			int flags = bp_i->b_flags;
			img_file.open(tab_name[io_dev], ios::in | ios::out | ios::binary);

			if (!img_file.good()) {
				Utility::Panic("IO failed to open disk image file\n");
			}
			else {
				img_file.seekg(offset, std::ios::beg); // 从文件开头偏移
				if(flags & Buf::BufFlag::B_READ)
					img_file.read((char*)bp_i->b_addr, bp_i->b_wcount);
				else
					img_file.write((char*)bp_i->b_addr, bp_i->b_wcount);
			}
			img_file.close();// IO结束
			/* 完成IO请求后，将该Buf块的Done置1 */
			bp_i->b_flags |= Buf::B_DONE;
			/* 并把该Bug从IO请求队列里取出 */
			this->d_tab[dp]->d_actf = bp_i->av_forw;
		}
		this->d_tab[dp]->d_active = 0;
	}
	return;
}

/*
* 设备队列初始化指向自己
*/
Devtab::Devtab()
{
	this->d_active = 0;
	this->d_errcnt = 0;
	this->b_forw = (Buf *)this; // 设别块管理
	this->b_back = (Buf *)this;
	this->d_actf = NULL; // IO请求队列
	this->d_actl = NULL;

}
Devtab::~Devtab()
{
	//nothing to do here
}