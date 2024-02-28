#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include "stdbool.h"

struct hashmap_bucket {
  char *key;
  void *data;
  struct hashmap_bucket *next;
};

struct hashmap {
  struct hashmap_bucket **bucket_array;
  size_t size;
  size_t count;
};

void hashmap_create(struct hashmap *self);
void hashmap_destroy(struct hashmap *self);
bool hashmap_set(struct hashmap *self, char *key, void *data);
void **hashmap_get(const struct hashmap *self, const char *key);

#endif // ifndef HASHMAP_H
