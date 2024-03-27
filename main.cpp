#include"header.h"
#include "Kernel.h"
#include "Utility.h"
#include "FileManager.h"

char* buffer;
/*
* �� name ���� User �ṹ�� u_dirp��mode ���� u_arg[1]��
* ���ŵ��� FileManager::Open()������ u_ar0���������ļ���fd��
*/
int Fopen(char* name, int mode)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = name;
	u.u_arg[1] = mode;
	u.u_ar0 = -1;
	fmgr.Open();
	return u.u_ar0;
}

/*
 * ����Ϊ�����ļ����� name ���� User �ṹ�� u_dirp��mode ��
 * �� u_arg[1]�����ŵ��� FileManager::Creat()������ u_ar0��
 * �������ļ���fd��
 */
int Fcreat(char* name)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = name;
	u.u_arg[1] = Inode::IRWXU;
	u.u_ar0 = -1;
	fmgr.Creat();
	return u.u_ar0;
}

/* �ر��Ѵ򿪵��ļ����� fd ���� u_arg[0]�����ŵ�
�� FileManager::Close()��*/
void Fclose(int fd)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_ar0 = -1;
	u.u_arg[0] = fd;
	fmgr.Close();
}

/*���ļ��ж�ȡ���ݡ��� fd ���� u_arg[0]��buffer תΪ int �ʹ�
 * �� u_arg[1]��length ���� u_arg[2]�����ŵ���FileManager::Read()��
 * ���� u_ar0������ȡ�����ֽ�����
*/
int Fread(int fd, char* buffer, int length)
{
	User& u = Kernel::Instance().GetUser();
	u.u_error = User::_NOERROR;
	u.u_ar0 = -1;
	u.u_arg[0] = fd;
	u.u_arg[1] = int(buffer);
	u.u_arg[2] = length;

	FileManager& fmgr = Kernel::Instance().GetFileManager();
	fmgr.Read();
	if (u.u_error == User::_EBADF) {
		cout << "bad file number" << endl;
	}
	return u.u_ar0;
}

/* Ϊ���ļ���д�����ݡ��� fd ���� u_arg[0]��buffer 
 * תΪ int �ʹ��� u_arg[1]��length ���� u_arg[2]�����ŵ���
 * FileManager::Write()������ u_ar0����д����ֽ���
 */
int Fwrite(int fd, char* buffer, int length)
{
	User& u = Kernel::Instance().GetUser();
	u.u_error = User::_NOERROR;
	u.u_ar0 = 0;
	u.u_arg[0] = fd;
	u.u_arg[1] = int(buffer);
	u.u_arg[2] = length;

	FileManager& fmgr = Kernel::Instance().GetFileManager();
	fmgr.Write();
	if (u.u_error == User::_EBADF) {
		cout << "bad file number" << endl;
	}
	return u.u_ar0;
}

/* ��ʾ��ǰĿ¼ */
void LS()
{
	User& u = Kernel::Instance().GetUser();
	u.u_error = User::_NOERROR;
	int fd = Fopen(u.u_curdir, File::FREAD);
	char dir_read[32] = "";
	while (Fread(fd,dir_read,sizeof(DirectoryEntry)))
	{
		DirectoryEntry* cur = (DirectoryEntry*)dir_read;
		if (cur->m_ino == 0)
			continue;
		//if (cur->m_name[0] != '.') �����ڼ䣬��ȫ����ʾ
			cout << cur->m_name << endl;

		memset(dir_read, 0, 32);
	}
	Fclose(fd);
}

/* �ı乤��Ŀ¼ 
* �� name ���� u_dirp����תΪint �ʹ��� u_arg[0]������ FileManager::ChDir()
*/
void Cd(char* name)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = name;
	u.u_arg[0] = int(name);
	fmgr.ChDir();
}

/* Ϊ����Ŀ¼�ļ����� name ���� u_dirp��
 * 040755(Ĭ��ģʽ)���� u_arg[1]������ 
 * FileManager::MkNod()�� */
void Mkdir(char* name)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = name;
	u.u_arg[1] = FileManager::DEFAULT_MODE;
	fmgr.MkNod();
}

/* ����Ϊɾ���ļ����� name ���� u_dirp������
FileManager::Unlink() */
void Funlink(char* name)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = name;
	u.u_ar0 = -1;
	fmgr.UnLink();
}

/* ����Ϊ����Ӳ���ӡ�����dst��src������
FileManager::link() */
void Flink(char* src, char* dst)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = src;
	u.u_arg[1] = (int)dst;
	u.u_ar0 = -1;
	fmgr.Link();
}

/*  ����Ϊ�ض�λ�ļ���ǰ��дָ�롣�� fd ���� u_arg[0]��
 * pos ���� u_arg[1]��whence ���� u_arg[2]�����ŵ���
 * FileManager::Seek()������ u_ar0�� */
int Fseek(int fd, int pos, int whence)
{
	User& u = Kernel::Instance().GetUser();
	u.u_error = User::_NOERROR;
	u.u_ar0 = 0;
	u.u_arg[0] = fd;
	u.u_arg[1] = pos;
	u.u_arg[2] = whence;

	FileManager& filemanager = Kernel::Instance().GetFileManager();
	filemanager.Seek();

	return u.u_ar0;
}

