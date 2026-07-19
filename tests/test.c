#include "hashtable.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {

    // Force every key into the same bucket.
    HashTable *ht = ht_create(1);

    ht_set(ht, "apple",  "red",    strlen("red") + 1, 3);
    ht_set(ht, "banana", "yellow", strlen("yellow") + 1, 3);
    ht_set(ht, "orange", "orange", strlen("orange") + 1, 3);
    ht_set(ht, "grape",  "purple", strlen("purple") + 1, 3);

    printf("Before expiration:\n");

    const char *value;

    value = ht_get(ht, "apple");
    printf("apple  : %s\n", value ? value : "Expired");

    value = ht_get(ht, "banana");
    printf("banana : %s\n", value ? value : "Expired");

    value = ht_get(ht, "orange");
    printf("orange : %s\n", value ? value : "Expired");

    value = ht_get(ht, "grape");
    printf("grape  : %s\n", value ? value : "Expired");

    sleep(4);

    printf("\nAfter expiration:\n");

    value = ht_get(ht, "apple");
    printf("apple  : %s\n", value ? value : "Expired");

    value = ht_get(ht, "banana");
    printf("banana : %s\n", value ? value : "Expired");

    value = ht_get(ht, "orange");
    printf("orange : %s\n", value ? value : "Expired");

    value = ht_get(ht, "grape");
    printf("grape  : %s\n", value ? value : "Expired");

    printf("\nExists:\n");
    printf("apple  : %d\n", ht_exists(ht, "apple"));
    printf("banana : %d\n", ht_exists(ht, "banana"));
    printf("orange : %d\n", ht_exists(ht, "orange"));
    printf("grape  : %d\n", ht_exists(ht, "grape"));

    ht_destroy(ht);

    return 0;
}
