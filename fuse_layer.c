#define _FILE_OFFSET_BITS 64
#define _XOPEN_SOURCE 600
#define FUSE_USE_VERSION 26
#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fuse_layer.h"

#define BLOCKSIZE (1024 * 1024)

static int min(int a, int b) { return (a<b)?a:b; }
static int max(int a, int b) { return (a>b)?a:b; }

static int d_getattr(const char *path, struct stat *buf)
{
	FILE *fp = fopen("/var/log/fuse/ded.log", "a");

	if (fp)
	{
		fprintf(fp, "Getattr: %s\n", path);
		fclose(fp);
	}
	else
	{
		exit(1);
	}

	memset(buf, 0, sizeof (struct stat));

	if (strcmp(path+1, "hello") == 0)
	{
		buf->st_mode = S_IFREG | 0444;
		buf->st_blksize = BLOCKSIZE;
		buf->st_size = 6;
		buf->st_blocks = (buf->st_size + BLOCKSIZE - 1)/BLOCKSIZE;
		buf->st_nlink = 1;
	}
	else if (strcmp(path, "/") == 0)
	{
		buf->st_mode = S_IFDIR | 0444;
		buf->st_blksize = 1024;
		buf->st_blocks = 1;
		buf->st_size = 1024;
		buf->st_nlink = 2;
	}

	return 0;
}

static int d_open(const char *path, struct fuse_file_info *fi)
{
	FILE *fp = fopen("/var/log/fuse/ded.log", "a");

	if (fp)
	{
		fprintf(fp, "Open: %s\n", path);
		fclose(fp);
	}
	else
	{
		exit(1);
	}

	if (strcmp(path+1, "hello") == 0)
	{
		if ((fi->flags & 3) == O_RDONLY)
		{
			fi->fh = 1;
			return 0;
		}
		else
		{
			return -EACCES;
		}
	}

	return -ENOENT;
}

static int d_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	FILE *fp = fopen("/var/log/fuse/ded.log", "a");

	if (fp)
	{
		fprintf(fp, "Read: %s, %lu, %lu, %lu\n", path, size, offset, fi->fh);
		fflush(fp);
	}
	else
	{
		exit(1);
	}

	size_t bytes_copied = 0;
	if (fi->fh == 1)
	{
		if (fp)
		{
			fprintf(fp, "Read: Here!\n");
			fflush(fp);
		}

		size_t start = min(offset, 6);
		size_t end = min(size + offset, 6);
		bytes_copied = end - start;

		if (fp)
		{
			fprintf(fp, "Read: %ld-%ld=%ld\n", end,start,bytes_copied);
			fflush(fp);
			close(fp);
		}

		if (bytes_copied == 0)
		{
			return 0;
		}

		memcpy(buf, "hello\n" + start, bytes_copied);
		return bytes_copied;
	}

	if (fp)
	{
		fprintf(fp, "End read: %ld\n", bytes_copied);
		fflush(fp);
		close(fp);
	}

	return 0;
}

static int d_release(const char *path, struct fuse_file_info *fi)
{
	return 0; // Do nothing
}

static int d_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
	FILE *fp = fopen("/var/log/fuse/ded.log", "a");

	if (fp)
	{
		fprintf(fp, "Readdir: %s\n", path);
		fclose(fp);
	}
	else
	{
		exit(1);
	}

	if (strcmp(path, "/"))
	{
		return -ENOENT;
	}

	filler(buf, ".", 0, 0);
	filler(buf, "..", 0, 0);
	filler(buf, "hello", 0, 0);

	return 0;
}

static struct fuse_operations fops = {
	.getattr = d_getattr,
	.open    = d_open,
	.read    = d_read,
	.release = d_release,
	.readdir = d_readdir
};

int initialize_filesystem(const char *mountpoint)
{
	const char *pname = "dedup";
	const char *argv[2] = {pname, mountpoint};

	return fuse_main(2, argv, &fops, NULL);
}