/*
* ��ȫ�رն����ļ�ϵͳSecondaryFS�����໺��д�ؾ����ļ�
*/
void shutdown()
{
	FileSystem& FileSys = Kernel::Instance().GetFileSystem();
	/*
	 * @comment ��SuperBlock������ڴ渱�����µ�
	 * �洢�豸��SuperBlock��ȥ
	 * ����Buf���ӳ�д����ҳȫ��д��
	 * ������InodeTable�е��ڴ�inode�ڵ�
	*/	
	FileSys.Update();
}

/* 
 * ����Ϊ���ⲿ�ļ������ڲ��ļ����������������� API ��ʵ��Ҳ��
 * ���� API ����ϣ����ȵ��� Fcreat ������Ϊ intername ��
 * �ļ������Ŵ��ⲿ�ļ�ÿ�ζ�ȡ 1024 ���ֽ�ֱ���ļ�β��ÿ��
 * ȡһ�Σ����� Fwrite ���ڲ��ļ�д���ȡ�������ݣ������� Fclose �ر��ļ���
*/
void Fin(char* extername, char* intername)
{
	fstream exterfile;
	int fd;
	int bufferSize = 1024;
	exterfile.open(extername, ios::in | ios::binary);
	if (!exterfile.good()) {
		cout << "�ⲿ�ļ� "<< extername <<" ��ʧ��" << endl;
	}
	else {
		if ((fd = Fcreat(intername)) < 0) {
			exterfile.close();
			return;
		}
		buffer = new char[bufferSize];
		int i = 0;
		while (!exterfile.eof()) {
			i++;
			if (i == 7)
				i = 7;
			exterfile.read(buffer, bufferSize);
			streamsize bytesRead = exterfile.gcount();
			if (bytesRead > 0) {
				Fwrite(fd, buffer, bytesRead);
			}
		}
		if (buffer) {
			delete[] buffer;
			buffer = nullptr;
		}
		Fclose(fd);
		exterfile.close();
	}
}

/* 
* ����Ϊ���ڲ��ļ������ⲿ�ļ������ȵ��� Fopen 
* ����Ϊ intername ���ļ������Ŵ��ڲ��ļ�ÿ�ζ�ȡ 1024 ���ֽ�
* ֱ���ļ�β��ÿ��ȡһ�Σ�����ȡ��������д���ⲿ�ļ��������� Fclose �ر��ļ���
*/
void Fout(char* intername, char* extername)
{
	fstream exterfile;
	int fd;
	int bufferSize = 1024;
	exterfile.open(extername, ios::out | ios::binary);
	if (!exterfile.good()) {
		cout << "�ⲿ�ļ� " << extername << " ��ʧ��" << endl;
	}
	else {
		if ((fd = Fopen(intername, File::FREAD)) < 0) {
			exterfile.close();
			return;
		}
		buffer = new char[bufferSize];
		int bytesRead;
		while (bytesRead = Fread(fd, buffer, bufferSize)) {
			exterfile.write(buffer, bytesRead);
		}
		if (buffer) {
			delete[] buffer;
			buffer = nullptr;
		}
		Fclose(fd);
		exterfile.close();
	}
}


