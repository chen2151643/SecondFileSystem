#include "DiskDriver.h"
#include "DirectoryEntry.h"
#include "FileSystem.h"
#include "INode.h"

DiskDriver g_DiskDriver;

DiskDriver::DiskDriver(){
	nblkdev = 0;
}
DiskDriver::~DiskDriver(){
	for (int i = 0; i < DiskDriver::DISK_MAX_CNT; i++)
		if (d_tab[i] != NULL)
			delete d_tab[i];
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
*/
void init_spb(SuperBlock& spb)
{
	spb.s_isize = FileSystem::INODE_ZONE_SIZE; /* INode区所占数据块数量*/
	spb.s_fsize = FileSystem::DATA_ZONE_END_SECTOR + 1;  /* 磁盘数据块总数 */

	//第一组99块（区分栈底） 其他都是一百块一组 剩下的被超级快直接管理
	spb.s_nfree = (FileSystem::DATA_ZONE_SIZE - 99) % 100;

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
		spb.s_inode[i] = FileSystem::INODE_INIT_NUM+i;//注：这里只是diskinode的编号，真正取用的时候要进行盘块的转换

	spb.s_fmod = 0;
	spb.s_ronly = 0;
}

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

int DiskDriver::disk_init()
{
	SuperBlock spb;
	init_spb(spb);
	DiskInode* dinode = new DiskInode[FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR];

	//设置rootDiskInode的初始值 和初始的home目录的初始值
	// 根目录
	dinode[1].d_mode = Inode::IFDIR; //文件类型为目录
	dinode[1].d_mode |= Inode::IEXEC; //对文件的执行权限
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR;
	dinode[1].d_nlink = 1;
	dinode[1].d_size = 4 * sizeof(DirectoryEntry);

	DirectoryEntry dir1[4] = {
		{1,"."},
		{1,".."},
		{2,"home"},
		{3,"etc"}
	};
	// /home目录
	dinode[2].d_mode = Inode::IFDIR; //文件类型为目录
	dinode[2].d_mode |= Inode::IEXEC; //对文件的执行权限
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR - 1;
	dinode[2].d_nlink = 1;
	dinode[2].d_size = 2 * sizeof(DirectoryEntry);

	DirectoryEntry dir2[2] = {
		{2,"."},
		{1,".."},
	};
	// /etc目录
	dinode[3].d_mode = Inode::IFDIR; //文件类型为目录
	dinode[3].d_mode |= Inode::IEXEC; //对文件的执行权限
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR - 2;
	dinode[3].d_nlink = 1;
	dinode[3].d_size = 3 * sizeof(DirectoryEntry);

	DirectoryEntry dir3[3] = {
		{3,"."},
		{1,".."},
		{4,"mount.txt"}
	};
	// /etc/mount.txt文件
	dinode[4].d_mode = 0; //文件类型为目录
	dinode[4].d_mode |= Inode::IREAD; //对文件的读权限
	dinode[4].d_mode |= Inode::IWRITE; //对文件的读权限
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR - 3;
	dinode[4].d_nlink = 1;
	dinode[4].d_size = 0;

	char* datablock = new char[FileSystem::DATA_ZONE_SIZE * 512];
	memset(datablock, 0, FileSystem::DATA_ZONE_SIZE * 512);
	init_datablock(datablock);

	//装填初始目录文件信息
	//inode区前2块，data区末2块
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 1), dir1, dinode[1].d_size);
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 2), dir2, dinode[2].d_size);
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 3), dir3, dinode[3].d_size);
	// #4号不用，因为初始为空

	// 写入superblock
	// 写入inode区
	// 写入数据块区
	img_file.write((char*)&spb, sizeof(SuperBlock)); 
	//cout << sizeof(SuperBlock) << endl;
	img_file.write((char*)dinode, FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR * sizeof(DiskInode));
	//cout << FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR * sizeof(DiskInode) << endl;
	img_file.write(datablock, FileSystem::DATA_ZONE_SIZE * 512);
	//cout << FileSystem::DATA_ZONE_SIZE * 512 << endl;

	return 0;
}

// 由Kernel的InitDiskDriver调用
void DiskDriver::Initialize()
{
	img_file.open(DiskDriver::DISK_FILE_NAME, ios::in | ios::binary);
	// 尝试打开文件，若失败则说明未存在磁盘镜像文件，需创建并初始化
	if (!img_file.good()) {
		// File does not exist or could not be opened
		// 磁盘镜像文件不存在
		img_file.clear();
		// 创建磁盘镜像文件
		img_file.open(DiskDriver::DISK_FILE_NAME, ios::out | ios::binary);
		if (!img_file.good()) {
			cout << "创建磁盘文件失败，failed" << endl;
			exit(-1);
		}
		d_tab[nblkdev++] =new Devtab(DiskDriver::DISK_FILE_NAME);
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

		*/
		img_file.close();
	}
}

Devtab::Devtab(const char* name)
{
	this->d_active = 0;
	this->d_errcnt = 0;
	this->b_forw = NULL;
	this->b_back = NULL;
	this->d_actf = NULL;
	this->d_actl = NULL;

	int i = 0;
	while ((dev_name[i++] = name[i++]) != 0);
	// 将名字赋给dev_name
}
Devtab::~Devtab()
{
	//nothing to do here
}