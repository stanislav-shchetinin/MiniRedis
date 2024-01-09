//
// Created by stass on 09.01.2024.
//

#ifndef MINIREDIS_HASHTABLE_H
#define MINIREDIS_HASHTABLE_H

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>

struct HNode {
    struct HNode *next;
    uint64_t hcode;
};

struct HTab {
    struct HNode **tab;
    size_t mask;
    size_t size;
};

bool cmp(struct HNode *, struct HNode *);
void h_init(struct HTab *htab, size_t n);
void h_insert(struct HTab *htab, struct HNode *node);
struct HNode **h_lookup(struct HTab *htab, struct HNode *key, bool (*cmp)(struct HNode *, struct HNode *));
struct HNode *h_detach(struct HTab *htab, struct HNode **from);


#endif //MINIREDIS_HASHTABLE_H
