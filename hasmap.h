#ifndef HASHMAP_H
#define HASHMAP_H

#include "stdbool.h"
#include <stddef.h>

struct ast_node;

struct hashmap_bucket {
  char *key;
  union hashmap_val_union {
    double d;
    struct ast_node *ast_node;
  } data;
  struct hashmap_bucket *next;
};

struct hashmap {
  struct hashmap_bucket **bucket_array;
  size_t size;
  size_t count;
};

void hashmap_create(struct hashmap *self);
void hashmap_destroy(struct hashmap *self);
bool hashmap_set(struct hashmap *self, char *key, union hashmap_val_union data);
union hashmap_val_union *hashmap_get(const struct hashmap *self,
                                     const char *key);

#endif // ifndef HASHMAP_H
