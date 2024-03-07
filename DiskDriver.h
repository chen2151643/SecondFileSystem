#pragma once

#include <fstream>
#include "header.h"
#include "Buf.h"

class Devtab
{
public:
	static const int DEV_NAME_SIZE = 28;
	Devtab(const char* name);
	~Devtab();

public:
	char dev_name[DEV_NAME_SIZE]; //��������ļ���
	int d_active; //��0��Ӳ��æ
	int d_errcnt; // ��ǰIO�����������
	Buf* b_forw; //�豸������У��������������ô��̵����л���
	Buf* b_back; 
	Buf* d_actf; // IO������У����׻���IOִ���У����໺��IO�����ȴ�ִ��
	Buf* d_actl;
};

/*
* �ۺ���BlockDevice��DeviceManager�������Ĺ���
* ��Ϊû���ַ��豸���԰�BlockDevice��DeviceManager�ϲ�
* �����ļ�ϵͳ������Ϊ ������̵��ļ�������ͨ���ļ����򿪶�Ӧ���豸
*/
class DiskDriver
{
private:
	static const int BLOCK_SIZE = 512;
	static const char* DISK_FILE_NAME; //��Ӳ���ļ���
	static const int DISK_MAX_CNT = 3;
	int disk_init();
	fstream img_file;

public:
	DiskDriver();
	~DiskDriver();

	void Initialize(); //��ʼ�������ļ�
	void IO(Buf* bp);

public:
	int nblkdev;
	/* ��ʼ��ʱ�������ǵ�һ������ϵͳ
	*  ���/etc/mount.txt �ж������̵�������Ϣ
	*  ��ʽΪ �豸��x  disk_name
	*/
	Devtab* d_tab[DISK_MAX_CNT];
};