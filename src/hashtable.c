#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

static const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
static const uint64_t FNV_PRIME = 1099511628211ULL;

static bool ht_resize(HashTable *ht);

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

     if(initial_capacity==0) return NULL;

    HashTable *ht = malloc(sizeof(HashTable));

    if(ht==NULL) return NULL;

    ht->buckets = calloc(initial_capacity, sizeof(Entry *));
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

bool ht_set(HashTable *ht, const char *key, const char *value, size_t value_len, int ttl_seconds){

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

    time_t current_time = time(NULL);
    curr->expires_at = current_time + ttl_seconds;
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

    time_t current_time = time(NULL);
    new_entry->expires_at = current_time + ttl_seconds;

    new_entry->next = NULL;

    if(prev==NULL){
        ht->buckets[bucket_index]= new_entry;
    }

    else{
        prev->next = new_entry;
    }
     
    ht->size++;
    double load_factor = (double)ht->size / ht->bucket_count;

      if (load_factor >= 0.75) {
    (void)ht_resize(ht);
     }
    return true;
}

const char* ht_get(HashTable *ht, const char *key){

    if(ht==NULL || key==NULL) return NULL;

    uint64_t hash = hash_key(key);
    size_t bucket_index = hash % ht->bucket_count;

    Entry *curr = ht->buckets[bucket_index];
    Entry *prev = NULL;
    time_t current_time = time(NULL);
    
    while(curr){
        

        if(curr->expires_at <= current_time){

              if(prev==NULL){

                  ht->buckets[bucket_index] = curr->next;
                  free(curr->key);
                  free(curr->value);
                  free(curr);
                  curr=ht->buckets[bucket_index];
                  continue;

              }
              else{

                Entry *next = curr->next;
                prev->next = next;
                free(curr->key);
                free(curr->value);
                free(curr);
                curr=next;
                continue;

              }
        }


        if(strcmp(curr->key, key)==0){

            return curr->value;

        }

        prev = curr;
        curr=curr->next;

     }
    
    return NULL; 
}

bool ht_exists(HashTable *ht, const char *key){

    const char* answer = ht_get(ht, key);
    return answer != NULL;

}

bool ht_delete(HashTable *ht, const char *key){
    if(ht==NULL || key==NULL) return false;

    uint64_t hash = hash_key(key);
    size_t bucket_index = hash % ht->bucket_count;

    Entry * curr = ht->buckets[bucket_index];
    Entry *prev = NULL;

    while(curr){

        if(strcmp(curr->key , key)==0){
            if(prev==NULL){
                ht->buckets[bucket_index]= curr->next;
            }
            else{
                prev->next = curr->next;
            }

            free(curr->value);
            free(curr->key);
            free(curr);

            ht->size--;
            return true;
        }

        prev = curr;
        curr=curr->next;
    }
    
    return false;
}

static bool ht_resize(HashTable *ht){

    size_t new_bucket_count = ht->bucket_count * 2;
    Entry **new_buckets = calloc(new_bucket_count, sizeof(Entry *));
    if(new_buckets == NULL) return false;

    for(size_t i=0; i < ht->bucket_count; i++){

        Entry *curr = ht->buckets[i];

        while(curr){

            Entry *next = curr->next;
            uint64_t hash = hash_key(curr->key);
            size_t new_bucket_index = hash % new_bucket_count;

            curr->next = new_buckets[new_bucket_index];
            new_buckets[new_bucket_index]=curr;

            curr=next;

        }
    }

    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->bucket_count = new_bucket_count;
    return true;
}







    



