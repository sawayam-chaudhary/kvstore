#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
static const uint64_t FNV_PRIME = 1099511628211ULL;

static uint64_t hash_key(const char *key){
    const char *ptr=key;
    uint64_t hash = FNV_OFFSET_BASIS;

    while(*ptr != '\0'){
        hash ^= (unsigned char)*ptr;
        hash *= FNV_PRIME;
        ptr++;
    }

    return hash;
}
    

HashTable *ht_create(size_t initial_capacity){
    HashTable *ht = malloc(sizeof(HashTable));

    if(ht==NULL) return NULL;

    ht->buckets = calloc(sizeof(initial_capacity), sizeof(Entry *));
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

bool ht_set(HashTable *ht, const char *key, const char *value, size_t value_len){

    if(ht==NULL || key == NULL || value == NULL) return false;

    uint64_t hash = hash_key(key);
    size_t bucket_index = hash % ht->bucket_count;

    Entry *curr = ht->buckets[bucket_index];
    Entry *prev=NULL;

    while(curr){

    if(strcmp(curr->key, key)==0){

    char *new_value = malloc(value_len + 1);
    if(new_value==NULL) return false;

    memcpy(new_value, value, value_len + 1);

    free(curr->value);
    curr->value = new_value;
    curr->value_len = value_len;
    return true;
    }
    
    prev=curr;
    curr=curr->next;
    }

    Entry* new_entry = malloc(sizeof(Entry));
    if(new_entry==NULL) return false;

    size_t key_len = strlen(key);

    new_entry->key = malloc(key_len+1);
    if(new_entry->key==NULL){
        free(new_entry);
        return false;
    }
    memcpy(new_entry->key, key, key_len + 1);

    new_entry->value = malloc(value_len + 1);
    if(new_entry->value==NULL){
        free(new_entry->key);
        free(new_entry);
        return false;
    }
    memcpy(new_entry->value, value, value_len+1);

    new_entry->value_len = value_len;
    new_entry->expires_at = 0;
    new_entry->next = NULL;

    if(prev==NULL){
        ht->buckets[bucket_index]= new_entry;
    }

    else{
        prev->next = new_entry;
    }

    ht->size++;
    return true;
}

    



