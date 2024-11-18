#include "hash.h"
#include <stdlib.h>
#include <string.h>

#define START_CAPACITY 16
#define HASH_P 73
#define RESIZE_FACTOR 4

static size_t hash_func(const char *key)
{
	size_t hash = 0;
	size_t p = 1;
	for (size_t i = 0; i < strlen(key); ++i) {
		hash += p * key[i];
		p *= HASH_P;
	}
	return hash;
}

static int resize(struct hash_table *table)
{
	size_t new_capacity = 2 * table->capacity;
	struct hash_table_bucket *buckets = malloc(new_capacity * sizeof(struct hash_table_bucket));
	if (!buckets) {
		return -1;
	}

	for (size_t i = 0; i < new_capacity; ++i) {
		LIST_INIT(buckets + i);
	}

	for (size_t i = 0; i < table->capacity; ++i) {
		while (!LIST_EMPTY(table->buckets + i)) {
			struct hash_table_bucket_node *node = LIST_FIRST(table->buckets + i);
			size_t new_node_number = hash_func(node->key) % new_capacity;
			LIST_REMOVE(node, entries);
			LIST_INSERT_HEAD(buckets + new_node_number, node, entries);
		}
	}
	free(table->buckets);
	table->buckets = buckets;
	table->capacity = new_capacity;
	return 0;
}

int hash_table_init(struct hash_table *table)
{
	table->capacity = START_CAPACITY;
	table->size = 0;
	table->buckets = malloc(table->capacity * sizeof(struct hash_table_bucket));
	if (!table->buckets) {
		return -1;
	}
	for (size_t i = 0; i < table->capacity; ++i) {
		LIST_INIT(table->buckets + i);
	}

	return 0;
}

void *hash_table_add(struct hash_table *table, char *key, void *value)
{
	size_t bucket_num = hash_func(key) % table->capacity;
	struct hash_table_bucket_node *node;
	LIST_FOREACH(node, table->buckets + bucket_num, entries)
	{
		if (!strcmp(node->key, key)) {
			void *old_value = node->value;
			node->value = value;
			return old_value;
		}
	};
	node = malloc(sizeof(struct hash_table_bucket_node));
	if (!node) {
		return NULL; // todo: fix bad API
	}
	node->key = key;
	node->value = value;
	LIST_INSERT_HEAD(table->buckets + bucket_num, node, entries);
	table->size++;
	if (table->size / table->capacity > RESIZE_FACTOR) {
		resize(table);
	}
	return NULL; // todo: fix bad API
}

static struct hash_table_bucket_node *find_node(struct hash_table *table, char *key)
{
	size_t bucket_num = hash_func(key) % table->capacity;
	struct hash_table_bucket_node *node;
	LIST_FOREACH(node, table->buckets + bucket_num, entries)
	{
		if (!strcmp(key, node->key)) {
			break;
		}
	}
	return node;
}

void *hash_table_find(struct hash_table *table, char *key)
{
	struct hash_table_bucket_node *node = find_node(table, key);
	if (!node) {
		return NULL;
	}
	return node->value;
}

void *hash_table_remove(struct hash_table *table, char *key)
{
	struct hash_table_bucket_node *node = find_node(table, key);
	if (!node) {
		return NULL;
	}

	LIST_REMOVE(node, entries);
	free(node->key);
	void *value = node->value;
	free(node);
	return value;
}

void hash_table_drop(struct hash_table *table)
{
	for (size_t i = 0; i < table->capacity; ++i) {
		while (!LIST_EMPTY(table->buckets + i)) {
			struct hash_table_bucket_node *node = LIST_FIRST(table->buckets + i);
			LIST_REMOVE(node, entries);
			free(node->key);
			free(node->value);
			free(node);
		}
	}
	free(table->buckets);
}