// ��������ĺ��� return 1��ʾ������0��ʾ�˳�ϵͳ
int parseCommand(const string& command) {
	// ʹ���ַ��������зִ�
	stringstream ss(command);
	string token;
	vector<string> tokens;
	int ret = 1; // 1��ʾ������0��ʾ�˳�ϵͳ
	// ����������洢�� tokens ��
	while (ss >> token) {
		tokens.push_back(token);
	}

	if (tokens[0] == "help" && tokens.size() == 1) {
		Utility::Usage(-1);
	}
	else if (tokens[0] == "exit")
	{
		cout << "exitǿ���˳�ϵͳ" << endl;
		ret = 0;
	}
	else if (tokens[0] == "shutdown") {
		shutdown();
		cout << "System safely exit" << endl;
		ret = 0;
	}
	else if (tokens[0] == "fopen") {
		if (tokens.size() - 1 != 1) {
			Utility::Usage(Utility::u_FOPEN);
		}
		else {
			int fd = Fopen(const_cast<char*>(tokens[1].c_str()), File::FREAD | File::FWRITE);
			if (fd >= 0)
				cout << "fopen return fd:" << fd << endl;
			else
				cout << "��Ч fd:" << fd << endl;
		}
	}
	else if (tokens[0] == "fcreate") {
		if (tokens.size() - 1 != 1) {
			Utility::Usage(Utility::u_CREATE);
		}
		else {
			int fd = Fcreat(const_cast<char*>(tokens[1].c_str()));
			if (fd >= 0)
				cout << "fcreate return fd:" << fd << endl;
			else
				cout << "�����ļ�ʧ�� fd:" << fd << endl;
		}
	}
	else if (tokens[0] == "fclose") {
		if (tokens.size() - 1 != 1) {
			Utility::Usage(Utility::u_FCLOSE);
		}
		else {
			try {
				int fd = stoi(tokens[1]);
				Fclose(fd);
			}
			catch (const std::invalid_argument& e) {
				std::cerr << "Invalid argument: " << e.what() << std::endl;
				Utility::Usage(Utility::u_FCLOSE);
			}
		}
	}
	else if(tokens[0] == "fread"){
		if (tokens.size() - 1 != 2) {
			Utility::Usage(Utility::u_FREAD);
		}
		else {
			try {
				int length = stoi(tokens[2]);
				int fd = stoi(tokens[1]);
				buffer = new char[length + 1];
				memset(buffer, 0, length + 1);
				int res = Fread(fd, buffer, length);
				cout << "fread return:" << res << endl;
				cout << "fread:" << buffer << endl;
				delete buffer;
				buffer = NULL;
			}
			catch (const std::invalid_argument& e) {
				std::cerr << "Invalid argument: " << e.what() << std::endl;
				Utility::Usage(Utility::u_FREAD);
			}
		}
	}
	else if(tokens[0] == "ls"){
		if (tokens.size() - 1 != 0) {
			Utility::Usage(Utility::u_LS);
		}
		else {
			LS();
		}
	}
	else if (tokens[0] == "fwrite") {
		if (tokens.size() - 1 != 3) {
			Utility::Usage(Utility::u_FWRITE);
		}
		else {
			try {
				int length = stoi(tokens[3]);
				int fd = stoi(tokens[1]);
				buffer = new char[length + 1];
				memset(buffer, 0, length + 1);
				memcpy(buffer, tokens[2].c_str(), Utility::Min(length,tokens[2].size()));
				int res = Fwrite(fd, buffer, length);
				cout << "fwrite return:" << res << endl;
				if (buffer != NULL) {
					delete buffer;
					buffer = NULL;
				}
			}
			catch (const invalid_argument& e) {
				std::cerr << "Invalid argument: " << e.what() << std::endl;
				Utility::Usage(Utility::u_FWRITE);
			}
		}
	}
	else if(tokens[0] == "cd"){
		if (tokens.size() - 1 != 1) {
			Utility::Usage(Utility::u_CD);
		}
		else {
			Cd(const_cast<char*>(tokens[1].c_str()));
		}
	}
	else if (tokens[0] == "mkdir") {
		if (tokens.size() - 1 != 1) {
			Utility::Usage(Utility::u_MKDIR);
		}
		else {
			Mkdir(const_cast<char*>(tokens[1].c_str()));
		}
	}
	else if (tokens[0] == "funlink")
	{
		if (tokens.size() - 1 != 1) {
			Utility::Usage(Utility::u_FUNLINK);
		}
		else {
			Funlink(const_cast<char*>(tokens[1].c_str()));
		}
	}
	else if (tokens[0] == "flink")
	{
		if (tokens.size() - 1 != 2) {
			Utility::Usage(Utility::u_FLINK);
		}
		else {
			Flink(const_cast<char*>(tokens[1].c_str()), const_cast<char*>(tokens[2].c_str()));
		}
	}
	else if (tokens[0] == "fseek")
	{
		if (tokens.size() - 1 != 2) {
			Utility::Usage(Utility::u_LSEEK);
		}
		else {
			try {
				int offset = stoi(tokens[2]);
				int fd = stoi(tokens[1]);
				int res = Fseek(fd, offset, SEEK_SET);
			}
			catch (const std::invalid_argument& e) {
				std::cerr << "Invalid argument: " << e.what() << std::endl;
				Utility::Usage(Utility::u_LSEEK);
			}
		}
	}
	else if(tokens[0] == "fin"){
		if (tokens.size() - 1 != 2) {
			Utility::Usage(Utility::u_FIN);
		}
		else {
			Fin(const_cast<char*>(tokens[1].c_str()), const_cast<char*>(tokens[2].c_str()));
		}
	}
	else if(tokens[0] == "fout"){
		if (tokens.size() - 1 != 2) {
			Utility::Usage(Utility::u_FOUT);
		}
		else {
			Fout(const_cast<char*>(tokens[1].c_str()), const_cast<char*>(tokens[2].c_str()));
		}
	}
	else
		cout << "ϵͳ�޴�����" << endl;

	return ret;
}

int main()
{
	/* �ں˳�ʼ������Ҫ���ں�ȫ�ֶ�������ù�ϵ����
	�Լ�������̵Ĵ�������ʼ�������Ѵ���������һ��*/
	Kernel::Instance().Initialize();
	cout << endl;
	string command;
	cout << "Welcome to the CC's SecondFileSysten!" << endl << endl;
	string output;
	int state = 1;
	while (state) {
		string dir = Kernel::Instance().GetUser().u_curdir;
		output = "[root@SecondFS " + dir + "]# ";
		cout << output;
		// ��ȡ�û����������
		getline(cin, command);
		if (command.size() == 0)
			continue;
		// ����û����� 'exit'�����˳�ѭ��
		if (command == "exit") {
			cout << "Exiting terminal simulation..." << endl;
			break;
		}

		// ��������
		state = parseCommand(command);
	}

	return 0;
}