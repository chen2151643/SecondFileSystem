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
	std::ifstream file("disk_image.img", std::ios::binary | std::ios::ate); // ���ļ������ļ�ָ���Ƶ��ļ�ĩβ

	if (file.is_open()) {
		std::streampos size = file.tellg(); // ��ȡ�ļ�ָ���λ�ã����ļ���С
		std::cout << "�ļ���СΪ��" << size << " �ֽ�" << std::endl;
		file.close();
	}
	else {
		std::cout << "�޷����ļ�" << std::endl;
	}
	exit(-1);
}
*/

const char* DiskDriver::DISK_FILE_NAME = "disk_image.img";
/*
* ��ʼ��s_isize��s_fsize
* ���п�����ʼ��
* ����inode����
*/
void init_spb(SuperBlock& spb)
{
	spb.s_isize = FileSystem::INODE_ZONE_SIZE; /* INode����ռ���ݿ�����*/
	spb.s_fsize = FileSystem::DATA_ZONE_END_SECTOR + 1;  /* �������ݿ����� */

	//��һ��99�飨����ջ�ף� ��������һ�ٿ�һ�� ʣ�µı�������ֱ�ӹ���
	spb.s_nfree = (FileSystem::DATA_ZONE_SIZE - 99) % 100;

	//������ֱ�ӹ���Ŀ����̿�ĵ�һ���̿���̿��
	//��������
	int start_last_datablk = FileSystem::DATA_ZONE_START_SECTOR;
	//��ĩβ3��������ʼĿ¼�Ĵ洢
	for (;;)
		if ((start_last_datablk + 100 - 1) < FileSystem::DATA_ZONE_END_SECTOR - FileSystem::DATA_INIT_SECTOR)//�ж�ʣ���̿��Ƿ���100��
			start_last_datablk += 100;
		else
			break;
	start_last_datablk--; // ���ڵ�һ��ֻ��99��
	for (int i = 0; i < spb.s_nfree; i++)
		spb.s_free[i] = start_last_datablk + i;

	spb.s_ninode = 100;
	// ����INODE_INIT_NUM��INODE������ʼĿ¼�ļ���inode
	for (int i = 1; i <= spb.s_ninode; i++)
		spb.s_inode[i] = FileSystem::INODE_INIT_NUM+i;//ע������ֻ��diskinode�ı�ţ�����ȡ�õ�ʱ��Ҫ�����̿��ת��

	spb.s_fmod = 0;
	spb.s_ronly = 0;
}

void init_datablock(char* data)
{
	struct {
		int nfree;//������еĸ���
		int free[100];//������е�������
	}tmp_table;

	int last_datablk_num = FileSystem::DATA_ZONE_SIZE - FileSystem::DATA_INIT_SECTOR;//δ�����������̿������
	//ע:�������ӷ�,����ĳ�ʼ��������
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
				tmp_table.free[j] = 0;//ջ��
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

	//����rootDiskInode�ĳ�ʼֵ �ͳ�ʼ��homeĿ¼�ĳ�ʼֵ
	// ��Ŀ¼
	dinode[1].d_mode = Inode::IFDIR; //�ļ�����ΪĿ¼
	dinode[1].d_mode |= Inode::IEXEC; //���ļ���ִ��Ȩ��
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR;
	dinode[1].d_nlink = 1;
	dinode[1].d_size = 4 * sizeof(DirectoryEntry);

	DirectoryEntry dir1[4] = {
		{1,"."},
		{1,".."},
		{2,"home"},
		{3,"etc"}
	};
	// /homeĿ¼
	dinode[2].d_mode = Inode::IFDIR; //�ļ�����ΪĿ¼
	dinode[2].d_mode |= Inode::IEXEC; //���ļ���ִ��Ȩ��
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR - 1;
	dinode[2].d_nlink = 1;
	dinode[2].d_size = 2 * sizeof(DirectoryEntry);

	DirectoryEntry dir2[2] = {
		{2,"."},
		{1,".."},
	};
	// /etcĿ¼
	dinode[3].d_mode = Inode::IFDIR; //�ļ�����ΪĿ¼
	dinode[3].d_mode |= Inode::IEXEC; //���ļ���ִ��Ȩ��
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR - 2;
	dinode[3].d_nlink = 1;
	dinode[3].d_size = 3 * sizeof(DirectoryEntry);

	DirectoryEntry dir3[3] = {
		{3,"."},
		{1,".."},
		{4,"mount.txt"}
	};
	// /etc/mount.txt�ļ�
	dinode[4].d_mode = 0; //�ļ�����ΪĿ¼
	dinode[4].d_mode |= Inode::IREAD; //���ļ��Ķ�Ȩ��
	dinode[4].d_mode |= Inode::IWRITE; //���ļ��Ķ�Ȩ��
	dinode->d_addr[0] = FileSystem::DATA_ZONE_END_SECTOR - 3;
	dinode[4].d_nlink = 1;
	dinode[4].d_size = 0;

	char* datablock = new char[FileSystem::DATA_ZONE_SIZE * 512];
	memset(datablock, 0, FileSystem::DATA_ZONE_SIZE * 512);
	init_datablock(datablock);

	//װ���ʼĿ¼�ļ���Ϣ
	//inode��ǰ2�飬data��ĩ2��
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 1), dir1, dinode[1].d_size);
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 2), dir2, dinode[2].d_size);
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 3), dir3, dinode[3].d_size);
	// #4�Ų��ã���Ϊ��ʼΪ��

	// д��superblock
	// д��inode��
	// д�����ݿ���
	img_file.write((char*)&spb, sizeof(SuperBlock)); 
	//cout << sizeof(SuperBlock) << endl;
	img_file.write((char*)dinode, FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR * sizeof(DiskInode));
	//cout << FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR * sizeof(DiskInode) << endl;
	img_file.write(datablock, FileSystem::DATA_ZONE_SIZE * 512);
	//cout << FileSystem::DATA_ZONE_SIZE * 512 << endl;

	return 0;
}

// ��Kernel��InitDiskDriver����
void DiskDriver::Initialize()
{
	img_file.open(DiskDriver::DISK_FILE_NAME, ios::in | ios::binary);
	// ���Դ��ļ�����ʧ����˵��δ���ڴ��̾����ļ����贴������ʼ��
	if (!img_file.good()) {
		// File does not exist or could not be opened
		// ���̾����ļ�������
		img_file.clear();
		// �������̾����ļ�
		img_file.open(DiskDriver::DISK_FILE_NAME, ios::out | ios::binary);
		if (!img_file.good()) {
			cout << "���������ļ�ʧ�ܣ�failed" << endl;
			exit(-1);
		}
		d_tab[nblkdev++] =new Devtab(DiskDriver::DISK_FILE_NAME);
		disk_init(); // ���̸�ʽ��
		img_file.close(); //�رղ�����
	}
	else {
		// �ļ��Ѵ��ڣ���˵����һ���ʼ�����Ĵ����ļ�
		// ���̳�ʼ�������
		// ��������ɹ��򿪣���Ҫ�ر�
		/*
			���ڿ��Ǽ�����ع��ܣ�Ϊ���ڳ�ʼ��ʱ��ȡ�ѹ��ص�Ӳ����Ϣ
			��ϵͳ��������Ϣ������Ӳ�̸�Ŀ¼��  "/etc/mount.txt" ��
			����֮��������ļ���д�����ڴ˴����϶����������ļ�������

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
	// �����ָ���dev_name
}
Devtab::~Devtab()
{
	//nothing to do here
}