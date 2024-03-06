#pragma once

#include <fstream>
#include "header.h"
#include "Buf.h"

class DiskDriver
{
private:
	static const int BLOCK_SIZE = 512;
	static const char* DISK_FILE_NAME;
	int disk_init();
	fstream img_file;

public:
	DiskDriver();
	~DiskDriver();

	void Initialize(); //初始化镜像文件
	void IO(Buf* bp);
};