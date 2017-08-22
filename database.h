#ifndef DEDUP_DATABASE_H
#define DEDUP_DATABASE_H

#define FILESYSTEM_ENTRY_TYPE_UNKNOWN   0
#define FILESYSTEM_ENTRY_TYPE_FILE      1
#define FILESYSTEM_ENTRY_TYPE_DIRECTORY 2

typedef size_t entry_t

typedef struct _dedup_filesystem_entry
{
	entry_t parentdir,
	entry_t selfid,
	int entry_type,
	char *filename,
} dd_fse_t;

int list_directory(const char* dirpath, dd_fse_t **results, size_t size, off_t offset);
int list_directory(entry_t dir_id, dd_fse_t **results, size_t size, off_t offset);


#endif
