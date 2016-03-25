#include "common.h"
#include <string.h>
#define min(a,b) ((a)>(b)?(b):(a))

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
	{"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
	{"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
	{"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
	{"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
	{"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
	{"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
	{"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
	{"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
	{"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
	{"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
	{"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
	{"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

#define FILE_MAX 25
typedef struct {
	bool opened;
	uint32_t offset;
} Fstate;
static Fstate file_state[FILE_MAX+3];//stdin,stdout,stderr
int fs_open(const char *pathname, int flags)	/* Ignore flags */
{
	Log("open %s",pathname);
	int i;
	for(i=0;i<FILE_MAX;++i)
	{
		if(strcmp(file_table[i].name,pathname)==0)
		{
			file_state[i+3].opened=true;
			file_state[i+3].offset=0;
			return i+3;
		}
	}
	assert(0);
	return 0;
}
int fs_read(int fd, void *buf, int len)
{
	if(fd<3)
	{
		//Ignore Operations
		return 0;
	}
	int i=fd-3;
	Log("read %d %p %d:%x",fd,buf,len,file_state[i+3].offset);
	
	assert(file_state[i+3].opened);
	uint32_t offset=file_state[i+3].offset+file_table[i].disk_offset;
	int readbyte=min(len,file_table[i].size-file_state[i+3].offset);
	assert(readbyte>=0);
	ide_read(buf,offset,readbyte);
	file_state[i+3].offset+=readbyte;
	Log("%d\n",readbyte);
	return readbyte;
}
int fs_write(int fd, void *buf, int len)
{
	//Log("write %d %p %d",fd,buf,len);
	assert(fd>=3);
	int i=fd-3;
	assert(file_state[i+3].opened);
	uint32_t offset=file_state[i+3].offset+file_table[i].disk_offset;
	int writebyte=min(len,file_table[i].size-file_state[i+3].offset);
	assert(writebyte>=0);
	ide_write(buf,offset,writebyte);
	file_state[i+3].offset+=writebyte;
	return writebyte;
}
int fs_lseek(int fd, int offset, int whence)
{
	//Log("seek %d %d %d",fd,offset,whence);
	assert(fd>=3);
	int i=fd-3;
	assert(file_state[i+3].opened);
	switch(whence)
	{
	case SEEK_SET:
		file_state[i+3].offset=offset;break;
	case SEEK_CUR:
		file_state[i+3].offset+=offset;break;
	case SEEK_END:
		file_state[i+3].offset=file_table[i].size+offset;break;
	default:
		Log("%x\n",whence);
		Log("%x\n",SEEK_SET);
		Log("%x\n",SEEK_CUR);
		Log("%x\n",SEEK_END);
		assert(0);
		return -1;
	}
	return file_state[i+3].offset;
}
int fs_close(int fd)
{
	//Log("close %d",fd);
	assert(fd>=3);
	int i=fd-3;
	file_state[i+3].opened=false;
	return 0;
}
