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
	/* ��ʼ��ʱ�������ǵ�һ������ϵͳ
	*  ���/etc/mount.txt �ж������̵�������Ϣ
	*  ��ʽΪ �豸��x  disk_name
	*  ��δʵ��
	*/
class DiskDriver
{
private:
	static const int BLOCK_SIZE = 512;
	static const char* DISK_FILE_NAME; //��Ӳ���ļ���
	static const int DISK_MAX_CNT = 3;
	static const int NSECTOR = 18000;
	int disk_init();
	fstream img_file;

public:
	DiskDriver();
	~DiskDriver();

	bool Initialize(); //��ʼ�������ļ�
	void IO(Buf* bp);
	int GetNBlkDev();
	Devtab* GetDevtab(short dev);
public:
	static const short ROOTDEV = 0;
	static const int NODEV = -1;	/* NODEV�豸�� */
private:
	int nblkdev;
	Devtab* d_tab[DISK_MAX_CNT];
	char* tab_name[DISK_MAX_CNT]; //���Ӳ���ļ��������ڴ�Ӳ���ж�����
};