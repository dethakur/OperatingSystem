#ifndef _TARFS_H
#define _TARFS_H

#include<sys/process.h>
#define NAME_MAX 255

typedef int64_t ssize_t;
typedef uint64_t size_t;
typedef uint64_t off_t;


/*
#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
*/


extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

//enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };
enum { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };
char* cwd;

struct posix_header_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

typedef struct posix_header_ustar tarfs;
uint64_t tarfs_init(char *,process*);


int tarfs_open(char *,int);
ssize_t tarfs_read(int,void *,size_t);
int tarfs_close(int);
int tarfs_read_next(int,int,char *);
//void *tarfs_opendir(char *);
int tarfs_getdents(int,char *,int);
int tarfs_closedir(void *);
int tarfs_chdir(char *);
off_t tarfs_lseek(int,off_t,int);

//file descriptor table
//tarfs *fdtable[10];  //each entry in array is a tarfs pointer
//int64_t offset_tbl[10];
//int maxfd;
int maxinode; 

//maxfd=4; //temp hardcoding

/*
// directories
#define NAME_MAX 255
struct dirent
{
	long d_ino;
	int d_off;
	unsigned short d_reclen;
	char d_name [NAME_MAX+1];
};
struct fdMetadata{
	int fileDescriptor;
	char buffer[2048];
	int size;
	int offset;
};
*/

struct dirent *tarfs_readdir(void *);


int convert_to_int(volatile char *);
int strcmp(char *,char *);
int strcmpn(char *,char *,int);
int strlen(char *);
void tarfs_memcpy(char *,char *,int size);


#endif
