#include <fuse.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static int read_directory(const char* path, void *buf, fuse_fill_dir_t filler,off_t offset,struct fuse_file_info* f);
static int read_file(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi);
static int make_directory(const char* path, mode_t mode);
static int open_file(const char* path, struct fuse_file_info* fi);
static int get_attribute(const char* path, struct stat* stat);

