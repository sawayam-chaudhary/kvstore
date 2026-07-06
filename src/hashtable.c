#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

HashTable *ht_create(size_t initial_capacity){
    HashTable *ht = malloc(sizeof(HashTable));

    if(ht==NULL) return NULL;

    ht->buckets = calloc(sizeof(initial_capacity, sizeof(Entry *)));
    if(ht->buckets==NULL){
         free(ht);
         return NULL;
    }

    ht->bucket_count = initial_capacity;
    ht->size=0;

    return ht;
}

void ht_destroy(HashTable *ht){

    if(ht==NULL) return;
    for(size_t i=0; i< ht->bucket_count; i++){
        Entry * curr_entry = ht->buckets[i];
        while(curr_entry){
            Entry * next_entry = curr_entry->next;
            free(curr_entry->key);
            free(curr_entry->value);
            free(curr_entry);
            curr_entry=next_entry;
        }
    }
    free(ht->buckets);
    free(ht);
}
