#include "hashtable.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void test_set_get(void);
void test_delete(void);
void test_binary_value(void);
void test_update_value(void);
void test_save_load(void);
void test_save_load_ttl(void);
void test_corrupted_file(void);

int main(void)
{
    test_set_get();
    test_delete();
    test_binary_value();
    test_update_value();
    test_save_load();
    test_save_load_ttl();
    test_corrupted_file();

    printf("All tests passed!\n");

    return 0;
}

void test_set_get(void){

    HashTable *ht = ht_create(16);

    assert(ht != NULL);

    const char payload[] = "red";

    assert(ht_set(ht, "apple", payload, sizeof(payload) - 1, 100));

    size_t len = 0;

    const void *value = ht_get(ht, "apple", &len);

    assert(value != NULL);
    assert(len == sizeof(payload) - 1);
    assert(memcmp(value, payload, len) == 0);

    assert(ht_exists(ht, "apple"));

    ht_destroy(ht);
}

void test_delete(void){
    HashTable *ht = ht_create(16);

    assert(ht != NULL);

    const char payload[] = "red";
    assert(ht_set(ht, "apple", payload, sizeof(payload) - 1, 100));

    assert(ht_delete(ht, "apple"));
    assert(!ht_exists(ht, "apple"));

    size_t len = 0;

    const void *value = ht_get(ht, "apple", &len);

    assert(value == NULL);

    ht_destroy(ht);
}

void test_binary_value(void){
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    const char payload[] = {'a', 'b', '\0', 'c', 'd'};
    assert(ht_set(ht, "apple", payload, sizeof(payload), 100));
    
    size_t len = 0;
    const void *value = ht_get(ht, "apple", &len);
    assert(value != NULL);
    assert(len == sizeof(payload));
    assert(memcmp(value, payload, len)==0);

    ht_destroy(ht);
}

void test_update_value(void){
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    const char payload1[] = "red";
    const char payload2[] = "blue";

    assert(ht_set(ht, "colour", payload1, sizeof(payload1) - 1, 100));
    assert(ht_set(ht, "colour", payload2, sizeof(payload2) - 1, 100));

    size_t len = 0;
    const void *value = ht_get(ht, "colour", &len);

    assert(value != NULL);
    assert(len == sizeof(payload2) - 1);
    assert(memcmp(value, payload2, len) == 0);

    ht_destroy(ht);
}

void test_ttl_expiration(void){
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    const char payload[] = "red";
    assert(ht_set(ht, "colour", payload, sizeof(payload) - 1, 5));

    size_t len = 0;
    assert(ht_get(ht, "colour", &len) != NULL);

    sleep(6);

    assert(ht_get(ht, "colour", &len) == NULL);
    assert(!ht_exists(ht, "colour"));
    
    ht_destroy(ht);
}

void test_save_load(void){
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    const char value1[] = "red";
    const char value2[] = "blue";

    assert(ht_set(ht, "apple", value1, sizeof(value1) - 1, 300));
    assert(ht_set(ht, "baka", value2, sizeof(value2) - 1, 300));

    assert(ht_save(ht, "test.db"));

    ht_destroy(ht);

    ht = ht_create(16);

    assert(ht_load(ht, "test.db"));

    size_t len;

    const void *value;

    value = ht_get(ht, "apple", &len);
    assert(value != NULL);
    assert(memcmp(value, value1, len)==0);

    value=ht_get(ht, "baka", &len);
    assert(value != NULL);
    assert(memcmp(value, value2, len)==0);

    remove("test.db");

    ht_destroy(ht);
}

void test_save_load_ttl(void){
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    const char payload[] = "red";
    assert(ht_set(ht, "apple", payload, sizeof(payload) -1, 5));
    
    assert(ht_save(ht, "test1.db"));
    ht_destroy(ht);

    sleep(6);
    
    ht=ht_create(16);
    assert(ht != NULL);
    assert(ht_load(ht, "test1.db"));

    size_t len = 0;

    assert(ht_get(ht, "apple", &len) == NULL);

    remove("test1.db");

    ht_destroy(ht);
}

void test_corrupted_file(void){
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    const char payload[] = "red";
    assert(ht_set(ht, "apple", payload, sizeof(payload) - 1, 100));
    assert(ht_save(ht, "corrupt.db"));
    ht_destroy(ht);

    int result = truncate("corrupt.db", 5);
    assert(result == 0);

    HashTable *ht2 = ht_create(16);
    assert(ht2 != NULL);
    assert(!ht_load(ht2, "corrupt.db"));

    remove("corrupt.db");
    ht_destroy(ht2);
}
    
    
