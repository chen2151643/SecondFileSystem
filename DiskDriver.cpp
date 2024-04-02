#include "DiskDriver.h"
#include "DirectoryEntry.h"
#include "FileSystem.h"
#include "INode.h"
#include "Utility.h"

DiskDriver g_DiskDriver;

DiskDriver::DiskDriver(){
	nblkdev = 0;
	for (int i = 0; i < DiskDriver::DISK_MAX_CNT; i++) {
		tab_name[i] = new char[29]; // Ԥ��һ���ֽ����ڴ洢�ַ�����β�Ŀ��ַ� '\0'
		std::memset(tab_name[i], 0, 29); // ��������ڴ�����
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
* ���￼������Ӳ�̳�ʼ�ļ��ṹ�Ĺ��죬��ǰд�����õ���inode
* ֮�����ƺ����ǳ�ʼ��ֻ���ļ��ṹ��ʼ��һ����Ŀ¼
* ����Ŀ¼��д��
*/
void init_spb(SuperBlock& spb)
{
	spb.s_isize = FileSystem::INODE_ZONE_SIZE; /* INode����ռ���ݿ�����*/
	spb.s_fsize = FileSystem::DATA_ZONE_END_SECTOR + 1;  /* �������ݿ����� */

	//��һ��99�飨����ջ�ף� ��������һ�ٿ�һ�� ʣ�µı�������ֱ�ӹ���
	spb.s_nfree = (FileSystem::DATA_ZONE_SIZE - FileSystem::DATA_INIT_SECTOR - 99) % 100;

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
		spb.s_inode[i-1] = FileSystem::INODE_INIT_NUM+i;//ע������ֻ��diskinode�ı�ţ�����ȡ�õ�ʱ��Ҫ�����̿��ת��
	// �˴���Ҫ�޸ģ����ļ�ϵͳ���ƺ�д���ʼ�ļ���Ŀ¼��������Щ�̶���ʼ�����̵ĺ�����

	spb.s_fmod = 0;
	spb.s_ronly = 0;
}

/*
* �����������п��������
*/
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

/*
* disk_init�ǻ����Ѵ򿪵�img_file���ļ������Դ򿪵Ĵ��̽��г�ʼ���ĺ���
* ���ƺ�˴�д���ĳ�ʼĿ¼�ṹ���������ʼ��ֻ����Ŀ¼���ɣ���˿����ڷ���Ӳ��
*/
int DiskDriver::disk_init()
{
	SuperBlock spb;
	init_spb(spb);
	DiskInode* dinode = new DiskInode[FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR];

	//����rootDiskInode�ĳ�ʼֵ �ͳ�ʼ��homeĿ¼�ĳ�ʼֵ
	// ��Ŀ¼
	dinode[1].d_mode = Inode::IFDIR; //�ļ�����ΪĿ¼
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

	//װ���ʼĿ¼�ļ���Ϣ
	memcpy(datablock + 512 * (FileSystem::DATA_ZONE_SIZE - 1), dir1, dinode[1].d_size);

	// д��superblock
	// д��inode��
	// д�����ݿ���
	img_file.write((char*)&spb, sizeof(SuperBlock)); 
	img_file.write((char*)dinode, FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR * sizeof(DiskInode));
	img_file.write(datablock, FileSystem::DATA_ZONE_SIZE * 512);

	return 0;
}

// ��Kernel��InitDiskDriver����
// ���д��̵ĸ�ʽ��
bool DiskDriver::Initialize()
{
	bool is_disk_format = true;
	img_file.open(DiskDriver::DISK_FILE_NAME, ios::in | ios::binary);
	// ���Դ��ļ�����ʧ����˵��δ���ڴ��̾����ļ����贴������ʼ��
	if (!img_file.good()) {
		// File does not exist or could not be opened
		// ���̾����ļ�������
		img_file.clear();
		// �������̾����ļ�
		img_file.open(DiskDriver::DISK_FILE_NAME, ios::out | ios::binary);
		if (!img_file.good()) {
			cout << "failed to create disk image file" << endl;
			exit(-1);
		}
		// �˴�Ϊ������Ӳ�̵Ŀ��豸��
		d_tab[nblkdev++] =new Devtab();
		Utility::StringCopy(DiskDriver::DISK_FILE_NAME,tab_name[0]);

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
			��Ҫ��nblkdev�ĸ��º�new Devtab�Ĵ������ļ������Ǵ����ļ���
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
* �����豸��dev����ָ���ӦDevtab�ṹ��ָ��
*/
Devtab* DiskDriver::GetDevtab(short dev)
{
	if (dev < 0 || dev >= this->nblkdev) {
		//cerr << "�豸�Ų��Ϸ�" << endl;
		Utility::Panic("dev number illegal");
	}
	return this->d_tab[dev];
}

/* �������̵��û�ϵͳ��д��Ϊͬ������һ��IO�����IO���е�����
 ʵ���ϣ�ÿ��IO������ֻ��һ��IO����飬����Ϊ1����Ϊ��һ�����һ��
 */
void DiskDriver::IO(Buf* bp)
{
	/* �жϿ���Ƿ�Ϸ� */
	if (bp->b_blkno >= DiskDriver::NSECTOR)
	{
		/* ���ó����־ */
		bp->b_flags |= Buf::B_ERROR;
		Utility::Panic("out of number of block, error");
	}
	
	/* ��bp����I/O������еĶ�β����ʱI/O�����Ѿ��˻�����������ʽ����bp->av_forw == NULL��־�������β */
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

	/* ��������IO����ȫ����� */
	if (this->d_tab[dp]->d_active == 0)		/* ���̿��� */
	{
		this->d_tab[dp]->d_active++;	/* I/O������в��գ����ÿ�����æ��־ */
		/* �����ÿ��豸��IO������� */

		for (Buf* bp_i = this->d_tab[dp]->d_actf ;bp_i!= NULL;bp_i = bp_i->av_forw) {
			/* ������Buf��Ķ�д���� */
			/* һ��IO */
			int io_dev= bp_i->b_dev;
			int offset = bp_i->b_blkno * DiskDriver::BLOCK_SIZE; 
			int flags = bp_i->b_flags;
			img_file.open(tab_name[io_dev], ios::in | ios::out | ios::binary);

			if (!img_file.good()) {
				Utility::Panic("IO failed to open disk image file\n");
			}
			else {
				img_file.seekg(offset, std::ios::beg); // ���ļ���ͷƫ��
				if(flags & Buf::BufFlag::B_READ)
					img_file.read((char*)bp_i->b_addr, bp_i->b_wcount);
				else
					img_file.write((char*)bp_i->b_addr, bp_i->b_wcount);
			}
			img_file.close();// IO����
			/* ���IO����󣬽���Buf���Done��1 */
			bp_i->b_flags |= Buf::B_DONE;
			/* ���Ѹ�Bug��IO���������ȡ�� */
			this->d_tab[dp]->d_actf = bp_i->av_forw;
		}
		this->d_tab[dp]->d_active = 0;
	}
	return;
}

/*
* �豸���г�ʼ��ָ���Լ�
*/
Devtab::Devtab()
{
	this->d_active = 0;
	this->d_errcnt = 0;
	this->b_forw = (Buf *)this; // �������
	this->b_back = (Buf *)this;
	this->d_actf = NULL; // IO�������
	this->d_actl = NULL;

}
Devtab::~Devtab()
{
	//nothing to do here
}