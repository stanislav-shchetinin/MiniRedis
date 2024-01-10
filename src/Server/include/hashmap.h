#ifndef MINIREDIS_HASHMAP_H
#define MINIREDIS_HASHMAP_H

#include <unistd.h>
#include "hashtable.h"

#define RESIZING_WORK 128
#define MAX_LOAD 8
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) );})

struct HMap {
    struct HTab ht1;
    struct HTab ht2;
    size_t resizing_pos;
};

struct Entry {
    struct HNode node;
    char* key;
    char* val;
};

void hm_help_resizing(struct HMap *hmap);
struct HNode *hm_lookup(struct HMap *hmap, struct HNode *key, bool (*cmp)(struct HNode *, struct HNode *));
void hm_start_resizing(struct HMap *hmap);
void hm_insert(struct HMap *hmap, struct HNode *node);
struct HNode *hm_pop(struct HMap *hmap, struct HNode *key, bool (*cmp)(struct HNode *, struct HNode *));
bool entry_eq(struct HNode *lhs, struct HNode *rhs);

#endif //MINIREDIS_HASHMAP_H
