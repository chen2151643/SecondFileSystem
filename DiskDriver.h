#pragma once

#include <fstream>
#include "header.h"
#include "Buf.h"

class Devtab
{
public:
	Devtab();
	~Devtab();

public:
	int d_active; //非0，硬盘忙
	int d_errcnt; // 当前IO操作出错次数
	Buf* b_forw; //设备缓存队列，用来管理分配给该磁盘的所有缓存
	Buf* b_back; 
	Buf* d_actf; // IO请求队列，队首缓存IO执行中，其余缓存IO操作等待执行
	Buf* d_actl;
};

/*
* 综合了BlockDevice和DeviceManager和驱动的功能
* 因为没有字符设备所以把BlockDevice和DeviceManager合并
* 二级文件系统的驱动为 镜像磁盘的文件名，可通过文件名打开对应块设备
*/
	/* 初始化时，若不是第一次启动系统
	*  则从/etc/mount.txt 中读挂载盘的配置信息
	*  格式为 设备号x  disk_name
	*  暂未实现
	*/
class DiskDriver
{
private:
	static const int BLOCK_SIZE = 512;
	static const char* DISK_FILE_NAME; //主硬盘文件名
	static const int DISK_MAX_CNT = 3;
	static const int NSECTOR = 18000;
	int disk_init();
	fstream img_file;

public:
	DiskDriver();
	~DiskDriver();

	bool Initialize(); //初始化镜像文件
	void IO(Buf* bp);
	int GetNBlkDev();
	Devtab* GetDevtab(short dev);
public:
	static const short ROOTDEV = 0;
	static const int NODEV = -1;	/* NODEV设备号 */
private:
	int nblkdev;
	Devtab* d_tab[DISK_MAX_CNT];
	char* tab_name[DISK_MAX_CNT]; //存放硬盘文件名，用于从硬盘中读数据
};