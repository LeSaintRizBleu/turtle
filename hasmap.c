#define _DEFAULT_SOURCE
#include "hasmap.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t fnv1a_hash(const char *key) {
  const size_t fnv_offset_basis = 14695981039346656037ULL;
  const size_t fnv_prime = 1099511628211ULL;
  size_t hash = fnv_offset_basis;
  for (size_t i = 0; key[i]; i++) {
    hash ^= key[i];
    hash *= fnv_prime;
  }
  return hash;
}

void hashmap_create(struct hashmap *self) {
  self->size = 8;
  self->count = 0;
  self->bucket_array = calloc(self->size, sizeof(struct hashmap_bucket *));
  assert(self->bucket_array);
}

void hashmap_destroy(struct hashmap *self) {
  for (size_t i = 0; i < self->size; i++) {
    struct hashmap_bucket *b = self->bucket_array[i];
    while (b) {
      struct hashmap_bucket *next = b->next;
      free(b);
      b = next;
    }
  }
  free(self->bucket_array);
}

bool hashmap_set(struct hashmap *self, char *key,
                 union hashmap_val_union data) {
  if (self->count + 1 >= self->size) {
    // rehash
    size_t old_size = self->size;
    self->size *= 2;
    self->bucket_array = reallocarray(self->bucket_array, self->size,
                                      sizeof(struct hashmap_bucket *));
    for (size_t i = 0; i < old_size; ++i) {
      struct hashmap_bucket *b = self->bucket_array[i];
      struct hashmap_bucket **p = &self->bucket_array[i];
      while (b) {
        struct hashmap_bucket *next = b->next;
        const size_t hash = fnv1a_hash(b->key);
        const size_t place = hash % self->size;
        if (place != i) {
          b->next = self->bucket_array[place];
          self->bucket_array[place] = b;
          *p = next;
        } else {
          p = &b->next;
        }
        b = next;
      }
    }
  }
  size_t hash = fnv1a_hash(key);
  size_t index = hash % self->size;
  struct hashmap_bucket *b = self->bucket_array[index];
  while (b && strcmp(b->key, key)) {
    b = b->next;
  }
  self->count++;
  if (b) {
    b->data = data;
    return false;
  } else {
    struct hashmap_bucket *new = malloc(sizeof(struct hashmap_bucket));
    new->data = data;
    new->key = key;
    new->next = self->bucket_array[index];
    self->bucket_array[index] = new;
    return true;
  }
}

union hashmap_val_union *hashmap_get(const struct hashmap *self,
                                     const char *key) {
  size_t hash = fnv1a_hash(key);
  size_t index = hash % self->size;
  struct hashmap_bucket *b = self->bucket_array[index];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return &b->data;
    }
    b = b->next;
  }
  return NULL;
}
