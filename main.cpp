#include"header.h"
#include "Kernel.h"
#include "Utility.h"
#include "FileManager.h"

char* buffer;
/*
* 将 name 传给 User 结构的 u_dirp，mode 传给 u_arg[1]，
* 接着调用 FileManager::Open()，返回 u_ar0，即创建文件的fd。
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
 * 作用为创建文件。将 name 传给 User 结构的 u_dirp，mode 传
 * 给 u_arg[1]，接着调用 FileManager::Creat()，返回 u_ar0，
 * 即创建文件的fd。
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

/* 关闭已打开的文件。将 fd 传给 u_arg[0]，接着调
用 FileManager::Close()。*/
void Fclose(int fd)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_ar0 = -1;
	u.u_arg[0] = fd;
	fmgr.Close();
}

/*从文件中读取数据。将 fd 传给 u_arg[0]，buffer 转为 int 型传
 * 给 u_arg[1]，length 传给 u_arg[2]，接着调用FileManager::Read()，
 * 返回 u_ar0，即读取到的字节数。
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

/* 为向文件中写入数据。将 fd 传给 u_arg[0]，buffer 
 * 转为 int 型传给 u_arg[1]，length 传给 u_arg[2]，接着调用
 * FileManager::Write()，返回 u_ar0，即写入的字节数
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

/* 显示当前目录 */
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
		//if (cur->m_name[0] != '.') 调试期间，先全部显示
			cout << cur->m_name << endl;

		memset(dir_read, 0, 32);
	}
	Fclose(fd);
}

/* 改变工作目录 
* 将 name 传给 u_dirp，并转为int 型传给 u_arg[0]，调用 FileManager::ChDir()
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

/* 为创建目录文件。将 name 传给 u_dirp，
 * 040755(默认模式)传给 u_arg[1]，调用 
 * FileManager::MkNod()。 */
void Mkdir(char* name)
{
	User& u = Kernel::Instance().GetUser();
	FileManager& fmgr = Kernel::Instance().GetFileManager();
	u.u_error = User::_NOERROR;
	u.u_dirp = name;
	u.u_arg[1] = FileManager::DEFAULT_MODE;
	fmgr.MkNod();
}

/* 作用为删除文件。将 name 传给 u_dirp，调用
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

/* 作用为建立硬链接。建立dst到src的链接
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

/*  作用为重定位文件当前读写指针。将 fd 传给 u_arg[0]，
 * pos 传给 u_arg[1]，whence 传给 u_arg[2]，接着调用
 * FileManager::Seek()，返回 u_ar0。 */
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
* 安全关闭二级文件系统SecondaryFS。将脏缓存写回镜像文件
*/
void shutdown()
{
	FileSystem& FileSys = Kernel::Instance().GetFileSystem();
	/*
	 * @comment 将SuperBlock对象的内存副本更新到
	 * 存储设备的SuperBlock中去
	 * 并把Buf中延迟写的脏页全部写回
	 * 并更新InodeTable中的内存inode节点
	*/	
	FileSys.Update();
}

/* 
 * 作用为将外部文件传入内部文件（附带创建）。该 API 的实现也是
 * 几个 API 的组合，首先调用 Fcreat 创建名为 intername 的
 * 文件，接着从外部文件每次读取 1024 个字节直至文件尾，每读
 * 取一次，调用 Fwrite 向内部文件写入读取到的内容，最后调用 Fclose 关闭文件。
*/
void Fin(char* extername, char* intername)
{
	fstream exterfile;
	int fd;
	int bufferSize = 1024;
	exterfile.open(extername, ios::in | ios::binary);
	if (!exterfile.good()) {
		cout << "外部文件 "<< extername <<" 打开失败" << endl;
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
* 作用为将内部文件传到外部文件。首先调用 Fopen 
* 打开名为 intername 的文件，接着从内部文件每次读取 1024 个字节
* 直至文件尾，每读取一次，将读取到的内容写入外部文件，最后调用 Fclose 关闭文件。
*/
void Fout(char* intername, char* extername)
{
	fstream exterfile;
	int fd;
	int bufferSize = 1024;
	exterfile.open(extername, ios::out | ios::binary);
	if (!exterfile.good()) {
		cout << "外部文件 " << extername << " 打开失败" << endl;
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


// 解析命令的函数 return 1表示继续，0表示退出系统
int parseCommand(const string& command) {
	// 使用字符串流进行分词
	stringstream ss(command);
	string token;
	vector<string> tokens;
	int ret = 1; // 1表示继续，0表示退出系统
	// 将命令及参数存储到 tokens 中
	while (ss >> token) {
		tokens.push_back(token);
	}

	if (tokens[0] == "help" && tokens.size() == 1) {
		Utility::Usage(-1);
	}
	else if (tokens[0] == "exit")
	{
		cout << "exit强制退出系统" << endl;
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
				cout << "无效 fd:" << fd << endl;
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
				cout << "创建文件失败 fd:" << fd << endl;
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
		cout << "系统无此命令" << endl;

	return ret;
}

int main()
{
	/* 内核初始化，主要是内核全局对象的引用关系建立
	以及镜像磁盘的创建及初始化，若已存在跳过这一步*/
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
		// 读取用户输入的命令
		getline(cin, command);
		if (command.size() == 0)
			continue;
		// 如果用户输入 'exit'，则退出循环
		if (command == "exit") {
			cout << "Exiting terminal simulation..." << endl;
			break;
		}

		// 解析命令
		state = parseCommand(command);
	}

	return 0;
}