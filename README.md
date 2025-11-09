Console File Explorer (C++ / Linux)

This project is a console based file explorer built using C++17 filesystem library.
User can manage files & folders directly through terminal like Linux commands.

Features / Commands Supported
Command	Description
```
ls [-a] [path]	List files and directories (show hidden if -a)
cd [path]	Change current directory
pwd	Show current working directory
cp <src> <dst>	Copy file or folder
mv <src> <dst>	Move or rename file or folder
rm <path> [-r]	Remove file or folder (-r recursive)
mkdir <dir>	Create new directory
touch <file>	Create new empty file
chmod <octal> <file>	Change permissions (ex: chmod 755 main.cpp)
search <pattern>	Search file/folder recursively
info <file>	Show file information
clear	Clear screen
help	Show command list
exit	Quit program
```
Requirements

Linux or WSL

g++ compiler (C++17 or later)
```
make
```
Install required tools (if not installed):
```
sudo apt update
sudo apt install -y build-essential make git
```
Build Project
```
make
```
Run Program
```
./explorer
```
Example Demo
```
[exp] /home/user $ mkdir test
[exp] /home/user $ cd test
[exp] /home/user/test $ touch file1.txt
[exp] /home/user/test $ ls
file1.txt
[exp] /home/user/test $ search file
/home/user/test/file1.txt
```

Project Structure
```
.
├── main.cpp
├── explorer.cpp
├── explorer.hpp
├── Makefile
└── README.md
```

Purpose of this Project

to understand linux file system operations using C++

to learn how to make a console based shell style application

to practice OOP + C++ filesystem API

Author

Pritik Kumar Sha

License

Free to use for learning & academic purpose.
