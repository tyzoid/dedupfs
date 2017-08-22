#include <mysql.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "database.h"

static int get_directory_id(const char*, entry_t*);
static int initialize_connection(const char*, const unsigned int, const char*, const char*, const char*);
static MYSQL* get_db_connection();


static MYSQL *_self_mysql_connection = 0;

static int initialize_connection(const char* server, const unsigned int port, const char* user, const char* pass, const char* db)
{
	if (_self_mysql_connection)
	{
		mysql_close(_self_mysql_connection);
		_self_mysql_connection = 0;
	}

	_self_mysql_connection = mysql_init(0);
	if (!mysql_real_connect(_self_mysql_connection, server, user, pass, db, port, 0, 0))
	{
		mysql_close(_self_mysql_connection);
		_self_mysql_connection = 0;
		return 1;
	}

	return 0;
}

static MYSQL* get_db_connection()
{
	if (_self_mysql_connection)
	{
		return _self_mysql_connection;
	}
	else
	{
		if (initialize_connection(
			"127.0.0.1",
			3306,
			"filesystem",
			"CoKmAv6THoXgZZsJb0zHCccIA7zL0QY3ZV1Lof2L",
			"fs"))
		{
			return 0;
		}

		return _self_mysql_connection;
	}
}

static int get_directory_id(const char *dirpath, entry_t *dir_id)
{
	// Path must begin with a slash
	if (*dirpath != '/') return -ENOENT;

	MYSQL *connection = db_get_connection();

	char query[512];

	size_t pathlen = 0;
	size_t depth = 0;

	const char *dp = dirpath;
	while (*dp)
	{
		pathlen++;

		// Ignore leading '/'
		if (*++dp == '/')
		{
			depth++;
		}
	}

	// Subtract one, as we're not storing leading '/'
	char *pathelems = alloca((pathlen - 1) * sizeof(char));
	char **pathparts = alloca(depth * sizeof(char *));

	dp = dirpath;
	char *npath = pathelems;
	char **ppart = pathparts;

	*(ppart++) = npath;

	// Ignore leading '/'
	while (*++dp)
	{
		if (*dp == '/')
		{
			*(npath++) = 0;
			*(ppart++) = npath;
		}
		else
		{
			*(npath++) = *dp;
		}
	}

	entry_t parent_id = 0;
	for (size_t i = 0; i < depth, i++)
	{
		// TODO: Sanitize pathparts[i] for mysql special characters.
		// For now, disallow these characters.
		snprintf(query, 512, "select id from directory where parent_id='%lu' and name='%s'", parent_id, pathparts[i]);

		if (mysql_query(connection, query)) {
			fprintf(stderr, "%s\n", mysql_error(connection));
			exit(1);
		}

		MYSQL_RES *result = mysql_store_result(connection);

		MYSQL_ROW row = 0;

		// There should only be a maximum of one row.
		if ((row = mysql_fetch_row(result)) != 0)
		{
			parent_id = strtol(row[0], 0, 10);
		}
		else
		{
			return -ENOENT;
		}
	}

	*dir_id = parent_id;

	return 0;
}

int list_directory(const char *dirpath, dd_fse_t **results, size_t size, off_t offset)
{
	entry_t dir_id = 0;

	int result = get_directory_id(dirpath, &dir_id);
	if (result < 0) return result;

	return list_directory(dir_id, results, size, offset);
}

int list_directory(entry_t dir_id, dd_fse_t **results, size_t size, off_t offset)
{
	
}

	if (mysql_query(connection, "select id, vid, claimed from requests where claimed is null order by id asc limit 1")) {
		fprintf(stderr, "%s\n", mysql_error(connection));
		exit(1);
	}

	MYSQL_RES *result = mysql_store_result(connection);

	MYSQL_ROW row = 0;

	if ((row = mysql_fetch_row(result)) != 0)
		printf("%s %s %s\n", row[0], row[1], row[2]);

	if (row == 0) return 0;

	errno = 0;
	unsigned long vid = strtol(row[0], 0, 10);
	if (errno) return -2;

	strcpy(*video_id, row[1]);

	mysql_free_result(result);

	char *query = malloc(128);

	snprintf(query, 128, "update requests set claimed=NOW() where id=%lu and claimed is null", vid);

	if (mysql_query(connection, query)) {
		fprintf(stderr, "%s\n", mysql_error(connection));
		exit(1);
	}

	if (!mysql_affected_rows(connection)) return 0;

	return 1;
}

int removeVideoCode(const char *video_code, MYSQL *connection)
{
	char *query = malloc(128);
	char *escaped_vid = malloc(32);

	mysql_real_escape_string(connection, escaped_vid, video_code, 32);
	snprintf(query, 128, "delete from requests where vid='%s'", escaped_vid);

	if (mysql_query(connection, query)) {
		fprintf(stderr, mysql_error(connection));
		exit(1);
	}

	return 0;
}
