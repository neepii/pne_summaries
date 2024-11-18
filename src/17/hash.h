#pragma once

#include <stddef.h>
#include <sys/queue.h>

struct hash_table_bucket_node
{
	char *key;
	void *value;
	LIST_ENTRY(hash_table_bucket_node) entries;
};

LIST_HEAD(hash_table_bucket, hash_table_bucket_node);

struct hash_table
{
	size_t size;
	size_t capacity;
	struct hash_table_bucket *buckets;
};

int hash_table_init(struct hash_table *table);

void *hash_table_add(struct hash_table *table, char *key, void *value);

void *hash_table_remove(struct hash_table *table, char *key);

void *hash_table_find(struct hash_table *table, char *key);

void hash_table_drop(struct hash_table *table);
