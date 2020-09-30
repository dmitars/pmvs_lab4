#define FUSE_USE_VERSION 30
#define _FILE_OFFSET_BITS 64

#include "../include/task1.h"

#define LINE_NUMBER 95

char file_system_path[256] = "";
char* example_txt = "Hello world! Student Dmitriy Tarasenko,1823195";
char test_txt[LINE_NUMBER][50];
char* readme_txt = "Student Dmitriy Tarasenko,1823195";

static int get_attribute(const char* path, struct stat *st){
	st->st_uid = getuid();
	memset(st,0,sizeof(struct stat));
	if(strcmp(path,"/") == 0){
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2;
	}else if(strcmp(path,"/bar") == 0){
		st->st_mode = S_IFDIR | 000;
		st->st_nlink = 3;
	}else if(strcmp(path,"/foo") == 0){
		st->st_mode = S_IFDIR | 711;
		st->st_nlink = 3;
	}else if(strcmp(path, "/bar/readme.txt") == 0){
		st->st_mode = S_IFREG | 555;
		st->st_nlink = 1;
	}else if(strcmp(path, "/bar/baz") == 0){
		st->st_mode = S_IFDIR | 444;
		st->st_nlink = 2;
	}else if(strcmp(path, "/bar/baz/example") == 0){
		st->st_mode = S_IFREG | 222;
		st->st_nlink = 1;
		st->st_size = strlen(example_txt);
	}else if(strcmp(path,"/foo/test.txt") == 0){
		st->st_mode = S_IFREG | 000;
		st->st_nlink = 1;
		int size = 0;
		for(int i = 0;i<LINE_NUMBER;i++){
			size+=strlen(test_txt[i])+1;
		}
		st->st_size = size;
	}else if(strcmp(path,"/foo/bin") == 0){
		st->st_mode = S_IFDIR | 766;
		st->st_nlink = 2;
	}else if(strcmp(path,"/foo/bin/touch") == 0){
		st->st_mode = S_IFREG | 777;
		st->st_nlink = 1;
		struct stat buffer;
		stat("/usr/bin/touch",&buffer);
		st->st_size = buffer.st_size;
	}else{
		return -ENOENT;
	}
	return 0;
}

static int read_directory(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * fi){
	(void) offset;
	(void) fi;
	filler(buf,".",NULL,0);
	filler(buf,"..",NULL,0);
	if(strcmp(path,"/") == 0){
		filler(buf,"foo",NULL,0);
		filler(buf,"bar",NULL,0);
		if(strcmp(file_system_path,"/mnt/fuse/")!=0){
			filler(buf,file_system_path+1,NULL,0);
		}
		return 0;
	}else if (strcmp(path,"/foo") == 0){
		filler(buf,"bin",NULL,0);
		filler(buf,"test.txt",NULL,0);
		return 0;
	}else if(strcmp(path,"/bar") == 0){
		filler(buf,"baz",NULL,0);
		filler(buf,"readme.txt",NULL,0);
		return 0;
	}else if(strcmp(path,"/foo/bin") == 0){
		filler(buf,"touch",NULL,0);
		return 0;
	}else if(strcmp(path,"/bar/baz") == 0){
		filler(buf,"example",NULL,0);
		return 0;
	}
	return -ENOENT;
}

static int read_file(const char* path, char* buf, size_t size,off_t offset, struct fuse_file_info* fi){
	size_t length;
	(void) fi;
	char* read_data;

	if(strcmp(path,"/bar/readme.txt") == 0){
		length = strlen(readme_txt);
		read_data = readme_txt;
	}else if(strcmp(path,"/bar/baz/example") == 0){
		length = strlen(example_txt);
		read_data = example_txt;
	}else if(strcmp(path,"/foo/test.txt") == 0){
		char temp[LINE_NUMBER*50+400];
		strcpy(temp,"");
		for(int i = 0;i<LINE_NUMBER;i++){
			strcat(temp,test_txt[i]);
			strcat(temp,"\n");
		}
		length = strlen(temp);
		read_data = temp;
	}else if(strcmp(path,"/foo/bin/touch") == 0){
		struct stat touch_stat;
		stat("/usr/bin/touch",&touch_stat);
		length = touch_stat.st_size;
		
		FILE* f = fopen("/usr/bin/touch","rb");
		unsigned char buffer[length];
		fread(buffer,length,1,f);
		read_data = buffer;
		fclose(f);
	}else{
		return -ENOENT;
	}

	

	if(offset<length){
		if(offset+size>length){
			size = length-offset;
		}
		memcpy(buf, read_data+offset,size);
		return size;
	}

	return 0;
}

static int open_file(const char* path, struct fuse_file_info* fi){
	/*int fd;
	char fpath[256];
	strcpy(fpath,file_system_path);
	strcat(fpath,path);
	printf("file will be opened: %s\n",fpath);
	fd = open(fpath,fi->flags);
	if(fd<0){
		return -ENOENT;
	}
	fi->fh = fd;*/
	return 0;
}

static int make_directory(const char* path, mode_t mode){
	if(mkdir(path,mode == -1)){
		return -errno;
	}
	return 0;
}

int main(int argc, char** argv){
	if(argc<2){
		printf("incorrect number of parameters\n");
		return 0;
	}
	strcpy(file_system_path,argv[1]);
	char number_line[8];
	for(int i = 0;i<LINE_NUMBER;i++){
		sprintf(number_line,"%d: ",i+1);
		strcat(test_txt[i],number_line);
		strcat(test_txt[i],"Some text");
	}

	 struct fuse_operations operations = {
		.getattr = get_attribute,
		.readdir = read_directory,
		.read = read_file,
		.open = open_file,
		.mkdir = make_directory,
	};
	return fuse_main(argc,argv,&operations,NULL);
}
