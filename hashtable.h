#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <time.h>
#include <stdbool.h>

typedef struct Entry{
    char *key;
    char *value;
    size_t value_len;
    time_t expires_at; //reserved for phase one
    struct Entry *next;
} Entry;

typedef struct HashTable{
    Entry **buckets;
    size_t bucket_count;
    size_t size;
} HashTable;

/*
 * Ownership contract:
 *   - ht_set() copies key and value into table-owned memory.
 *   - ht_get()'s returned pointer is valid only until the next
 *     mutating call (ht_set, ht_delete, or an internal resize).
 *   - ht_destroy() frees everything the table owns.
 */


HashTable *ht_create(size_t initial_capacity);
void ht_destroy(HashTable *ht);
bool ht_set(HashTable *ht, const char *key, const char *value, size_t value_len); // using const to maintain the contract that user input wouldn't be modified. It will be just used for modification inside the table
const char* ht_get(HashTable *ht, const char *key, size_t *out_len);   // an outer parameter out_len to bypass limitation of single return and store how many values are read in it
bool ht_delete(HashTable *ht, const char *key); //using false if key not found.
bool ht_exists(HashTable *ht, const char *key);

#endif // HASHTABLE_H

