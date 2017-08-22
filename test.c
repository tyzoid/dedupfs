#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
	struct stat s;
	memset(&s, 0, sizeof s);

	stat("fuse_layer.h", &s);

	printf("st_dev: %ld\n", s.st_dev);
	printf("st_ino: %ld\n", s.st_ino);
	printf("st_mode: %ld\n", s.st_mode);
	printf("st_nlink: %ld\n", s.st_nlink);
	printf("st_uid: %ld\n", s.st_uid);
	printf("st_gid: %ld\n", s.st_gid);
	printf("st_rdev: %ld\n", s.st_rdev);
	printf("st_size: %ld\n", s.st_size);
	printf("st_blksize: %ld\n", s.st_blksize);
	printf("st_blocks: %ld\n", s.st_blocks);

	printf("st_atime: %ld\n", s.st_atime);
	printf("st_mtime: %ld\n", s.st_mtime);
	printf("st_ctime: %ld\n", s.st_ctime);

	return 0;
}
